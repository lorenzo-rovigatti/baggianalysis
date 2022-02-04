/*
 * FormFactor.cpp
 *
 *  Created on: 20 dic 2019
 *      Author: lorenzo
 */

#include "FormFactor.h"

#include "../utils/Random.h"

namespace ba {

FormFactor::FormFactor(vector_scalar q_modules, int q_repetitions, bool compute_on_molecules) :
				SystemObservable<std::map<double, double>>(),
				_q_modules(q_modules),
				_q_repetitions(q_repetitions),
				_compute_on_molecules(compute_on_molecules) {
}

FormFactor::~FormFactor() {

}

void FormFactor::clear_b_factors() {
	_b_factors.clear();
}

void FormFactor::set_b_factors(vector_scalar b_factors) {
	_b_factors = b_factors;
}

void FormFactor::analyse_system(std::shared_ptr<System> system) {
	if(_compute_on_molecules) {
		if(system->molecules().size() == 0) {
			std::string error = fmt::format("FormFactor: No molecules found, aborting");
			throw std::runtime_error(error);
		}
		for(auto molecule: system->molecules()) {
			analyse_particle_set(molecule);
		}
	}
	else {
		analyse_particle_set(system);
	}
}

void FormFactor::analyse_particle_set(std::shared_ptr<ParticleSet> p_set) {
	vec3 com = p_set->com();

	bool has_b_factors = _b_factors.size() > 0;

	if(has_b_factors && _b_factors.size() != p_set->N()) {
		std::string error = fmt::format("FormFactor: The size of the b-factor vector ({}) is different from the number of particles ({})", _b_factors.size(), p_set->N());
		throw std::runtime_error(error);
	}

	for(auto q_module: _q_modules) {
		for(int i = 0; i < _q_repetitions; i++) {
			vec3 new_q = BARANDOM.random_vector_on_sphere() * q_module;
			double sq_cos = 0.;
			double sq_sin = 0.;
			for(uint p_idx = 0; p_idx < p_set->N(); p_idx++) {
				auto p = p_set->particles()[p_idx];
				vec3 r = p->position() - com;
				double b_factor = (has_b_factors) ? _b_factors[p_idx] : 1;

				double qr = glm::dot(new_q, r);
				sq_cos += b_factor * std::cos(qr);
				sq_sin += b_factor * std::sin(qr);
			}
			_result[q_module] += (SQR(sq_cos) + SQR(sq_sin)) / p_set->N();
		}
	}

	_times_called++;
}

std::map<double, double> FormFactor::_finalised_result() {
	std::map<double, double> result(_result);
	for(auto &p: result) {
		p.second /= _times_called * _q_repetitions;
	}
	return result;
}

#ifdef PYTHON_BINDINGS

void export_FormFactor(py::module &m) {
	py::class_<FormFactor, std::shared_ptr<FormFactor>> obs(m, "FormFactor", R"pbdoc(
Compute the form factor of a system or the average form factor of the molecules contained in the system.

The mathematical definition of the form factor is the same as `structure factor <https://en.wikipedia.org/wiki/Structure_factor>`_,
with the difference that the form factor is to be intended as the scattering response of single objects.
As a result, the size of the simulation box does not set the smallest wave vector that can be used in its computation.
)pbdoc");

	obs.def(py::init<vector_scalar, int, bool>(), py::arg("q_modules"), py::arg("q_repetitions"), py::arg("compute_on_molecules"), R"pbdoc(
Parameters
----------
q_modules: List(float)
    The modules of the wave vectors that will be used to compute the form factor.
q_repetitions: int
    The number of random realisations of each wave vector module that will be used to compute the form factor.
compute_on_molecules: bool
    If True, the final form factor will be computed as the average form factor of the molecules composing the system.
	)pbdoc");

	obs.def("analyse_particle_set", &FormFactor::analyse_particle_set, py::arg("p_set"), R"pbdoc(
Compute the form factor of the given :class:`ParticleSet`.

Parameters
----------
p_set: :class:`ParticleSet`
    The object containing the input particles.
)pbdoc");

	obs.def("clear_b_factors", &FormFactor::clear_b_factors, R"pbdoc(
	Reset the b-factors to their default values (b = 1 for each particle).
)pbdoc");

	obs.def("set_b_factors", &FormFactor::set_b_factors, py::arg("b_factors"), R"pbdoc(
	Set the particles' b-factors (that is, the scattering amplitudes) that will be used to compute the form factor.

	Parameters
	----------
	b_factors : List(float)
		The vector of b-factors to be used in the calculation of the P(q). The length of the vector should be equal to the number of particles in the system.
)pbdoc");

	PY_EXPORT_SYSTEM_OBS(obs, FormFactor);
}

#endif

} /* namespace ba */
