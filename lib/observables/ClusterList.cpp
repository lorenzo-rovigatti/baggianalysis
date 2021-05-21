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
	_clusters = utils::make_clusters(frame, utils::ClusterPolicy::NONBONDED);

	_result.clear();
	for(const auto &cluster : _clusters) {
		_result.push_back(cluster.size());
	}
}

const std::vector<std::set<int>> &ClusterList::clusters() const {
	return _clusters;
}

#ifdef PYTHON_BINDINGS

void export_ClusterList(py::module &m) {
	py::class_<ClusterList, std::shared_ptr<ClusterList>> obs(m, "ClusterList", "Partition particles into clusters and return their size.");

	obs.def(py::init<>(), "The default constructor takes no parameters");
	obs.def_property_readonly("clusters", &ClusterList::clusters, "Return the list of clusters of the system analysed last.");

	PY_EXPORT_SYSTEM_OBS(obs, ClusterList);
}

#endif

} /* namespace ba */
