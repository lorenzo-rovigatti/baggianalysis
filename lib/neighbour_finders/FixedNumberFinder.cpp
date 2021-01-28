/*
 * FixedNumberFinder.cpp
 *
 *  Created on: 6 mar 2020
 *      Author: lorenzo
 */

#include "FixedNumberFinder.h"

namespace ba {

FixedNumberFinder::FixedNumberFinder(uint N, double initial_cutoff) :
				_N(N),
				_cutoff(initial_cutoff),
				_lists(true) {
	_cutoff_sqr = _cutoff * _cutoff;
}

FixedNumberFinder::~FixedNumberFinder() {

}

void FixedNumberFinder::set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) {
	if(particles.size() < _N + 1) {
		std::string error = fmt::format("Cannot find %d neighbours per particle if there are only %d particles", _N, particles.size());
		throw std::runtime_error(error);
	}

	_lists.init_cells(particles, box, _cutoff);
	uint N_orders = _lists.cell_shifts().size();

	for(auto p : particles) {
		std::vector<Neighbour> possible_neighbours;

		auto p_cell = _lists.get_cell(p->position());
		uint order = 0;
		bool done = false;
		while(!done) {
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
							possible_neighbours.emplace_back(Neighbour(q, std::sqrt(distance_sqr)));
						}
					}

					current = _lists.next[current];
				}
			}
			order++;
			done = possible_neighbours.size() == _N || order == N_orders;
		}

		std::sort(possible_neighbours.begin(), possible_neighbours.end());
		for(uint i = 0; i < _N && i < possible_neighbours.size(); i++) {
			p->add_neighbour(possible_neighbours[i].q);
		}
	}
}

#ifdef PYTHON_BINDINGS

void export_FixedNumberFinder(py::module &m) {
	py::class_<FixedNumberFinder, NeighbourFinder, std::shared_ptr<FixedNumberFinder>> finder(m, "FixedNumberFinder", R"pbdoc(
        Define as neighbours of a particle the N particles that are closest to it. By default, a cutoff of 1.5 is used to look for neighbours. If not enough neighbours are found, the cutoff is increased. 
    )pbdoc");

	finder.def(py::init<int, double>(), py::arg("N"), py::arg("cutoff") = 1.5, R"pbdoc(
        The constructor takes two mandatory arguments.

        Parameters
        ----------
        N : int
            The number of neighbours N that each particle will have at the end of the finding procedure.
        cutoff : float
            The smallest cutoff that will be used to look for neighbours.
	)pbdoc");
}

#endif

} /* namespace ba */
