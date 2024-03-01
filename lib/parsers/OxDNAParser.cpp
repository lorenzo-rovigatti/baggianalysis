/*
 * OxDNAParser.cpp
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#include "OxDNAParser.h"

#include "../topology/Topology.h"
#include "oxDNA_topology_parsers/NoTopology.h"
#include "oxDNA_topology_parsers/TSP.h"
#include "oxDNA_topology_parsers/DetailedPolymer.h"
#include "oxDNA_topology_parsers/DNA.h"
#include "../utils/strings.h"

#include <glm/gtx/orthonormalize.hpp>

namespace ba {

OxDNAParser::OxDNAParser() :
				BaseParser() {
	_topology_parser = std::make_shared<oxDNA_topology::NoTopology>();
}

OxDNAParser::OxDNAParser(std::string topology_file) :
				BaseParser() {
	_topology_parser = std::make_shared<oxDNA_topology::Default>(topology_file);
}

OxDNAParser::OxDNAParser(std::shared_ptr<oxDNA_topology::Default> topology_parser) {
	_topology_parser = topology_parser;
}

OxDNAParser::~OxDNAParser() {

}

void OxDNAParser::set_orientation_inserter(OxDNAOrientationInserter inserter) {
	_orientation_inserter = inserter;
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
			utils::trim(line);
			if(configuration.good() && line.size() > 0) {
				split = utils::split(line);
				vec3 position, velocity, angular_velocity;
				glm::dmat3 orientation_matrix;

				try {
					position = vec3(utils::lexical_cast<double>(split[0]), utils::lexical_cast<double>(split[1]), utils::lexical_cast<double>(split[2]));
					if(split.size() > 9) {
						velocity = vec3(utils::lexical_cast<double>(split[9]), utils::lexical_cast<double>(split[10]), utils::lexical_cast<double>(split[11]));
						angular_velocity = vec3(utils::lexical_cast<double>(split[12]), utils::lexical_cast<double>(split[13]), utils::lexical_cast<double>(split[14]));
					}

					vec3 a1(utils::lexical_cast<double>(split[3]), utils::lexical_cast<double>(split[4]), utils::lexical_cast<double>(split[5]));
					vec3 a3(utils::lexical_cast<double>(split[6]), utils::lexical_cast<double>(split[7]), utils::lexical_cast<double>(split[8]));

					// build the orientation matrix
					glm::normalize(a1);
					glm::normalize(a3);
					vec3 a2 = glm::cross(a3, a1);

					orientation_matrix = glm::dmat3(a1, a2, a3);
					glm::orthonormalize(orientation_matrix);
				}
				catch(utils::bad_lexical_cast &e) {
					std::string error = fmt::format("The box line '{}' found in the configuration is not valid", line);
					throw std::runtime_error(error);
				}

				particle_type p_type = _topology_parser->type(current_index);
				std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(current_index, p_type, position, velocity));
				_orientation_inserter(new_particle, orientation_matrix);
				new_particle->set_angular_velocity(angular_velocity);
				syst->add_particle(new_particle);
				current_index++;
			}
		}
	}

	if(_topology_parser->has_N() && syst->N() != _topology_parser->N()) {
		std::string error = fmt::format("The number of particles found in the configuration file ({}) is different from what specified in the topology file ({})", syst->N(), _topology_parser->N());
		throw std::runtime_error(error);
	}

	auto topology = Topology::make_empty_topology();
	auto bonded_neighbours = _topology_parser->bonded_neighbours();
	if(bonded_neighbours.size() == syst->N()) {
		for(auto p : syst->particles()) {
			for(auto neigh : bonded_neighbours[p->index()]) {
				topology->add_bond(p->index(), neigh);
			}
		}
	}

	topology->apply(syst.get());

	return syst;
}

#ifdef PYTHON_BINDINGS

void export_OxDNAParser(py::module &m) {
	py::module sub_m = m.def_submodule("oxDNA_topology");

	oxDNA_topology::export_Default(sub_m);
	oxDNA_topology::export_TSP(sub_m);
	oxDNA_topology::export_NoTopology(sub_m);
	oxDNA_topology::export_DetailedPolymer(sub_m);
	oxDNA_topology::export_DNA(sub_m);

	py::class_<OxDNAParser, BaseParser, std::shared_ptr<OxDNAParser>> parser(m, "OxDNAParser", "Use oxDNA/topology files to build systems.");

	parser.def(py::init<>(), R"pbdoc(
Initialise the parser without any topology file.
)pbdoc");

	parser.def(py::init<std::string>(), py::arg("topology_file"), R"pbdoc(
Initialise the parser by using the :class:`~baggianalysis.core.oxDNA_topology.Default` topology parser to parse the given topology file.

Parameters
----------
    topology_file: str
        The topology file to be used for initialisation.
)pbdoc");

	parser.def(py::init<std::shared_ptr<oxDNA_topology::Default>>(), py::arg("topology_parser"), R"pbdoc(
Initialise the parser by using the given topology parser.

Parameters
----------
	topology:
		An oxDNA topology parser, which should be an instance of an object inheriting from the :class:`~baggianalysis.core.oxDNA_topology.Default` topology parser.
)pbdoc");

	parser.def("set_orientation_inserter", &OxDNAParser::set_orientation_inserter, py::arg("inserter"), R"pbdoc(
Set the callable that will be used to initialise the orientation vectors of each particle. The following example will correctly initialise two-patch particles out
of oxDNA configurations::

    P1 = np.array([0., 0.5, 0.])
    P2 = np.array([0., -0.5, 0.])
    def add_patches(p, orientation_matrix):
        p.add_orientation_vector(orientation_matrix @ P1)
        p.add_orientation_vector(orientation_matrix @ P2)

    oxDNA_parser.set_orientation_inserter(add_patches)

Parameters
----------
    inserter:
        A callable that takes a particle and its orientation matrix and adds to the former the orientation vectors due to the latter.
)pbdoc");

	parser.def("topology_parser", &OxDNAParser::topology_parser, "Return the topology parser associated to this parser.");
}

#endif

} /* namespace ba */
