/*
 * clusters.h
 *
 *  Created on: Apr 2, 2021
 *      Author: lorenzo
 */

#ifndef UTILS_CLUSTERS_H_
#define UTILS_CLUSTERS_H_

#include "../defs.h"

#include <set>

namespace ba {

class ParticleSet;

namespace utils {

enum ClusterPolicy {
	BONDED,
	NONBONDED,
	BOTH
};

std::vector<std::set<int>> make_clusters(std::shared_ptr<ParticleSet> ps, ClusterPolicy policy);

}

}

#endif /* UTILS_CLUSTERS_H_ */
