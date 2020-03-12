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

class LAMMPSDataFileExporter: public BaseExporter {
public:
	LAMMPSDataFileExporter() = delete;
	LAMMPSDataFileExporter(std::string atom_style, std::string basename);
	virtual ~LAMMPSDataFileExporter();

	void write(std::shared_ptr<System> system, std::string suffix) override;

private:
	std::string _particle_line(std::shared_ptr<Particle> p);

	std::string _atom_style;
	std::string _basename;
};

#ifdef PYTHON_BINDINGS

void export_LAMMPSDataFileExporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_LAMMPSDATAFILEEXPORTER_H_ */
