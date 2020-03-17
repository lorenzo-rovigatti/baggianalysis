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
	std::shared_ptr<System> new_syst(std::make_shared<System>());

	new_syst->time = this->time;
	new_syst->box = this->box;

	return new_syst;
}

int System::available_index() const {
	auto idxs = indexes();
	auto largest_idx_it = std::max_element(idxs.begin(), idxs.end());
	if(largest_idx_it == idxs.end()) {
		return 0;
	}
	else {
		return *largest_idx_it + 1;
	}
}

std::vector<std::shared_ptr<ParticleSet>> &System::molecules() {
	return _molecules;
}

const std::vector<std::shared_ptr<ParticleSet>> &System::molecules() const {
	return _molecules;
}

#ifdef PYTHON_BINDINGS

void export_System(py::module &m) {
	py::class_<System, ParticleSet, std::shared_ptr<System>> system(m, "System", R"pbdoc(
		 Store a configuration. 
	)pbdoc");

	system.def(py::init<>(), R"pbdoc(
		 The constructor takes no arguments. 
	)pbdoc");
	system.def("empty_copy", &System::empty_copy, R"pbdoc(
		 Return a copy of this sytem (same time and box, no particles). 
	)pbdoc");
	system.def("available_index", &System::available_index, R"pbdoc(
		 Return particle index that is available (that is, that is not used by any of the particles already present in the system).
	)pbdoc");
	// here we tell pybind11 which of the two molecules() methods we want to have bindings for
	system.def("molecules", (std::vector<std::shared_ptr<ParticleSet>> &(System::*)())(&System::molecules));
	system.def_readwrite("time", &System::time);
	system.def_readwrite("box", &System::box);

}

#endif

} /* namespace ba */
