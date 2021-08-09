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
		throw std::runtime_error("Particles with invalid indexes cannot be added to a System");
	}

	if(_particles_by_id.count(p->index()) > 0) {
		std::string error = fmt::format("A particle with index '{}' has already been added to this System", p->index());
		throw std::runtime_error(error);
	}

	_particles.push_back(p);
	_particles_by_id.emplace(p->index(), p);
}

void ParticleSet::remove_particle(std::shared_ptr<Particle> p) {
	// TODO: we need to remove angles and dihedrals too
	for(auto link : p->bonded_neighbours()) {
		auto neigh = link.other();
		neigh->remove_bonded_neighbour(link);
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

#ifdef PYTHON_BINDINGS

void export_ParticleSet(py::module &m) {
	py::class_<ParticleSet, std::shared_ptr<ParticleSet>> particle_set(m, "ParticleSet", "A set of particles.");

	particle_set.def(py::init<>());
	particle_set.def("N", &ParticleSet::N);
	particle_set.def("indexes", &ParticleSet::indexes);
	particle_set.def("types", &ParticleSet::types);
	particle_set.def("masses", &ParticleSet::masses);
	particle_set.def("charges", &ParticleSet::charges);
	particle_set.def("positions", &ParticleSet::positions);
	particle_set.def("velocities", &ParticleSet::velocities);
	particle_set.def("mass", &ParticleSet::mass);
	particle_set.def("charge", &ParticleSet::charge);
	particle_set.def("com", &ParticleSet::com);
	particle_set.def("average_velocity", &ParticleSet::velocity);
	// here we tell pybind11 which of the two particles() methods we want to have bindings for
	particle_set.def("particles", (std::vector<std::shared_ptr<Particle>> &(ParticleSet::*)())(&ParticleSet::particles));
	particle_set.def("add_particle", &ParticleSet::add_particle);
	particle_set.def("remove_particle", &ParticleSet::remove_particle);
	particle_set.def("remove_particle_by_id", &ParticleSet::remove_particle_by_id);
	particle_set.def("id_exists", &ParticleSet::id_exists);
	particle_set.def("particle_by_id", &ParticleSet::particle_by_id);
	particle_set.def("sort_particles_by_id", &ParticleSet::sort_particles_by_id);
	particle_set.def_property("name", &ParticleSet::name, &ParticleSet::set_name);
}

#endif

} /* namespace ba */
