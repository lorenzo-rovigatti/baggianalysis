/*
 * CutoffFinder.cpp
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#include "CutoffFinder.h"

namespace ba {

CutoffFinder::CutoffFinder(double cutoff) :
				NeighbourFinder(),
				_cutoff(cutoff),
				_cutoff_sqr(SQR(cutoff)) {

}

CutoffFinder::~CutoffFinder() {

}

void CutoffFinder::set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) {
	for(auto p_it = particles.begin(); p_it != particles.end(); p_it++) {
		auto p = *p_it;

		for(auto q_it = p_it + 1; q_it != particles.end(); q_it++) {
			auto q = *q_it;

			vec3 distance = p->position() - q->position();
			// periodic boundary conditions
			distance -= glm::round(distance / box) * box;
			double distance_sqr = glm::dot(distance, distance);

			if(distance_sqr < _cutoff_sqr) {
				p->add_neighbour(q);
			}
		}
	}
}

#ifdef PYTHON_BINDINGS

void export_CutoffFinder(py::module &m) {
	pybind11::class_<CutoffFinder, NeighbourFinder, std::shared_ptr<CutoffFinder>> finder(m, "CutoffFinder");

	finder
		.def(py::init<double>());
}

#endif

} /* namespace ba */
