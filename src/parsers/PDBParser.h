/*
 * PDBParser.h
 *
 *  Created on: 14 apr 2026
 *      Author: GitHub Copilot
 */

#ifndef PARSERS_PDBPARSER_H_
#define PARSERS_PDBPARSER_H_

#include "BaseParser.h"

namespace ba {

class PDBParser: public BaseParser {
public:
	PDBParser();
	virtual ~PDBParser();

	virtual std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override;
};

void export_PDBParser(py::module &m);

} /* namespace ba */

#endif /* PARSERS_PDBPARSER_H_ */