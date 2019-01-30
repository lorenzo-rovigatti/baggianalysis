/*
 * BaseParser.h
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_BASEPARSER_H_
#define PARSERS_BASEPARSER_H_

#include "../System.h"

#include <fstream>

namespace ba {

class BaseParser {
public:
	BaseParser();
	virtual ~BaseParser();

	virtual std::shared_ptr<System> parse(std::ifstream &configuration) = 0;
};

} /* namespace ba */

#endif /* PARSERS_BASEPARSER_H_ */
