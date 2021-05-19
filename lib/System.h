/*
 * System.h
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "particles/ParticleSet.h"
#include "particles/Particle.h"

namespace ba {

class System : public ParticleSet {
public:
	System();
	virtual ~System();

	std::shared_ptr<System> empty_copy() const;

	int available_index() const;

	void add_particle(std::shared_ptr<Particle> p) override;

	void reindex_particles(int base_index);

	void rescale_lengths(double factor);

	/// Return all the subsets stored in the set
	std::vector<std::shared_ptr<ParticleSet>> &molecules();

	/// Return all the subsets stored in the set (const version)
	const std::vector<std::shared_ptr<ParticleSet>> &molecules() const;

	void normalise_molecules();

	void bring_particles_in_box(bool shift_by_half_box = false);

	ullint time = 0;
	vec3 box = vec3(0., 0., 0.);

private:
	std::vector<std::shared_ptr<ParticleSet>> _molecules;
	int _largest_idx = -1;
};

#ifdef PYTHON_BINDINGS

void export_System(py::module &m);

#endif

} /* namespace ba */

#endif /* SYSTEM_H_ */
