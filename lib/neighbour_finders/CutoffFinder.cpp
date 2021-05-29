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
				_cutoff_sqr(SQR(cutoff)) {

}

CutoffFinder::CutoffFinder(double cutoff, NeighbourFunction neigh_function) :
				NeighbourFinder(),
				_cutoff(cutoff),
				_cutoff_sqr(SQR(cutoff)),
				_neigh_function(neigh_function) {

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

						if(distance_sqr < _cutoff_sqr && _neigh_function(p, q, distance)) {
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
	py::class_<CutoffFinder, NeighbourFinder, std::shared_ptr<CutoffFinder>> finder(m, "CutoffFinder", R"pbdoc(
        Define as neighbours of a particle all those particles that are closer than the given cutoff and, optionally, satisfy further conditions set by a custom function. 
    )pbdoc");

	finder.def(py::init<double>(), py::arg("cutoff"), R"pbdoc(
The default constructor takes a single parameter setting the cutoff.

Parameters
----------
cutoff : double
	The cutoff used to define neighbours.
    )pbdoc");

	finder.def(py::init<double, NeighbourFunction>(), py::arg("cutoff"), py::arg("neighbour_function"), R"pbdoc(
This constructor takes two parameters: the cutoff and a callable that can be used to enforce further conditions.

Parameters
----------
cutoff : double
	The cutoff used to define neighbours.
mapper: callable
    A callable that takes two particles and a vector that is the distance between the two particles, and returns a boolean, which should be True if the two particles are neighbours, False otherwise.
    )pbdoc");
}

#endif

} /* namespace ba */
