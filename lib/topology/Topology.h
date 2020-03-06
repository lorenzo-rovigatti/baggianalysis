/*
 * Topology.h
 *
 *  Created on: Nov 9, 2019
 *      Author: lorenzo
 */

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include "../defs.h"
#include "../python_defs.h"

#include <array>
#include <set>

namespace ba {

class System;

using Bond = std::array<int, 2>;
using Angle = std::array<int, 3>;
using Dihedral = std::array<int, 4>;

class Topology;
using TopologyParser = std::function<void(std::string, std::shared_ptr<Topology>)>;

class Topology: public std::enable_shared_from_this<Topology> {
public:
	Topology(std::shared_ptr<System> system);
	virtual ~Topology();

	static std::shared_ptr<Topology> make_empty_topology();
	static std::shared_ptr<Topology> make_topology_from_file(std::string filename, TopologyParser parser);

	void add_bond(int p, int q);
	void add_angle(int p, int q, int r);
	void add_dihedral(int p, int q, int r, int s);

	void apply(std::shared_ptr<System> system);

	const std::set<Bond> &bonds() const;

protected:
	Topology();
	Topology(const Topology &) = delete;

	void _fill_molecules(std::shared_ptr<System> system);

	/// the number of particles contained in the system that this topology was initialised for
	uint _N_in_system;

	std::set<Bond> _bonds;
	std::set<Angle> _angles;
	std::set<Dihedral> _dihedrals;

	std::vector<std::set<int>> _molecules;
};

#ifdef PYTHON_BINDINGS

void export_Topology(py::module &m);

#endif

} /* namespace ba */

#endif /* TOPOLOGY_H_ */
