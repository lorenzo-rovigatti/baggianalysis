/*
 * DNA.h
 *
 *  Created on: 29 sep 2023
 *      Author: lorenzo
 */

#ifndef PARSERS_OXDNA_TOPOLOGY_PARSERS_DNA_H_
#define PARSERS_OXDNA_TOPOLOGY_PARSERS_DNA_H_

#include "Default.h"

namespace ba {
namespace oxDNA_topology {

class DNA: public Default {
public:
	DNA(std::string topology_file);
	virtual ~DNA();

	particle_type type(int) override;

protected:
	std::vector<particle_type> _types;
};

#ifdef PYTHON_BINDINGS

void export_DNA(py::module &m);

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */

#endif /* PARSERS_OXDNA_TOPOLOGY_PARSERS_DNA_H_ */
