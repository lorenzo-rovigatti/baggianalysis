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

	void write_topology(std::shared_ptr<System> system, std::string filename) override;

private:
	void _write_system_to_stream(std::shared_ptr<System> system, std::ostream &output) override;

	bool _also_print_N_A;
};

#ifdef PYTHON_BINDINGS

void export_GenericOxDNAExporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_GENERICOXDNAEXPORTER_H_ */
