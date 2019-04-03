/*
 * Particles.h
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#ifndef PARTICLES_PARTICLES_H_
#define PARTICLES_PARTICLES_H_

#include "defs.h"
#include "python_defs.h"

namespace ba {

class Particles {
public:
	Particles();
	virtual ~Particles();

	uint N() const;

	const std::vector<particle_type> &types() const {
		return _types;
	}

	std::vector<particle_type> &types() {
		return _types;
	}

	const vector_vec3 &positions() const {
		return _positions;
	}

	vector_vec3 &positions() {
		return _positions;
	}

	const vector_vec3 &velocities() const {
		return _velocities;
	}

	vector_vec3 &velocities() {
		return _velocities;
	}

private:
	std::vector<particle_type> _types;
	vector_vec3 _positions;
	vector_vec3 _velocities;
};

#ifdef PYTHON_BINDINGS

void export_Particles(py::module &m);

#endif

} /* namespace ba */

#endif /* PARTICLES_PARTICLES_H_ */
