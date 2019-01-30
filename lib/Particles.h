/*
 * Particles.h
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#ifndef PARTICLES_PARTICLES_H_
#define PARTICLES_PARTICLES_H_

#include "defs.h"

namespace ba {

class Particles {
public:
	Particles();
	virtual ~Particles();

	uint N() const;

	std::vector<particle_type> types;
	vector_vec3 positions;
	vector_vec3 velocities;
};

} /* namespace ba */

#endif /* PARTICLES_PARTICLES_H_ */
