/*
 * LJKAParser.h
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#ifndef PARSERS_LJKAPARSER_H_
#define PARSERS_LJKAPARSER_H_

#include "BaseParser.h"

namespace ba {

class LJKAParser: public BaseParser {
public:
	LJKAParser();
	virtual ~LJKAParser();

	virtual std::shared_ptr<System> parse(std::ifstream &configuration) override;
};

} /* namespace ba */

#endif /* PARSERS_LJKAPARSER_H_ */
