/*
 * TopologyParsers.cpp
 *
 *  Created on: 14 nov 2019
 *      Author: lorenzo
 */

#include "TopologyParsers.h"

#include "../utils/strings.h"

#include <fstream>

namespace ba {

void parse_microgel_bondfile(std::string filename, std::shared_ptr<Topology> topology) {
	std::ifstream input(filename);

	std::string line;
	std::getline(input, line);
	std::getline(input, line);

	while(input.good()) {
		std::getline(input, line);
		auto split = utils::split(utils::trim(line));

		if(split.size() == 2) {
			// the first line contains idx n_neighs
			int p_idx = utils::lexical_cast<int>(split[0]) - 1;
			uint n_neighs = utils::lexical_cast<uint>(split[1]);

			// the second line contains the indexes of all neighbours
			std::getline(input, line);
			split = utils::split(utils::trim(line));
			if(split.size() != n_neighs) {
				std::string error = boost::str(boost::format("Particle %d seems to have %u neighbours, should be %u") % p_idx % split.size() % n_neighs);
				throw std::runtime_error(error);
			}

			for(uint i = 0; i < split.size(); i++) {
				int q_idx = utils::lexical_cast<int>(split[i]) - 1;
				topology->add_bond(p_idx, q_idx);
			}
		}
	}

	input.close();
}

#ifdef PYTHON_BINDINGS

void export_TopologyParsers(py::module &m) {
	m.def("parse_microgel_bondfile", &parse_microgel_bondfile);
}

#endif

} /* namespace ba */
