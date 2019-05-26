/*
 * LAMMPSDumpParser.h
 *
 *  Created on: May 26, 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_LAMMPSDUMPPARSER_H_
#define PARSERS_LAMMPSDUMPPARSER_H_

#include "BaseParser.h"

namespace ba {

class LAMMPSDumpParser: public BaseParser {
public:
	LAMMPSDumpParser();
	virtual ~LAMMPSDumpParser();

	virtual std::shared_ptr<System> parse(std::ifstream &configuration) override;

private:
	struct HeaderData {
		uint N = 0;
		vec3 box = vec3(0., 0., 0.);
		ullint time_step = 0;
		bool empty = false;
	};

	HeaderData _parse_headers(std::ifstream &configuration, std::shared_ptr<System> syst);
};

#ifdef PYTHON_BINDINGS

void export_LAMMPSDumpParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_LAMMPSDUMPPARSER_H_ */
