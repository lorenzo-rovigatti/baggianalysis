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
				_index(Particle::_current_index),
				_type(t),
				_position(pos),
				_velocity(vel) {
	Particle::_current_index++;
}

Particle::~Particle() {

}

void Particle::add_bonded_neighbour(std::shared_ptr<Particle> new_neighbour) {
	_bonded_neighbours.insert(new_neighbour);
	new_neighbour->_bonded_neighbours.insert(new_neighbour);
}

void Particle::add_neighbour(std::shared_ptr<Particle> new_neighbour) {
	_neighbours.insert(new_neighbour);
}

#ifdef PYTHON_BINDINGS

void export_Particle(py::module &m) {
	py::class_<Particle, std::shared_ptr<Particle>> particle(m, "Particle");

	particle
		.def(py::init<>())
		.def(py::init<particle_type, vec3, vec3>())
		.def("bonded_neighbours", &Particle::bonded_neighbours)
		.def("add_bonded_neighbour", &Particle::add_bonded_neighbour)
		.def("neighbours", &Particle::neighbours)
		.def("add_neighbour", &Particle::add_neighbour)
		.def_property_readonly("index", &Particle::index)
		.def_property("type", &Particle::type, &Particle::set_type)
		.def_property("position", &Particle::position, &Particle::set_position)
		.def_property("velocity", &Particle::velocity, &Particle::set_velocity);
}

#endif

} /* namespace ba */
