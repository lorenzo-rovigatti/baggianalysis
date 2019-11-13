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
	GroParser(double dt);
	virtual ~GroParser();

	virtual std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override;

protected:
	double _dt;
};

#ifdef PYTHON_BINDINGS

void export_GroParser(py::module &m);

#endif


} /* namespace ba */

#endif /* PARSERS_GROPARSER_H_ */
