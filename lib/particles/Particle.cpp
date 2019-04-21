/*
 * Particle.cpp
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#include "Particle.h"

namespace ba {

int Particle::_current_index = 0;

Particle::Particle() :
				Particle(DEFAULT_PARTICLE_TYPE, vec3(0., 0., 0.), vec3(0., 0., 0.)) {

}

Particle::Particle(particle_type t, vec3 pos, vec3 vel) :
				Particle(Particle::_current_index, t, pos, vel) {
	Particle::_current_index++;
}

Particle::Particle(int index, particle_type t, vec3 pos, vec3 vel) :
				_index(index),
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
		.def(py::init<particle_type, vec3, vec3>())
		.def(py::init<int, particle_type, vec3, vec3>())
		.def_property("index", &Particle::index, &Particle::set_index)
		.def_property("type", &Particle::type, &Particle::set_type)
		.def_property("position", &Particle::position, &Particle::set_position)
		.def_property("velocity", &Particle::velocity, &Particle::set_velocity);
}

#endif

} /* namespace ba */
