/*
 * ParticleSet.cpp
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#include "ParticleSet.h"

#include "Particle.h"

#include <algorithm>
#include <numeric>

namespace ba {

using namespace std;

ParticleSet::ParticleSet() :
		_name("") {

}

ParticleSet::ParticleSet(ParticleSet *source, int indexes_shift) {
	// we first copy the individual particle details (but no bonds, dihedrals, etc.)
	for(auto p : source->_particles) {
		auto new_p = p->make_copy(p->index() + indexes_shift);
		this->add_particle(new_p);
	}

	// now we can update the topology
	for(uint i = 0; i < source->N(); i++) {
		auto original_p = source->particles()[i];
		auto new_p = this->particles()[i];

		// bonds
		for(auto b: original_p->bonded_neighbours()) {
			int other_new_index = b.other()->index() + indexes_shift;
			auto other = this->particle_by_id(other_new_index);
			new_p->add_bonded_neighbour(b.type, other);
		}

		// angles
		for(auto a: original_p->bonded_angles()) {
			auto a0 = this->particle_by_id(a[0]->index() + indexes_shift);
			auto a1 = this->particle_by_id(a[1]->index() + indexes_shift);
			auto a2 = this->particle_by_id(a[2]->index() + indexes_shift);
			new_p->add_bonded_angle(a.type, a0, a1, a2);
		}

		// dihedrals
		for(auto d: original_p->bonded_angles()) {
			auto d0 = this->particle_by_id(d[0]->index() + indexes_shift);
			auto d1 = this->particle_by_id(d[1]->index() + indexes_shift);
			auto d2 = this->particle_by_id(d[2]->index() + indexes_shift);
			auto d3 = this->particle_by_id(d[3]->index() + indexes_shift);
			new_p->add_bonded_dihedral(d.type, d0, d1, d2, d3);
		}
	}
}

ParticleSet::~ParticleSet() {

}

std::shared_ptr<ParticleSet> ParticleSet::make_copy(int indexes_shift) {
	std::shared_ptr<ParticleSet> new_set = std::make_shared<ParticleSet>(this, indexes_shift);

	return new_set;
}

std::string ParticleSet::name() const {
	return _name;
}

void ParticleSet::set_name(const std::string &name) {
	_name = name;
}

uint ParticleSet::N() const {
	return _particles.size();
}

std::vector<int> ParticleSet::indexes() const {
	vector<int> ids;
	std::transform(_particles.begin(), _particles.end(), back_inserter(ids), [](const auto &p) {return p->index();});
	return ids;
}

vector<particle_type> ParticleSet::types() const {
	vector<particle_type> ts;
	std::transform(_particles.begin(), _particles.end(), back_inserter(ts), [](const auto &p) {return p->type();});
	return ts;
}

vector<double> ParticleSet::masses() const {
	vector<double> ms;
	std::transform(_particles.begin(), _particles.end(), back_inserter(ms), [](const auto &p) {return p->mass();});
	return ms;
}

vector<double> ParticleSet::charges() const {
	vector<double> cs;
	std::transform(_particles.begin(), _particles.end(), back_inserter(cs), [](const auto &p) {return p->charge();});
	return cs;
}

vector_vec3 ParticleSet::positions() const {
	vector_vec3 poss;
	std::transform(_particles.begin(), _particles.end(), back_inserter(poss), [](const auto &p) {return p->position();});
	return poss;
}

vector_vec3 ParticleSet::velocities() const {
	vector_vec3 vels;
	std::transform(_particles.begin(), _particles.end(), back_inserter(vels), [](const auto &p) {return p->velocity();});
	return vels;
}

double ParticleSet::mass() const {
	return std::accumulate(_particles.begin(), _particles.end(), 0., [](double tot, std::shared_ptr<Particle> p) { return tot + p->mass(); });
}

double ParticleSet::charge() const {
	return std::accumulate(_particles.begin(), _particles.end(), 0., [](double tot, std::shared_ptr<Particle> p) { return tot + p->charge(); });
}

vec3 ParticleSet::com() const {
	vec3 com(0., 0., 0.);
	double tot_mass = 0.;
	for(auto p : _particles) {
		com += p->mass() * p->position();
		tot_mass += p->mass();
	}
	com /= tot_mass;
	return com;
}

void ParticleSet::set_com(vec3 new_com) {
	vec3 curr_com = com();
	vec3 shift = new_com - curr_com;

	for(auto p : _particles) {
		p->shift(shift);
	}
}

vec3 ParticleSet::velocity() const {
	vec3 v(0., 0., 0.);
	for(auto p : _particles) {
		v += p->velocity();
	}
	return v;
}

std::vector<std::shared_ptr<Particle>> &ParticleSet::particles() {
	return _particles;
}

const std::vector<std::shared_ptr<Particle>> &ParticleSet::particles() const {
	return _particles;
}

void ParticleSet::add_particle(std::shared_ptr<Particle> p) {
	if(p->index() == Particle::invalid_index) {
		throw std::runtime_error("Particles with invalid indexes cannot be added to a ParticleSet");
	}

	if(_particles_by_id.count(p->index()) > 0) {
		std::string error = fmt::format("A particle with index '{}' has already been added to this ParticleSet", p->index());
		throw std::runtime_error(error);
	}

	_particles.push_back(p);
	_particles_by_id.emplace(p->index(), p);
}

void ParticleSet::remove_particle(std::shared_ptr<Particle> p) {
	// TODO: we need to remove angles and dihedrals too
	for(auto link : p->bonded_neighbours()) {
		auto neigh = link.other();
		neigh->remove_bonded_neighbour(p);
	}

	_particles_by_id.erase(p->index());
	auto p_it = std::find(_particles.begin(), _particles.end(), p);
	_particles.erase(p_it, p_it + 1);
}

void ParticleSet::remove_particle_by_id(int p_id) {
	remove_particle(particle_by_id(p_id));
}

bool ParticleSet::id_exists(int index) const {
	return _particles_by_id.find(index) != _particles_by_id.end();
}

std::shared_ptr<Particle> ParticleSet::particle_by_id(int index) const {
	try {
		return _particles_by_id.at(index);
	}
	catch (std::out_of_range &) {
		std::string error = fmt::format("A particle with index '{}' does not exist", index);
		throw std::runtime_error(error);
	}
}

void ParticleSet::sort_particles_by_id() {
	auto comp_operator = [](const std::shared_ptr<Particle> &a, const std::shared_ptr<Particle> &b) -> bool {
		return a->index() < b->index();
	};

	std::sort(_particles.begin(), _particles.end(), comp_operator);
}

std::vector<std::shared_ptr<Particle>> ParticleSet::select(std::function<bool(std::shared_ptr<Particle>)> pred) const {
	std::vector<std::shared_ptr<Particle>> res;

	std::copy_if(_particles.begin(), _particles.end(), std::back_inserter(res), pred);

	return res;
}

#ifdef PYTHON_BINDINGS

void export_ParticleSet(py::module &m) {
	py::class_<ParticleSet, std::shared_ptr<ParticleSet>> particle_set(m, "ParticleSet", "A set of particles.");

	particle_set.def(py::init<>(), "The constructor takes no parameters.");

	particle_set.def("make_copy", &ParticleSet::make_copy, py::arg("indexes_shift"), R"pbdoc(
Make a copy of the set and of the particles therein, shifting all the particle indexes by the given value.

Parameters
----------
indexes_shift: int
    The shift to be added to the index of each particle.
)pbdoc");

	particle_set.def("N", &ParticleSet::N, "The number of particles stored in the set.");

	particle_set.def("indexes", &ParticleSet::indexes, R"pbdoc(
Return a vector containing the index of all the particles.

The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios (or cache the result).
)pbdoc");
	particle_set.def("types", &ParticleSet::types, R"pbdoc(
Return a vector containing the type of all the particles.

The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios (or cache the result).
)pbdoc");
	particle_set.def("masses", &ParticleSet::masses, R"pbdoc(
Return a vector containing the mass of all the particles.

The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios (or cache the result).
)pbdoc");
	particle_set.def("charges", &ParticleSet::charges, R"pbdoc(
Return a vector containing the charge of all the particles.

The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios (or cache the result).
)pbdoc");
	particle_set.def("positions", &ParticleSet::positions, R"pbdoc(
Return a vector containing the position of all the particles.

The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios (or cache the result).
)pbdoc");
	particle_set.def("velocities", &ParticleSet::velocities, R"pbdoc(
Return a vector containing the velocity of all the particles.

The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios (or cache the result).
)pbdoc");

	particle_set.def("mass", &ParticleSet::mass, "The total mass stored in the set");

	particle_set.def("charge", &ParticleSet::charge, "The total charge stored in the set");

	particle_set.def("com", &ParticleSet::com, "The centre of mass of the set");

	particle_set.def("set_com", &ParticleSet::set_com, py::arg("new_com"), R"pbdoc(
Set the centre of mass of the set to a new value.

Parameters
----------
new_com: numpy.ndarray
	The new centre of mass of the set.
)pbdoc");

	particle_set.def("velocity", &ParticleSet::velocity, "The total velocity of the set");

	// here we tell pybind11 which of the two particles() methods we want to have bindings for
	particle_set.def("particles", (std::vector<std::shared_ptr<Particle>> &(ParticleSet::*)())(&ParticleSet::particles),
			"The list of particles stored in the set");

	particle_set.def("add_particle", &ParticleSet::add_particle, py::arg("p"), R"pbdoc(
Add ``p`` to the set.

Parameters
----------
p: :class:`Particle`
    The particle to be added to the set.
)pbdoc");

	particle_set.def("remove_particle", &ParticleSet::remove_particle, py::arg("p"), R"pbdoc(
Remove ``p`` from the set.

Parameters
----------
p: :class:`Particle`
	The particle to be removed from the set.
)pbdoc");

	particle_set.def("remove_particle_by_id", &ParticleSet::remove_particle_by_id, py::arg("p_id"), R"pbdoc(
Remove the particle with given index from the set.

Parameters
----------
p_idx: int
	The index of the particle to be removed from the set.
)pbdoc");

	particle_set.def("id_exists", &ParticleSet::id_exists, py::arg("p_idx"), R"pbdoc(
Check whether a particle with the given index is stored in the set.

Parameters
----------
p_idx: int
	The index to be checked.

Returns
-------
bool
    True if the particle is stored in the set, False otherwise.
)pbdoc");

	particle_set.def("particle_by_id", &ParticleSet::particle_by_id, py::arg("p_idx"), R"pbdoc(
Retrieve the particle with the given index.

Parameters
----------
p_idx: int
	The index of the particle to be retrieved.

Returns
-------
:class:`Particle`
	The particle with the given index.
)pbdoc");

	particle_set.def("select", &ParticleSet::select, py::arg("predicate"), R"pbdoc(
Return all those particles for which the given predicate evaluates to True.

Here is an example that creates a list of particles having a non-negative position along the x axis::

	selected = system.select(lambda p: p.position[0] > 0)  

Note that the returned list contains references to and not copies of the particles stored in this set.

Parameters
----------
predicate: callable
	A callable that takes a particle and returns True if the particle should be returned, False otherwise.

Returns
-------
List(:class:`Particle`)
	The list of particles for which the predicate evaluates to True.
)pbdoc");

	particle_set.def("sort_particles_by_id", &ParticleSet::sort_particles_by_id, "Sort the particles according to their index");
	particle_set.def_property("name", &ParticleSet::name, &ParticleSet::set_name, R"pbdoc(
The name of the object. This can be used to keep track of particular particle sets (*e.g.* when dealing with :meth:`System.molecules()`).
)pbdoc");
}

#endif

} /* namespace ba */
