/*
 * OxDNAParser.cpp
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#include "GenericOxDNAParser.h"

#include "../utils/strings.h"

namespace ba {

OxDNATopologyParser::OxDNATopologyParser(std::string topology_file) {
	std::ifstream topology(topology_file);

	if(!topology.good()) {
		std::string error = boost::str(boost::format("Topology file '%s' not found") % topology_file);
		throw std::runtime_error(error);
	}

	topology >> _N;
	topology >> _N_A;
	if(!topology.good()) {
		_N_A = 0;
	}

	topology.close();
}

particle_type OxDNATopologyParser::type(int p_idx) {
	if(_N_A == 0 || (uint) p_idx < _N_A) {
		return "0";
	}
	return "1";
}

GenericOxDNAParser::GenericOxDNAParser(std::string topology_file) :
				BaseParser() {
	_topology_parser = std::make_shared<OxDNATopologyParser>(topology_file);
}

GenericOxDNAParser::GenericOxDNAParser(std::shared_ptr<OxDNATopologyParser> topology_parser) {
	_topology_parser = topology_parser;
}

GenericOxDNAParser::~GenericOxDNAParser() {

}

std::shared_ptr<System> GenericOxDNAParser::_parse_stream(std::ifstream &configuration) {
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
	catch(boost::bad_lexical_cast &e) {
		std::string error = boost::str(boost::format("The timestep value '%s' found in the oxDNA configuration cannot be cast to an integer") % split[2]);
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
	catch(boost::bad_lexical_cast &e) {
		std::string error = boost::str(boost::format("The box line '%s' found in the oxDNA configuration is not valid") % line);
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
		std::string error = boost::str(boost::format("The number of particles found in the configuration file (%d) is different from what specified in the topology file (%d)") % syst->N() % _topology_parser->N());
		throw std::runtime_error(error);
	}

	return syst;
}

#ifdef PYTHON_BINDINGS

void export_GenericOxDNAParser(py::module &m) {
	py::class_<OxDNATopologyParser, std::shared_ptr<OxDNATopologyParser>> top_parser(m, "OxDNATopologyParser");

	top_parser.def(py::init<std::string>());

	py::class_<GenericOxDNAParser, BaseParser, std::shared_ptr<GenericOxDNAParser>> parser(m, "GenericOxDNAParser");

	parser.def(py::init<std::string>());
	parser.def(py::init<std::shared_ptr<OxDNATopologyParser>>());
}

#endif

} /* namespace ba */
