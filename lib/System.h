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

	/// Return all the subsets stored in the set
	std::vector<std::shared_ptr<ParticleSet>> &molecules();

	/// Return all the subsets stored in the set (const version)
	const std::vector<std::shared_ptr<ParticleSet>> &molecules() const;

	ullint time = 0;
	vec3 box = vec3(0., 0., 0.);

private:
	std::vector<std::shared_ptr<ParticleSet>> _molecules;
};

#ifdef PYTHON_BINDINGS

void export_System(py::module &m);

#endif

} /* namespace ba */

#endif /* SYSTEM_H_ */
