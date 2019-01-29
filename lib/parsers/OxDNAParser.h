/*
 * OxDNAParser.h
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_OXDNAPARSER_H_
#define PARSERS_OXDNAPARSER_H_

#include "../System.h"

#include <fstream>

namespace ba {

class OxDNAParser {
public:
	OxDNAParser() = delete;
	virtual ~OxDNAParser() = delete;

	static bool parse(std::shared_ptr<System> syst, std::ifstream &topology, std::ifstream &configuration);
};

} /* namespace ba */

#endif /* PARSERS_OXDNAPARSER_H_ */
