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

class ParticleSet;

class Particle: public std::enable_shared_from_this<Particle> {
public:
	Particle() = delete;
	Particle(int index);
	Particle(int index, particle_type nt);
	Particle(int index, particle_type nt, vec3 pos);
	Particle(int index, particle_type nt, vec3 pos, vec3 vel);
	virtual ~Particle();

	int index() const {
		return _index;
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

	void shift(const vec3 &&delta) {
		_position += delta;
	}

	const vec3 &velocity() const {
		return _velocity;
	}

	void set_velocity(const vec3 &nv) {
		_velocity = nv;
	}

	const std::vector<vec3> &orientation_vectors() const {
		return _orientation_vectors;
	}

	void set_orientation_vectors(const std::vector<vec3> &no) {
		_orientation_vectors = no;
	}

	void add_orientation_vector(const vec3 v) {
		_orientation_vectors.emplace_back(v);
	}

	void clear_orientation_vectors() {
		_orientation_vectors.clear();
	}

	const std::set<std::shared_ptr<Particle>> &bonded_neighbours() const {
		return _bonded_neighbours;
	}

	void add_bonded_neighbour(std::shared_ptr<Particle> new_neighbour);
	void remove_bonded_neighbour(std::shared_ptr<Particle> to_remove);

	const std::set<std::shared_ptr<Particle>> &neighbours() const {
		return _neighbours;
	}

	void add_neighbour(std::shared_ptr<Particle> new_neighbour);
	void remove_neighbour(std::shared_ptr<Particle> to_remove);

	std::shared_ptr<ParticleSet> molecule() const {
		return _molecule;
	}
	void set_molecule(std::shared_ptr<ParticleSet> new_molecule);

protected:
	static int _current_index;

	int _index;
	particle_type _type = DEFAULT_PARTICLE_TYPE;
	vec3 _position = vec3(0., 0., 0.);
	vec3 _velocity = vec3(0., 0., 0.);
	std::vector<vec3> _orientation_vectors;

	std::set<std::shared_ptr<Particle>> _bonded_neighbours;
	std::set<std::shared_ptr<Particle>> _neighbours;

	std::shared_ptr<ParticleSet> _molecule;
};

#ifdef PYTHON_BINDINGS

void export_Particle(py::module &m);

#endif

} /* namespace ba */

#endif /* PARTICLES_PARTICLE_H_ */
