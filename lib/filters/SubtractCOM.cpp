/*
 * SubtractCOM.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "SubtractCOM.h"

using namespace std;

namespace ba {

SubtractCOM::SubtractCOM() {

}

SubtractCOM::~SubtractCOM() {

}

shared_ptr<System> SubtractCOM::filter(std::shared_ptr<const System> syst) {
	shared_ptr<System> new_syst(syst->empty_copy());

	vec3 com = syst->com();
	vec3 v_avg = syst->average_velocity();

	for(auto p : syst->particles()) {
		shared_ptr<Particle> new_particle(new Particle(*p.get()));
		new_particle->set_position(new_particle->position() - com);
		new_particle->set_velocity(new_particle->velocity() - v_avg);
		new_syst->add_particle(new_particle);
	}

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_SubtractCOM(py::module &m) {
	py::class_<SubtractCOM, BaseFilter, std::shared_ptr<SubtractCOM>> filter(m, "SubtractCOM");

	filter
		.def(py::init<>())
		.def("filter", &SubtractCOM::filter);
}

#endif

} /* namespace ba */
