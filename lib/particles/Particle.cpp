/*
 * Particle.cpp
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#include "Particle.h"

#include "ParticleSet.h"

namespace ba {

Particle::Particle(int index) :
				_index(index) {

}

Particle::Particle(int index, particle_type t) :
				_index(index),
				_type(t) {

}

Particle::Particle(int index, particle_type t, vec3 pos) :
				_index(index),
				_type(t),
				_position(pos) {
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

	particle.def(py::init<int>());
	particle.def(py::init<int, particle_type>());
	particle.def(py::init<int, particle_type, vec3, vec3>());
	particle.def("add_bonded_neighbour", &Particle::add_bonded_neighbour);
	particle.def("add_neighbour", &Particle::add_neighbour);
	particle.def_property_readonly("molecule", &Particle::molecule);
	particle.def_property_readonly("bonded_neighbours", &Particle::bonded_neighbours);
	particle.def_property_readonly("neighbours", &Particle::neighbours);
	particle.def_property_readonly("index", &Particle::index);
	particle.def_property("type", &Particle::type, &Particle::set_type);
	particle.def_property("position", &Particle::position, &Particle::set_position);
	particle.def_property("velocity", &Particle::velocity, &Particle::set_velocity);
}

#endif

} /* namespace ba */
