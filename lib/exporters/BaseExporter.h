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

#include "../System.h"

namespace ba {

class BaseExporter {
public:
	BaseExporter();
	virtual ~BaseExporter();

	virtual void write(std::shared_ptr<System> system, std::string suffix) = 0;
};

#ifdef PYTHON_BINDINGS

/**
 * @brief Trampoline class for BaseExporter.
 */
class PyBaseExporter : public BaseExporter {
public:
	using BaseExporter::BaseExporter;

	void write(std::shared_ptr<System> system, std::string suffix) override {
		PYBIND11_OVERLOAD_PURE( // @suppress("Unused return value")
			void,
			BaseExporter,
			write,
			system,
			suffix
		);
	}
};

void export_BaseExporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_BASEEXPORTER_H_ */
