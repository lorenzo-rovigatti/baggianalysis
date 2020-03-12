/*
 * Cogli1Exporter.cpp
 *
 *  Created on: 5 mar 2020
 *      Author: lorenzo
 */

#include "Cogli1Exporter.h"

#include "../utils/strings.h"

#include <fstream>

namespace ba {

Cogli1Exporter::Cogli1Exporter() {

}

Cogli1Exporter::Cogli1Exporter(Cogli1Mapper mapper) : _mapper(mapper) {

}

Cogli1Exporter::~Cogli1Exporter() {

}

void Cogli1Exporter::write(std::shared_ptr<System> system, std::string filename) {
	std::ofstream output_file(filename);

	output_file << boost::format(".Box:%lf,%lf,%lf") % system->box[0] % system->box[1] % system->box[2] << std::endl;

	for(auto p : system->particles()) {
		output_file << _mapper(p.get()) << std::endl;
	}

	output_file.close();
}

#ifdef PYTHON_BINDINGS

void export_Cogli1Exporter(py::module &m) {
	py::class_<Cogli1Exporter, BaseExporter, std::shared_ptr<Cogli1Exporter>> exporter(m, "Cogli1Exporter", R"pbdoc(
		 Export configurations to the cogli1 file format, used for visualisation purposes.
	)pbdoc");

	exporter.def(py::init<>(), R"pbdoc(
		 This constructor makes the exporter print each particle as a red sphere of radius 0.5.
	)pbdoc");

	// docstrings of overloaded constructors need to be unindented or they won't be formatted correctly by sphinx
	exporter.def(py::init<Cogli1Mapper>(), R"pbdoc(
This constructor takes as a parameter a callable that maps each particle into a string that will be printed by the exporter.

Parameters
----------
mapper : callable
    A callable that takes a particle and returns the corresponding cogli1 string.
	)pbdoc");
}

#endif

} /* namespace ba */
