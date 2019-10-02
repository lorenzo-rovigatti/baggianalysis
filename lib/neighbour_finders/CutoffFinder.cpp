/*
 * CutoffFinder.cpp
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#include "CutoffFinder.h"

#include <array>

namespace ba {

CutoffFinder::CutoffFinder(double cutoff) :
				NeighbourFinder(),
				_cutoff(cutoff),
				_cutoff_sqr(SQR(cutoff)),
				_lists(true) {

}

CutoffFinder::~CutoffFinder() {

}

void CutoffFinder::set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) {
	_lists.init_cells(particles, box, _cutoff);

	for(auto p : particles) {
		auto p_cell = _lists.get_cell(p->position());
		for(auto order : std::array<int, 2> { 0, 1 }) {
			for(auto shift : _lists.cell_shifts()[order]) {
				auto cell = p_cell + shift;
				cell[0] = (cell[0] + _lists.N_cells_side[0]) % _lists.N_cells_side[0];
				cell[1] = (cell[1] + _lists.N_cells_side[1]) % _lists.N_cells_side[1];
				cell[2] = (cell[2] + _lists.N_cells_side[2]) % _lists.N_cells_side[2];
				int cell_idx = cell[0] + _lists.N_cells_side[0] * (cell[1] + cell[2] * _lists.N_cells_side[1]);

				int current = _lists.heads[cell_idx];
				while(current != -1) {
					auto q = particles[current];

					if(p != q) {
						vec3 distance = p->position() - q->position();
						// periodic boundary conditions
						distance -= glm::round(distance / box) * box;
						double distance_sqr = glm::dot(distance, distance);

						if(distance_sqr < _cutoff_sqr) {
							p->add_neighbour(q);
							q->add_neighbour(p);
						}
					}

					current = _lists.next[current];
				}
			}
		}
	}
}

#ifdef PYTHON_BINDINGS

void export_CutoffFinder(py::module &m) {
	py::class_<CutoffFinder, NeighbourFinder, std::shared_ptr<CutoffFinder>> finder(m, "CutoffFinder");

	finder
		.def(py::init<double>());
}

#endif

} /* namespace ba */
