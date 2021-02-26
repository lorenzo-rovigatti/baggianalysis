/*
 * Default.h
 *
 *  Created on: 11 gen 2021
 *      Author: lorenzo
 */

#ifndef PARSERS_OXDNA_TOPOLOGY_PARSERS_DEFAULT_H_
#define PARSERS_OXDNA_TOPOLOGY_PARSERS_DEFAULT_H_

#include "../../defs.h"
#include "../../python_defs.h"

#include <fstream>

namespace ba {
namespace oxDNA_topology {

class Default {
public:
	Default(std::string topology_file);
	virtual ~Default();

	uint N();
	uint N_A();

	virtual particle_type type(int);

protected:
	Default() {}

	uint _N = 0;
	uint _N_A = 0;
};

#ifdef PYTHON_BINDINGS

/**
 * @brief Trampoline class for Default.
 */
class PyDefault: public Default {
public:
	using Default::Default;

	particle_type type(int idx) override {
		PYBIND11_OVERLOAD(
				particle_type,
				Default,
				type,
				idx
		);
	}
};

void export_Default(py::module &m);

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */

#endif /* PARSERS_OXDNA_TOPOLOGY_PARSERS_DEFAULT_H_ */
