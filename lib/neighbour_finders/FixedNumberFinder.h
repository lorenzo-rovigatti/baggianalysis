/*
 * FixedNumberFinder.h
 *
 *  Created on: 6 mar 2020
 *      Author: lorenzo
 */

#ifndef NEIGHBOUR_FINDERS_FIXEDNUMBERFINDER_H_
#define NEIGHBOUR_FINDERS_FIXEDNUMBERFINDER_H_

#include "NeighbourFinder.h"

#include "../utils/CellLists.h"

namespace ba {

class FixedNumberFinder: public NeighbourFinder {
public:
	FixedNumberFinder(uint N);
	FixedNumberFinder(uint N, double initial_cutoff=1.5);
	virtual ~FixedNumberFinder();

	void set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) override;

private:
	uint _N;
	double _cutoff;
	double _cutoff_sqr;

	CellLists _lists;
};

#ifdef PYTHON_BINDINGS

void export_FixedNumberFinder(py::module &m);

#endif

} /* namespace ba */

#endif /* NEIGHBOUR_FINDERS_FIXEDNUMBERFINDER_H_ */
