/*
 * GroParser.cpp
 *
 *  Created on: 31 gen 2019
 *      Author: lorenzo
 */

#include "GroParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace ba {

GroParser::GroParser(double dt) :
				BaseParser(),
				_dt(dt) {

}

GroParser::~GroParser() {

}

std::shared_ptr<System> GroParser::parse(std::ifstream &configuration) {
	std::string line;
	std::vector<std::string> split;

	// timestep line
	std::getline(configuration, line);
	if(!configuration.good()) return nullptr;

	std::shared_ptr<System> syst(new System);

	auto time_pos = line.find("t=");
	if(time_pos == std::string::npos) {
		std::string error = boost::str(boost::format("Malformed first line '%s' (cannot find 't=')") % line);
		throw std::runtime_error(error);
	}

	try {
		std::string time_string = boost::trim_copy(line.substr(time_pos + 2));
		double time_double = boost::lexical_cast<double>(time_string);
		syst->time = std::round(time_double / _dt);
	}
	catch(boost::bad_lexical_cast &e) {
		std::string error = boost::str(boost::format("The timestep '%s' found in the .gro configuration cannot be cast to a double") % split[2]);
		throw std::runtime_error(error);
	}

	// number of particles line
	std::getline(configuration, line);
	uint N;
	try {
		N =  boost::lexical_cast<uint>(boost::trim_copy(line));
	}
	catch(boost::bad_lexical_cast &e) {
		std::string error = boost::str(boost::format("The number of particles '%s' found in the .gro configuration cannot be cast to an integer") % split[2]);
		throw std::runtime_error(error);
	}

	std::vector<std::string> inserted_name;
	for(uint i = 0; i < N; i++) {
		std::getline(configuration, line);

		// taken by http://manual.gromacs.org/archive/5.0.3/online/gro.html
		std::string residue_number = line.substr(0, 5);
		std::string residue_name = line.substr(5, 5);
		std::string atom_name = line.substr(10, 5);
		std::string atom_number = line.substr(15, 5);

		std::shared_ptr<Particle> new_particle(new Particle());
		new_particle->set_type(atom_name);

		std::string x = boost::trim_copy(line.substr(20, 8));
		std::string y = boost::trim_copy(line.substr(28, 8));
		std::string z = boost::trim_copy(line.substr(36, 8));

		try {
			new_particle->set_position(vec3(boost::lexical_cast<double>(x), boost::lexical_cast<double>(y), boost::lexical_cast<double>(z)));
		}
		catch(boost::bad_lexical_cast &e) {
			std::string error = boost::str(boost::format("The position of the %u-th particle (%s, %s, %s) cannot be cast to a vector of floating-point numbers") % i % x % y % z);
			throw std::runtime_error(error);
		}

		std::string vx = boost::trim_copy(line.substr(44, 8));
		std::string vy = boost::trim_copy(line.substr(52, 8));
		std::string vz = boost::trim_copy(line.substr(60, 8));

		try {
			new_particle->set_velocity(vec3(boost::lexical_cast<double>(vx), boost::lexical_cast<double>(vy), boost::lexical_cast<double>(vz)));
		}
		catch(boost::bad_lexical_cast &e) {
			std::string error = boost::str(boost::format("The velocity of the %u-th particle (%s, %s, %s) cannot be cast to a vector of floating-point numbers") % i % vx % vy % vz);
			throw std::runtime_error(error);
		}

		syst->add_particle(new_particle);
	}

	// box line
	std::getline(configuration, line);
	std::string to_split = boost::trim_copy(line);
	boost::split(split, to_split, boost::is_any_of(" "), boost::algorithm::token_compress_on);
	try {
		syst->box[0] = boost::lexical_cast<double>(boost::trim_copy(split[0]));
		syst->box[1] = boost::lexical_cast<double>(boost::trim_copy(split[1]));
		syst->box[2] = boost::lexical_cast<double>(boost::trim_copy(split[2]));
	}
	catch(boost::bad_lexical_cast &e) {
		std::string error = boost::str(boost::format("The box line '%s' found in the .gro configuration is not valid") % line);
		throw std::runtime_error(error);
	}

	return syst;
}

#ifdef PYTHON_BINDINGS

void export_GroParser(py::module &m) {
	pybind11::class_<GroParser, BaseParser, std::shared_ptr<GroParser>> parser(m, "GroParser");

	parser
		.def(py::init<double>())
		.def("parse", &GroParser::parse);
}

#endif

} /* namespace ba */
