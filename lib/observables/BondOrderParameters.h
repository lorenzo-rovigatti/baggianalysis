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
#include <map>
#include <complex>

#include "interfaces/SystemObservable.h"

namespace ba {

using particle_bops = std::map<int, std::vector<std::complex<double>>>;

class BondOrderParameters: public SystemObservable<std::vector<vector_scalar>> {
public:
	BondOrderParameters(std::set<int> orders_to_compute, bool compute_bar_qs = true, bool compute_qs = false, bool compute_bar_ws = false, bool compute_ws = false);
	BondOrderParameters() = delete;
	virtual ~BondOrderParameters();

	void analyse_system(std::shared_ptr<System> frame) override;

private:
	void _set_particle_bops(std::shared_ptr<Particle> p, particle_bops &bops, const vec3 &box);

	std::set<int> _orders_to_compute;
	bool _compute_avg_qs = true;
	bool _compute_qs = false;
	bool _compute_avg_ws = false;
	bool _compute_ws = false;
};

#ifdef PYTHON_BINDINGS

void export_BondOrderParameters(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_BONDORDERPARAMETERS_H_ */
