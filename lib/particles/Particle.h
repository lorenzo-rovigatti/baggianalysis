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

	void set_type(const particle_type &nt) {
		_type = nt;
	}

	const vec3 &position() const {
		return _position;
	}

	void set_position(const vec3 &np) {
		_position = np;
	}

	const vec3 &velocity() const {
		return _velocity;
	}

	void set_velocity(const vec3 &nv) {
		_velocity = nv;
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
