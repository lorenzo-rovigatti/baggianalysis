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
	py::class_<Particle, std::shared_ptr<Particle>> particle(m, "Particle", R"pbdoc(
        A simulation particle.
	)pbdoc");

	particle.def(py::init<int>(), py::arg("index"), R"pbdoc(
This constructor takes the particle index as its only parameter.

Parameters
----------
index : int
    The index of the particle. Note that this value cannot be changed later on.
	)pbdoc");

	particle.def(py::init<int, particle_type>());

	particle.def(py::init<int, particle_type, vec3>());

	particle.def(py::init<int, particle_type, vec3, vec3>());

	particle.def("add_bonded_neighbour", &Particle::add_bonded_neighbour, py::arg("q"), R"pbdoc(
        Add particle ``q`` to the list of the current particle's bonded neighbours and the current particle to 
        the list of ``q``'s bonded neighbours.

        Parameters
        ----------
        q : :class:`Particle`
            The new bonded neighbour.
	)pbdoc");

	particle.def("add_neighbour", &Particle::add_neighbour, py::arg("q"), R"pbdoc(
        Add particle ``q`` to the list of this particle's neighbours. Contrary to :meth:`add_bonded_neighbour`, 
        the current particle is `not` added to the list of ``q``'s neighbours.

        Parameters
        ----------
        q : :class:`Particle`
            The new neighbour.
	)pbdoc");

	particle.def_property_readonly("molecule", &Particle::molecule, R"pbdoc(
		The molecule to which this particle belongs.

		:type: :class:`ParticleSet`
	)pbdoc");

	particle.def_property_readonly("bonded_neighbours", &Particle::bonded_neighbours, R"pbdoc(
		A list of this particle's bonded neighbours.

		:type: Set(:class:`Particle`)
	)pbdoc");

	particle.def_property_readonly("neighbours", &Particle::neighbours, R"pbdoc(
		A list of this particle's neighbours.

		:type: Set(:class:`Particle`)
	)pbdoc");

	particle.def_property_readonly("index", &Particle::index, R"pbdoc(
		The particle index.

		:type: int
	)pbdoc");

	particle.def_property("type", &Particle::type, &Particle::set_type, R"pbdoc(
		The particle type.

		:type: string
	)pbdoc");

	particle.def_property("position", &Particle::position, &Particle::set_position);

	particle.def_property("velocity", &Particle::velocity, &Particle::set_velocity);

	particle.def_property("orientation_vectors", &Particle::orientation_vectors, &Particle::set_orientation_vectors);
}

#endif

} /* namespace ba */
