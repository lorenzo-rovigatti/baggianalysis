/*
 * OxDNAParser.h
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_OXDNAPARSER_H_
#define PARSERS_OXDNAPARSER_H_

#include "BaseParser.h"
#include "oxDNA_topology_parsers/Default.h"

#include <fstream>

namespace ba {

using OxDNAOrientationInserter = std::function<void(std::shared_ptr<Particle>, glm::dmat3 &)>;

class OxDNATopologyParser {
public:
	OxDNATopologyParser(std::string topology_file);
	virtual ~OxDNATopologyParser() {}

	uint N() {
		return _N;
	}

	virtual particle_type type(int);

private:
	uint _N = 0;
	uint _N_A = 0;
};

class OxDNAParser: public BaseParser {
public:
	OxDNAParser(std::string topology_file);
	OxDNAParser(std::shared_ptr<oxDNA_topology::Default> topology_parser);
	virtual ~OxDNAParser();

	void set_orientation_inserter(OxDNAOrientationInserter inserter);

	std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override;

protected:
	std::shared_ptr<oxDNA_topology::Default> _topology_parser;

	OxDNAOrientationInserter _orientation_inserter = [](std::shared_ptr<Particle> p, glm::dmat3 &m) {
		p->add_orientation_vector(m[0]);
		p->add_orientation_vector(m[1]);
		p->add_orientation_vector(m[2]);
	};
};

#ifdef PYTHON_BINDINGS

void export_OxDNAParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_OXDNAPARSER_H_ */
