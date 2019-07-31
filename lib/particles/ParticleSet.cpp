/*
 * ParticleSet.cpp
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#include "ParticleSet.h"

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
	for(auto p : particles()) {
		com += p->position();
	}
	com /= this->N();
	return com;
}

vec3 ParticleSet::average_velocity() const {
	vec3 v_avg(0., 0., 0.);
	for(auto p : particles()) {
		v_avg += p->velocity();
	}
	v_avg /= this->N();
	return v_avg;
}

std::vector<std::shared_ptr<Particle>> &ParticleSet::writable_particles() {
	return _particles;
}

const std::vector<std::shared_ptr<Particle>> &ParticleSet::particles() const {
	return _particles;
}

void ParticleSet::add_particle(std::shared_ptr<Particle> p) {
	_particles.push_back(p);
}

#ifdef PYTHON_BINDINGS

void export_ParticleSet(py::module &m) {
	py::class_<ParticleSet, std::shared_ptr<ParticleSet>> parser(m, "ParticleSet");

	parser
		.def(py::init<>())
		.def("N", &ParticleSet::N)
		.def("types", &ParticleSet::types)
		.def("positions", &ParticleSet::positions)
		.def("velocities", &ParticleSet::velocities)
		.def("com", &ParticleSet::com)
		.def("average_velocity", &ParticleSet::average_velocity)
		.def("particles", &ParticleSet::particles)
		.def("writable_particles", &ParticleSet::writable_particles)
		.def("add_particle", &ParticleSet::add_particle)
		.def_property("name", &ParticleSet::name, &ParticleSet::set_name);
}

#endif

} /* namespace ba */
