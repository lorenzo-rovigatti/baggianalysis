/*
 * System.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include "System.h"

namespace ba {

System::System() {

}

System::~System() {

}

std::shared_ptr<System> System::empty_copy() const {
	std::shared_ptr<System> new_syst(new System());

	new_syst->time = this->time;
	new_syst->box = this->box;

	return new_syst;
}

vec3 System::com() const {
	vec3 com(0., 0., 0.);
	for(auto &pos : particles.positions) {
		com += pos;
	}
	com /= particles.N();
	return com;
}

vec3 System::average_velocity() const {
	vec3 v_avg(0., 0., 0.);
	for(auto &vel : particles.velocities) {
		v_avg += vel;
	}
	v_avg /= particles.N();
	return v_avg;
}

#ifdef PYTHON_BINDINGS

void export_System(py::module &m) {
	py::class_<System, std::shared_ptr<System>> parser(m, "System");

	parser
		.def(py::init<>())
		.def("empty_copy", &System::empty_copy)
		.def("com", &System::com)
		.def("average_velocity", &System::average_velocity)
		.def_readwrite("particles", &System::particles);
}

#endif

} /* namespace ba */
