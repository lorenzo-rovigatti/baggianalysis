/*
 * LJKAParser.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "LJKAParser.h"

#include "../utils/strings.h"

namespace ba {

LJKAParser::LJKAParser() :
				BaseParser() {

}

LJKAParser::~LJKAParser() {

}

std::shared_ptr<System> LJKAParser::_parse_stream(std::ifstream &configuration) {
	std::string line;
	uint N, NA;

	// line containing the timestep and the number of particles
	std::getline(configuration, line);
	if(!configuration.good()) return nullptr;

	std::shared_ptr<System> syst(std::make_shared<System>());

	auto split = utils::split(line);
	try {
		syst->time = utils::lexical_cast<ullint>(utils::trim_copy(split[0]));
		N = utils::lexical_cast<uint>(utils::trim_copy(split[2]));
		NA = utils::lexical_cast<uint>(utils::trim_copy(split[3]));
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The timestep value '{}' found in the LJKA configuration cannot be cast to an integer", split[2]);
		throw std::runtime_error(error);
	}

	// box line
	std::getline(configuration, line);
	std::string to_split = utils::trim_copy(line);
	split = utils::split(to_split);
	try {
		syst->box[0] = utils::lexical_cast<double>(utils::trim_copy(split[0]));
		syst->box[1] = utils::lexical_cast<double>(utils::trim_copy(split[1]));
		syst->box[2] = utils::lexical_cast<double>(utils::trim_copy(split[2]));
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The box line '{}' found in the LJKA configuration is not valid", line);
		throw std::runtime_error(error);
	}

	for(uint i = 0; i < N; i++) {
		std::getline(configuration, line);
		split = utils::split(line);
		vec3 position = vec3(utils::lexical_cast<double>(split[0]), utils::lexical_cast<double>(split[1]), utils::lexical_cast<double>(split[2]));

		particle_type type = (i < NA) ? "0" : "1";
		std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(syst->available_index(), type, position, vec3(0., 0., 0.)));
		syst->add_particle(new_particle);
	}

	return syst;
}

#ifdef PYTHON_BINDINGS

void export_LJKAParser(py::module &m) {
	py::class_<LJKAParser, BaseParser, std::shared_ptr<LJKAParser>> parser(m, "LJKAParser");

	parser
		.def(py::init<>());
}

#endif

} /* namespace ba */
