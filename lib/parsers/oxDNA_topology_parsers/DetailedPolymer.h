/*
 * DetailedPolymer.h
 *
 *  Created on: Feb 15, 2022
 *      Author: lorenzo
 */

#ifndef PARSERS_OXDNA_TOPOLOGY_PARSERS_DETAILEDPOLYMER_H_
#define PARSERS_OXDNA_TOPOLOGY_PARSERS_DETAILEDPOLYMER_H_

#include "Default.h"

namespace ba {
namespace oxDNA_topology {

class DetailedPolymer: public Default {
public:
	DetailedPolymer(std::string topology_file);
	virtual ~DetailedPolymer();

	particle_type type(int) override;
	const std::vector<std::vector<int>> &bonded_neighbours() const override;

protected:
	std::vector<particle_type> _types;
};

#ifdef PYTHON_BINDINGS

void export_DetailedPolymer(py::module &m);

#endif

} /* namespace oxDNA_topology */
} /* namespace ba */

#endif /* PARSERS_OXDNA_TOPOLOGY_PARSERS_DETAILEDPOLYMER_H_ */
