/*
 * GenericOxDNAExporter.cpp
 *
 *  Created on: Jun 17, 2020
 *      Author: lorenzo
 */

#include "GenericOxDNAExporter.h"

#include <fstream>

namespace ba {

GenericOxDNAExporter::GenericOxDNAExporter(bool also_print_N_A) :
				_also_print_N_A(also_print_N_A) {

}

GenericOxDNAExporter::~GenericOxDNAExporter() {

}

void GenericOxDNAExporter::write(std::shared_ptr<System> system, std::string filename) {
	uint N = system->N();

	std::string topology_file = boost::str(boost::format("topology_%s") % filename);
	std::ofstream topology(topology_file);

	topology << N;
	if(_also_print_N_A) {
		int N_A = 0;
		for(auto p : system->particles()) {
			if(p->type() == "0") {
				N_A++;
			}
		}
		topology << " " << N_A << std::endl;
	}

	topology.close();

	std::ofstream configuration(filename);

	configuration << boost::format("t = %llu\n") % system->time;
	configuration << boost::format("b = %lf %lf %lf\n") % system->box[0] % system->box[1] % system->box[2];
	configuration << boost::format("E = 0 0 0\n");
	// TODO update with angular data
	for(auto p : system->particles()) {
		configuration << boost::format("%lf %lf %lf ") % p->position()[0] % p->position()[1] % p->position()[2];
		configuration << boost::format("1 0 0 ");
		configuration << boost::format("0 1 0 ");
		configuration << boost::format("%lf %lf %lf ") % p->velocity()[0] % p->velocity()[1] % p->velocity()[2];
		configuration << boost::format("0 0 0\n");
	}

	configuration.close();
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
