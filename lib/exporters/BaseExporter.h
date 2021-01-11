/*
 * BaseExporter.h
 *
 *  Created on: Mar 1, 2020
 *      Author: lorenzo
 */

#ifndef EXPORTERS_BASEEXPORTER_H_
#define EXPORTERS_BASEEXPORTER_H_

#include "../defs.h"
#include "../python_defs.h"

namespace ba {

class System;
class BaseTrajectory;

class BaseExporter {
public:
	BaseExporter();
	virtual ~BaseExporter();

	virtual void write(std::shared_ptr<System> system, std::string filename);
	virtual void write_trajectory(std::shared_ptr<BaseTrajectory> trajectory, std::string filename);
	virtual void write_topology(std::shared_ptr<System> system, std::string filename);

private:
	virtual void _write_system_to_stream(std::shared_ptr<System> system, std::ostream &output);
	virtual std::string _topology_filename_from_output_filename(std::string filename);
};

#ifdef PYTHON_BINDINGS

void export_BaseExporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_BASEEXPORTER_H_ */
