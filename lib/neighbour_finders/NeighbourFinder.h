/*
 * NNFinder.h
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#ifndef NEIGHBOUR_FINDERS_NEIGHBOURFINDER_H_
#define NEIGHBOUR_FINDERS_NEIGHBOURFINDER_H_

#include "../particles/Particle.h"

namespace ba {

class NeighbourFinder {
public:
	NeighbourFinder();
	virtual ~NeighbourFinder();

	virtual void set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) = 0;
};

#ifdef PYTHON_BINDINGS

class PyNeighbourFinder : public NeighbourFinder {
public:
	using NeighbourFinder::NeighbourFinder;

	void set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) override {
		PYBIND11_OVERLOAD_PURE( // @suppress("Unused return value")
				void,
				NeighbourFinder,
				set_neighbours,
				particles,
				box
		);
	}
};

void export_NeighbourFinder(py::module &m);

#endif

} /* namespace ba */

#endif /* NEIGHBOUR_FINDERS_NEIGHBOURFINDER_H_ */
