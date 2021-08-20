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

/**
 * @brief A utility struct used by some finders.
 */
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

class NeighbourFinder {
public:
	NeighbourFinder();
	virtual ~NeighbourFinder();

	void include_bonded_neighbours(bool include);

	virtual void set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) = 0;

protected:
	bool _include_bonded_neighbours = false;

	virtual void _add_neighbour(std::shared_ptr<Particle> add_to, std::shared_ptr<Particle> to_be_added);
	virtual void _add_neighbour(std::shared_ptr<Particle> add_to, std::string type, std::shared_ptr<Particle> to_be_added);
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
