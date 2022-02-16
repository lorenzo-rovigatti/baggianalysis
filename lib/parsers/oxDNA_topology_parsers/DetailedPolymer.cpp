/*
 * DetailedPolymer.cpp
 *
 *  Created on: Feb 15, 2022
 *      Author: lorenzo
 */

#include "DetailedPolymer.h"

namespace ba {
namespace oxDNA_topology {

DetailedPolymer::DetailedPolymer(std::string topology_file) {
	std::ifstream topology(topology_file);

	if(!topology.good()) {
		std::string error = fmt::format("Topology file '{}' not found", topology_file);
		throw std::runtime_error(error);
	}

	int N_chains;

	topology >> _N;
	topology >> N_chains;

	_types.resize(_N, "0");
	_bonded_neighbours.resize(_N);

	for(unsigned int i = 0; i < _N; i++) {
		unsigned int p_idx, n_bonds;
		topology >> p_idx;

		if(!topology.good()) {
			std::string error = fmt::format("The topology should contain two lines per particle, but it seems there is info for only {} particles\n", i);
			throw std::runtime_error(error);
		}

		topology >> _types[i];
		topology >> n_bonds;

		if(i != p_idx) {
			std::string error = fmt::format("There is something wrong with the topology file. Expected index {}, found {}\n", i, p_idx);
			throw std::runtime_error(error);
		}

		for(unsigned int j = 0; j < n_bonds; j++) {
			unsigned int n_idx;
			topology >> n_idx;
			// the >> operator always leaves '\n', which is subsequently picked up by getline if we don't explicitly ignore it
			topology.ignore();

			if(n_idx >= _N) {
				std::string error = fmt::format("The topology file contains a link between particles {} and {}, but the largest possible index in the configuration is {}", p_idx, n_idx, _N);
				throw std::runtime_error(error);
			}

			_bonded_neighbours[p_idx].push_back(n_idx);
		}
	}

	topology.close();
}

DetailedPolymer::~DetailedPolymer() {

}

particle_type DetailedPolymer::type(int idx) {
	return _types[idx];
}

const std::vector<std::vector<int>> &DetailedPolymer::bonded_neighbours() const {
	return _bonded_neighbours;
}

#ifdef PYTHON_BINDINGS

void export_DetailedPolymer(py::module &m) {
	py::class_<DetailedPolymer, std::shared_ptr<DetailedPolymer>, Default> parser(m, "DetailedPolymer");

	parser.def(py::init<std::string>());
}

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */
