/*
 * OxDNAParser.cpp
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#include "GenericOxDNAParser.h"

#include "../utils/strings.h"

namespace ba {

GenericOxDNAParser::GenericOxDNAParser(std::string topology_file) :
				BaseParser(),
				_topology_file(topology_file) {

}

GenericOxDNAParser::~GenericOxDNAParser() {

}

std::shared_ptr<System> GenericOxDNAParser::_parse_stream(std::ifstream &configuration) {
	std::ifstream topology(_topology_file);

	if(!topology.good()) {
		std::string error = boost::str(boost::format("Topology file '%s' not found") % _topology_file);
		throw std::runtime_error(error);
	}

	uint N;
	topology >> N;

	topology.close();

	std::string line;

	// timestep line
	std::getline(configuration, line);
	if(!configuration.good()) return nullptr;

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

				std::shared_ptr<Particle> new_particle(std::make_shared<Particle>("0", position, velocity));
				syst->add_particle(new_particle);
			}
		}
	}

	if(syst->N() != N) {
		std::string error = boost::str(boost::format("The number of particles found in the configuration file (%d) is different from what specified in the topology file (%d)") % syst->N() % N);
		throw std::runtime_error(error);
	}

	return syst;
}

#ifdef PYTHON_BINDINGS

void export_GenericOxDNAParser(py::module &m) {
	py::class_<GenericOxDNAParser, BaseParser, std::shared_ptr<GenericOxDNAParser>> parser(m, "GenericOxDNAParser");

	parser
		.def(py::init<std::string>());
}

#endif

} /* namespace ba */
