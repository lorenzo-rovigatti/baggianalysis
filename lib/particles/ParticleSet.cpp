/*
 * ParticleSet.cpp
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#include "ParticleSet.h"

#include "Particle.h"

#include <algorithm>

namespace ba {

using namespace std;

ParticleSet::ParticleSet() :
		_name("") {

}

ParticleSet::~ParticleSet() {

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

vec3 ParticleSet::com() const {
	vec3 com(0., 0., 0.);
	for(auto p : _particles) {
		com += p->position();
	}
	com /= this->N();
	return com;
}

vec3 ParticleSet::average_velocity() const {
	vec3 v_avg(0., 0., 0.);
	for(auto p : _particles) {
		v_avg += p->velocity();
	}
	v_avg /= this->N();
	return v_avg;
}

std::vector<std::shared_ptr<Particle>> &ParticleSet::particles() {
	return _particles;
}

const std::vector<std::shared_ptr<Particle>> &ParticleSet::particles() const {
	return _particles;
}

void ParticleSet::add_particle(std::shared_ptr<Particle> p) {
	if(_particles_by_id.count(p->index()) > 0) {
		std::string error = boost::str(boost::format("A particle with index '%d' has already been added to this System") % p->index());
		throw std::runtime_error(error);
	}

	_particles.push_back(p);
	_particles_by_id.emplace(p->index(), p);
}

void ParticleSet::remove_particle(std::shared_ptr<Particle> p) {
	for(auto neigh : p->bonded_neighbours()) {
		neigh->remove_bonded_neighbour(p);
	}

	_particles_by_id.erase(p->index());
	auto p_it = std::find(_particles.begin(), _particles.end(), p);
	_particles.erase(p_it, p_it + 1);
}

void ParticleSet::remove_particle_by_id(int p_id) {
	remove_particle(particle_by_id(p_id));
}

std::shared_ptr<Particle> ParticleSet::particle_by_id(int index) const {
	if(_particles_by_id.count(index) == 0) {
		std::string error = boost::str(boost::format("A particle with index '%d' does not exist") % index);
		throw std::runtime_error(error);
	}

	return _particles_by_id.at(index);
}

void ParticleSet::sort_particles_by_id() {
	auto comp_operator = [](const std::shared_ptr<Particle> &a, const std::shared_ptr<Particle> &b) -> bool {
		return a->index() < b->index();
	};

	std::sort(_particles.begin(), _particles.end(), comp_operator);
}

#ifdef PYTHON_BINDINGS

void export_ParticleSet(py::module &m) {
	py::class_<ParticleSet, std::shared_ptr<ParticleSet>> particle_set(m, "ParticleSet");

	particle_set
		.def(py::init<>())
		.def("N", &ParticleSet::N)
		.def("types", &ParticleSet::types)
		.def("positions", &ParticleSet::positions)
		.def("velocities", &ParticleSet::velocities)
		.def("com", &ParticleSet::com)
		.def("average_velocity", &ParticleSet::average_velocity)
		// here we tell pybind11 which of the two particles() methods we want to have bindings for
		.def("particles", (std::vector<std::shared_ptr<Particle>> &(ParticleSet::*)())(&ParticleSet::particles))
		.def("add_particle", &ParticleSet::add_particle)
		.def("remove_particle", &ParticleSet::remove_particle)
		.def("remove_particle_by_id", &ParticleSet::remove_particle_by_id)
		.def("particle_by_id", &ParticleSet::particle_by_id)
		.def("sort_particles_by_id", &ParticleSet::sort_particles_by_id)
		.def_property("name", &ParticleSet::name, &ParticleSet::set_name);
}

#endif

} /* namespace ba */
