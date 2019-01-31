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

	virtual std::shared_ptr<System> parse(std::ifstream &configuration) override;

protected:
	double _dt;

protected:
	std::size_t _time_pos_in_string(std::string &s);
};

} /* namespace ba */

#endif /* PARSERS_GROPARSER_H_ */
