/*
 * LAMMPSDataFileExporter.cpp
 *
 *  Created on: Mar 1, 2020
 *      Author: lorenzo
 */

#include "LAMMPSDataFileExporter.h"

namespace ba {

LAMMPSDataFileExporter::LAMMPSDataFileExporter(std::string basename) {

}

LAMMPSDataFileExporter::~LAMMPSDataFileExporter() {

}

void LAMMPSDataFileExporter::write(std::string suffix) {

}

#ifdef PYTHON_BINDINGS

void export_LAMMPSDataFileExporter(py::module &m) {
	py::class_<LAMMPSDataFileExporter, BaseExporter, std::shared_ptr<LAMMPSDataFileExporter>> exporter(m, "LAMMPSDataFileExporter", R"pbdoc(
		 Export configurations to the LAMMPS data file format. 
	)pbdoc");

	exporter.def(py::init<std::string>(), R"pbdoc(
		 Default constructor. 
	)pbdoc");
}

#endif

} /* namespace ba */
