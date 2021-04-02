/*
 * SANNFinder.h
 *
 *  Created on: Aug 8, 2019
 *      Author: lorenzo
 */

#ifndef NEIGHBOUR_FINDERS_SANNFINDER_H_
#define NEIGHBOUR_FINDERS_SANNFINDER_H_

#include "NeighbourFinder.h"

#include "../utils/CellLists.h"

namespace ba {

class SANNFinder: public NeighbourFinder {
public:
	enum SymmetryPolicy {
		NO_ACTION,
		SYMMETRISE_BY_ADDING,
		SYMMETRISE_BY_REMOVING
	};

	SANNFinder(double max_distance, SymmetryPolicy policy);
	virtual ~SANNFinder();

	void set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) override;

private:
	double _max_distance;
	CellLists _lists;
	SymmetryPolicy _policy;
	const std::string _bond_type = "SANN";

	// TODO: move the logic of making the neighbour lists symmetric to NeighbourFinder
	void _symmetrise_by_adding(std::vector<std::shared_ptr<Particle>> particles);
	void _symmetrise_by_removing(std::vector<std::shared_ptr<Particle>> particles);
};

#ifdef PYTHON_BINDINGS

void export_SANNFinder(py::module &m);

#endif

} /* namespace ba */

#endif /* NEIGHBOUR_FINDERS_SANNFINDER_H_ */
