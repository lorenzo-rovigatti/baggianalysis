/*
 * ParticleSet.h
 *
 *  Created on: 16 apr 2019
 *      Author: lorenzo
 */

#ifndef PARTICLES_PARTICLESET_H_
#define PARTICLES_PARTICLESET_H_

#include "../defs.h"
#include "../python_defs.h"

#include <map>

namespace ba {

class Particle;

class ParticleSet {
public:
	/**
	 * @brief Construct the object
	 */
	ParticleSet();
	virtual ~ParticleSet();

	/// Return the name of the set
	std::string name() const;

	/// Set the name of the set
	void set_name(const std::string &name);

	/// Return the number of particles
	uint N() const;

	/**
	 * @brief Return a vector containing the index of all the particles
	 *
	 * The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios.
	 *
	 * @return a vector of particle indexes
	 */
	std::vector<int> indexes() const;

	/**
	 * @brief Return a vector containing all particle types
	 *
	 * The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios.
	 *
	 * @return a vector of particle types
	 */
	std::vector<particle_type> types() const;

	/**
	 * @brief Return a vector containing all particle masses
	 *
	 * The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios.
	 *
	 * @return a vector of particle masses
	 */
	std::vector<double> masses() const;

	/**
	 * @brief Return a vector containing all particle charges
	 *
	 * The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios.
	 *
	 * @return a vector of particle masses
	 */
	std::vector<double> charges() const;

	/**
	 * @brief Return a vector containing all particle positions
	 *
	 * The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios.
	 *
	 * @return a vector of particle positions
	 */
	vector_vec3 positions() const;

	/**
	 * @brief Return a vector containing all particle velocities
	 *
	 * The vector is created ex novo every time this method is called. Do not use in performance-critical scenarios.
	 *
	 * @return a vector of particle velocities
	 */
	vector_vec3 velocities() const;

	/// Return the sum of the masses of the particles contained in the set
	double mass() const;

	/// Return the total charge contained in the set
	double charge() const;

	/// Return the centre of mass of the set
	vec3 com() const;

	/// Return the velocity of the set
	vec3 velocity() const;

	/// Return all the particles stored in the set
	std::vector<std::shared_ptr<Particle>> &particles();

	/// Return all the particles stored in the set (const version)
	const std::vector<std::shared_ptr<Particle>> &particles() const;

	/// Add a particle to the set
	virtual void add_particle(std::shared_ptr<Particle> p);

	/**
	 * @brief Remove a particle from the set.
	 *
	 * @param p the particle to be removed
	 */
	void remove_particle(std::shared_ptr<Particle> p);

	/**
	 * @brief Remove the particle with the given id from the set.
	 *
	 * @param p_id the index of the particle to be removed
	 */
	void remove_particle_by_id(int p_id);

	bool id_exists(int index) const;

	/// Return the particle with the given id. Throw an exception if the particle does not exist.
	std::shared_ptr<Particle> particle_by_id(int index) const;

	/// Sort the particles according to their index.
	void sort_particles_by_id();

protected:
	std::string _name;

	std::vector<std::shared_ptr<Particle>> _particles;
	std::map<int, std::shared_ptr<Particle>> _particles_by_id;
};

#ifdef PYTHON_BINDINGS

void export_ParticleSet(py::module &m);

#endif

} /* namespace ba */

#endif /* PARTICLES_PARTICLESET_H_ */
