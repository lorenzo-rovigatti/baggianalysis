/*
 * BaseParser.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "BaseParser.h"

namespace ba {

BaseParser::BaseParser() {

}

BaseParser::~BaseParser() {

}

std::shared_ptr<System> BaseParser::make_system(std::ifstream &configuration) {
	auto new_system = _parse_stream(configuration);

	return new_system;
}

std::shared_ptr<System> BaseParser::make_system(std::string conf_name) {
	auto new_system = _parse_file(conf_name);

	return new_system;
}

std::shared_ptr<System> BaseParser::_parse_file(std::string conf_name) {
	std::ifstream conf_file(conf_name);
	auto new_system = _parse_stream(conf_file);
	conf_file.close();

	return new_system;
}

#ifdef PYTHON_BINDINGS

void export_BaseParser(py::module &m) {
	py::class_<BaseParser, PyBaseParser, std::shared_ptr<BaseParser>> parser(m, "BaseParser");

	parser
		.def(py::init<>())
		// we export to python the version that takes the filename only
		.def("make_system", (std::shared_ptr<System> (BaseParser::*)(std::string)) &BaseParser::make_system)
		.def("_parse_file", (std::shared_ptr<System> (BaseParser::*)(std::string)) &BaseParser::_parse_file);
}

#endif

} /* namespace ba */
