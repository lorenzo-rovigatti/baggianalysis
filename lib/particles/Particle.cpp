/*
 * Particle.cpp
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#include "Particle.h"

#include "ParticleSet.h"

namespace ba {

int Particle::_current_index = 0;

Particle::Particle() :
				Particle(DEFAULT_PARTICLE_TYPE, vec3(0., 0., 0.), vec3(0., 0., 0.)) {

}

Particle::Particle(int index) :
				Particle(index, DEFAULT_PARTICLE_TYPE, vec3(0., 0., 0.), vec3(0., 0., 0.)) {

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

void Particle::add_bonded_neighbour(std::shared_ptr<Particle> new_neighbour) {
	_bonded_neighbours.insert(new_neighbour);
	new_neighbour->_bonded_neighbours.insert(shared_from_this());
}

void Particle::remove_bonded_neighbour(std::shared_ptr<Particle> to_remove) {
	_bonded_neighbours.erase(to_remove);
}

void Particle::add_neighbour(std::shared_ptr<Particle> new_neighbour) {
	_neighbours.insert(new_neighbour);
}

void Particle::remove_neighbour(std::shared_ptr<Particle> to_remove) {
	_neighbours.erase(to_remove);
}

void Particle::set_molecule(std::shared_ptr<ParticleSet> new_molecule) {
	_molecule = new_molecule;
}

#ifdef PYTHON_BINDINGS

void export_Particle(py::module &m) {
	py::class_<Particle, std::shared_ptr<Particle>> particle(m, "Particle");

	particle
		.def(py::init<>())
		.def(py::init<int>())
		.def(py::init<particle_type, vec3, vec3>())
		.def(py::init<int, particle_type, vec3, vec3>())
		.def("add_bonded_neighbour", &Particle::add_bonded_neighbour)
		.def("add_neighbour", &Particle::add_neighbour)
		.def_property_readonly("molecule", &Particle::molecule)
		.def_property_readonly("bonded_neighbours", &Particle::bonded_neighbours)
		.def_property_readonly("neighbours", &Particle::neighbours)
		.def_property_readonly("index", &Particle::index)
		.def_property("type", &Particle::type, &Particle::set_type)
		.def_property("position", &Particle::position, &Particle::set_position)
		.def_property("velocity", &Particle::velocity, &Particle::set_velocity);
}

#endif

} /* namespace ba */
