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

std::shared_ptr<Particle> Particle::make_copy(int index) {
	auto new_particle = std::make_shared<Particle>(index);

	new_particle->_type = _type;
	new_particle->_mass = _mass;
	new_particle->_charge = _charge;
	new_particle->_position = _position;
	new_particle->_velocity = _velocity;
	new_particle->_orientation_vectors = _orientation_vectors;

	return new_particle;
}

Particle::~Particle() {

}

void Particle::add_bonded_neighbour(std::string type, std::shared_ptr<Particle> new_neighbour) {
	_bonded_neighbours.emplace(type, new_neighbour);
	new_neighbour->_bonded_neighbours.emplace(type, shared_from_this());
}

void Particle::add_bonded_neighbour(std::shared_ptr<Particle> new_neighbour) {
	add_bonded_neighbour(DEFAULT_LINK_TYPE, new_neighbour);
}

void Particle::remove_bonded_neighbour(ParticleBond to_remove) {
	_bonded_neighbours.erase(to_remove);
}

void Particle::add_bonded_angle(std::string type, std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3) {
	_bonded_angles.emplace(type, p1, p2, p3);
}

void Particle::add_bonded_angle(std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3) {
	_bonded_angles.emplace(DEFAULT_LINK_TYPE, p1, p2, p3);
}

void Particle::add_bonded_angle(ParticleAngle new_angle) {
	_bonded_angles.insert(new_angle);
}

void Particle::remove_bonded_angle(ParticleAngle to_remove) {
	_bonded_angles.erase(to_remove);
}

void Particle::add_bonded_dihedral(std::string type, std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3, std::shared_ptr<Particle> p4) {
	_bonded_dihedrals.emplace(type, p1, p2, p3, p4);
}

void Particle::add_bonded_dihedral(std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3, std::shared_ptr<Particle> p4) {
	_bonded_dihedrals.emplace(DEFAULT_LINK_TYPE, p1, p2, p3, p4);
}

void Particle::add_bonded_dihedral(ParticleDihedral new_dihedral) {
	_bonded_dihedrals.insert(new_dihedral);
}

void Particle::remove_bonded_dihedral(ParticleDihedral to_remove) {
	_bonded_dihedrals.erase(to_remove);
}

void Particle::add_neighbour(std::string type, std::shared_ptr<Particle> new_neighbour) {
	_neighbours.emplace(type, new_neighbour);
}

void Particle::add_neighbour(std::shared_ptr<Particle> new_neighbour) {
	add_neighbour(DEFAULT_PARTICLE_TYPE, new_neighbour);
}

void Particle::remove_neighbour(ParticleBond to_remove) {
	_neighbours.erase(to_remove);
}

bool Particle::has_neighbour(std::shared_ptr<Particle> q) {
	return std::find_if(_neighbours.begin(), _neighbours.end(), [q](const ParticleBond &pb) { return pb.other() == q; }) != _neighbours.end();
}

void Particle::set_molecule(std::shared_ptr<ParticleSet> new_molecule) {
	_molecule = std::weak_ptr<ParticleSet>(new_molecule);
}

#ifdef PYTHON_BINDINGS

void export_Particle(py::module &m) {
	py::class_<ParticleBond, std::shared_ptr<ParticleBond>> particle_bond(m, "ParticleBond");
	particle_bond.def("other", &ParticleBond::other);

	py::class_<ParticleAngle, std::shared_ptr<ParticleAngle>> particle_angle(m, "ParticleAngle");

	py::class_<ParticleDihedral, std::shared_ptr<ParticleDihedral>> particle_dihedral(m, "ParticleDihedral");

	py::class_<Particle, std::shared_ptr<Particle>> particle(m, "Particle", R"pbdoc(
        A simulation particle.
	)pbdoc");

	particle.def(py::init<int>(), py::arg("index"), R"pbdoc(
Build a new particle by specifying its index.

Parameters
----------
index : int
    The index of the particle. Note that this value cannot be changed later on.
	)pbdoc");

	particle.def(py::init<int, particle_type>(), py::arg("index"), py::arg("type"), R"pbdoc(
Build a new particle by specifying its index and type.

Parameters
----------
index : int
    The index of the particle. Note that this value cannot be changed later on.
type : str
    The type of the particle.
type
)pbdoc");

	particle.def(py::init<int, particle_type, vec3>(), py::arg("index"), py::arg("type"), py::arg("position"), R"pbdoc(
Build a new particle by specifying its index, type and position.

Parameters
----------
index : int
    The index of the particle. Note that this value cannot be changed later on.
type : str
    The type of the particle.
position : numpy.ndarray
    The position of the particle
)pbdoc");

	particle.def(py::init<int, particle_type, vec3, vec3>(), py::arg("index"), py::arg("type"), py::arg("position"), py::arg("velocity"), R"pbdoc(
Build a new particle by specifying its index, type, position and velocity.

Parameters
----------
index : int
    The index of the particle. Note that this value cannot be changed later on.
type : str
    The type of the particle.
position : numpy.ndarray
    The position of the particle.
velocity : numpy.ndarray
    The velocity of the particle.
)pbdoc");

	particle.def("make_copy", &Particle::make_copy, py::arg("index"), R"pbdoc(
Return a copy of this particle with the given index. Note that neighbours and bonded neighbours, angles and dihedrals are not copied.

Parameters
----------
index : int
    The index of the new particle. 
	)pbdoc");

	particle.def("add_orientation_vector", &Particle::add_orientation_vector, py::arg("v"), R"pbdoc(
The molecule to which this particle belongs.

Parameters
----------
v : numpy.ndarray
    The new orientation vector.
	)pbdoc");

	particle.def("add_bonded_neighbour", static_cast<void (Particle::*)(std::string, std::shared_ptr<Particle>)>(&Particle::add_bonded_neighbour), py::arg("type"), py::arg("q"), R"pbdoc(
Add a bond of the given type between the current particle and ``q``.
Parameters
----------
type : str
    The type of bond shared by the two particles.
q : :class:`Particle`
	The new bonded neighbour.
	)pbdoc");

	particle.def("add_bonded_neighbour", static_cast<void (Particle::*)(std::shared_ptr<Particle>)>(&Particle::add_bonded_neighbour), py::arg("q"), R"pbdoc(
Add a bond of the default type between the current particle and ``q``.

Parameters
----------
q : :class:`Particle`
	The new bonded neighbour.
	)pbdoc");

	particle.def("add_bonded_angle", static_cast<void (Particle::*)(std::string, std::shared_ptr<Particle>, std::shared_ptr<Particle>, std::shared_ptr<Particle>)>(&Particle::add_bonded_angle),
			py::arg("type"), py::arg("p1"), py::arg("p2"), py::arg("p3"), R"pbdoc(
Add an angle of the given type (defined by three particles) this particle is involved in. Note that the current particle must be one of the three particles given as parameters.

Parameters
----------
type : str
    The type of the angle.
p1 : :class:`Particle`
	The first particle participating in the angle.
p2 : :class:`Particle`
	The second particle participating in the angle.
p3 : :class:`Particle`
	The third particle participating in the angle.
    )pbdoc");

	particle.def("add_bonded_angle", static_cast<void (Particle::*)(std::shared_ptr<Particle>, std::shared_ptr<Particle>, std::shared_ptr<Particle>)>(&Particle::add_bonded_angle),
			py::arg("p1"), py::arg("p2"), py::arg("p3"), R"pbdoc(
Add an angle of the default type (defined by three particles) this particle is involved in. Note that the current particle must be one of the three particles given as parameters.

Parameters
----------
p1 : :class:`Particle`
	The first particle participating in the angle.
p2 : :class:`Particle`
	The second particle participating in the angle.
p3 : :class:`Particle`
	The third particle participating in the angle.
    )pbdoc");

	particle.def("add_bonded_dihedral", static_cast<void (Particle::*)(std::string, std::shared_ptr<Particle>, std::shared_ptr<Particle>, std::shared_ptr<Particle>, std::shared_ptr<Particle>)>(&Particle::add_bonded_dihedral),
			py::arg("type"), py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), R"pbdoc(
Add a dihedral of the given type (defined by four particles) this particle is involved in. Note that the current particle must be one of the four particles given as parameters.

Parameters
----------
type : str
    The type of the dihedral.
p1 : :class:`Particle`
	The first particle participating in the dihedral.
p2 : :class:`Particle`
	The second particle participating in the dihedral.
p3 : :class:`Particle`
	The third particle participating in the dihedral.
p4 : :class:`Particle`
	The fourth particle participating in the dihedral.
    )pbdoc");

	particle.def("add_bonded_dihedral", static_cast<void (Particle::*)(std::shared_ptr<Particle>, std::shared_ptr<Particle>, std::shared_ptr<Particle>, std::shared_ptr<Particle>)>(&Particle::add_bonded_dihedral),
			py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), R"pbdoc(
Add a dihedral of the default type (defined by four particles) this particle is involved in. Note that the current particle must be one of the four particles given as parameters.

Parameters
----------
p1 : :class:`Particle`
	The first particle participating in the dihedral.
p2 : :class:`Particle`
	The second particle participating in the dihedral.
p3 : :class:`Particle`
	The third particle participating in the dihedral.
p4 : :class:`Particle`
	The fourth particle participating in the dihedral.
    )pbdoc");

	particle.def("add_neighbour", static_cast<void (Particle::*)(std::string, std::shared_ptr<Particle>)>(&Particle::add_neighbour), py::arg("type"), py::arg("q"), R"pbdoc(
Add particle ``q`` as a bond of the given type to the list of this particle's neighbours. Contrary to :meth:`add_bonded_neighbour`, 
the current particle is `not` added to the list of ``q``'s neighbours.

Parameters
----------
type : str
    The type of bond shared by the two particles.
q : :class:`Particle`
	The new neighbour.
	)pbdoc");

	particle.def("add_neighbour", static_cast<void (Particle::*)(std::shared_ptr<Particle>)>(&Particle::add_neighbour), py::arg("q"), R"pbdoc(
Add particle ``q`` as a bond of the default type to the list of this particle's neighbours. Contrary to :meth:`add_bonded_neighbour`, 
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

	particle.def_property_readonly("bonded_angles", &Particle::bonded_angles, R"pbdoc(
        A list of :class:`ParticleSet` s storing the angles (*i.e.* triplets of particles) this particle is involved in.

        :type: Set(:class:`ParticleSet`)
	)pbdoc");

	particle.def_property_readonly("bonded_dihedrals", &Particle::bonded_dihedrals, R"pbdoc(
        A list of :class:`ParticleSet` s storing the dihedrals (*i.e.* list of four particles) this particle is involved in.

        :type: Set(:class:`ParticleSet`)
	)pbdoc");

	particle.def_property_readonly("neighbours", &Particle::neighbours, R"pbdoc(
		A list of this particle's neighbours.

		:type: Set(:class:`Particle`)
	)pbdoc");

	particle.def_property_readonly("index", &Particle::index, R"pbdoc(
		The particle's index. This is a read-only property because changing the index of the particle should be done with the methods provided by 
        :class:`ParticleSet` and :class:`System` only.

		:type: int
	)pbdoc");

	particle.def_property("orientation_vectors", &Particle::orientation_vectors, &Particle::set_orientation_vectors, R"pbdoc(
		The particle's orientation vectors.

		:type: List(numpy.ndarray)
	)pbdoc");

	particle.def_property("type", &Particle::type, &Particle::set_type, R"pbdoc(
		The particle's type.

		:type: string
	)pbdoc");

	particle.def_property("mass", &Particle::mass, &Particle::set_mass, R"pbdoc(
		The particle's mass.

		:type: float
	)pbdoc");

	particle.def_property("charge", &Particle::charge, &Particle::set_charge, R"pbdoc(
		The particle's charge.

		:type: float
	)pbdoc");

	particle.def_property("position", &Particle::position, &Particle::set_position, R"pbdoc(
		The particle's position.

		:type: numpy.ndarray
	)pbdoc");

	particle.def_property("velocity", &Particle::velocity, &Particle::set_velocity, R"pbdoc(
		The particle's velocity.

		:type: numpy.ndarray
	)pbdoc");

	particle.def_property("orientation_vectors", &Particle::orientation_vectors, &Particle::set_orientation_vectors, R"pbdoc(
        A list of vectors related to the particle's orientation. Depending on the specific model it may contain the 
        list of interaction centres or the components of the orientation matrix.

		:type: List(numpy.ndarray)
	)pbdoc");
}

#endif

} /* namespace ba */
