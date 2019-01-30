/*
 * OxDNAParser.h
 *
 *  Created on: 29 gen 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_OXDNAPARSER_H_
#define PARSERS_OXDNAPARSER_H_

#include "BaseParser.h"

#include <fstream>

namespace ba {

class OxDNAParser: public BaseParser {
public:
	OxDNAParser(std::string topology_file);
	virtual ~OxDNAParser();

	virtual std::shared_ptr<System> parse(std::ifstream &configuration) override;

protected:
	std::string _topology_file;
};

} /* namespace ba */

#endif /* PARSERS_OXDNAPARSER_H_ */
