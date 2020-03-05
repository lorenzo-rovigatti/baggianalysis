/*
 * Cogli1Exporter.h
 *
 *  Created on: 5 mar 2020
 *      Author: lorenzo
 */

#ifndef EXPORTERS_COGLI1EXPORTER_H_
#define EXPORTERS_COGLI1EXPORTER_H_

#include "BaseExporter.h"

namespace ba {

class Cogli1Exporter: public BaseExporter {
public:
	Cogli1Exporter();
	virtual ~Cogli1Exporter();

	void write(std::shared_ptr<System> system, std::string filename) override;
};

#ifdef PYTHON_BINDINGS

void export_Cogli1Exporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_COGLI1EXPORTER_H_ */
