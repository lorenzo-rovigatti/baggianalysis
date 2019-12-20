/*
 * Topology.cpp
 *
 *  Created on: Nov 9, 2019
 *      Author: lorenzo
 */

#include "Topology.h"

#include "../System.h"
#include "../particles/Particle.h"

#include <algorithm>

namespace ba {

Topology::Topology() {

}

Topology::~Topology() {

}

std::shared_ptr<Topology> Topology::make_empty_topology() {
	return std::shared_ptr<Topology>(new Topology());
}

std::shared_ptr<Topology> Topology::make_topology_from_file(std::string filename, TopologyParser parser) {
	std::shared_ptr<Topology> new_topology = std::shared_ptr<Topology>(new Topology());
	parser(filename, new_topology);

	return new_topology;
}

void Topology::add_bond(int p, int q) {
	Bond new_bond({p, q});
	_indexes.insert(new_bond.begin(), new_bond.end());
	_bonds.emplace_back(new_bond);
}

void Topology::add_angle(int p, int q, int r) {
	Angle new_angle({p, q, r});
	_angles.emplace_back(new_angle);
}

void Topology::add_dihedral(int p, int q, int r, int s) {
	Dihedral new_dihedral({p, q, r, s});
	_dihedrals.emplace_back(new_dihedral);
}

void Topology::apply(std::shared_ptr<System> system) {
	auto &particles = system->particles();

	// reset all the molecules
	system->molecules().clear();

	// add all bonds between particles
	for(auto bond: _bonds) {
		auto p_idx = bond[0];
		auto q_idx = bond[1];

		_check_index(system, p_idx);
		_check_index(system, q_idx);

		auto p = particles[p_idx];
		auto q = particles[q_idx];
		p->add_bonded_neighbour(q);
	}

	static uint N_in_system = 0;
	// we build the molecules only once
	if(_molecules.size() == 0) {
		N_in_system = system->N();

		// these maps might be substituted by a single boost::bimap
		std::map<int, int> index_to_cluster;
		std::map<int, std::set<int>> cluster_to_molecule;

		for(auto index: _indexes) {
			index_to_cluster[index] = index;
		}

		// we have to use a std::function instead of a lambda because there is no way (I know of) to write a recursive lambda function.
		std::function<void(std::shared_ptr<Particle>)> flip_neighs = [&](std::shared_ptr<Particle> p) -> void {
			for(auto neigh: p->bonded_neighbours()) {
				if(index_to_cluster[neigh->index()] > index_to_cluster[p->index()]) {
					index_to_cluster[neigh->index()] = index_to_cluster[p->index()];
					flip_neighs(neigh);
				}
			}
		};

		// assign the same cluster to particles connected by bonds and create new clusters for unpaired particles
		for(auto particle: system->particles()) {
			// if particle is not involved in any bonds it's the only particle contained in its associated molecule
			if(particle->bonded_neighbours().size() == 0) {
				index_to_cluster[particle->index()] = particle->index();
			}
			else {
				flip_neighs(particle);
			}
		}

		// create a cluster -> molecule map
		for(auto particle: system->particles()) {
			int cluster = index_to_cluster[particle->index()];
			if(cluster_to_molecule.find(cluster) == cluster_to_molecule.end()) {
				cluster_to_molecule[cluster] = std::set<int>({particle->index()});
			}
			else {
				cluster_to_molecule[cluster].insert(particle->index());
			}
		}

		for(auto pair: cluster_to_molecule) {
			_molecules.emplace_back(pair.second);
		}
	}

	if(N_in_system != system->N()) {
		std::string error = boost::str(boost::format("The topology was initialised on a System containing %u particles and hence it cannot be applied to a System containing %u particles") % N_in_system % system->N());
		throw std::runtime_error(error);
	}

	for(auto &molecule: _molecules) {
		std::shared_ptr<ParticleSet> new_molecule = std::make_shared<ParticleSet>();
		for(auto index: molecule) {
			auto particle = system->particles()[index];
			new_molecule->add_particle(particle);
			particle->molecule() = new_molecule;
		}

		std::string mol_name = boost::str(boost::format("mol%u") % system->molecules().size());
		system->molecules().emplace_back(new_molecule);
	}
}

void Topology::_check_index(std::shared_ptr<System> &particle_set, int idx) {
	if(idx < 0 || idx >= (int) particle_set->N()) {
		std::string error = boost::str(boost::format("Invalid index %d found in the topology") % idx);
		throw std::runtime_error(error);
	}
}

#ifdef PYTHON_BINDINGS

void export_Topology(py::module &m) {
	py::class_<Topology, std::shared_ptr<Topology>> topology(m, "Topology");

	topology
		.def_static("make_empty_topology", &Topology::make_empty_topology)
		.def_static("make_topology_from_file", &Topology::make_topology_from_file)
		.def("add_bond", &Topology::add_bond)
		.def("add_angle", &Topology::add_angle)
		.def("add_dihedral", &Topology::add_dihedral)
		.def("apply", &Topology::apply);
}

#endif

} /* namespace ba */
