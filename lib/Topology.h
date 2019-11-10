/*
 * Topology.h
 *
 *  Created on: Nov 9, 2019
 *      Author: lorenzo
 */

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include "defs.h"
#include "python_defs.h"

#include <array>
#include <set>

namespace ba {

class System;

using Bond = std::array<int, 2>;
using Angle = std::array<int, 3>;
using Dihedral = std::array<int, 4>;

class Topology {
public:
	Topology();
	virtual ~Topology();

	void add_bond(int p, int q);
	void add_angle(int p, int q, int r);
	void add_dihedral(int p, int q, int r, int s);

	void apply(std::shared_ptr<System> &system);

protected:
	void _check_index(std::shared_ptr<System> &particle_set, int idx);

	std::set<int> _indexes;
	std::vector<Bond> _bonds;
	std::vector<Angle> _angles;
	std::vector<Dihedral> _dihedrals;

	std::vector<std::set<int>> _molecules;
};

#ifdef PYTHON_BINDINGS

void export_Topology(py::module &m);

#endif

} /* namespace ba */

#endif /* TOPOLOGY_H_ */
