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

Topology::Topology() : _N_in_system(0) {

}

Topology::Topology(std::shared_ptr<System> system) {
	for(auto p: system->particles()) {
		for(auto q: p->bonded_neighbours()) {
			if(p->index() < q->index()) {
				Bond new_bond({p->index(), q->index()});
				_bonds.emplace(new_bond);
			}
		}
	}

	_fill_molecules(system);
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
	_bonds.emplace(new_bond);
}

void Topology::add_angle(int p, int q, int r) {
	Angle new_angle({p, q, r});
	_angles.emplace(new_angle);
}

void Topology::add_dihedral(int p, int q, int r, int s) {
	Dihedral new_dihedral({p, q, r, s});
	_dihedrals.emplace(new_dihedral);
}

void Topology::apply(std::shared_ptr<System> system) {
	// reset all the molecules
	system->molecules().clear();

	// add all bonds between particles
	for(auto bond: _bonds) {
		auto p_idx = bond[0];
		auto q_idx = bond[1];

		try {
			auto p = system->particle_by_id(p_idx);
			auto q = system->particle_by_id(q_idx);
			p->add_bonded_neighbour(q);
		}
		catch (std::runtime_error &e) {
			std::string error = boost::str(boost::format("The following error occurred while applying the topology to a System:\n\t'%s'") % e.what());
			throw std::runtime_error(error);
		}
	}

	// we build the molecules only once
	if(_molecules.size() == 0) {
		_fill_molecules(system);
	}

	if(_N_in_system != system->N()) {
		std::string error = boost::str(boost::format("The topology was initialised on a System containing %u particles and hence it cannot be applied to a System containing %u particles") % _N_in_system % system->N());
		throw std::runtime_error(error);
	}

	for(auto &molecule: _molecules) {
		std::shared_ptr<ParticleSet> new_molecule = std::make_shared<ParticleSet>();
		for(auto index: molecule) {
			auto particle = system->particle_by_id(index);
			new_molecule->add_particle(particle);
			particle->set_molecule(new_molecule);
		}

		std::string mol_name = boost::str(boost::format("mol_%u") % system->molecules().size());
		new_molecule->set_name(mol_name);
		system->molecules().emplace_back(new_molecule);
	}
}

const std::set<Bond> &Topology::bonds() const {
	return _bonds;
}

void Topology::_fill_molecules(std::shared_ptr<System> system) {
	_N_in_system = system->N();
	_molecules.clear();

	// these maps might be substituted by a single boost::bimap
	std::map<int, int> index_to_cluster;
	std::map<int, std::set<int>> cluster_to_molecule;

	for(auto index: system->indexes()) {
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

	if(_molecules.size() == 1) {
		BOOST_LOG_TRIVIAL(info)<<"Topology: Found 1 molecule";
	}
	else {
		BOOST_LOG_TRIVIAL(info)<<"Topology: Found " << _molecules.size() << " molecules";
	}
}

#ifdef PYTHON_BINDINGS

void export_Topology(py::module &m) {
	py::class_<Topology, std::shared_ptr<Topology>> topology(m, "Topology");

	topology.def_static("make_empty_topology", &Topology::make_empty_topology);
	topology.def_static("make_topology_from_file", &Topology::make_topology_from_file);
	topology.def("add_bond", &Topology::add_bond);
	topology.def("add_angle", &Topology::add_angle);
	topology.def("add_dihedral", &Topology::add_dihedral);
	topology.def("apply", &Topology::apply);
	topology.def_property_readonly("bonds", &Topology::bonds);
}

#endif

} /* namespace ba */
