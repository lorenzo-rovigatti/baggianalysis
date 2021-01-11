/*
 * TSP.h
 *
 *  Created on: 11 gen 2021
 *      Author: lorenzo
 */

#ifndef PARSERS_OXDNA_TOPOLOGY_PARSERS_TSP_H_
#define PARSERS_OXDNA_TOPOLOGY_PARSERS_TSP_H_

#include "Default.h"

namespace ba {
namespace oxDNA_topology {

class TSP: public Default {
public:
	TSP(std::string topology_file);
	virtual ~TSP();

	uint N_stars() const;
	uint N_monomers_per_arm(uint star) const;
	uint N_arms(int star) const;

private:
	uint _N_stars = 0;
	std::vector<uint> _N_monomers_per_arm;
	std::vector<uint> _N_arms;
};

#ifdef PYTHON_BINDINGS

void export_TSP(py::module &m);

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */

#endif /* PARSERS_OXDNA_TOPOLOGY_PARSERS_TSP_H_ */
