/*
 * ClusterDistribution.cpp
 *
 *  Created on: Apr 3, 2021
 *      Author: lorenzo
 */

#include "ClusterDistribution.h"

#include "../utils/clusters.h"

namespace ba {

ClusterDistribution::ClusterDistribution() {

}

ClusterDistribution::~ClusterDistribution() {

}

void ClusterDistribution::analyse_system(std::shared_ptr<System> frame) {
	auto clusters = utils::make_clusters(frame, utils::ClusterPolicy::NONBONDED);

	_result.clear();
	for(const auto &cluster : clusters) {
		_result.push_back(cluster.size());
	}
}

#ifdef PYTHON_BINDINGS

void export_ClusterDistribution(py::module &m) {
	py::class_<ClusterDistribution, std::shared_ptr<ClusterDistribution>> obs(m, "ClusterDistribution");

	obs.def(py::init<>());

	PY_EXPORT_SYSTEM_OBS(obs, ClusterDistribution);
}

#endif

} /* namespace ba */
