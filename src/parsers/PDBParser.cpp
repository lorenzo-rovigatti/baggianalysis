/*
 * PDBParser.cpp
 *
 *  Created on: 14 apr 2026
 *      Author: GitHub Copilot
 */

#include "PDBParser.h"

#include "../utils/strings.h"

#include <cctype>
#include <unordered_map>

namespace {

std::string _normalise_element_symbol(const std::string &raw_element, const std::string &atom_name) {
	std::string symbol = raw_element;
	if(symbol.size() == 0) {
		symbol = atom_name;
	}

	std::string letters_only;
	letters_only.reserve(symbol.size());
	for(auto c : symbol) {
		if(std::isalpha(static_cast<unsigned char>(c))) {
			letters_only.push_back(c);
		}
	}

	if(letters_only.size() == 0) {
		return "";
	}

	if(raw_element.size() == 0) {
		letters_only = letters_only.substr(0, 1);
	}
	else if(letters_only.size() > 2) {
		letters_only = letters_only.substr(0, 2);
	}

	letters_only[0] = std::toupper(static_cast<unsigned char>(letters_only[0]));
	for(std::size_t i = 1; i < letters_only.size(); i++) {
		letters_only[i] = std::tolower(static_cast<unsigned char>(letters_only[i]));
	}

	return letters_only;
}

double _sans_b_factor_from_element(const std::string &element) {
	static const std::unordered_map<std::string, double> sans_b_factors = {
		{ "H", -3.7390 },
		{ "D", 6.6710 },
		{ "C", 6.6460 },
		{ "N", 9.3600 },
		{ "O", 5.8030 },
		{ "P", 5.1300 },
		{ "S", 2.8470 },
		{ "Na", 3.6300 },
		{ "Mg", 5.3750 },
		{ "Cl", 9.5770 },
		{ "K", 3.6700 },
		{ "Ca", 4.7000 },
		{ "Fe", 9.4500 },
		{ "Zn", 5.6800 }
	};

	auto it = sans_b_factors.find(element);
	if(it == sans_b_factors.end()) {
		return 1.;
	}

	return it->second;
}

}

namespace ba {

PDBParser::PDBParser() :
				BaseParser() {

}

PDBParser::~PDBParser() {

}

std::shared_ptr<System> PDBParser::_parse_stream(std::ifstream &configuration) {
	std::shared_ptr<System> syst(std::make_shared<System>());

	std::string line;
	bool started = false;
	uint atom_count = 0;

	while(std::getline(configuration, line)) {
		std::string record = (line.size() >= 6) ? utils::trim_copy(line.substr(0, 6)) : utils::trim_copy(line);

		if(record == "MODEL") {
			started = true;
			if(line.size() >= 14) {
				std::string model_serial = utils::trim_copy(line.substr(10, 4));
				if(model_serial.size() > 0) {
					try {
						syst->time = utils::lexical_cast<ullint>(model_serial);
					}
					catch(utils::bad_lexical_cast &) {
						std::string error = fmt::format("The MODEL serial '{}' found in the PDB configuration cannot be cast to an integer", model_serial);
						throw std::runtime_error(error);
					}
				}
			}

			continue;
		}

		if(record == "CRYST1") {
			started = true;
			if(line.size() < 33) {
				std::string error = fmt::format("The CRYST1 line '{}' found in the PDB configuration is too short", line);
				throw std::runtime_error(error);
			}

			std::string a = utils::trim_copy(line.substr(6, 9));
			std::string b = utils::trim_copy(line.substr(15, 9));
			std::string c = utils::trim_copy(line.substr(24, 9));

			try {
				syst->box[0] = utils::lexical_cast<double>(a);
				syst->box[1] = utils::lexical_cast<double>(b);
				syst->box[2] = utils::lexical_cast<double>(c);
			}
			catch(utils::bad_lexical_cast &) {
				std::string error = fmt::format("The CRYST1 line '{}' found in the PDB configuration is not valid", line);
				throw std::runtime_error(error);
			}

			continue;
		}

		if(record == "ATOM" || record == "HETATM") {
			started = true;
			if(line.size() < 54) {
				std::string error = fmt::format("The line relative to the {}-th particle is too short to contain coordinates: '{}'", atom_count, line);
				throw std::runtime_error(error);
			}

			std::string atom_serial = utils::trim_copy(line.substr(6, 5));
			std::string atom_name = utils::trim_copy(line.substr(12, 4));
			std::string element = (line.size() >= 78) ? utils::trim_copy(line.substr(76, 2)) : "";
			std::string inferred_element = _normalise_element_symbol(element, atom_name);

			int p_idx;
			try {
				p_idx = utils::lexical_cast<int>(atom_serial);
			}
			catch(utils::bad_lexical_cast &) {
				std::string error = fmt::format("The serial '{}' of the {}-th particle found in the PDB configuration cannot be cast to an integer", atom_serial, atom_count);
				throw std::runtime_error(error);
			}

			std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(p_idx));
			if(element.size() > 0) {
				new_particle->set_type(element);
			}
			else {
				new_particle->set_type(atom_name);
			}
			new_particle->set_b_factor(_sans_b_factor_from_element(inferred_element));

			std::string x = utils::trim_copy(line.substr(30, 8));
			std::string y = utils::trim_copy(line.substr(38, 8));
			std::string z = utils::trim_copy(line.substr(46, 8));

			try {
				new_particle->set_position(vec3(utils::lexical_cast<double>(x), utils::lexical_cast<double>(y), utils::lexical_cast<double>(z)));
			}
			catch(utils::bad_lexical_cast &) {
				std::string error = fmt::format("The position of the {}-th particle ({}, {}, {}) cannot be cast to a vector of floating-point numbers", atom_count, x, y, z);
				throw std::runtime_error(error);
			}

			new_particle->set_velocity(vec3(0., 0., 0.));
			syst->add_particle(new_particle);
			atom_count++;

			continue;
		}

		if(record == "ENDMDL" || record == "END") {
			if(started) {
				return syst;
			}
		}
	}

	if(started) {
		return syst;
	}

	return nullptr;
}

void export_PDBParser(py::module &m) {
	py::class_<PDBParser, BaseParser, std::shared_ptr<PDBParser>> parser(m, "PDBParser", R"pbdoc(
Parse PDB files by reading ATOM/HETATM entries and optional CRYST1 box data.

If MODEL records are present, the parser reads one model at a time and uses the MODEL serial as the frame time.
)pbdoc");

	parser.def(py::init<>());
}

} /* namespace ba */