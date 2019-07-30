/*
 * System.h
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "particles/ParticleSet.h"
#include "neighbour_finders/NNFinder.h"

namespace ba {

class System : public ParticleSet {
public:
	System();
	virtual ~System();

	std::shared_ptr<System> empty_copy() const;

	void set_neighbours();

public:
	ullint time = 0;
	vec3 box = vec3(0., 0., 0.);

private:
	bool _neighbours_set = false;
	std::shared_ptr<NNFinder> _nn_finder;
};

#ifdef PYTHON_BINDINGS

void export_System(py::module &m);

#endif

} /* namespace ba */

#endif /* SYSTEM_H_ */
