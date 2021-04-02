/*
 * SANNFinder.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: lorenzo
 */

#include "SANNFinder.h"

namespace ba {

SANNFinder::SANNFinder(double max_distance, SymmetryPolicy policy) :
				NeighbourFinder(),
				_max_distance(max_distance),
				_lists(true),
				_policy(policy) {

}

SANNFinder::~SANNFinder() {

}

void SANNFinder::set_neighbours(std::vector<std::shared_ptr<Particle>> particles, const vec3 &box) {
	double cutoff = _max_distance;
	double cutoff_sqr = SQR(cutoff);

	_lists.init_cells(particles, box, cutoff);

	for(auto p : particles) {
		std::vector<Neighbour> possible_neighbours;

		auto p_cell = _lists.get_cell(p->position());
		for(auto order : std::array<int, 2> { 0 }) {
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

		size_t neigh_number;

		if(possible_neighbours.size() < 3) {
			std::string error = fmt::format("Particle {} has fewer than 3 neighbours ({})", p->index(), possible_neighbours.size());
			BA_WARNING("Particle {} has fewer than 3 neighbours ({})", p->index(), possible_neighbours.size());
			neigh_number = possible_neighbours.size();
		}
		else {
			// sort according to the relative distance between each neighbour and p
			std::sort(possible_neighbours.begin(), possible_neighbours.end());

			// this will be the radius of the neighbour shell of particle p, which is defined as the sum of the distances of all n neighbours divided by (n - 2)
			double radius;
			// we start by considering the first 3 neighbours
			double distance_sum = 0.;
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
				BA_WARNING("The SANN radius of particle {} has not converged ({} neighbours are not enough)", p->index(), neigh_number);
			}
		}

		for(size_t i = 0; i < neigh_number; i++) {
			p->add_neighbour(_bond_type, possible_neighbours[i].q);
		}
	}

	// symmetrise the neighbour lists as requested by the user
	switch(_policy) {
	case SYMMETRISE_BY_ADDING:
		_symmetrise_by_adding(particles);
		break;
	case SYMMETRISE_BY_REMOVING:
		_symmetrise_by_removing(particles);
		break;
	default:
		break;
	}
}

void SANNFinder::_symmetrise_by_adding(std::vector<std::shared_ptr<Particle>> particles) {
	for(auto p : particles) {
		ParticleBond p_bond(_bond_type, p);
		for(auto neigh : p->neighbours()) {
			auto q = neigh.other();
			if(q->has_neighbour(p)) {
				q->add_neighbour(_bond_type, p);
			}
		}
	}
}

void SANNFinder::_symmetrise_by_removing(std::vector<std::shared_ptr<Particle>> particles) {
	for(auto p : particles) {
		std::vector<std::shared_ptr<Particle>> to_remove;
		for(auto neigh : p->neighbours()) {
			auto q = neigh.other();
			if(q->has_neighbour(p)) {
				to_remove.push_back(q);
			}
		}

		for(auto q : to_remove) {
			p->remove_neighbour(ParticleBond(_bond_type, q));
		}
	}
}

#ifdef PYTHON_BINDINGS

void export_SANNFinder(py::module &m) {
	py::class_<SANNFinder, NeighbourFinder, std::shared_ptr<SANNFinder>> finder(m, "SANNFinder");

	finder.def(py::init<double, SANNFinder::SymmetryPolicy>());

	py::enum_<SANNFinder::SymmetryPolicy>(finder, "SymmetryPolicy")
	    .value("NO_ACTION", SANNFinder::SymmetryPolicy::NO_ACTION)
	    .value("SYMMETRISE_BY_ADDING", SANNFinder::SymmetryPolicy::SYMMETRISE_BY_ADDING)
		.value("SYMMETRISE_BY_REMOVING", SANNFinder::SymmetryPolicy::SYMMETRISE_BY_REMOVING)
	    .export_values();
}

#endif

} /* namespace ba */
