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
	for(auto q_module: _q_modules) {
		_result[q_module] = 0.;
	}
}

FormFactor::~FormFactor() {

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

	for(auto q_module: _q_modules) {
		for(int i = 0; i < _q_repetitions; i++) {
			vec3 new_q = BARANDOM.random_vector_on_sphere() * q_module;
			double sq_cos = 0.;
			double sq_sin = 0.;
			for(auto p: p_set->particles()) {
				vec3 r = p->position() - com;
				double qr = glm::dot(new_q, r);
				sq_cos += std::cos(qr);
				sq_sin += std::sin(qr);
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
	py::class_<FormFactor, std::shared_ptr<FormFactor>> obs(m, "FormFactor");

	obs.def(py::init<vector_scalar, int, bool>());
	obs.def("analyse_particle_set", &FormFactor::analyse_particle_set);

	PY_EXPORT_SYSTEM_OBS(obs, FormFactor);
}

#endif

} /* namespace ba */
