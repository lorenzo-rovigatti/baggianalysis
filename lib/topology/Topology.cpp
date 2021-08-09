/*
 * Topology.cpp
 *
 *  Created on: Nov 9, 2019
 *      Author: lorenzo
 */

#include "Topology.h"

#include "../System.h"
#include "../particles/Particle.h"
#include "../utils/clusters.h"

#include <algorithm>
#include <queue>

namespace ba {

Topology::Topology() :
				_N_in_system(0) {

}

Topology::Topology(std::shared_ptr<System> system) {
	for(auto p : system->particles()) {
		for(auto link : p->bonded_neighbours()) {
			auto q = link.other();
			if(p->index() < q->index()) {
				add_bond(link.type, p->index(), q->index());
			}
		}

		for(auto link : p->bonded_angles()) {
			int p = link[0]->index();
			int q = link[1]->index();
			int r = link[2]->index();
			add_angle(link.type, p, q, r);
		}

		for(auto link : p->bonded_dihedrals()) {
			int p = link[0]->index();
			int q = link[1]->index();
			int r = link[2]->index();
			int s = link[3]->index();
			add_dihedral(link.type, p, q, r, s);
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
	_build_clusters = true;
	_bonds.insert(TopologyBond(p, q));
}

void Topology::add_bond(std::string type, int p, int q) {
	_build_clusters = true;
	_bonds.insert(TopologyBond(type, p, q));
	_bond_types.insert(type);
}

void Topology::add_angle(int p, int q, int r) {
	_build_clusters = true;
	_angles.insert(TopologyAngle(p, q, r));
}

void Topology::add_angle(std::string type, int p, int q, int r) {
	_build_clusters = true;
	_angles.insert(TopologyAngle(type, p, q, r));
	_angle_types.insert(type);
}

void Topology::add_dihedral(int p, int q, int r, int s) {
	_build_clusters = true;
	_dihedrals.insert(TopologyDihedral(p, q, r, s));
}

void Topology::add_dihedral(std::string type, int p, int q, int r, int s) {
	_build_clusters = true;
	_dihedrals.insert(TopologyDihedral(type, p, q, r, s));
	_dihedral_types.insert(type);
}

void Topology::enable_checks() {
	_disable_checks = false;
}

void Topology::disable_checks() {
	_disable_checks = true;
}

void Topology::remove_unappliable_links(std::shared_ptr<System> system) {
	auto all_indexes = system->indexes();

	auto remover = [system](auto &link_set) {
		for(auto it = link_set.begin(); it != link_set.end();) {
			auto bond = *it;

			bool remove = false;
			for(auto p_idx : it->particles) {
				if(!system->id_exists(p_idx)) {
					remove = true;
				}
			}
			if(remove) {
				it = link_set.erase(it);
			}
			else {
				++it;
			}
		}
	};

	remover(_bonds);
	remover(_angles);
	remover(_dihedrals);
	_build_clusters = true;
}

void Topology::apply(std::shared_ptr<System> system) {
	// reset all the molecules
	system->molecules().clear();

	_set_bonded_links(system);

	if(_build_clusters) {
		_fill_clusters(system);
		_build_clusters = false;
	}

	if(_N_in_system != system->N() && !_disable_checks) {
		std::string error = fmt::format("The topology was initialised on a System containing {} particles and hence it cannot be applied to a System containing {} particles", _N_in_system, system->N());
		_raise_error(error);
	}

	for(auto &cluster : _clusters) {
		std::shared_ptr<ParticleSet> new_molecule = std::make_shared<ParticleSet>();
		for(auto index : cluster) {
			try {
				auto particle = system->particle_by_id(index);
				new_molecule->add_particle(particle);
				particle->set_molecule(new_molecule);
			}
			catch (std::runtime_error &e) {
				std::string error = fmt::format("The following error occurred while applying the topology to a System:\n\t'{}'", e.what());
				_raise_error(error);
			}
		}

		std::string mol_name = fmt::format("mol_{}", system->molecules().size());
		new_molecule->set_name(mol_name);
		system->molecules().push_back(new_molecule);
	}
}

const std::vector<std::set<int>> &Topology::clusters() const {
	return _clusters;
}

const std::set<TopologyBond> &Topology::bonds() const {
	return _bonds;
}

const std::set<std::string> &Topology::bond_types() const {
	return _bond_types;
}

const std::set<TopologyAngle> &Topology::angles() const {
	return _angles;
}

const std::set<std::string> &Topology::angle_types() const {
	return _angle_types;
}

const std::set<TopologyDihedral> &Topology::dihedrals() const {
	return _dihedrals;
}

const std::set<std::string> &Topology::dihedral_types() const {
	return _dihedral_types;
}

void Topology::_raise_error(std::string msg) {
	if(!_disable_checks) {
		throw std::runtime_error(msg);
	}
}

void Topology::_fill_clusters(std::shared_ptr<System> system) {
	_N_in_system = system->N();
	_clusters = utils::make_clusters(system, utils::ClusterPolicy::BONDED);

	if(_clusters.size() == 1) {
		BA_DEBUG("Topology: Found 1 cluster");
	}
	else {
		BA_DEBUG("Topology: Found {} clusters", _clusters.size());
	}
}

void Topology::_set_bonded_links(std::shared_ptr<System> system) {
	// add links (bonds, angles, dihedrals) to particles
	for(auto bond : _bonds) {
		auto p_idx = bond[0];
		auto q_idx = bond[1];

		try {
			auto p = system->particle_by_id(p_idx);
			auto q = system->particle_by_id(q_idx);
			p->add_bonded_neighbour(bond.type, q);
		}
		catch (std::runtime_error &e) {
			std::string error = fmt::format("The following error occurred while applying the topology to a System:\n\t'{}'", e.what());
			_raise_error(error);
		}
	}

	for(const TopologyAngle &angle : _angles) {
		try {
			ParticleAngle new_angle(angle.type, system->particle_by_id(angle[0]), system->particle_by_id(angle[1]), system->particle_by_id(angle[2]));
			for(int i = 0; i < 3; i++) {
				new_angle[i]->add_bonded_angle(new_angle);
			}
		}
		catch (std::runtime_error &e) {
			std::string error = fmt::format("The following error occurred while applying the topology to a System:\n\t'{}'", e.what());
			_raise_error(error);
		}
	}

	for(auto &dihedral : _dihedrals) {
		try {
			ParticleDihedral new_dihedral(dihedral.type, system->particle_by_id(dihedral[0]), system->particle_by_id(dihedral[1]), system->particle_by_id(dihedral[2]), system->particle_by_id(dihedral[3]));
			for(int i = 0; i < 4; i++) {
				new_dihedral[i]->add_bonded_dihedral(new_dihedral);
			}
		}
		catch (std::runtime_error &e) {
			std::string error = fmt::format("The following error occurred while applying the topology to a System:\n\t'{}'", e.what());
			_raise_error(error);
		}
	}
}

#ifdef PYTHON_BINDINGS

void export_Topology(py::module &m) {
	py::class_<TopologyBond, std::shared_ptr<TopologyBond>> topology_bond(m, "TopologyBond");
	topology_bond.def_readonly("particles", &TopologyBond::particles);
	topology_bond.def_readonly("type", &TopologyBond::type);

	py::class_<TopologyAngle, std::shared_ptr<TopologyAngle>> topology_angle(m, "TopologyAngle");
	topology_angle.def_readonly("particles", &TopologyAngle::particles);
	topology_angle.def_readonly("type", &TopologyAngle::type);

	py::class_<TopologyDihedral, std::shared_ptr<TopologyDihedral>> topology_dihedral(m, "TopologyDihedral");
	topology_dihedral.def_readonly("particles", &TopologyDihedral::particles);
	topology_dihedral.def_readonly("type", &TopologyDihedral::type);


	py::class_<Topology, std::shared_ptr<Topology>> topology(m, "Topology", R"pbdoc(
This class manages the connections and links between the particles of a system.

Here the term ``topology`` refers to the way atoms/particles are partitioned into clusters. In its simplest form, a topology is just a list of
links between particles. These links can be shared between two, three, four or more particles. While the latter are pretty rare, the others 
are quite common. Connections between two, three and four particles are here called `bonds`, `angles` and `dihedrals`. Each link has also associated
a link type, which is ``1`` by default.


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

	topology.def("add_bond", static_cast<void (Topology::*)(int, int)>(&Topology::add_bond), py::arg("p"), py::arg("q"), R"pbdoc(
        Adds a bond between a pair of particles.

        Parameters
        ----------
		p : int
			The index of the first particle of the pair.
		q : int
			The index of the second particle of the pair.
	)pbdoc");

	topology.def("add_angle", static_cast<void (Topology::*)(int, int, int)>(&Topology::add_angle), py::arg("p"), py::arg("q"), py::arg("r"), R"pbdoc(
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

	topology.def("add_dihedral", static_cast<void (Topology::*)(int, int, int, int)>(&Topology::add_dihedral), py::arg("p"), py::arg("q"), py::arg("r"), py::arg("s"), R"pbdoc(
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

	topology.def("remove_unappliable_links", &Topology::remove_unappliable_links, py::arg("system"), R"pbdoc(
Remove from the current topology all those links (bonds, angles, dihedrals) that refer to particles that do not exist in the given system. 
This procedure makes it so the given system will contain the correct set of molecules once :meth:`apply` is called on it. Note that this 
operation is **irreversible**.

Parameters
----------
system : :class:`System`
	The system whose particles will be used to identify the links to be removed.
	)pbdoc");

	topology.def("apply", &Topology::apply, py::arg("system"), R"pbdoc(
Applies the current topology to the given system, adding the bonds to the particles and partitioning them into clusters.

Parameters
----------
system : :class:`System`
	The target system.
	)pbdoc");

	topology.def_property_readonly("bonds", &Topology::bonds, R"pbdoc(
        List(:class:`TopologyBond`): The list of bonds stored in the topology.
	)pbdoc");

	topology.def_property_readonly("angles", &Topology::angles, R"pbdoc(
		List(:class:`TopologyAngle`): The list of angles stored in the topology.
	)pbdoc");

	topology.def_property_readonly("dihedrals", &Topology::dihedrals, R"pbdoc(
		List(:class:`TopologyDihedral`): The list of dihedrals stored in the topology.
	)pbdoc");

	topology.def_property_readonly("clusters", &Topology::clusters, R"pbdoc(
		List(Set(int)): The list of clusters stored in the topology. Each cluster is a set of particle ids that belong to that cluster.
	)pbdoc");
}

#endif

} /* namespace ba */
