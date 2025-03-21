/*
 * DetailedPolymer.cpp
 *
 *  Created on: Feb 15, 2022
 *      Author: lorenzo
 */

#include "DetailedPolymer.h"

#include "../../utils/strings.h"

namespace ba {
namespace oxDNA_topology {

DetailedPolymer::DetailedPolymer(std::string topology_file) {
	std::ifstream topology(topology_file);

	if(!topology.good()) {
		std::string error = fmt::format("Topology file '{}' not found", topology_file);
		throw std::runtime_error(error);
	}

	std::string line;
	std::getline(topology, line);
	auto split = utils::split(line);

	_N = utils::lexical_cast<int>(split[0]);

	_types.resize(_N, "0");
	_bonded_neighbours.resize(_N);
	for(unsigned int i = 0; i < _N; i++) {
		unsigned int p_idx, n_bonds;

		std::getline(topology, line);
		split = utils::split(line);
		if(!topology.good()) {
			std::string error = fmt::format("The topology should contain two lines per particle, but it seems there is info for only {} particles\n", i);
			throw std::runtime_error(error);
		}

		p_idx = utils::lexical_cast<int>(split[0]);
		if(split.size() == 3) {
			_types[i] = utils::lexical_cast<int>(split[1]);
			n_bonds = utils::lexical_cast<int>(split[2]);
		}
		else if(split.size() == 4) {
			// chain = utils::lexical_cast<int>(split[1]); // we could use it to double check the integrity of the topology, but for now it's too much of a hassle 
			_types[i] = utils::lexical_cast<int>(split[2]);
			n_bonds = utils::lexical_cast<int>(split[3]);
		}
		else {
			std::string error = fmt::format("The first line relative to particle {} contains {} fields, should be either 3 or 4", p_idx, split.size());
			throw std::runtime_error(error);
		}

		if(i != p_idx) {
			std::string error = fmt::format("There is something wrong with the topology file. Expected index {}, found {}\n", i, p_idx);
			throw std::runtime_error(error);
		}

		std::getline(topology, line);
		split = utils::split(line);
		// the second condition avoids raising an error if line is empty
		if(n_bonds != split.size() && utils::trim_copy(line).size() != 0) {
			std::string error = fmt::format("The second line relative to particle {} contains {} fields, but in the previous line the number of links is {}", p_idx, split.size(), n_bonds);
			throw std::runtime_error(error);
		}

		for(unsigned int j = 0; j < n_bonds; j++) {
			unsigned int n_idx = utils::lexical_cast<int>(split[j]);

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
