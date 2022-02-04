/*
 * GyrationRadius.cpp
 *
 *  Created on: 20 dic 2019
 *      Author: lorenzo
 */

#include "GyrationRadius.h"

#include "../utils/Random.h"

namespace ba {

GyrationRadius::GyrationRadius(bool compute_on_molecules) :
				SystemObservable<double>(),
				_compute_on_molecules(compute_on_molecules) {
}

GyrationRadius::~GyrationRadius() {

}

void GyrationRadius::analyse_system(std::shared_ptr<System> system) {
	if(_compute_on_molecules) {
		if(system->molecules().size() == 0) {
			std::string error = fmt::format("GyrationRadius: No molecules found, aborting");
			throw std::runtime_error(error);
		}
		for(auto molecule: system->molecules()) {
			analyse_particle_set(molecule, system->box);
		}
	}
	else {
		analyse_particle_set(system, system->box);
	}
}

void GyrationRadius::analyse_particle_set(std::shared_ptr<ParticleSet> p_set, const vec3 &box) {
	double rg2 = 0;

	for(uint i = 0; i < p_set->N(); i++) {
		auto p = p_set->particles()[i];
		for(uint j = i + 1; j < p_set->N(); j++) {
			auto q = p_set->particles()[j];

			vec3 distance = p->position() - q->position();
			distance -= glm::round(distance / box) * box;
			rg2 += glm::dot(distance, distance);
		}
	}
	_result += std::sqrt(rg2 / SQR(p_set->N()));

	_times_called++;
}

double GyrationRadius::_finalised_result() {
	return _result / _times_called;
}

#ifdef PYTHON_BINDINGS

void export_GyrationRadius(py::module &m) {
	py::class_<GyrationRadius, std::shared_ptr<GyrationRadius>> obs(m, "GyrationRadius", R"pbdoc(
Compute the gyration radius of a system or the average gyration radius of the molecules contained in the system.
)pbdoc");

	obs.def(py::init<bool>(), py::arg("compute_on_molecules"), R"pbdoc(
Parameters
----------
compute_on_molecules: bool
    If True, the final gyration radius will be computed as the average gyration radius of the molecules composing the system.
	)pbdoc");

	obs.def("analyse_particle_set", &GyrationRadius::analyse_particle_set, py::arg("p_set"), py::arg("box"), R"pbdoc(
Compute the gyration radius of the given :class:`ParticleSet`.

Parameters
----------
p_set: :class:`ParticleSet`
    The object containing the input particles.
box: numpy.ndarray
        The simulation box.
)pbdoc");

	PY_EXPORT_SYSTEM_OBS(obs, GyrationRadius);
}

#endif

} /* namespace ba */
