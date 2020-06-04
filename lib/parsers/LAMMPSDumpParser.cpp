/*
 * LAMMPSDumpParser.cpp
 *
 *  Created on: May 26, 2019
 *      Author: lorenzo
 */

#include "LAMMPSDumpParser.h"

#include "../utils/strings.h"

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

		std::string to_split = utils::trim(line);
		auto split = utils::split(to_split);

		if(split.size() < 5) {
			std::string error = boost::str(boost::format("The LAMMPS dump file should contain at least 5 columns (%u found") % split.size());
			throw std::runtime_error(error);
		}

		int p_idx = utils::lexical_cast<int>(split[0]);
		std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(p_idx));
		new_particle->set_type(split[1]);

		try {
			vec3 pos(utils::lexical_cast<double>(split[2]),
					utils::lexical_cast<double>(split[3]),
					utils::lexical_cast<double>(split[4]));
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
				hd.time_step = utils::lexical_cast<ullint>(utils::trim(line));
			}
			else if(boost::contains(line, "NUMBER OF ATOMS")) {
				std::getline(configuration, line);
				try {
					hd.N = utils::lexical_cast<uint>(utils::trim(line));
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
					std::string to_split = utils::trim(line);
					auto split = utils::split(to_split);
					try {
						double lower = utils::lexical_cast<double>(split[0]);
						double upper = utils::lexical_cast<double>(split[1]);
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
