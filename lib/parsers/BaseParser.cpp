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

#ifdef PYTHON_BINDINGS

//void export_BaseParser(py::module &m) {
//	pybind11::class_<BaseParser, PyBaseParser, std::shared_ptr<BaseParser>> parser(m, "BaseParser");
//
//	parser
//		.def(py::init<>())
//		.def("parse", &BaseParser::parse);
//}

#endif

} /* namespace ba */
