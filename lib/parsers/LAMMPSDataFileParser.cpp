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
				BaseParser(),
				_atom_style(atom_style) {

	_LAMMPS_section_keywords = std::vector<std::string>( {
			"Atoms", "Velocities", "Masses", "Ellipsoids", "Lines", "Triangles", "Bodies",
			"Bonds", "Angles", "Dihedrals", "Impropers",
			"Pair Coeffs", "PairIJ Coeffs", "Bond Coeffs", "Angle Coeffs", "Dihedral Coeffs", "Improper Coeffs",
			"BondBond Coeffs", "BondAngle Coeffs", "MiddleBondTorsion Coeffs", "EndBondTorsion Coeffs",
			"AngleTorsion Coeffs", "AngleAngleTorsion Coeffs", "BondBond13 Coeffs", "AngleAngle Coeffs"
	});

	if(_atom_style == "bond") {
		_type_index = 2;
		_pos_starting_index = 3;
	}
	else if(_atom_style == "atomic") {
		_type_index = 1;
		_pos_starting_index = 2;
	}
	else {
		std::string error = fmt::format("Unsupported LAMMPS atom_style '{}'", atom_style);
		throw std::runtime_error(error);
	}
}

LAMMPSDataFileParser::~LAMMPSDataFileParser() {

}

std::shared_ptr<System> LAMMPSDataFileParser::_parse_stream(std::ifstream &configuration) {
	std::shared_ptr<System> syst(std::make_shared<System>());

	auto header_data = _parse_headers(configuration);

	if(header_data.empty) {
		return nullptr;
	}

	// there is time information in a LAMMPS data file
	syst->time = 0;
	syst->box = header_data.box;

	std::string line;
	do {
		line = _read_line(configuration);
	} while(line != "Atoms");

	for(uint i = 0; i < header_data.N_atoms; i++) {
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

		try {
			vec3 pos(
					utils::lexical_cast<double>(split[_pos_starting_index]),
					utils::lexical_cast<double>(split[_pos_starting_index + 1]),
					utils::lexical_cast<double>(split[_pos_starting_index + 2]));
			new_particle->set_position(pos);
		}
		catch(boost::bad_lexical_cast &e) {
			std::string error = fmt::format("The position of the {}-th particle ({}, {}, {}) cannot be cast to a vector of floating-point numbers", i, split[2], split[3], split[4]);
			throw std::runtime_error(error);
		}

		syst->add_particle(new_particle);
	}

	syst->sort_particles_by_id();

	return syst;
}

LAMMPSDataFileParser::HeaderData LAMMPSDataFileParser::_parse_headers(std::ifstream &configuration) {
	LAMMPSDataFileParser::HeaderData hd;

	// this list (taken from LAMMPS docs) may turn out to be useful in the future
	std::vector<std::string> header_entries = {
			"atoms", "bonds", "angles", "dihedrals", "impropers", "atom types", "bond types", "angle types",
			"dihedral types", "improper types", "extra bond per atom", "extra angle per atom", "extra dihedral per atom",
			"extra improper per atom", "extra special per atom", "ellipsoids", "lines", "triangles", "bodies",
			"xlo xhi", "ylo yhi", "zlo zhi", "xy xz yz"
	};

	std::string line;
	// according to the docs, the first line of the header should be always skipped
	std::getline(configuration, line);

	bool done = false;
	while(!done) {
		line = _read_line(configuration);

		if(!configuration.good() || std::find(_LAMMPS_section_keywords.begin(), _LAMMPS_section_keywords.end(), line) != _LAMMPS_section_keywords.end()) {
			done = true;
		}
		else {
			if(line.size() > 0) {
				auto split = utils::split(line);
				if(boost::ends_with(line, "atoms")) {
					hd.N_atoms = utils::lexical_cast<int>(split[0]);
				}
				else if(boost::ends_with(line, "bonds")) {
					hd.N_bonds = utils::lexical_cast<int>(split[0]);
				}
				else if(boost::ends_with(line, "atom types")) {
					hd.atom_types = utils::lexical_cast<int>(split[0]);
				}
				else if(boost::ends_with(line, "bond types")) {
					hd.bond_types = utils::lexical_cast<int>(split[0]);
				}
				else if(boost::ends_with(line, "xlo xhi")) {
					hd.box[0] = _parse_box_line(split);
				}
				else if(boost::ends_with(line, "ylo yhi")) {
					hd.box[1] = _parse_box_line(split);
				}
				else if(boost::ends_with(line, "zlo zhi")) {
					hd.box[2] = _parse_box_line(split);
				}
			}
		}
	}

	return hd;
}

double LAMMPSDataFileParser::_parse_box_line(std::vector<std::string> split_line) {
	try {
		double lower = utils::lexical_cast<double>(split_line[0]);
		double upper = utils::lexical_cast<double>(split_line[1]);
		return upper - lower;
	}
	catch(boost::bad_lexical_cast &e) {
		std::string error = fmt::format("The box values '{} {}' found in the LAMMPS dump configuration are not valid", split_line[0], split_line[1]);
		throw std::runtime_error(error);
	}
}

std::string LAMMPSDataFileParser::_read_line(std::ifstream &configuration) {
	std::string line;
	std::getline(configuration, line);
	// remove possible comments and trim the resulting string
	return utils::trim(utils::split(line, "#")[0]);
}

#ifdef PYTHON_BINDINGS

void export_LAMMPSDataFileParser(py::module &m) {
	py::class_<LAMMPSDataFileParser, BaseParser, std::shared_ptr<LAMMPSDataFileParser>> parser(m, "LAMMPSDataFileParser");

	parser.def(py::init<std::string>());
}

#endif

} /* namespace ba */
