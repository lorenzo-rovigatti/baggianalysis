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

#ifdef PYTHON_BINDINGS

/**
 * @brief Trampoline class for BaseParser.
 */
class PyBaseParser : public BaseParser {
public:
	using BaseParser::BaseParser;

	std::shared_ptr<System> parse(std::ifstream &configuration) override {
		PYBIND11_OVERLOAD_PURE(
				std::shared_ptr<System>,
				BaseParser,
				parse,
				configuration
		);

		// suppress warnings
		return std::shared_ptr<System>(new System());
	}
};

void export_BaseParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_BASEPARSER_H_ */
