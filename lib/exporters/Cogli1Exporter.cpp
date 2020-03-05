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

Cogli1Exporter::~Cogli1Exporter() {

}

void Cogli1Exporter::write(std::shared_ptr<System> system, std::string filename) {
	std::ofstream output_file(filename);

	output_file << boost::format(".Box:%lf,%lf,%lf") % system->box[0] % system->box[1] % system->box[2] << std::endl;

	for(auto p : system->particles()) {
		output_file << boost::format("%lf %lf %lf @ 0.5 C[red]") % p->position()[0] % p->position()[1] % p->position()[2] << std::endl;
	}

	output_file.close();
}

#ifdef PYTHON_BINDINGS

void export_Cogli1Exporter(py::module &m) {
	py::class_<Cogli1Exporter, BaseExporter, std::shared_ptr<Cogli1Exporter>> exporter(m, "Cogli1Exporter", R"pbdoc(
		 Export configurations to the cogli1 file format, used for visualisation purposes. 
	)pbdoc");

	exporter.def(py::init<>(), R"pbdoc(
		 Constructor. 
	)pbdoc");
}

#endif

} /* namespace ba */
