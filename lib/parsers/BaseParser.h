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

class Topology;

class BaseParser {
public:
	BaseParser();
	virtual ~BaseParser();

	void set_topology(std::shared_ptr<Topology> new_topology);
	virtual void use_topology(bool use);

	/**
	 * @brief Build a new System by parsing an already open stream
	 *
	 * @param configuration the stream to be parsed
	 * @return a new System
	 */
	std::shared_ptr<System> make_system(std::ifstream &configuration);

	/**
	 * @brief Build a new System by opening and then parsing a file
	 *
	 * @param configuration the name of the file to be parsed
	 * @return a new System
	 */
	std::shared_ptr<System> make_system(std::string conf_name);

	/**
	 * @brief Build a system out of the given file
	 *
	 * This method is defined public because it has to be exported to python
	 *
	 * @param conf_name the name of the file to be parsed
	 * @return a new System
	 */
	virtual std::shared_ptr<System> _parse_file(std::string conf_name);

protected:
	virtual std::shared_ptr<System> _parse_stream(std::ifstream &configuration);

	void _apply_topology(std::shared_ptr<System> system);

	std::shared_ptr<Topology> _topology = nullptr;
	bool _use_topology = true;
};

#ifdef PYTHON_BINDINGS

/**
 * @brief Trampoline class for BaseParser.
 */
class PyBaseParser : public BaseParser {
public:
	using BaseParser::BaseParser;

	std::shared_ptr<System> _parse_file(std::string conf_name) override {
		PYBIND11_OVERLOAD(
			std::shared_ptr<System>,
			BaseParser,
			_parse_file,
			conf_name
		);

		// suppress warnings
		return std::shared_ptr<System>(std::make_shared<System>());
	}

	std::shared_ptr<System> _parse_stream(std::ifstream &configuration) override {
		PYBIND11_OVERLOAD(
			std::shared_ptr<System>,
			BaseParser,
			_parse_stream,
			configuration
		);
	}
};

void export_BaseParser(py::module &m);

#endif

} /* namespace ba */

#endif /* PARSERS_BASEPARSER_H_ */
