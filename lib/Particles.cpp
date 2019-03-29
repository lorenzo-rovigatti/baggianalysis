/*
 * Particles.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include "Particles.h"

namespace ba {

Particles::Particles() {

}

Particles::~Particles() {

}

uint Particles::N() const {
	return types.size();
}

#ifdef PYTHON_BINDINGS

void export_Particles(py::module &m) {
	py::class_<Particles, std::shared_ptr<Particles>> parser(m, "Particles");

	parser
		.def(py::init<>())
		.def("N", &Particles::N)
		.def_readwrite("types", &Particles::types)
		.def_readwrite("positions", &Particles::positions)
		.def_readwrite("velocities", &Particles::velocities);
}

#endif

} /* namespace ba */
