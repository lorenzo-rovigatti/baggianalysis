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

std::shared_ptr<System> BaseParser::open_parse_close(std::string conf_name) {
	std::ifstream conf_file(conf_name);
	auto new_system = this->parse(conf_file);
	conf_file.close();

	return new_system;
}

#ifdef PYTHON_BINDINGS

void export_BaseParser(py::module &m) {
	pybind11::class_<BaseParser, PyBaseParser, std::shared_ptr<BaseParser>> parser(m, "BaseParser");

	parser
		.def(py::init<>())
		.def("parse", &BaseParser::parse)
		.def("open_parse_close", &BaseParser::open_parse_close);
}

#endif

} /* namespace ba */
