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
	return types().size();
}

#ifdef PYTHON_BINDINGS

void export_Particles(py::module &m) {
	py::class_<Particles, std::shared_ptr<Particles>> parser(m, "Particles");

	// these are needed when binding methods that are overloaded
	using types_type = const std::vector<particle_type> &(Particles::*) () const;
	using vectors_type = const vector_vec3 &(Particles::*) () const;

	parser
		.def(py::init<>())
		.def("N", &Particles::N)
		.def_property_readonly("types", static_cast<types_type>(&Particles::types))
		.def_property_readonly("positions", static_cast<vectors_type>(&Particles::positions))
		.def_property_readonly("velocities", static_cast<vectors_type>(&Particles::velocities));
}

#endif

} /* namespace ba */
