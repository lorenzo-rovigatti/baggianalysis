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

class BaseExporter {
public:
	BaseExporter();
	virtual ~BaseExporter();

	virtual void write(std::string suffix) = 0;
};

#ifdef PYTHON_BINDINGS

/**
 * @brief Trampoline class for BaseExporter.
 */
class PyBaseExporter : public BaseExporter {
public:
	using BaseExporter::BaseExporter;

	void write(std::string suffix) override {
		PYBIND11_OVERLOAD_PURE(
			void,
			BaseExporter,
			write,
			std::string
		);
	}
};

void export_BaseExporter(py::module &m);

#endif

} /* namespace ba */

#endif /* EXPORTERS_BASEEXPORTER_H_ */
