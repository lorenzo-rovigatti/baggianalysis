/*
 * TopologyParsers.cpp
 *
 *  Created on: 14 nov 2019
 *      Author: lorenzo
 */

#include "TopologyParsers.h"

#include "../utils/strings.h"
#include "../parsers/LAMMPSDataFileParser.h"

#include <fstream>

namespace ba {

void parse_microgel_bondfile(std::string filename, std::shared_ptr<Topology> topology) {
	std::ifstream input(filename);

	if(!input.good()) {
		std::string error = boost::str(boost::format("The topology file '%s' is unreadable") % filename);
		throw std::runtime_error(error);
	}

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

void parse_LAMMPS_topology(std::string filename, std::shared_ptr<Topology> topology) {
	std::ifstream input(filename);

	if(!input.good()) {
		std::string error = boost::str(boost::format("The topology file '%s' is unreadable") % filename);
		throw std::runtime_error(error);
	}

	std::string line;
	std::getline(input, line);

	uint N_bonds = 0;
	uint N_angles = 0;
	uint N_dihedrals = 0;

	std::vector<std::string> stop_on = {"Angles", "Bonds", "Dihedrals"};

	enum status {
		HEADERS,
		ANGLES,
		BONDS,
		DIHEDRALS
	};

	while(input.good()) {
		std::getline(input, line);
		line = utils::trim(utils::split(line, "#")[0]);

		auto split = utils::split(line);
		if(boost::ends_with(line, "angles")) {
			N_angles = utils::lexical_cast<int>(split[0]);
		}
		else if(boost::ends_with(line, "bonds")) {
			N_bonds = utils::lexical_cast<int>(split[0]);
		}
		else if(boost::ends_with(line, "dihedrals")) {
			N_dihedrals = utils::lexical_cast<int>(split[0]);
		}

		if(line == "Angles") {
			for(uint i = 0; i < N_angles; i++) {
				std::getline(input, line);
				// skip empty lines
				if(line.size() == 0) {
					i--;
					continue;
				}

				auto split = utils::split(line);
				topology->add_angle(
						utils::lexical_cast<int>(split[2]),
						utils::lexical_cast<int>(split[3]),
						utils::lexical_cast<int>(split[4]));
			}
		}
		else if(line == "Bonds") {
			for(uint i = 0; i < N_bonds; i++) {
				std::getline(input, line);
				// skip empty lines
				if(line.size() == 0) {
					i--;
					continue;
				}

				auto split = utils::split(line);
				int id_1 = utils::lexical_cast<int>(split[2]);
				int id_2 = utils::lexical_cast<int>(split[3]);
				topology->add_bond(id_1, id_2);
			}
		}
		else if(line == "Dihedrals") {
			for(uint i = 0; i < N_dihedrals; i++) {
				std::getline(input, line);
				// skip empty lines
				if(line.size() == 0) {
					i--;
					continue;
				}

				auto split = utils::split(line);
				topology->add_dihedral(
						utils::lexical_cast<int>(split[2]),
						utils::lexical_cast<int>(split[3]),
						utils::lexical_cast<int>(split[4]),
						utils::lexical_cast<int>(split[5]));
			}
		}
	}

	input.close();
}

#ifdef PYTHON_BINDINGS

void export_TopologyParsers(py::module &m) {
	m.def("parse_microgel_bondfile", &parse_microgel_bondfile);
	m.def("parse_LAMMPS_topology", &parse_LAMMPS_topology);
}

#endif

} /* namespace ba */
