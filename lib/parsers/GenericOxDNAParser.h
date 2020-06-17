/*
 * OxDNAParser.h
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_GENERICOXDNAPARSER_H_
#define PARSERS_GENERICOXDNAPARSER_H_

#include "BaseParser.h"

#include <fstream>

namespace ba {

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

class GenericOxDNAParser: public BaseParser {
public:
	GenericOxDNAParser(std::string topology_file);
	GenericOxDNAParser(std::shared_ptr<OxDNATopologyParser> topology_parser);
	virtual ~GenericOxDNAParser();

	virtual std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override;

protected:
	std::shared_ptr<OxDNATopologyParser> _topology_parser;
};

#ifdef PYTHON_BINDINGS

void export_GenericOxDNAParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_GENERICOXDNAPARSER_H_ */
