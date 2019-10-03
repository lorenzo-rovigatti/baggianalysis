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

	/**
	 * @brief Build a new System by parsing an already open stream
	 *
	 * @param configuration the stream to be parsed
	 * @return a new System
	 */
	virtual std::shared_ptr<System> parse_stream(std::ifstream &configuration) = 0;

	/**
	 * @brief Build a new System by opening and then parsing a file
	 *
	 * @param configuration the name of the file to be parsed
	 * @return a new System
	 */
	virtual std::shared_ptr<System> parse_file(std::string conf_name);
};

#ifdef PYTHON_BINDINGS

/**
 * @brief Trampoline class for BaseParser.
 */
class PyBaseParser : public BaseParser {
public:
	using BaseParser::BaseParser;

	std::shared_ptr<System> parse_stream(std::ifstream &configuration) override {
		PYBIND11_OVERLOAD_PURE(
			std::shared_ptr<System>,
			BaseParser,
			parse,
			configuration
		);

		// suppress warnings
		return std::shared_ptr<System>(std::make_shared<System>());
	}

	std::shared_ptr<System> parse_file(std::string conf_name) override {
		PYBIND11_OVERLOAD(
			std::shared_ptr<System>,
			BaseParser,
			parse_file,
			conf_name
		);

		// suppress warnings
		return std::shared_ptr<System>(std::make_shared<System>());
	}
};

void export_BaseParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_BASEPARSER_H_ */
