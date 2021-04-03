/*
 * ClusterDistribution.cpp
 *
 *  Created on: Apr 3, 2021
 *      Author: lorenzo
 */

#include "ClusterList.h"

#include "../utils/clusters.h"

namespace ba {

ClusterList::ClusterList() {

}

ClusterList::~ClusterList() {

}

void ClusterList::analyse_system(std::shared_ptr<System> frame) {
	auto clusters = utils::make_clusters(frame, utils::ClusterPolicy::NONBONDED);

	_result.clear();
	for(const auto &cluster : clusters) {
		_result.push_back(cluster.size());
	}
}

#ifdef PYTHON_BINDINGS

void export_ClusterList(py::module &m) {
	py::class_<ClusterList, std::shared_ptr<ClusterList>> obs(m, "ClusterList", "Partition particles into clusters and return their size.");

	obs.def(py::init<>(), "The default constructor takes no parameters");

	PY_EXPORT_SYSTEM_OBS(obs, ClusterList);
}

#endif

} /* namespace ba */
