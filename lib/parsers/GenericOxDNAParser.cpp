/*
 * OxDNAParser.cpp
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#include "GenericOxDNAParser.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace ba {

GenericOxDNAParser::GenericOxDNAParser(std::string topology_file) :
				BaseParser(),
				_topology_file(topology_file) {

}

GenericOxDNAParser::~GenericOxDNAParser() {

}

std::shared_ptr<System> GenericOxDNAParser::parse(std::ifstream &configuration) {
	std::ifstream topology(_topology_file);

	if(!topology.good()) {
		std::string error = boost::str(boost::format("Topology file '%s' not found") % _topology_file);
		throw std::runtime_error(error);
	}

	uint N;
	topology >> N;

	topology.close();

	std::string line;
	std::vector<std::string> split;

	// timestep line
	getline(configuration, line);
	if(!configuration.good()) return nullptr;

	std::shared_ptr<System> syst(new System);

	boost::split(split, line, boost::is_any_of(" "));
	try {
		syst->time = boost::lexical_cast<ullint>(boost::trim_copy(split[2]));
	}
	catch(boost::bad_lexical_cast &e) {
		std::string error = boost::str(boost::format("The timestep value '%s' found in the oxDNA configuration cannot be cast to an integer") % split[2]);
		throw std::runtime_error(error);
	}

	// box line
	getline(configuration, line);
	boost::split(split, line, boost::is_any_of(" "));
	try {
		syst->box[0] = boost::lexical_cast<double>(boost::trim_copy(split[2]));
		syst->box[1] = boost::lexical_cast<double>(boost::trim_copy(split[3]));
		syst->box[2] = boost::lexical_cast<double>(boost::trim_copy(split[4]));
	}
	catch(boost::bad_lexical_cast &e) {
		std::string error = boost::str(boost::format("The box line '%s' found in the oxDNA configuration is not valid") % line);
		throw std::runtime_error(error);
	}

	// energy line
	getline(configuration, line);

	bool end = false;
	while(configuration.good() && !end) {
		if(configuration.peek() == 't') {
			end = true;
		}
		else {
			getline(configuration, line);
			if(configuration.good() && line.size() > 0) {
				boost::split(split, line, boost::is_any_of(" "));
				vec3 position = vec3(boost::lexical_cast<double>(split[0]), boost::lexical_cast<double>(split[1]), boost::lexical_cast<double>(split[2]));
				vec3 velocity = vec3(boost::lexical_cast<double>(split[9]), boost::lexical_cast<double>(split[10]), boost::lexical_cast<double>(split[11]));

				std::shared_ptr<Particle> new_particle(new Particle("0", position, velocity));
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
	pybind11::class_<GenericOxDNAParser, BaseParser, std::shared_ptr<GenericOxDNAParser>> parser(m, "GenericOxDNAParser");

	parser
		.def(py::init<std::string>())
		.def("parse", &GenericOxDNAParser::parse);
}

#endif

} /* namespace ba */
