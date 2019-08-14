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

class CutoffFinder: public NeighbourFinder {
public:
	CutoffFinder(double cutoff);
	virtual ~CutoffFinder();

	void set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) override;

private:
	double _cutoff;
	double _cutoff_sqr;

	CellLists _lists;
};

#ifdef PYTHON_BINDINGS

void export_CutoffFinder(py::module &m);

#endif

} /* namespace ba */

#endif /* NEIGHBOUR_FINDERS_CUTOFFFINDER_H_ */
