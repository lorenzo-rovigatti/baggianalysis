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

	void _set_id_shift(std::shared_ptr<System> system);
	int _shifted(int idx);
	std::map<particle_type, int> _ba_LAMMPS_type_map(std::shared_ptr<System> system);
	std::string _particle_line(std::shared_ptr<Particle> p, int int_type);

	int _id_shift = 0;
	std::string _atom_style;
	std::vector<std::string> _supported_atom_styles = {"atomic", "bond", "full", "molecular"};
};

#ifdef PYTHON_BINDINGS

void export_LAMMPSDataFileExporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_LAMMPSDATAFILEEXPORTER_H_ */
