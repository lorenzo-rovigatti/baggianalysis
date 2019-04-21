/*
 * ParticleSet.h
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#ifndef PARTICLES_PARTICLESET_H_
#define PARTICLES_PARTICLESET_H_

#include "Particle.h"

namespace ba {

class ParticleSet {
public:
	ParticleSet();
	virtual ~ParticleSet();

	std::string name() const;
	void set_name(const std::string &name);

	uint N() const;
	std::vector<particle_type> types() const;
	vector_vec3 positions() const;
	vector_vec3 velocities() const;

	vec3 com() const;
	vec3 average_velocity() const;

	const std::vector<std::shared_ptr<Particle>> &particles() const;
	void add_particle(std::shared_ptr<Particle> p);

protected:
	std::string _name;

	std::vector<std::shared_ptr<Particle>> _particles;
	std::vector<ParticleSet> _subsets;
};

#ifdef PYTHON_BINDINGS

void export_ParticleSet(py::module &m);

#endif

} /* namespace ba */

#endif /* PARTICLES_PARTICLESET_H_ */
