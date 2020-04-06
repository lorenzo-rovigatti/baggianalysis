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
		 The default constructor does not take any parameters.
	)pbdoc");

	exporter.def("write", &BaseExporter::write, R"pbdoc(
		 Print out the configuration.

         Parameters
         ----------
             system : :class:`System`
                 The :class:`System` to be printed.
             filename : str
                 The name of the output file. 
	)pbdoc");
}

#endif

} /* namespace ba */
