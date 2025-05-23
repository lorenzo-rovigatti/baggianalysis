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

shared_ptr<System> SubtractCOM::filter(std::shared_ptr<System> syst) {
	shared_ptr<System> new_syst(syst->empty_copy());

	vec3 com = syst->com();
	vec3 v_avg = syst->velocity();

	for(auto p : syst->particles()) {
		shared_ptr<Particle> new_particle = p->make_copy(p->index());
		new_particle->set_position(new_particle->position() - com);
		new_particle->set_velocity(new_particle->velocity() - v_avg);
		new_syst->add_particle(new_particle);
	}

	new_syst->copy_topology_from(syst);

	return new_syst;
}

void export_SubtractCOM(py::module &m) {
	py::class_<SubtractCOM, BaseFilter, std::shared_ptr<SubtractCOM>> filter(m, "SubtractCOM", R"pbdoc(
A filter that removes the centre of mass position and velocity from each particle.
	)pbdoc");

	filter.def(py::init<>(), R"pbdoc(
		The constructor does not take any parameters.
	)pbdoc");
}

} /* namespace ba */
