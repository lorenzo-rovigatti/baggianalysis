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

struct ConvexHullTriangle {
	ConvexHullTriangle(vec3 &p1, vec3 &p2, vec3 &p3, vec3 &nnormal) {
		v1 = p1;
		v2 = p2;
		v3 = p3;
		normal = nnormal;
	}

	vec3 v1, v2, v3;
	vec3 normal;
};

struct ConvexHullResult {
	double volume;
	double area;
	std::vector<vec3> vertices;
	std::vector<ConvexHullTriangle> triangles;
};

class ConvexHull: public SystemObservable<ConvexHullResult> {
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
