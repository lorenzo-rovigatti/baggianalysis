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
		std::string error = fmt::format("The microgel topology file '{}' is unreadable", filename);
		throw std::runtime_error(error);
	}

	std::string line;
	std::getline(input, line);
	std::getline(input, line);

	int curr_idx = 0;
	while(input.good()) {
		std::getline(input, line);
		utils::trim(line);
		auto split = utils::split(line);

		if(line.size() > 0) {
			int p_idx;
			uint n_neighs;

			if(split.size() == 2) {
				// the first line contains idx n_neighs
				p_idx = utils::lexical_cast<int>(split[0]) - 1;
				n_neighs = utils::lexical_cast<uint>(split[1]);
			}
			else {
				std::string error = fmt::format("The '{}' line found in the microgel bondfile should have two fields, not {}", line, split.size());
				throw std::runtime_error(error);
			}

			// the second line contains the indexes of all neighbours
			std::getline(input, line);
			if(n_neighs > 0) {
				split = utils::split(utils::trim_copy(line));
				if(split.size() != n_neighs) {
					std::string error = fmt::format("Particle {} seems to have {} neighbours, should be {}", p_idx, split.size(), n_neighs);
					throw std::runtime_error(error);
				}

				for(uint i = 0; i < split.size(); i++) {
					int q_idx = utils::lexical_cast<int>(split[i]) - 1;
					topology->add_bond(p_idx, q_idx);
				}
			}
			curr_idx++;
		}
	}

	input.close();
}

void parse_polymer_bondfile(std::string filename, std::shared_ptr<Topology> topology) {
	std::ifstream input(filename);

	if(!input.good()) {
		std::string error = fmt::format("The polymer topology file '{}' is unreadable", filename);
		throw std::runtime_error(error);
	}

	int p_idx = 0;
	while(input.good()) {
		std::string line;
		std::getline(input, line);
		utils::trim(line);
		auto split = utils::split(line);

		if(line.size() > 0) {
			uint n_neighs;

			if(split.size() == 1) {
				n_neighs = utils::lexical_cast<uint>(split[0]);
			}
			else {
				std::string error = fmt::format("The '{}' line found in the polymer bondfile should have one field, not {}", line, split.size());
				throw std::runtime_error(error);
			}

			// the second line contains the indexes of all neighbours
			std::getline(input, line);
			if(n_neighs > 0) {
				split = utils::split(utils::trim_copy(line));
				if(split.size() != n_neighs) {
					std::string error = fmt::format("Particle {} seems to have {} neighbours, should be {}", p_idx, split.size(), n_neighs);
					throw std::runtime_error(error);
				}

				for(uint i = 0; i < split.size(); i++) {
					int q_idx = utils::lexical_cast<int>(split[i]) - 1;
					topology->add_bond(p_idx, q_idx);
				}
			}
			p_idx++;
		}
	}

	input.close();
}

std::shared_ptr<Topology> topology_from_LAMMPS_data_file(std::string filename) {
	LAMMPSDataFileParser parser("atomic");
	parser.make_system(filename);
	return parser.topology();
}

void parse_LAMMPS_topology(std::string filename, std::shared_ptr<Topology> topology) {
	std::ifstream input(filename);

	if(!input.good()) {
		std::string error = fmt::format("The topology file '{}' is unreadable", filename);
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
		line = utils::trim_copy(utils::split(line, "#")[0]);

		auto split = utils::split(line);
		if(utils::ends_with(line, "angles")) {
			N_angles = utils::lexical_cast<int>(split[0]);
		}
		else if(utils::ends_with(line, "bonds")) {
			N_bonds = utils::lexical_cast<int>(split[0]);
		}
		else if(utils::ends_with(line, "dihedrals")) {
			N_dihedrals = utils::lexical_cast<int>(split[0]);
		}

		if(line == "Angles") {
			for(uint i = 0; i < N_angles; i++) {
				std::getline(input, line);
				line = ba::utils::trim_copy(line);
				// skip empty lines
				if(line.size() == 0) {
					i--;
					continue;
				}

				auto split = utils::split(line);
				if(split.size() < 5) {
					std::string error = fmt::format("Invalid angle line '{}'", line);
					throw std::runtime_error(error);
				}

				topology->add_angle(
						utils::lexical_cast<int>(split[2]),
						utils::lexical_cast<int>(split[3]),
						utils::lexical_cast<int>(split[4]));
			}
		}
		else if(line == "Bonds") {
			for(uint i = 0; i < N_bonds; i++) {
				std::getline(input, line);
				line = ba::utils::trim_copy(line);
				// skip empty lines
				if(line.size() == 0) {
					i--;
					continue;
				}

				auto split = utils::split(line);
				if(split.size() < 4) {
					std::string error = fmt::format("Invalid bond line '{}'", line);
					throw std::runtime_error(error);
				}

				int id_1 = utils::lexical_cast<int>(split[2]);
				int id_2 = utils::lexical_cast<int>(split[3]);
				topology->add_bond(id_1, id_2);
			}
		}
		else if(line == "Dihedrals") {
			for(uint i = 0; i < N_dihedrals; i++) {
				std::getline(input, line);
				line = ba::utils::trim_copy(line);
				// skip empty lines
				if(line.size() == 0) {
					i--;
					continue;
				}

				auto split = utils::split(line);
				if(split.size() < 6) {
					std::string error = fmt::format("Invalid dihedral line '{}'", line);
					throw std::runtime_error(error);
				}

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
	m.def("parse_polymer_bondfile", &parse_polymer_bondfile);
	m.def("parse_LAMMPS_topology", &parse_LAMMPS_topology);
	m.def("topology_from_LAMMPS_data_file", &topology_from_LAMMPS_data_file);
}

#endif

} /* namespace ba */
