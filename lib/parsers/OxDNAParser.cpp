/*
 * OxDNAParser.cpp
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#include "OxDNAParser.h"

#include "oxDNA_topology_parsers/TSP.h"
#include "../utils/strings.h"

namespace ba {

OxDNAParser::OxDNAParser(std::string topology_file) :
				BaseParser() {
	_topology_parser = std::make_shared<oxDNA_topology::Default>(topology_file);
}

OxDNAParser::OxDNAParser(std::shared_ptr<oxDNA_topology::Default> topology_parser) {
	_topology_parser = topology_parser;
}

OxDNAParser::~OxDNAParser() {

}

std::shared_ptr<System> OxDNAParser::_parse_stream(std::ifstream &configuration) {
	std::string line;

	// timestep line
	std::getline(configuration, line);
	if(!configuration.good()) {
		return nullptr;
	}

	std::shared_ptr<System> syst(std::make_shared<System>());

	auto split = utils::split(line);
	try {
		syst->time = utils::lexical_cast<ullint>(split[2]);
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The timestep value '{}' found in the oxDNA configuration cannot be cast to an integer", split[2]);
		throw std::runtime_error(error);
	}

	// box line
	std::getline(configuration, line);
	split = utils::split(line);
	try {
		syst->box[0] = utils::lexical_cast<double>(split[2]);
		syst->box[1] = utils::lexical_cast<double>(split[3]);
		syst->box[2] = utils::lexical_cast<double>(split[4]);
	}
	catch(utils::bad_lexical_cast &e) {
		std::string error = fmt::format("The box line '{}' found in the oxDNA configuration is not valid", line);
		throw std::runtime_error(error);
	}

	// energy line
	std::getline(configuration, line);

	bool end = false;
	int current_index = 0;
	while(configuration.good() && !end) {
		if(configuration.peek() == 't') {
			end = true;
		}
		else {
			std::getline(configuration, line);
			if(configuration.good() && line.size() > 0) {
				split = utils::split(line);
				vec3 position = vec3(utils::lexical_cast<double>(split[0]),
						utils::lexical_cast<double>(split[1]),
						utils::lexical_cast<double>(split[2]));
				vec3 velocity = vec3(utils::lexical_cast<double>(split[9]),
						utils::lexical_cast<double>(split[10]),
						utils::lexical_cast<double>(split[11]));

				particle_type p_type = _topology_parser->type(current_index);
				std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(current_index, p_type, position, velocity));
				syst->add_particle(new_particle);
				current_index++;
			}
		}
	}

	if(syst->N() != _topology_parser->N()) {
		std::string error = fmt::format("The number of particles found in the configuration file ({}) is different from what specified in the topology file ({})", syst->N(), _topology_parser->N());
		throw std::runtime_error(error);
	}

	return syst;
}

#ifdef PYTHON_BINDINGS

void export_OxDNAParser(py::module &m) {
	py::module sub_m = m.def_submodule("oxDNA_topology");

	oxDNA_topology::export_Default(sub_m);
	oxDNA_topology::export_TSP(sub_m);

	py::class_<OxDNAParser, BaseParser, std::shared_ptr<OxDNAParser>> parser(m, "OxDNAParser");

	parser.def(py::init<std::string>());
	parser.def(py::init<std::shared_ptr<oxDNA_topology::Default>>());
}

#endif

} /* namespace ba */
