/*
 * NoTopology.h
 *
 *  Created on: 25 sep 2021
 *      Author: lorenzo
 */

#ifndef PARSERS_OXDNA_TOPOLOGY_PARSERS_NOTOPOLOGY_H_
#define PARSERS_OXDNA_TOPOLOGY_PARSERS_NOTOPOLOGY_H_

#include "Default.h"

namespace ba {
namespace oxDNA_topology {

class NoTopology: public Default {
public:
	NoTopology();
	virtual ~NoTopology();

	bool has_N() override {
		return false;
	}

	particle_type type(int) override;
};

#ifdef PYTHON_BINDINGS

void export_NoTopology(py::module &m);

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */

#endif /* PARSERS_OXDNA_TOPOLOGY_PARSERS_DNA_H_ */
