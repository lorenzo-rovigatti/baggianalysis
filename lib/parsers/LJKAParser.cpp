/*
 * LJKAParser.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "LJKAParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace ba {

LJKAParser::LJKAParser() :
				BaseParser() {

}

LJKAParser::~LJKAParser() {

}

std::shared_ptr<System> LJKAParser::parse_stream(std::ifstream &configuration) {
	std::string line;
	std::vector<std::string> split;
	uint N, NA;

	// line containing the timestep and the number of particles
	std::getline(configuration, line);
	if(!configuration.good()) return nullptr;

	std::shared_ptr<System> syst(new System);

	boost::split(split, line, boost::is_any_of(" "));
	try {
		syst->time = boost::lexical_cast<ullint>(boost::trim_copy(split[0]));
		N = boost::lexical_cast<uint>(boost::trim_copy(split[2]));
		NA = boost::lexical_cast<uint>(boost::trim_copy(split[3]));
	}
	catch(boost::bad_lexical_cast &e) {
		std::string error = boost::str(boost::format("The timestep value '%s' found in the LJKA configuration cannot be cast to an integer") % split[2]);
		throw std::runtime_error(error);
	}

	// box line
	std::getline(configuration, line);
	std::string to_split = boost::trim_copy(line);
	boost::split(split, to_split, boost::is_any_of(" "));
	try {
		syst->box[0] = boost::lexical_cast<double>(boost::trim_copy(split[0]));
		syst->box[1] = boost::lexical_cast<double>(boost::trim_copy(split[1]));
		syst->box[2] = boost::lexical_cast<double>(boost::trim_copy(split[2]));
	}
	catch(boost::bad_lexical_cast &e) {
		std::string error = boost::str(boost::format("The box line '%s' found in the LJKA configuration is not valid") % line);
		throw std::runtime_error(error);
	}

	for(uint i = 0; i < N; i++) {
		std::getline(configuration, line);
		boost::split(split, line, boost::is_any_of(" "));
		vec3 position = vec3(boost::lexical_cast<double>(split[0]), boost::lexical_cast<double>(split[1]), boost::lexical_cast<double>(split[2]));

		particle_type type = (i < NA) ? "0" : "1";
		std::shared_ptr<Particle> new_particle(new Particle(type, position, vec3(0., 0., 0.)));
		syst->add_particle(new_particle);
	}

	return syst;
}

#ifdef PYTHON_BINDINGS

void export_LJKAParser(py::module &m) {
	py::class_<LJKAParser, BaseParser, std::shared_ptr<LJKAParser>> parser(m, "LJKAParser");

	parser
		.def(py::init<>())
		.def("parse", &LJKAParser::parse_stream);
}

#endif

} /* namespace ba */
