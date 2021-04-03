/*
 * ClusterDistribution.h
 *
 *  Created on: Apr 3, 2021
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_CLUSTERDISTRIBUTION_H_
#define OBSERVABLES_CLUSTERDISTRIBUTION_H_

#include "interfaces/SystemObservable.h"

namespace ba {

class ClusterDistribution: public SystemObservable<std::vector<int>> {
public:
	ClusterDistribution();
	virtual ~ClusterDistribution();

	void analyse_system(std::shared_ptr<System> frame) override;
};

#ifdef PYTHON_BINDINGS

void export_ClusterDistribution(py::module &m);

#endif


} /* namespace ba */

#endif /* OBSERVABLES_CLUSTERDISTRIBUTION_H_ */
