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

class GenericOxDNAParser: public BaseParser {
public:
	GenericOxDNAParser(std::string topology_file);
	virtual ~GenericOxDNAParser();

	virtual std::shared_ptr<System> parse(std::ifstream &configuration) override;

protected:
	std::string _topology_file;
};

#ifdef PYTHON_BINDINGS

void export_GenericOxDNAParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_GENERICOXDNAPARSER_H_ */
