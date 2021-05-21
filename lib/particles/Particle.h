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
class Particle;

struct ParticleBond;

template<int size> struct ParticleLink;
using ParticleAngle = ParticleLink<3>;
using ParticleDihedral = ParticleLink<4>;

class Particle: public std::enable_shared_from_this<Particle> {
public:
	static const int invalid_index = -1;

	Particle() = delete;
	Particle(int index);
	Particle(int index, particle_type nt);
	Particle(int index, particle_type nt, vec3 pos);
	Particle(int index, particle_type nt, vec3 pos, vec3 vel);
	Particle(const Particle &) = delete;
	Particle &operator=(const Particle &) = delete;
	virtual ~Particle();

	std::shared_ptr<Particle> make_copy(int index);

	int index() const {
		return _index;
	}

	void set_index(int nidx) {
		_index = nidx;
	}

	const particle_type &type() const {
		return _type;
	}

	void set_type(const particle_type &nt) {
		_type = nt;
	}

	const double &mass() const {
		return _mass;
	}

	void set_mass(double nm) {
		_mass = nm;
	}

	const double &charge() const {
		return _charge;
	}

	void set_charge(double nc) {
		_charge = nc;
	}

	const vec3 &position() const {
		return _position;
	}

	void set_position(const vec3 &np) {
		_position = np;
	}

	void shift(const vec3 &delta) {
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

	const std::set<ParticleBond> &bonded_neighbours() const {
		return _bonded_neighbours;
	}
	void add_bonded_neighbour(std::string type, std::shared_ptr<Particle> new_neighbour);
	void add_bonded_neighbour(std::shared_ptr<Particle> new_neighbour);
	void remove_bonded_neighbour(ParticleBond to_remove);

	const std::set<ParticleAngle> &bonded_angles() const {
		return _bonded_angles;
	}
	void add_bonded_angle(std::string type, std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3);
	void add_bonded_angle(std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3);
	void add_bonded_angle(ParticleAngle new_angle);
	void remove_bonded_angle(ParticleAngle to_remove);

	const std::set<ParticleDihedral> &bonded_dihedrals() const {
		return _bonded_dihedrals;
	}
	void add_bonded_dihedral(std::string type, std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3, std::shared_ptr<Particle> p4);
	void add_bonded_dihedral(std::shared_ptr<Particle> p1, std::shared_ptr<Particle> p2, std::shared_ptr<Particle> p3, std::shared_ptr<Particle> p4);
	void add_bonded_dihedral(ParticleDihedral new_dihedral);
	void remove_bonded_dihedral(ParticleDihedral to_remove);

	const std::set<ParticleBond> &neighbours() const {
		return _neighbours;
	}

	void add_neighbour(std::string type, std::shared_ptr<Particle> new_neighbour);
	void add_neighbour(std::shared_ptr<Particle> new_neighbour);
	void remove_neighbour(ParticleBond to_remove);
	bool has_neighbour(std::shared_ptr<Particle> q);

	std::shared_ptr<ParticleSet> molecule() const {
		return _molecule.lock();
	}
	void set_molecule(std::shared_ptr<ParticleSet> new_molecule);

protected:
	static int _current_index;

	int _index;
	particle_type _type = DEFAULT_PARTICLE_TYPE;
	double _mass = 1.;
	double _charge = 0.;
	vec3 _position = vec3(0., 0., 0.);
	vec3 _velocity = vec3(0., 0., 0.);
	std::vector<vec3> _orientation_vectors;

	std::set<ParticleBond> _bonded_neighbours;
	std::set<ParticleAngle> _bonded_angles;
	std::set<ParticleDihedral> _bonded_dihedrals;

	std::set<ParticleBond> _neighbours;

	std::weak_ptr<ParticleSet> _molecule;
};

struct ParticleBond {
	ParticleBond(std::string nt, std::shared_ptr<Particle> nother) :
					type(nt),
					_other(nother) {

	}

	ParticleBond() = delete;

	virtual ~ParticleBond() {

	}

	std::shared_ptr<Particle> other() const {
		return _other.lock();
	}

	void set_other(std::shared_ptr<Particle> q) {
		_other = std::weak_ptr<Particle>(q);
	}

	bool operator<(const ParticleBond& rhs) const {
		if(type == rhs.type) {
			return other()->index() < rhs.other()->index();
		}
		return type < rhs.type;
	}

	std::string type = DEFAULT_LINK_TYPE;

private:
	std::weak_ptr<Particle> _other;
};

template<int size>
struct ParticleLink {
	template<typename ... T>
	ParticleLink(std::string nt, T&&... values) :
					type(nt),
					_particles( { std::forward<T>(values)... }) {

	}

	ParticleLink() = delete;

	virtual ~ParticleLink() {

	}

	typename std::shared_ptr<Particle> operator[](std::size_t idx) {
		return _particles[idx].lock();
	}

	const typename std::shared_ptr<Particle> &operator[](std::size_t idx) const {
		return _particles[idx].lock();
	}

	bool operator<(const ParticleLink& rhs) const {
		if(type == rhs.type) {
			bool smaller = true;
			for(int i = 0; i < size && smaller; i++) {
				auto p = _particles[i].lock();
				auto q = rhs._particles[i].lock();
				if(p->index() >= q->index()) {
					smaller = false;
				}
			}
			return smaller;
		}
		return type < rhs.type;
	}

	std::string type = DEFAULT_LINK_TYPE;

private:
	std::array<std::weak_ptr<Particle>, size> _particles;
};

#ifdef PYTHON_BINDINGS

void export_Particle(py::module &m);

#endif

} /* namespace ba */

#endif /* PARTICLES_PARTICLE_H_ */
