/*
 * System.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include "System.h"

namespace ba {

System::System() :
		ParticleSet() {
	set_name("system");
}

System::~System() {

}

std::shared_ptr<System> System::empty_copy() const {
	std::shared_ptr<System> new_syst(new System());

	new_syst->time = this->time;
	new_syst->box = this->box;

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_System(py::module &m) {
	py::class_<System, ParticleSet, std::shared_ptr<System>> parser(m, "System");

	parser
		.def(py::init<>())
		.def("empty_copy", &System::empty_copy)
		.def_readwrite("time", &System::time)
		.def_readwrite("box", &System::box);
}

#endif

} /* namespace ba */
