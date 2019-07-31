/*
 * BondOrderParameters.h
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_BONDORDERPARAMETERS_H_
#define OBSERVABLES_BONDORDERPARAMETERS_H_

#include "../System.h"

#include <set>

namespace ba {

class BondOrderParameters {
public:
	BondOrderParameters(std::set<int> orders_to_compute);
	virtual ~BondOrderParameters();

	std::vector<vector_scalar> compute(std::shared_ptr<System> frame);

private:
	vector_scalar _particle_bops(std::shared_ptr<Particle> p, const vec3 &box);

	std::set<int> _orders_to_compute;
};

#ifdef PYTHON_BINDINGS

void export_BondOrderParameters(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_BONDORDERPARAMETERS_H_ */
