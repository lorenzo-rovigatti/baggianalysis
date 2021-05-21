/*
 * ClusterList.h
 *
 *  Created on: Apr 3, 2021
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_CLUSTERLIST_H_
#define OBSERVABLES_CLUSTERLIST_H_

#include "interfaces/SystemObservable.h"

namespace ba {

class ClusterList: public SystemObservable<std::vector<int>> {
public:
	ClusterList();
	virtual ~ClusterList();

	void analyse_system(std::shared_ptr<System> frame) override;

	const std::vector<std::set<int>> &clusters() const;

private:
	std::vector<std::set<int>> _clusters;
};

#ifdef PYTHON_BINDINGS

void export_ClusterList(py::module &m);

#endif


} /* namespace ba */

#endif /* OBSERVABLES_CLUSTERLIST_H_ */
