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
	LAMMPSDataFileExporter(std::string basename);
	virtual ~LAMMPSDataFileExporter();

	void write(std::string suffix) override;

private:

};

#ifdef PYTHON_BINDINGS

void export_LAMMPSDataFileExporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_LAMMPSDATAFILEEXPORTER_H_ */
