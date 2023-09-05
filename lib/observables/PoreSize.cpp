/*
 * CumulativePoreSize.cpp
 *
 *  Created on: May 23, 2019
 *      Author: lorenzo
 */

#include "PoreSize.h"

#include "../utils/Random.h"

#include <glm/gtx/component_wise.hpp>
#include <cfloat>

namespace ba {

double radius_wrapper(unsigned n, const double *x, double *grad, void *f_data);
double constraint(unsigned n, const double *x, double *grad, void *f_data);

PoreSize::PoreSize(int N_attempts, double r_cut, double particle_radius, double max_time) :
				SystemObservable<vector_scalar>(),
				_N_attempts(N_attempts),
				_r_cut(r_cut),
				_particle_radius(particle_radius),
				_particle_radius_sqr(SQR(particle_radius)),
				_maxtime(max_time),
				_lists(true) {

	// initialise the optimisation machinery. This is the local solver used by the augmented Lagrangian method (https://nlopt.readthedocs.io/en/latest/NLopt_Algorithms/#augmented-lagrangian-algorithm)
	_local_opt = nlopt::opt(nlopt::LN_SBPLX, 3);
	_local_opt.set_xtol_rel(1e-6);
	_local_opt.set_maxtime(1.);

	_opt = nlopt::opt(nlopt::LN_AUGLAG, 3);
	_opt.set_local_optimizer(_local_opt);

	// we restrict the solver to the region of space comprised between 0 and the box size
	std::vector<double> lower_bounds( { 0., 0., 0. });
	_opt.set_lower_bounds(lower_bounds);

	_opt.set_max_objective(radius_wrapper, (void *) this);
	_opt.set_xtol_rel(1e-4);
	_opt.set_maxtime(_maxtime);
}

PoreSize::~PoreSize() {

}

double PoreSize::radius(const vec3 &centre) {
	auto centre_cell = _lists.get_cell(centre);

	bool done = false;
	double R_sqr = -1;
	// here we loop on the various cells, shell after shell
	for(unsigned int i = 0; i < _lists.cell_shifts().size() && !done; i++) {
		// the current shell might contain a particle that is closer than one found in the previous shell. We thus tell the code
		// to analyse also this shell and then quit the loop
		if(R_sqr > 0.) {
			done = true;
		}

		auto &shifts = _lists.cell_shifts()[i];
		for(auto &cell_shift : shifts) {
			auto cell = centre_cell + cell_shift;
			cell[0] = (cell[0] + _lists.N_cells_side[0]) % _lists.N_cells_side[0];
			cell[1] = (cell[1] + _lists.N_cells_side[1]) % _lists.N_cells_side[1];
			cell[2] = (cell[2] + _lists.N_cells_side[2]) % _lists.N_cells_side[2];
			int cell_idx = cell[0] + _lists.N_cells_side[0] * (cell[1] + cell[2] * _lists.N_cells_side[1]);

			int current = _lists.heads[cell_idx];
			while(current != -1) {
				vec3 distance = centre - current_frame->particles()[current]->position();
				// periodic boundary conditions
				distance -= glm::round(distance / current_frame->box) * current_frame->box;
				double distance_sqr = glm::dot(distance, distance);
				// if the point is inside one of the spheres we just return
				if(distance_sqr < _particle_radius_sqr) {
					return 0.;
				}
				if(R_sqr < 0. || distance_sqr < R_sqr) {
					R_sqr = distance_sqr;
				}

				current = _lists.next[current];
			}
		}
	}

	return sqrt(R_sqr) - _particle_radius;
}

void PoreSize::analyse_system(std::shared_ptr<System> frame) {
	current_frame = frame;
	_lists.init_cells(current_frame->particles(), current_frame->box, _r_cut);

	std::vector<double> upper_bounds( { frame->box[0], frame->box[1], frame->box[2] });
	_opt.set_upper_bounds(upper_bounds);

	for(int i = 0; i < _N_attempts; i++) {
		if(i > 0 && (i % (_N_attempts / 10) == 0)) {
			BA_INFO("{} steps completed", i);
		}
		current_position = vec3(BARANDOM.uniform() * frame->box[0], BARANDOM.uniform() * frame->box[1], BARANDOM.uniform() * frame->box[2]);

		// if the random position is inside one of the particles we disregard it
		if(radius(current_position) > 0.) {
			_opt.remove_inequality_constraints();
			_opt.add_inequality_constraint(constraint, this, 1e-6);

			std::vector<double> starting_position( { current_position[0], current_position[1], current_position[2] });
			double maximum_radius;

			try {
				_opt.optimize(starting_position, maximum_radius);
			}
			catch(std::exception &e) {
				BA_CRITICAL("nlopt failed: {}", e.what());
				exit(1);
			}

			if(maximum_radius > 0.) {
				_result.push_back(maximum_radius);
			}
		}
	}
}

double radius_wrapper(unsigned n, const double *x, double *grad, void *f_data) {
	PoreSize *ps = (PoreSize *) f_data;
	vec3 centre(x[0], x[1], x[2]);
	return ps->radius(centre);
}

double constraint(unsigned n, const double *x, double *grad, void *f_data) {
	PoreSize *ps = (PoreSize *) f_data;
	vec3 centre(x[0], x[1], x[2]);

	double R = ps->radius(centre);
	vec3 distance = centre - ps->current_position;
	// periodic boundary conditions
	distance -= glm::round(distance / ps->current_frame->box) * ps->current_frame->box;

	// nlopt requires constraints to have the form f(x) <= 0. Here we set d - R <= 0, where d is the
	// distance between the sphere centre and the position
	return sqrt(glm::dot(distance, distance)) - R;
}

#ifdef PYTHON_BINDINGS

void export_PoreSize(py::module &m) {
	py::class_<PoreSize, std::shared_ptr<PoreSize>> obs(m, "PoreSize", R"pbdoc(
Find the pore sizes of a system. Requires `NLopt <https://nlopt.readthedocs.io/en/latest/>`_.

The pore size distribution :math:`P_G(r)` is defined as in
`Bhattacharya and Gubbins (2006) <https://doi.org/10.1021/la052651k>`_. The
algorithm is schematically represented in the following figure 
(taken from `Sorichetti, Hugovieux and Kob (2020) <https://doi.org/10.1021/acs.macromol.9b02166>`_, 
where the algorithm was applied to a polymeric system):

.. image:: images/pore_size.png
  :width: 400

Given a randomly chosen point :math:`\mathbf r_p` in the void phase, one has to
find the sphere with the largest radius :math:`r` containing :math:`\mathbf r_p` and
which does not overlap with any particle. This problem can be reformulated
as that of maximizing the function

.. math::

	r(\mathbf r_c) \equiv \min_{i=1,\dots M} \{|\mathbf r_c - \mathbf r_i|\} - \sigma/2,
	\label{obj_function}

subject to the constraint 

.. math::
	|\mathbf r_c-\mathbf r_p| - r(\mathbf r_c) \leq 0,

where :math:`\mathbf r_c` is the position of the sphere's center and
:math:`\mathbf r_i` are the positions of the centers of the particles. If the
maximization of the function above is carried out for a
large enough number of points :math:`\mathbf r_p`, the resulting (normalized)
histogram of :math:`r` values will converge to :math:`P_G(r)`.

The problem of calculating :math:`P_G(r)` reduces therefore to a nonlinear
optimization problem, which can be solved with a standard algorithm. 
Here we use the open-source Sbplx algorithm of
the `NLopt library <https://nlopt.readthedocs.io/en/latest/>`_, which can handle discontinuous
objective functions.
)pbdoc");

	obs.def(py::init<int, double, double, double>(), py::arg("N_attempts"), py::arg("r_cut") = 1.0, py::arg("particle_radius") = 0.5, py::arg("max_time") = 1.0, R"pbdoc(
The constructur builds an observable that computes ``N_attempts`` different pore sizes by applying the algorithm described above 
for ``N_attempts`` random values of :math:`r_p`.

Parameters
----------
N_attempts: int
    The number of pore size evaluations that will be carried out per system.
r_cut: float
    The characteristic size of the particle-particle repulsion. Using the right number will speed up the computation but does not impact the quality of the results.
particle_radius: float
    The radius of the particles stored in the system. This is usually half ``r_cut``.
max_time: float
    The timeout (in seconds) beyond which NLopt will stop looking for a solution to the optimisation problem.
 
)pbdoc");
	obs.def("radius", &PoreSize::radius, py::arg("centre"), R"pbdoc(
Calculates the radius of the largest sphere centred in ``centre`` that does not overlap with any of the particles.

Parameters
----------
centre: numpy.ndarray
    The reference position

Returns
-------
float
    The radius of the largest sphere centred in ``centre`` that does not overlap with any of the particles.

)pbdoc");

	PY_EXPORT_SYSTEM_OBS(obs, PoreSize);
}

#endif

} /* namespace ba */
