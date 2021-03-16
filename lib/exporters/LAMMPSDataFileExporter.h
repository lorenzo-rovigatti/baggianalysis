/*
 * LAMMPSDataFileExporter.h
 *
 *  Created on: Mar 1, 2020
 *      Author: lorenzo
 */

#ifndef EXPORTERS_LAMMPSDATAFILEEXPORTER_H_
#define EXPORTERS_LAMMPSDATAFILEEXPORTER_H_

#include "BaseExporter.h"

namespace ba {

class Particle;

class LAMMPSDataFileExporter: public BaseExporter {
public:
	LAMMPSDataFileExporter() = delete;
	LAMMPSDataFileExporter(std::string atom_style);
	virtual ~LAMMPSDataFileExporter();

private:
	void _write_system_to_stream(std::shared_ptr<System> system, std::ostream &output) override;

	std::string _particle_line(std::shared_ptr<Particle> p);

	std::string _atom_style;
	std::vector<std::string> _supported_atom_styles = {"atomic", "bond", "full"};
};

#ifdef PYTHON_BINDINGS

void export_LAMMPSDataFileExporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_LAMMPSDATAFILEEXPORTER_H_ */
