/*
 * ConvexHull.h
 *
 *  Created on: 13 gen 2020
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_CONVEXHULL_H_
#define OBSERVABLES_CONVEXHULL_H_

#include "interfaces/SystemObservable.h"

namespace ba {

struct ConvexHullResult {
	double volume = 0.;
	double area = 0.;
};

class ConvexHull: public SystemObservable<std::vector<ConvexHullResult>> {
public:
	ConvexHull();
	virtual ~ConvexHull();

	void analyse_system(std::shared_ptr<System> system) override;
};

#ifdef PYTHON_BINDINGS

void export_ConvexHull(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_CONVEXHULL_H_ */
