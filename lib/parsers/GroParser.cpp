/*
 * GroParser.cpp
 *
 *  Created on: 31 gen 2019
 *      Author: lorenzo
 */

#include "GroParser.h"

#include "../utils/strings.h"

namespace ba {

GroParser::GroParser(double dt) :
				BaseParser(),
				_dt(dt) {

}

GroParser::~GroParser() {

}

std::shared_ptr<System> GroParser::_parse_stream(std::ifstream &configuration) {
	std::string line;

	// timestep line
	std::getline(configuration, line);
	if(!configuration.good()) return nullptr;

	std::shared_ptr<System> syst(std::make_shared<System>());

	auto time_pos = line.find("t=");
	if(time_pos == std::string::npos) {
		std::string error =fmt::format("Malformed first line '{}' (cannot find 't=')", line);
		throw std::runtime_error(error);
	}

	try {
		std::string time_string = utils::trim_copy(line.substr(time_pos + 2));
		double time_double = utils::lexical_cast<double>(time_string);
		syst->time = std::round(time_double / _dt);
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The timestep '{}' found in the .gro configuration cannot be cast to a double", line.substr(time_pos + 2));
		throw std::runtime_error(error);
	}

	// number of particles line
	std::getline(configuration, line);
	uint N;
	try {
		N = utils::lexical_cast<uint>(utils::trim_copy(line));
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The number of particles '{}' found in the .gro configuration cannot be cast to an integer", line);
		throw std::runtime_error(error);
	}

	std::vector<std::string> inserted_name;
	for(uint i = 0; i < N; i++) {
		std::getline(configuration, line);

		// taken by http://manual.gromacs.org/archive/5.0.3/online/gro.html
		std::string residue_number = utils::trim_copy(line.substr(0, 5));
		std::string residue_name = utils::trim_copy(line.substr(5, 5));
		std::string atom_name = utils::trim_copy(line.substr(10, 5));
		std::string atom_number = utils::trim_copy(line.substr(15, 5));

		std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(syst->available_index()));
		new_particle->set_type(atom_name);

		std::string x = utils::trim_copy(line.substr(20, 8));
		std::string y = utils::trim_copy(line.substr(28, 8));
		std::string z = utils::trim_copy(line.substr(36, 8));

		try {
			new_particle->set_position(vec3(utils::lexical_cast<double>(x), utils::lexical_cast<double>(y), utils::lexical_cast<double>(z)));
		}
		catch(utils::bad_lexical_cast &e) {
			std::string error = fmt::format("The position of the {}-th particle ({}, {}, {}) cannot be cast to a vector of floating-point numbers", i, x, y, z);
			throw std::runtime_error(error);
		}

		std::string vx = utils::trim_copy(line.substr(44, 8));
		std::string vy = utils::trim_copy(line.substr(52, 8));
		std::string vz = utils::trim_copy(line.substr(60, 8));

		try {
			new_particle->set_velocity(vec3(utils::lexical_cast<double>(vx), utils::lexical_cast<double>(vy), utils::lexical_cast<double>(vz)));
		}
		catch(utils::bad_lexical_cast &e) {
			std::string error = fmt::format("The velocity of the {}-th particle ({}, {}, {}) cannot be cast to a vector of floating-point numbers", i, vx, vy, vz);
			throw std::runtime_error(error);
		}

		syst->add_particle(new_particle);
	}

	// box line
	std::getline(configuration, line);
	std::string to_split = utils::trim_copy(line);
	auto split = utils::split(to_split);
	try {
		syst->box[0] = utils::lexical_cast<double>(split[0]);
		syst->box[1] = utils::lexical_cast<double>(split[1]);
		syst->box[2] = utils::lexical_cast<double>(split[2]);
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The box line '{}' found in the .gro configuration is not valid", line);
		throw std::runtime_error(error);
	}

	return syst;
}

#ifdef PYTHON_BINDINGS

void export_GroParser(py::module &m) {
	py::class_<GroParser, BaseParser, std::shared_ptr<GroParser>> parser(m, "GroParser");

	parser
		.def(py::init<double>());
}

#endif

} /* namespace ba */
