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
		Return a copy of this system (same time and box, no particles).

        Returns
        -------
        :class:`System`
            A new system with the same :attr:`time` and :attr:`box` of this one. 
	)pbdoc");
	system.def("available_index", &System::available_index, R"pbdoc(
		Return a particle index that can be used in the context of this system (that is, that is not used by any of the particles already present in the system).

        Returns
        -------
        int
            A number that can be used as a valid index for a new particle.
	)pbdoc");
	// here we tell pybind11 which of the two molecules() methods we want to have bindings for
	system.def("molecules", (std::vector<std::shared_ptr<ParticleSet>> &(System::*)())(&System::molecules), R"pbdoc(
		Return all the molecules contained in the system in the form of a list of :class:`ParticleSet`.

        Returns
        -------
        list(:class:`ParticleSet`)
            The list of molecules contained in the system.
	)pbdoc");
	system.def_readwrite("time", &System::time);
	system.def_readwrite("box", &System::box);

}

#endif

} /* namespace ba */
