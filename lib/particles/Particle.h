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

#include <set>

namespace ba {

class Particle {
public:
	Particle();
	Particle(particle_type nt, vec3 pos, vec3 vel);
	Particle(int index, particle_type nt, vec3 pos, vec3 vel);
	virtual ~Particle();

	int index() const {
		return _index;
	}

	void set_index(int ni) {
		_index = ni;
	}

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

	const std::set<std::shared_ptr<Particle>> bonded_neighbours() const {
		return _bonded_neighbours;
	}

	void add_bonded_neighbour(std::shared_ptr<Particle> new_neighbour);

	const std::set<std::shared_ptr<Particle>> neighbours() const {
		return _neighbours;
	}

	void add_neighbour(std::shared_ptr<Particle> new_neighbour);

protected:
	static int _current_index;

	int _index;
	particle_type _type;
	vec3 _position;
	vec3 _velocity;

	std::set<std::shared_ptr<Particle>> _bonded_neighbours;
	std::set<std::shared_ptr<Particle>> _neighbours;
};

#ifdef PYTHON_BINDINGS

void export_Particle(py::module &m);

#endif

} /* namespace ba */

#endif /* PARTICLES_PARTICLE_H_ */
