/*
 * System.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include "System.h"

#include "neighbour_finders/CutoffFinder.h"

namespace ba {

System::System() :
		ParticleSet() {
	set_name("system");

	_nn_finder = std::shared_ptr<NNFinder>(new CutoffFinder());
}

System::~System() {

}

std::shared_ptr<System> System::empty_copy() const {
	std::shared_ptr<System> new_syst(new System());

	new_syst->time = this->time;
	new_syst->box = this->box;

	return new_syst;
}

void System::set_neighbours() {
	_nn_finder->set_neighbours(_particles);
}

#ifdef PYTHON_BINDINGS

void export_System(py::module &m) {
	py::class_<System, std::shared_ptr<System>> parser(m, "System");

	parser
		.def(py::init<>())
		.def("empty_copy", &System::empty_copy)
		.def("set_neighbours", &System::set_neighbours);
}

#endif

} /* namespace ba */
