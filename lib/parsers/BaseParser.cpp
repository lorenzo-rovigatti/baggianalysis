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

std::shared_ptr<System> BaseParser::parse_file(std::string conf_name) {
	std::ifstream conf_file(conf_name);
	auto new_system = this->parse_stream(conf_file);
	conf_file.close();

	return new_system;
}

#ifdef PYTHON_BINDINGS

void export_BaseParser(py::module &m) {
	pybind11::class_<BaseParser, PyBaseParser, std::shared_ptr<BaseParser>> parser(m, "BaseParser");

	parser
		.def(py::init<>())
		.def("parse_file", &BaseParser::parse_file);
}

#endif

} /* namespace ba */
