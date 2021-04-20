/*
 * LAMMPSDataFileParser.cpp
 *
 *  Created on: May 26, 2019
 *      Author: lorenzo
 */

#include "LAMMPSDataFileParser.h"

#include "../utils/strings.h"

namespace ba {

LAMMPSDataFileParser::LAMMPSDataFileParser(std::string atom_style) :
				BaseParser() {


	if(atom_style == "atomic") {
		_type_index = 1;
		_pos_starting_index = 2;
	}
	else if(atom_style == "bond") {
		_type_index = 2;
		_pos_starting_index = 3;
	}
	else if(atom_style == "full") {
		_type_index = 2;
		_charge_index = 3;
		_pos_starting_index = 4;
	}
	else {
		std::string error = fmt::format("Unsupported LAMMPS atom_style '{}'", atom_style);
		throw std::runtime_error(error);
	}
}

LAMMPSDataFileParser::LAMMPSDataFileParser(int type_index, int pos_starting_index) :
				BaseParser(),
				_type_index(type_index),
				_pos_starting_index(pos_starting_index) {

}

LAMMPSDataFileParser::LAMMPSDataFileParser(int type_index, int pos_starting_index, int charge_index) :
				BaseParser(),
				_type_index(type_index),
				_pos_starting_index(pos_starting_index),
				_charge_index(charge_index) {

}

LAMMPSDataFileParser::~LAMMPSDataFileParser() {

}

std::shared_ptr<System> LAMMPSDataFileParser::_parse_stream(std::ifstream &configuration) {
	std::shared_ptr<System> syst(std::make_shared<System>());

	_topology = Topology::make_empty_topology();

	_header_data = _parse_headers(configuration);

	// there is no time information in a LAMMPS data file
	syst->time = 0;
	syst->box = _header_data.box;

	if(_header_data.empty) {
		throw std::runtime_error("The LAMMPS datafile did not contain any headers");
	}

	if(_header_data.N_atoms == 0) {
		BA_WARNING("The LAMMPS datafile seems to have non-empty headers but 0 atoms, be careful!");
		return syst;
	}

	std::string line;
	while(configuration.good()) {
		line = _read_line(configuration);
		if(line == "Masses") {
			_parse_masses(configuration);
		}
		else if(line == "Atoms") {
			_parse_atoms_data(configuration, syst);
		}
		else if(line == "Velocities") {
			_parse_velocities(configuration, syst);
		}
		else if(line == "Angles") {
			for(uint i = 0; i < _header_data.N_angles; i++) {
				std::getline(configuration, line);
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

				std::string type = split[1];
				_topology->add_angle(
						type,
						utils::lexical_cast<int>(split[2]),
						utils::lexical_cast<int>(split[3]),
						utils::lexical_cast<int>(split[4]));
			}
		}
		else if(line == "Bonds") {
			for(uint i = 0; i < _header_data.N_bonds; i++) {
				std::getline(configuration, line);
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

				std::string type = split[1];
				int id_1 = utils::lexical_cast<int>(split[2]);
				int id_2 = utils::lexical_cast<int>(split[3]);
				_topology->add_bond(type, id_1, id_2);
			}
		}
		else if(line == "Dihedrals") {
			for(uint i = 0; i < _header_data.N_dihedrals; i++) {
				std::getline(configuration, line);
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

				std::string type = split[1];
				_topology->add_dihedral(
						type,
						utils::lexical_cast<int>(split[2]),
						utils::lexical_cast<int>(split[3]),
						utils::lexical_cast<int>(split[4]),
						utils::lexical_cast<int>(split[5]));
			}
		}
	}

	syst->sort_particles_by_id();

	return syst;
}

LAMMPSDataFileParser::HeaderData LAMMPSDataFileParser::_parse_headers(std::ifstream &configuration) {
	LAMMPSDataFileParser::HeaderData hd;

	std::string line;
	// according to the docs, the first line of the header should be always skipped
	std::getline(configuration, line);

	bool done = false;
	while(!done) {
		auto conf_pos = configuration.tellg();
		line = _read_line(configuration);

		if(!configuration.good() || std::find(_LAMMPS_section_keywords.begin(), _LAMMPS_section_keywords.end(), line) != _LAMMPS_section_keywords.end()) {
			configuration.seekg(conf_pos);
			done = true;
		}
		else {
			if(line.size() > 0) {
				auto split = utils::split(line);
				if(utils::ends_with(line, "atoms")) {
					hd.N_atoms = utils::lexical_cast<int>(split[0]);
				}
				else if(utils::ends_with(line, "bonds")) {
					hd.N_bonds = utils::lexical_cast<int>(split[0]);
				}
				else if(utils::ends_with(line, "angles")) {
					hd.N_angles = utils::lexical_cast<int>(split[0]);
				}
				else if(utils::ends_with(line, "dihedrals")) {
					hd.N_dihedrals = utils::lexical_cast<int>(split[0]);
				}
				else if(utils::ends_with(line, "atom types")) {
					hd.atom_types = utils::lexical_cast<int>(split[0]);
				}
				else if(utils::ends_with(line, "bond types")) {
					hd.bond_types = utils::lexical_cast<int>(split[0]);
				}
				else if(utils::ends_with(line, "angle types")) {
					hd.angle_types = utils::lexical_cast<int>(split[0]);
				}
				else if(utils::ends_with(line, "dihedral types")) {
					hd.dihedral_types = utils::lexical_cast<int>(split[0]);
				}
				else if(utils::ends_with(line, "xlo xhi")) {
					hd.box[0] = _parse_box_line(split);
				}
				else if(utils::ends_with(line, "ylo yhi")) {
					hd.box[1] = _parse_box_line(split);
				}
				else if(utils::ends_with(line, "zlo zhi")) {
					hd.box[2] = _parse_box_line(split);
				}
			}
		}
	}

	if(hd.N_atoms == 0 && hd.N_bonds == 0 && hd.atom_types == 0 && hd.bond_types == 0 && hd.box == vec3(1., 1., 1.)) {
		hd.empty = true;
	}

	return hd;
}

void LAMMPSDataFileParser::_parse_masses(std::ifstream &configuration) {
	std::string line;
	bool done = false;
	while(!done) {
		auto conf_pos = configuration.tellg();
		line = _read_line(configuration);

		if(!configuration.good() || std::find(_LAMMPS_section_keywords.begin(), _LAMMPS_section_keywords.end(), line) != _LAMMPS_section_keywords.end()) {
			configuration.seekg(conf_pos);
			done = true;
		}
		else {
			if(line.size() > 0) {
				auto split = utils::split(line);
				if(split.size() != 2) {
					std::string error = fmt::format("The mass line '{}' is invalid", line);
					throw std::runtime_error(error);
				}

				particle_type pt = split[0];
				double mass = utils::lexical_cast<double>(split[1]);
				_masses[pt] = mass;
			}
		}
	}
}

void LAMMPSDataFileParser::_parse_atoms_data(std::ifstream &configuration, std::shared_ptr<System> syst) {
	std::string line;
	for(uint i = 0; i < _header_data.N_atoms; i++) {
		line = _read_line(configuration);
		if(!configuration.good()) {
			std::string error = fmt::format("The line relative to the {}-th particle cannot be read", i);
			throw std::runtime_error(error);
		}
		// skip empty lines
		if(line.size() == 0) {
			i--;
			continue;
		}

		auto split = utils::split(line);

		// the first column always contains the atom index
		int p_idx = utils::lexical_cast<int>(split[0]);
		std::string p_type = split[_type_index];

		std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(p_idx));
		new_particle->set_type(p_type);

		if(_masses.find(p_type) != _masses.end()) {
			new_particle->set_mass(_masses[p_type]);
		}

		if(_charge_index != -1) {
			double charge = utils::lexical_cast<double>(split[_charge_index]);
			new_particle->set_charge(charge);
		}

		try {
			vec3 pos(
					utils::lexical_cast<double>(split[_pos_starting_index]),
					utils::lexical_cast<double>(split[_pos_starting_index + 1]),
					utils::lexical_cast<double>(split[_pos_starting_index + 2]));
			new_particle->set_position(pos);
		}
		catch(utils::bad_lexical_cast &e) {
			std::string error = fmt::format("The position of the {}-th particle ({}, {}, {}) cannot be cast to a vector of floating-point numbers", i, split[2], split[3], split[4]);
			throw std::runtime_error(error);
		}

		syst->add_particle(new_particle);
	}
}

void LAMMPSDataFileParser::_parse_velocities(std::ifstream &configuration, std::shared_ptr<System> syst) {
	std::string line;
	for(uint i = 0; i < _header_data.N_atoms; i++) {
		line = _read_line(configuration);
		if(!configuration.good()) {
			std::string error = fmt::format("The line relative to the velocity of the {}-th particle cannot be read", i);
			throw std::runtime_error(error);
		}
		// skip empty lines
		if(line.size() == 0) {
			i--;
			continue;
		}

		auto split = utils::split(line);
		if(split.size() != 4) {
			std::string error = fmt::format("The line relative to the velocity of the {}-th particle is not valid", i);
			throw std::runtime_error(error);
		}

		try {
			int p_idx = utils::lexical_cast<int>(split[0]);

			vec3 vel(
					utils::lexical_cast<double>(split[1]),
					utils::lexical_cast<double>(split[2]),
					utils::lexical_cast<double>(split[3]));
			syst->particle_by_id(p_idx)->set_velocity(vel);
		}
		catch(utils::bad_lexical_cast &e) {
			std::string error = fmt::format("The velocity of the {}-th particle ({}, {}, {}) cannot be cast to a vector of floating-point numbers", i, split[1], split[2], split[3]);
			throw std::runtime_error(error);
		}
	}
}

double LAMMPSDataFileParser::_parse_box_line(std::vector<std::string> split_line) {
	try {
		double lower = utils::lexical_cast<double>(split_line[0]);
		double upper = utils::lexical_cast<double>(split_line[1]);
		return upper - lower;
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The box values '{} {}' found in the LAMMPS dump configuration are not valid", split_line[0], split_line[1]);
		throw std::runtime_error(error);
	}
}

std::string LAMMPSDataFileParser::_read_line(std::ifstream &configuration) {
	std::string line;
	std::getline(configuration, line);
	// remove possible comments and trim the resulting string
	return utils::trim_copy(utils::split(line, "#")[0]);
}

#ifdef PYTHON_BINDINGS

void export_LAMMPSDataFileParser(py::module &m) {
	py::class_<LAMMPSDataFileParser, BaseParser, std::shared_ptr<LAMMPSDataFileParser>> parser(m, "LAMMPSDataFileParser");

	parser.def(py::init<std::string>());
	parser.def(py::init<int, int>());
	parser.def(py::init<int, int, int>());

	parser.def_property_readonly("topology", &LAMMPSDataFileParser::topology);
}

#endif

} /* namespace ba */
