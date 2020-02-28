/*
 * LAMMPSDumpParser.cpp
 *
 *  Created on: May 26, 2019
 *      Author: lorenzo
 */

#include "LAMMPSDumpParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace ba {

LAMMPSDumpParser::LAMMPSDumpParser(bool rescaled_coords) :
				BaseParser(),
				_rescaled_coords(rescaled_coords) {

}

LAMMPSDumpParser::~LAMMPSDumpParser() {

}

std::shared_ptr<System> LAMMPSDumpParser::_parse_stream(std::ifstream &configuration) {
	std::shared_ptr<System> syst(std::make_shared<System>());

	auto header_data = _parse_headers(configuration, syst);

	if(header_data.empty) {
		return nullptr;
	}

	syst->time = header_data.time_step;
	syst->box = header_data.box;

	for(uint i = 0; i < header_data.N; i++) {
		std::string line;
		std::getline(configuration, line);
		if(!configuration.good()) {
			std::string error = boost::str(boost::format("The line relative to the %u-th particle cannot be read") % i);
			throw std::runtime_error(error);
		}

		std::string to_split = boost::trim_copy(line);
		std::vector<std::string> split;
		boost::split(split, to_split, boost::is_any_of(" "), boost::algorithm::token_compress_on);

		std::shared_ptr<Particle> new_particle(std::make_shared<Particle>());
		new_particle->set_type(split[1]);

		try {
			vec3 pos(boost::lexical_cast<double>(split[2]), boost::lexical_cast<double>(split[3]), boost::lexical_cast<double>(split[4]));
			if(_rescaled_coords) {
				pos *= header_data.box;
			}
			new_particle->set_position(pos);
		}
		catch(boost::bad_lexical_cast &e) {
			std::string error = boost::str(boost::format("The position of the %u-th particle (%s, %s, %s) cannot be cast to a vector of floating-point numbers") % i % split[2] % split[3] % split[4]);
			throw std::runtime_error(error);
		}

		syst->add_particle(new_particle);
	}

	return syst;
}

LAMMPSDumpParser::HeaderData LAMMPSDumpParser::_parse_headers(std::ifstream &configuration, std::shared_ptr<System> syst) {
	LAMMPSDumpParser::HeaderData hd;

	bool done = false;
	while(!done) {
		std::string line;
		std::getline(configuration, line);
		if(!configuration.good()) {
			hd.empty = true;
			// early return
			return hd;
		}
		if(boost::starts_with(line, "ITEM:")) {
			if(boost::contains(line, "TIMESTEP")) {
				std::getline(configuration, line);
				hd.time_step = boost::lexical_cast<ullint>(boost::trim_copy(line));
			}
			else if(boost::contains(line, "NUMBER OF ATOMS")) {
				std::getline(configuration, line);
				try {
					hd.N = boost::lexical_cast<uint>(boost::trim_copy(line));
				}
				catch(boost::bad_lexical_cast &e) {
					std::string error = boost::str(boost::format("The number of particles '%s' found in the LAMMPS dump configuration cannot be cast to an integer") % line);
					throw std::runtime_error(error);
				}
			}
			else if(boost::contains(line, "BOX BOUNDS")) {
				// the next three lines contains the box dimensions along the three axes
				for(uint i = 0; i < 3; i++) {
					std::getline(configuration, line);
					std::string to_split = boost::trim_copy(line);
					std::vector<std::string> split;
					boost::split(split, to_split, boost::is_any_of(" "), boost::algorithm::token_compress_on);
					try {
						double lower = boost::lexical_cast<double>(boost::trim_copy(split[0]));
						double upper = boost::lexical_cast<double>(boost::trim_copy(split[1]));
						hd.box[i] = upper - lower;
					}
					catch(boost::bad_lexical_cast &e) {
						std::string error = boost::str(boost::format("The box line '%s' found in the LAMMPS dump configuration is not valid") % line);
						throw std::runtime_error(error);
					}
				}
			}
			else if(boost::contains(line, "ITEM: ATOMS")) {
				done = true;
			}
		}
	}

	return hd;
}

#ifdef PYTHON_BINDINGS

void export_LAMMPSDumpParser(py::module &m) {
	py::class_<LAMMPSDumpParser, BaseParser, std::shared_ptr<LAMMPSDumpParser>> parser(m, "LAMMPSDumpParser");

	parser.def(py::init<bool>());
}

#endif

} /* namespace ba */
