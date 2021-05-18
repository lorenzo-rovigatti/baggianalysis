/*
 * clusters.cpp
 *
 *  Created on: Apr 2, 2021
 *      Author: lorenzo
 */

#include "clusters.h"

#include "../particles/ParticleSet.h"
#include "../particles/Particle.h"

#include <map>
#include <queue>

namespace ba {

namespace utils {

std::vector<std::set<int>> make_clusters(std::shared_ptr<ParticleSet> ps, ClusterPolicy policy) {
	std::vector<std::set<int>> clusters;

	std::function<std::set<ParticleBond>(std::shared_ptr<Particle>)> neigh_function;

	switch(policy) {
	case ClusterPolicy::BONDED:
		neigh_function = [](std::shared_ptr<Particle> p) {
			return p->bonded_neighbours();
		};
		break;
	case ClusterPolicy::NONBONDED:
		neigh_function = [](std::shared_ptr<Particle> p) {
			return p->neighbours();
		};
		break;
	case ClusterPolicy::BOTH:
		neigh_function = [](std::shared_ptr<Particle> p) {
			auto neighbours = p->neighbours();
			auto bonded_neighbours = p->bonded_neighbours();
			std::set<ParticleBond> all_neighs;
			std::set_union(neighbours.cbegin(), neighbours.cend(), bonded_neighbours.cbegin(), bonded_neighbours.cend(), std::inserter(all_neighs, all_neighs.begin()));
			return all_neighs;
		};
		break;
	default:
		break;
	}

	int N = ps->N();
	if(N == 0) {
		return clusters;
	}

	// these maps might be substituted by a single boost::bimap
	std::map<int, int> index_to_cluster;
	std::map<int, std::set<int>> cluster_to_molecule;

	for(auto index : ps->indexes()) {
		index_to_cluster[index] = index;
	}

	// this will contain the particles whose neighbours we have to loop on
	std::queue<int> next;
	auto curr_it = ps->particles().begin();
	next.push((*curr_it)->index());
	bool done = false;
	while(!done) {
		auto p = ps->particle_by_id(next.front());
		next.pop();
		for(auto link : neigh_function(p)) {
			auto neigh = link.other();
			if(index_to_cluster[neigh->index()] > index_to_cluster[p->index()]) {
				index_to_cluster[neigh->index()] = index_to_cluster[p->index()];
				next.push(neigh->index());
			}
		}
		if(next.empty()) {
			curr_it++;
			if(curr_it == ps->particles().end()) {
				done = true;
			}
			else {
				next.push((*curr_it)->index());
			}
		}
	}

	// create a cluster -> molecule map
	for(auto particle : ps->particles()) {
		int cluster = index_to_cluster[particle->index()];
		if(cluster_to_molecule.find(cluster) == cluster_to_molecule.end()) {
			cluster_to_molecule[cluster] = std::set<int>( { particle->index() });
		}
		else {
			cluster_to_molecule[cluster].insert(particle->index());
		}
	}

	for(auto pair : cluster_to_molecule) {
		clusters.emplace_back(pair.second);
	}

	return clusters;
}

}

}
