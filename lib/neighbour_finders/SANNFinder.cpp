/*
 * SANNFinder.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: lorenzo
 */

#include "SANNFinder.h"

namespace ba {

SANNFinder::SANNFinder() :
				NeighbourFinder(),
				_lists(true) {

}

SANNFinder::~SANNFinder() {

}

void SANNFinder::set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) {
	double cutoff = 2.0;
	double cutoff_sqr = SQR(cutoff);

	_lists.init_cells(particles, box, cutoff);

	for(auto p : particles) {
		std::vector<Neighbour> possible_neighbours;

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

						if(distance_sqr < cutoff_sqr) {
							possible_neighbours.emplace_back(Neighbour(q, std::sqrt(distance_sqr)));
						}
					}

					current = _lists.next[current];
				}
			}
		}

		if(possible_neighbours.size() < 3) {
			std::string error = boost::str(boost::format("Particle %d has fewer than 3 neighbours") % p->index());
			throw std::runtime_error(error);
		}

		// sort according to the relative distance between each neighbour and p
		std::sort(possible_neighbours.begin(), possible_neighbours.end());

		// this will be the radius of the neighbour shell of particle p, which is defined as the sum of the distances of all n neighbours divided by (n - 2)
		double radius;
		// we start by considering the first 3 neighbours
		double distance_sum = 0.;
		size_t neigh_number;
		for(neigh_number = 0; neigh_number < 3; neigh_number++) {
			distance_sum += possible_neighbours[neigh_number].distance;
		}
		radius = distance_sum;

		// now we iteratively include neighbours one by one until the SANN radius is smaller than the distance of the next neighbour
		while((neigh_number < possible_neighbours.size()) && (radius > possible_neighbours[neigh_number].distance)) {
			distance_sum += possible_neighbours[neigh_number].distance;
			neigh_number++;
			radius = distance_sum / (neigh_number - 2.);
		}

		if(neigh_number == possible_neighbours.size()) {
			std::string error = boost::str(boost::format("The SANN radius of particle %d has not converged (%d neighbours are not enough)") % p->index() % neigh_number);
			throw std::runtime_error(error);
		}

		for(size_t i = 0; i < neigh_number; i++) {
			p->add_neighbour(possible_neighbours[i].q);
		}
	}
}

#ifdef PYTHON_BINDINGS

void export_SANNFinder(py::module &m) {
	pybind11::class_<SANNFinder, NeighbourFinder, std::shared_ptr<SANNFinder>> finder(m, "SANNFinder");

	finder
		.def(py::init<>());
}

#endif

} /* namespace ba */
