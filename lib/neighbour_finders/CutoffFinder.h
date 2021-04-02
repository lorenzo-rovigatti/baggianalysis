/*
 * CutoffFinder.h
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#ifndef NEIGHBOUR_FINDERS_CUTOFFFINDER_H_
#define NEIGHBOUR_FINDERS_CUTOFFFINDER_H_

#include "NeighbourFinder.h"

#include "../utils/CellLists.h"

namespace ba {

using NeighbourFunction = std::function<bool(Particle *, Particle *)>;

class CutoffFinder: public NeighbourFinder {
public:
	CutoffFinder(double cutoff);
	CutoffFinder(double cutoff, NeighbourFunction neigh_function);
	virtual ~CutoffFinder();

	void set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) override;

private:
	double _cutoff;
	double _cutoff_sqr;

	CellLists _lists = CellLists(true);
	NeighbourFunction _neigh_function = [](Particle *, Particle *) { return true; };
};

#ifdef PYTHON_BINDINGS

void export_CutoffFinder(py::module &m);

#endif

} /* namespace ba */

#endif /* NEIGHBOUR_FINDERS_CUTOFFFINDER_H_ */
