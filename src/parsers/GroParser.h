/*
 * GroParser.h
 *
 *  Created on: 31 gen 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_GROPARSER_H_
#define PARSERS_GROPARSER_H_

#include "BaseParser.h"

namespace ba {

class GroParser: public BaseParser {
public:
	GroParser();
	GroParser(double dt);
	virtual ~GroParser();

	virtual std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override;

protected:
	double _dt = 1.0; // default time step in fs
};

void export_GroParser(py::module &m);

} /* namespace ba */

#endif /* PARSERS_GROPARSER_H_ */
