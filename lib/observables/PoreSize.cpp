/*
 * CumulativePoreSize.cpp
 *
 *  Created on: May 23, 2019
 *      Author: lorenzo
 */

#include "PoreSize.h"

#include <glm/gtx/component_wise.hpp>
#include <nlopt.hpp>
#include <cfloat>
#include <random>

namespace ba {

double radius_wrapper(unsigned n, const double *x, double *grad, void *f_data);
double constraint(unsigned n, const double *x, double *grad, void *f_data);

PoreSize::PoreSize(int N_attempts) :
				_N_attempts(N_attempts) {
	set_r_cut(1.0);
	set_particle_radius(1.0);
	set_maxtime(1.0);
}

PoreSize::~PoreSize() {

}

void PoreSize::set_r_cut(double r_cut) {
	_r_cut = r_cut;
}

void PoreSize::set_particle_radius(double particle_radius) {
	_particle_radius = particle_radius;
	_particle_radius_sqr = SQR(particle_radius);
}

void PoreSize::set_maxtime(double maxtime) {
	_maxtime = maxtime;
}

double PoreSize::radius(const vec3 &centre) {
	auto centre_cell = _get_cell(centre);

	bool done = false;
	double R_sqr = -1;
	// here we loop on the various cells, shell after shell
	for(unsigned int i = 0; i < _cell_shifts.size() && !done; i++) {
		// the current shell might contain a particle that is closer than one found in the previous shell. We thus tell the code
		// to analyse also this shell and then quit the loop
		if(R_sqr > 0.) {
			done = true;
		}

		auto &shifts = _cell_shifts[i];
		for(auto &cell_shift : shifts) {
			auto cell = centre_cell + cell_shift;
			cell[0] = (cell[0] + _N_cells_side[0]) % _N_cells_side[0];
			cell[1] = (cell[1] + _N_cells_side[1]) % _N_cells_side[1];
			cell[2] = (cell[2] + _N_cells_side[2]) % _N_cells_side[2];
			int cell_idx = cell[0] + _N_cells_side[0] * (cell[1] + cell[2] * _N_cells_side[1]);

			int current = _heads[cell_idx];
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

				current = _next[current];
			}
		}
	}

	return sqrt(R_sqr) - _particle_radius;
}

vector_scalar PoreSize::compute(std::shared_ptr<BaseTrajectory> trajectory) {
	vector_scalar results;

	std::random_device dev;
	// deterministic runs for debugging purposes, use dev() to "randomly" initialise the rng
	std::mt19937 rng(12345);
	std::uniform_real_distribution<double> uniform(0., 1.);

	// initialise the optimisation machinery
	nlopt::opt opt(nlopt::LN_COBYLA, 3);
	std::vector<double> lower_bounds({0., 0., 0.});
	opt.set_lower_bounds(lower_bounds);
	opt.set_max_objective(radius_wrapper, (void *) this);
	opt.set_xtol_rel(1e-4);
	opt.set_maxtime(_maxtime);

	auto frame = trajectory->next_frame();
	while(frame != nullptr) {
		current_frame = frame;
		_init_cells();

		std::vector<double> upper_bounds({frame->box[0], frame->box[1], frame->box[2]});
		opt.set_upper_bounds(upper_bounds);

		for(int i = 0; i < _N_attempts; i++) {
			if(i > 0 && (i % (_N_attempts / 10) == 0)) {
				std::cerr << i << " steps completed" << std::endl;
			}
			current_position = vec3(uniform(rng) * frame->box[0], uniform(rng) * frame->box[1], uniform(rng) * frame->box[2]);

			// if the random position is inside one of the particles we disregard it
			if(radius(current_position) > 0.) {
				opt.remove_inequality_constraints();
				opt.add_inequality_constraint(constraint, this, 1e-6);

				std::vector<double> starting_position({current_position[0], current_position[1], current_position[2]});
				double maximum_radius;

				try {
					opt.optimize(starting_position, maximum_radius);
				}
				catch(std::exception &e) {
					std::cerr << "nlopt failed: " << e.what() << std::endl;
					exit(1);
				}

				if(maximum_radius > 0.) {
					results.push_back(maximum_radius);
				}
			}
		}

		frame = trajectory->next_frame();
	}

	return results;
}

void PoreSize::_init_cells() {
	_next.clear();
	_heads.clear();
	_cell_shifts.clear();

	_N_cells_side = glm::floor(current_frame->box / _r_cut);
	for(int i = 0; i < 3; i++) {
		// everything becomes easier if the number of cells per side is odd
		if((_N_cells_side[i] % 2) == 0) {
			_N_cells_side[i]--;
		}
		if(_N_cells_side[i] < 3) {
			_N_cells_side[i] = 3;
		}
	}
	int N_cells = _N_cells_side[0] * _N_cells_side[1] * _N_cells_side[2];
	_next.resize(current_frame->N(), -1);
	_heads.resize(N_cells, -1);

	std::cerr << "Number of cells: " << N_cells << ", cells per box side: <" << _N_cells_side[0] << ", " << _N_cells_side[1] << ", " << _N_cells_side[2] << ">" << std::endl;

	for(unsigned int i = 0; i < current_frame->N(); i++) {
		int cell_idx = _get_cell_index(current_frame->particles()[i]->position());
		_next[i] = _heads[cell_idx];
		_heads[cell_idx] = i;
	}

	// here we generate lists of all possible shifts according to their order (the shell around the central cell they belong to)
	int max_shift = (glm::compMax(_N_cells_side) - 1) / 2;
	_cell_shifts.resize(max_shift);
	vec3 shift;
	for(shift[0] = -_N_cells_side[0] / 2; shift[0] <= _N_cells_side[0] / 2; shift[0]++) {
		for(shift[1] = -_N_cells_side[1] / 2; shift[1] <= _N_cells_side[1] / 2; shift[1]++) {
			for(shift[2] = -_N_cells_side[2] / 2; shift[2] <= _N_cells_side[2] / 2; shift[2]++) {
				auto shift_abs = glm::abs(shift);
				int shift_order = glm::compMax(shift_abs) - 1;
				if(shift_order == -1) {
					shift_order = 0;
				}
				_cell_shifts[shift_order].push_back(shift);
			}
		}
	}

	std::cerr << "Maximum shift order: " << max_shift << ", number of shifts per order:";
	for(auto &shift : _cell_shifts) {
		std::cerr << " " << shift.size();
	}
	std::cerr << std::endl;
}

int PoreSize::_get_cell_index(const vec3 &pos) const {
	int res = (int) ((pos[0] / current_frame->box[0] - floor(pos[0] / current_frame->box[0])) * (1.0 - DBL_EPSILON) * _N_cells_side[0]);
	res += _N_cells_side[0] * ((int) ((pos[1] / current_frame->box[1] - floor(pos[1] / current_frame->box[1])) * (1.0 - DBL_EPSILON) * _N_cells_side[1]));
	res += _N_cells_side[0] * _N_cells_side[1] * ((int) ((pos[2] / current_frame->box[2] - floor(pos[2] / current_frame->box[2])) * (1.0 - DBL_EPSILON) * _N_cells_side[2]));
	return res;
}

glm::ivec3 PoreSize::_get_cell(const vec3 &pos) const {
	int cell_index = _get_cell_index(pos);
	return glm::ivec3(cell_index % _N_cells_side[0], (cell_index / _N_cells_side[0]) % _N_cells_side[1], cell_index / (_N_cells_side[0] * _N_cells_side[1]));
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
	py::class_<PoreSize, std::shared_ptr<PoreSize>> parser(m, "PoreSize");

	parser
		.def(py::init<int>())
		.def("set_r_cut", &PoreSize::set_r_cut)
		.def("set_particle_radius", &PoreSize::set_particle_radius)
		.def("set_maxtime", &PoreSize::set_maxtime)
		.def("radius", &PoreSize::radius)
		.def("compute", &PoreSize::compute);
}

#endif

} /* namespace ba */
