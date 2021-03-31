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
#include <queue>

namespace ba {

Topology::Topology() :
				_N_in_system(0) {

}

Topology::Topology(std::shared_ptr<System> system) {
	for(auto p : system->particles()) {
		for(auto q : p->bonded_neighbours()) {
			if(p->index() < q->index()) {
				add_bond(p->index(), q->index());
			}
		}

		for(auto angle : p->bonded_angles()) {
			int p = angle->particles()[0]->index();
			int q = angle->particles()[1]->index();
			int r = angle->particles()[2]->index();
			add_angle(p, q, r);
		}

		for(auto dihedral : p->bonded_dihedrals()) {
			int p = dihedral->particles()[0]->index();
			int q = dihedral->particles()[1]->index();
			int r = dihedral->particles()[2]->index();
			int s = dihedral->particles()[3]->index();
			add_dihedral(p, q, r, s);
		}
	}

	_fill_clusters(system);
}

Topology::~Topology() {

}

std::shared_ptr<Topology> Topology::make_empty_topology() {
	return std::shared_ptr<Topology>(new Topology());
}

std::shared_ptr<Topology> Topology::make_topology_from_file(std::string filename, TopologyParser parser) {
	std::shared_ptr<Topology> new_topology = make_empty_topology();
	parser(filename, new_topology);

	return new_topology;
}

std::shared_ptr<Topology> Topology::make_topology_from_system(std::shared_ptr<System> system) {
	return std::make_shared<Topology>(system);
}

void Topology::add_bond(int p, int q) {
	_bonds.emplace(Bond({ p, q }));
}

void Topology::add_angle(int p, int q, int r) {
	_angles.insert(Angle({ p, q, r }));
}

void Topology::add_dihedral(int p, int q, int r, int s) {
	_dihedrals.insert(Dihedral({ p, q, r, s }));
}

void Topology::enable_checks() {
	_disable_checks = false;
}

void Topology::disable_checks() {
	_disable_checks = true;
}

void Topology::apply(std::shared_ptr<System> system) {
	// reset all the molecules
	system->molecules().clear();

	// add all bonds between particles
	for(auto bond : _bonds) {
		auto p_idx = bond[0];
		auto q_idx = bond[1];

		try {
			auto p = system->particle_by_id(p_idx);
			auto q = system->particle_by_id(q_idx);
			p->add_bonded_neighbour(q);
		}
		catch (std::runtime_error &e) {
			std::string error = fmt::format("The following error occurred while applying the topology to a System:\n\t'{}'", e.what());
			_raise_error(error);
		}
	}

	// we build the clusters only once
	if(_clusters.size() == 0) {
		_fill_clusters(system);
	}

	if(_N_in_system != system->N() && !_disable_checks) {
		std::string error = fmt::format("The topology was initialised on a System containing {} particles and hence it cannot be applied to a System containing {} particles", _N_in_system, system->N());
		_raise_error(error);
	}

	for(auto &cluster : _clusters) {
		std::shared_ptr<ParticleSet> new_molecule = std::make_shared<ParticleSet>();
		for(auto index : cluster) {
			auto particle = system->particle_by_id(index);
			new_molecule->add_particle(particle);
			particle->set_molecule(new_molecule);
		}

		std::string mol_name = fmt::format("mol_{}", system->molecules().size());
		new_molecule->set_name(mol_name);
		system->molecules().emplace_back(new_molecule);
	}

	for(auto &angle : _angles) {
		std::array<std::shared_ptr<Particle>, 3> particles({system->particle_by_id(angle[0]), system->particle_by_id(angle[1]), system->particle_by_id(angle[2])});
		for(auto particle : particles) {
			particle->add_bonded_angle(particles[0], particles[1], particles[2]);
		}
	}

	for(auto &dihedral : _dihedrals) {
		std::array<std::shared_ptr<Particle>, 4> particles({system->particle_by_id(dihedral[0]), system->particle_by_id(dihedral[1]), system->particle_by_id(dihedral[2]), system->particle_by_id(dihedral[3])});
		for(auto particle : particles) {
			particle->add_bonded_dihedral(particles[0], particles[1], particles[2], particles[3]);
		}
	}
}

const std::vector<std::set<int>> &Topology::clusters() const {
	return _clusters;
}

const std::set<Bond> &Topology::bonds() const {
	return _bonds;
}

const std::set<Angle> &Topology::angles() const {
	return _angles;
}

const std::set<Dihedral> &Topology::dihedrals() const {
	return _dihedrals;
}

void Topology::_raise_error(std::string msg) {
	if(!_disable_checks) {
		throw std::runtime_error(msg);
	}
}

void Topology::_fill_clusters(std::shared_ptr<System> system) {
	_N_in_system = system->N();
	_clusters.clear();

	if(_N_in_system == 0) {
		return;
	}

	// these maps might be substituted by a single boost::bimap
	std::map<int, int> index_to_cluster;
	std::map<int, std::set<int>> cluster_to_molecule;

	for(auto index : system->indexes()) {
		index_to_cluster[index] = index;
	}

	// this will contain the particles whose neighbours we have to loop on
	std::queue<int> next;
	auto curr_it = system->particles().begin();
	next.push((*curr_it)->index());
	bool done = false;
	while(!done) {
		auto p = system->particle_by_id(next.front());
		next.pop();
		for(auto neigh : p->bonded_neighbours()) {
			if(index_to_cluster[neigh->index()] > index_to_cluster[p->index()]) {
				index_to_cluster[neigh->index()] = index_to_cluster[p->index()];
				next.push(neigh->index());
			}
		}
		if(next.empty()) {
			curr_it++;
			if(curr_it == system->particles().end()) {
				done = true;
			}
			else {
				next.push((*curr_it)->index());
			}
		}
	}

	// create a cluster -> molecule map
	for(auto particle : system->particles()) {
		int cluster = index_to_cluster[particle->index()];
		if(cluster_to_molecule.find(cluster) == cluster_to_molecule.end()) {
			cluster_to_molecule[cluster] = std::set<int>( { particle->index() });
		}
		else {
			cluster_to_molecule[cluster].insert(particle->index());
		}
	}

	for(auto pair : cluster_to_molecule) {
		_clusters.emplace_back(pair.second);
	}

	if(_clusters.size() == 1) {
		BA_DEBUG("Topology: Found 1 cluster");
	}
	else {
		BA_DEBUG("Topology: Found {} clusters", _clusters.size());
	}
}

#ifdef PYTHON_BINDINGS

void export_Topology(py::module &m) {
	py::class_<Topology, std::shared_ptr<Topology>> topology(m, "Topology", R"pbdoc(
        This class manages the connections and links between the particles of a system.

        Here the term ``topology`` refers to the way atoms/particles are partitioned into clusters. In its simplest form, a topology is just a list of
        links between particles. These links can be shared between two, three, four or more particles. While the latter are pretty rare, the others 
        are quite common. Connections between two, three and four particles are here called `bonds`, `angles` and `dihedrals`. As of now, although all
        three classes of links are supported and stored in the topology, the only useful concept is the bond, which is used by the code to partition
        particles into clusters.

        It is important to remember that all the links in the topology are specified through using particle indexes, which are integer numbers. Once 
        the links have been added (either manually to an empty topology generated by :meth:`make_empty_topology` or parsed from a file by 
        :meth:`make_topology_from_file`) the topology can be applied to any system (see :meth:`apply`). Note that, by default, it is not possible to 
        apply the same topology to systems having different numbers of particles. This behaviour can be overridden by calling :meth:`disable_checks` 
        prior to :meth:`apply`. 
	)pbdoc");

	topology.def(py::init<std::shared_ptr<System>>(), R"pbdoc(
Instances of this class can be either built with the :meth:`make_empty_topology` or :meth:`make_topology_from_file` static methods or by directly
using a constructor that takes as its only parameter the :class:`System` instance whence the topology is extracted::

    # here we build or parse a system
    system = ...
    # and then create a topology out of its bonding pattern
    new_topology = ba.Topology(system)

)pbdoc");

	topology.def_static("make_empty_topology", &Topology::make_empty_topology, R"pbdoc(
        This static method builds an empty topology and returns it.

        Returns
        -------
        :class:`Topology`
            A new empty topology.
	)pbdoc");

	topology.def_static("make_topology_from_file", &Topology::make_topology_from_file, py::arg("filename"), py::arg("parser"), R"pbdoc(
        This static method uses a user-passed callable to build a topology out of a file.

        Parameters
        ----------
        filename : str
            The name of the file to parse.
        parser: callable
            A callable that takes a :obj:`str` and a :class:`Topology`. The former is the name of the file containing the topology details to be parsed, while
            the latter is the empty topology that will be initialised by the callable.
	)pbdoc");

	topology.def_static("make_topology_from_system", &Topology::make_topology_from_system, py::arg("system"), R"pbdoc(
		This static method generates a topology out of a system by using the bonded neighbours of each particle to build the list of bonds. 

		Parameters
		----------
		system : :class:`System`
			The input system.
    )pbdoc");

	topology.def("add_bond", &Topology::add_bond, py::arg("p"), py::arg("q"), R"pbdoc(
        Adds a bond between a pair of particles.

        Parameters
        ----------
		p : int
			The index of the first particle of the pair.
		q : int
			The index of the second particle of the pair.
	)pbdoc");

	topology.def("add_angle", &Topology::add_angle, py::arg("p"), py::arg("q"), py::arg("r"), R"pbdoc(
        Adds an angle formed by a triplet of particles.

        Parameters
        ----------
        p : int
            The index of the first particle of the triplet.
        q : int
            The index of the second (central) particle of the triplet.
        r : int
            The index of the third (and last) particle of the triplet.
    )pbdoc");

	topology.def("add_dihedral", &Topology::add_dihedral, py::arg("p"), py::arg("q"), py::arg("r"), py::arg("s"), R"pbdoc(
        Adds a dihedral formed by four particles.

        Parameters
        ----------
        p : int
            The index of the first particle.
        q : int
            The index of the second particle.
        r : int
            The index of the third particle.
        s : int
            The index of the fourth particle.
    )pbdoc");

	topology.def("enable_checks", &Topology::enable_checks, R"pbdoc(
        Makes the topology throw exceptions whenever errors occur during application to a system.
	)pbdoc");

	topology.def("disable_checks", &Topology::disable_checks, R"pbdoc(
        Disables exception throwing whenever errors occur during application to a system.
	)pbdoc");

	topology.def("apply", &Topology::apply, py::arg("system"), R"pbdoc(
        Applies the current topology to the given system, adding the bonds to the particles and partitioning them into clusters.

        Parameters
        ----------
        system : :class:`System`
            The target system.
	)pbdoc");

	topology.def_property_readonly("bonds", [](const Topology &t) {
		auto bonds = t.bonds();
		std::vector<Bond> v(bonds.size());
		std::copy(bonds.begin(), bonds.end(), v.begin());
		return v;
	}, R"pbdoc(
        List(List(int)): The list of bonds stored in the topology. Each bond is a two-element list storing the ids of a pair of bonded particles. 
        Since Python does not support sets of lists, we internally turn the c++ std::set into a std::vector which is then returned and converted into 
        a Python object. This may be a performance bottleneck. 
	)pbdoc");

	topology.def_property_readonly("angles", [](const Topology &t) {
		auto angles = t.angles();
		std::vector<Angle> v(angles.size());
		std::copy(angles.begin(), angles.end(), v.begin());
		return v;
	}, R"pbdoc(
		List(List(int)): The list of angles stored in the topology. Each angle is a three-element list storing the ids of a triplet of particles involved in angle. 
		Since Python does not support sets of lists, we internally turn the c++ std::set into a std::vector which is then returned and converted into 
		a Python object. This may be a performance bottleneck. 
	)pbdoc");

	topology.def_property_readonly("dihedrals", [](const Topology &t) {
		auto dihedrals = t.dihedrals();
		std::vector<Dihedral> v(dihedrals.size());
		std::copy(dihedrals.begin(), dihedrals.end(), v.begin());
		return v;
	}, R"pbdoc(
		List(List(int)): The list of dihedrals stored in the topology. Each dihedral is a four-element list storing the ids of four particles involved in a dihedral. 
		Since Python does not support sets of lists, we internally turn the c++ std::set into a std::vector which is then returned and converted into 
		a Python object. This may be a performance bottleneck. 
	)pbdoc");

	topology.def_property_readonly("clusters", &Topology::clusters, R"pbdoc(
		List(Set(int)): The list of clusters stored in the topology. Each cluster is a set of particle ids that belong to that cluster.
	)pbdoc");
}

#endif

} /* namespace ba */
