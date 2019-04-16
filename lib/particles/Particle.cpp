/*
 * Particle.cpp
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#include "Particle.h"

namespace ba {

Particle::Particle() {

}

Particle::Particle(particle_type t, vec3 pos, vec3 vel) :
				_type(t),
				_position(pos),
				_velocity(vel) {

}

Particle::~Particle() {

}

#ifdef PYTHON_BINDINGS

void export_Particle(py::module &m) {
	py::class_<Particle, std::shared_ptr<Particle>> parser(m, "Particle");

	parser
		.def(py::init<>())
		.def(py::init<particle_type, vec3, vec3>());
//		.def_readwrite("type", &Particle::_type)
//		.def_readwrite("position", &Particle::_position)
//		.def_readwrite("velocity", &Particle::_velocity);
}

#endif

} /* namespace ba */
