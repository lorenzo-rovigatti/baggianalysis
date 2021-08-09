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

template<int size>
struct TopologyLink {
	template<typename ... T>
	TopologyLink(T&&... values) :
					particles( { std::forward<T>(values)... }) {

	}

	template<typename ... T>
	TopologyLink(std::string ntype, T&&... values) :
					type(ntype),
					particles( { std::forward<T>(values)... }) {

	}

	TopologyLink() = delete;

	typename std::array<int, size>::value_type &operator[](std::size_t idx) {
		return particles[idx];
	}

	const typename std::array<int, size>::value_type &operator[](std::size_t idx) const {
		return particles[idx];
	}

	bool operator<(const TopologyLink& rhs) const {
		if(type == rhs.type) {
			return particles < rhs.particles;
		}
		return type < rhs.type;
	}

	std::string type = DEFAULT_LINK_TYPE;
	std::array<int, size> particles;
};

using TopologyBond = TopologyLink<2>;
using TopologyAngle = TopologyLink<3>;
using TopologyDihedral = TopologyLink<4>;

class Topology;
using TopologyParser = std::function<void(std::string, std::shared_ptr<Topology>)>;

class Topology: public std::enable_shared_from_this<Topology> {
public:
	Topology(std::shared_ptr<System> system);
	virtual ~Topology();

	static std::shared_ptr<Topology> make_empty_topology();
	static std::shared_ptr<Topology> make_topology_from_file(std::string filename, TopologyParser parser);
	static std::shared_ptr<Topology> make_topology_from_system(std::shared_ptr<System> system);

	void add_bond(int p, int q);
	void add_bond(std::string type, int p, int q);
	void add_angle(int p, int q, int r);
	void add_angle(std::string type, int p, int q, int r);
	void add_dihedral(int p, int q, int r, int s);
	void add_dihedral(std::string type, int p, int q, int r, int s);

	void enable_checks();
	void disable_checks();

	void remove_unappliable_links(std::shared_ptr<System> system);

	void apply(std::shared_ptr<System> system);

	const std::vector<std::set<int>> &clusters() const;

	const std::set<TopologyBond> &bonds() const;
	const std::set<std::string> &bond_types() const;

	const std::set<TopologyAngle> &angles() const;
	const std::set<std::string> &angle_types() const;

	const std::set<TopologyDihedral> &dihedrals() const;
	const std::set<std::string> &dihedral_types() const;

protected:
	Topology();
	Topology(const Topology &) = delete;

	void _raise_error(std::string msg);
	void _fill_clusters(std::shared_ptr<System> system);
	void _set_bonded_links(std::shared_ptr<System> system);

	bool _disable_checks = false;
	bool _build_clusters = true;

	/// the number of particles contained in the system that this topology was initialised for
	uint _N_in_system;

	std::set<TopologyBond> _bonds;
	std::set<std::string> _bond_types;

	std::set<TopologyAngle> _angles;
	std::set<std::string> _angle_types;

	std::set<TopologyDihedral> _dihedrals;
	std::set<std::string> _dihedral_types;

	std::vector<std::set<int>> _clusters;
};

#ifdef PYTHON_BINDINGS

void export_Topology(py::module &m);

#endif

} /* namespace ba */

#endif /* TOPOLOGY_H_ */
