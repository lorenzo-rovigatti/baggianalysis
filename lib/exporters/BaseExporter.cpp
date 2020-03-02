/*
 * BaseExporter.cpp
 *
 *  Created on: Mar 1, 2020
 *      Author: lorenzo
 */

#include "BaseExporter.h"

namespace ba {

BaseExporter::BaseExporter() {

}

BaseExporter::~BaseExporter() {

}

#ifdef PYTHON_BINDINGS

void export_BaseExporter(py::module &m) {
	py::class_<BaseExporter, PyBaseExporter, std::shared_ptr<BaseExporter>> exporter(m, "BaseExporter", R"pbdoc(
		 Interface for configuration-exporting classes. 
	)pbdoc");

	exporter.def(py::init<>(), R"pbdoc(
		 Default constructor. 
	)pbdoc");
	exporter.def("write", &BaseExporter::write, R"pbdoc(
		 Print out the configuration. 
	)pbdoc");
}

#endif

} /* namespace ba */
