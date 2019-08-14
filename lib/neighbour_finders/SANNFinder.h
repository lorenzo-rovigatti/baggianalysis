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
	struct Neighbour {
		Neighbour(std::shared_ptr<Particle> n_q, double n_distance) :
						q(n_q),
						distance(n_distance) {

		}

		friend bool operator<(const Neighbour &l, const Neighbour &r) {
			return l.distance < r.distance;
		}

		std::shared_ptr<Particle> q;
		double distance;
	};

public:
	SANNFinder();
	virtual ~SANNFinder();

	virtual void set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) override;

private:
	CellLists _lists;
};

#ifdef PYTHON_BINDINGS

void export_SANNFinder(py::module &m);

#endif

} /* namespace ba */

#endif /* NEIGHBOUR_FINDERS_SANNFINDER_H_ */
