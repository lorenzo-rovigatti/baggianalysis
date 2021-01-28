/*
 * BaseExporter.cpp
 *
 *  Created on: Mar 1, 2020
 *      Author: lorenzo
 */

#include "BaseExporter.h"

#include "../System.h"
#include "../trajectories/BaseTrajectory.h"

namespace ba {

BaseExporter::BaseExporter() {

}

BaseExporter::~BaseExporter() {

}

void BaseExporter::write(std::shared_ptr<System> system, std::string filename) {
	write_topology(system, _topology_filename_from_output_filename(filename));

	std::ofstream output(filename);
	_write_system_to_stream(system, output);
	output.close();
}

void BaseExporter::write_trajectory(std::shared_ptr<BaseTrajectory> trajectory, std::string filename) {
	std::ofstream output(filename);

	trajectory->reset();
	auto system = trajectory->next_frame();
	write_topology(system, _topology_filename_from_output_filename(filename));

	while(system != nullptr) {
		_write_system_to_stream(system, output);
		system = trajectory->next_frame();
	}
	trajectory->reset();

	output.close();
}

void BaseExporter::write_topology(std::shared_ptr<System> system, std::string filename) {

}

std::string BaseExporter::_topology_filename_from_output_filename(std::string filename) {
	return fmt::format("topology_{}", filename);
}

void BaseExporter::_write_system_to_stream(std::shared_ptr<System> system, std::ostream &output) {
	throw std::runtime_error("Unimplemented _write_system_to_stream method called!");
}

#ifdef PYTHON_BINDINGS

void export_BaseExporter(py::module &m) {
	py::class_<BaseExporter, std::shared_ptr<BaseExporter>> exporter(m, "BaseExporter", R"pbdoc(
		 Interface for configuration-exporting classes. 
	)pbdoc");

	exporter.def(py::init<>(), R"pbdoc(
		 The default constructor does not take any parameters.
	)pbdoc");

	exporter.def("write", &BaseExporter::write, py::arg("system"), py::arg("filename"), R"pbdoc(
		Print out a single system.

        Parameters
        ----------
            system : :class:`System`
                The :class:`System` to be printed.
            filename : str
                The name of the output file. 
	)pbdoc");

	exporter.def("write_trajectory", &BaseExporter::write_trajectory, R"pbdoc(
        Print out a trajectory.

        Parameters
        ----------
		    trajectory : :class:`BaseTrajectory`
			    The trajectory to be printed.
            filename : str
                The name of the output file. 
	)pbdoc");

	exporter.def("write_topology", &BaseExporter::write_topology, R"pbdoc(
        Some formats have a topology file on top of configuration (and/or trajectory) files. By default this method is called
        by both :meth:`write` and :meth:`write_topology` and hence should be extended by child classes that export to formats that use
        topology files.

        Parameters
        ----------
		    system : :class:`System`
                The :class:`System` to be printed.
			filename : str
			    The name of the output topology file. 
	)pbdoc");
}

#endif

} /* namespace ba */
