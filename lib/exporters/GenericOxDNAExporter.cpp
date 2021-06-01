/*
 * GenericOxDNAExporter.cpp
 *
 *  Created on: Jun 17, 2020
 *      Author: lorenzo
 */

#include "GenericOxDNAExporter.h"

#include "../System.h"

#include <fstream>

namespace ba {

GenericOxDNAExporter::GenericOxDNAExporter(bool also_print_N_A) :
				_also_print_N_A(also_print_N_A) {

}

GenericOxDNAExporter::~GenericOxDNAExporter() {

}

void GenericOxDNAExporter::write_topology(std::shared_ptr<System> system, std::string filename) {
	std::ofstream topology(filename);

	topology << system->N();
	if(_also_print_N_A) {
		int N_A = 0;
		for(auto p : system->particles()) {
			if(p->type() == "0") {
				N_A++;
			}
		}
		topology << " " << N_A << std::endl;
	}
	else {
		topology << std::endl;
	}

	topology.close();
}

void GenericOxDNAExporter::_write_system_to_stream(std::shared_ptr<System> system, std::ostream &output) {
	output << fmt::format("t = {}\n", system->time);
	output << fmt::format("b = {} {} {}\n", system->box[0], system->box[1], system->box[2]);
	output << fmt::format("E = 0 0 0\n");

	for(auto p : system->particles()) {
		output << fmt::format("{} {} {} ", p->position()[0], p->position()[1], p->position()[2]);

		if(p->orientation_vectors().size() == 3) {
			vec3 a1 = p->orientation_vectors()[0];
			vec3 a3 = p->orientation_vectors()[2];
			output << fmt::format("{} {} {} ", a1[0], a1[1], a1[2]);
			output << fmt::format("{} {} {} ", a3[0], a3[1], a3[2]);
		}
		else {
			output << fmt::format("1 0 0 ");
			output << fmt::format("0 1 0 ");
		}
		output << fmt::format("{} {} {} ", p->velocity()[0], p->velocity()[1], p->velocity()[2]);
		output << fmt::format("0 0 0\n");
	}
}

#ifdef PYTHON_BINDINGS

void export_GenericOxDNAExporter(py::module &m) {
	py::class_<GenericOxDNAExporter, BaseExporter, std::shared_ptr<GenericOxDNAExporter>> exporter(m, "GenericOxDNAExporter", R"pbdoc(
        Export configurations to the oxDNA file format. The exporter will generate a topology and a configuration file.
        Note that the filename passed to the :meth:`write() <baggianalysis.core.BaseExporter.write>` method is used as-is for the 
        configuration file and as a suffix, prefixed by ``topology_``, for the topology file.  
	)pbdoc");

	exporter.def(py::init<bool>(), R"pbdoc(
The constructor takes one parameter which tells the exporter whether the topology line should also contain the number of particles of type A.

Parameters
----------
    also_print_N_A : bool
        If True, the exporter will print the number of particles of type A in addition to the number of particles in the topology file.
	)pbdoc");
}

#endif

} /* namespace ba */
