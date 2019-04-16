/*
 * Particle.h
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#ifndef PARTICLES_PARTICLE_H_
#define PARTICLES_PARTICLE_H_

#include "../defs.h"
#include "../python_defs.h"

namespace ba {

class Particle {
public:
	Particle();
	Particle(particle_type nt, vec3 pos, vec3 vel);
	virtual ~Particle();

	const particle_type &type() const {
		return _type;
	}

	particle_type &type() {
		return _type;
	}

	const vec3 &position() const {
		return _position;
	}

	vec3 &position() {
		return _position;
	}

	const vec3 &velocity() const {
		return _velocity;
	}

	vec3 &velocity() {
		return _velocity;
	}

protected:
	particle_type _type;
	vec3 _position;
	vec3 _velocity;
};

#ifdef PYTHON_BINDINGS

void export_Particle(py::module &m);

#endif

} /* namespace ba */

#endif /* PARTICLES_PARTICLE_H_ */
