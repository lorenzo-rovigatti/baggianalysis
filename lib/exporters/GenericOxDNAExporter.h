/*
 * GenericOxDNAExporter.h
 *
 *  Created on: Jun 17, 2020
 *      Author: lorenzo
 */

#ifndef EXPORTERS_GENERICOXDNAEXPORTER_H_
#define EXPORTERS_GENERICOXDNAEXPORTER_H_

#include "BaseExporter.h"

namespace ba {

class GenericOxDNAExporter: public BaseExporter {
public:
	GenericOxDNAExporter(bool also_print_N_A);
	virtual ~GenericOxDNAExporter();

	void write(std::shared_ptr<System> system, std::string filename) override;

private:
	bool _also_print_N_A;
};

#ifdef PYTHON_BINDINGS

void export_GenericOxDNAExporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_GENERICOXDNAEXPORTER_H_ */
