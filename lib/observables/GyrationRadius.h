/*
 * GyrationRadius.h
 *
 *  Created on: 01 feb 2022
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_GYRATIONRADIUS_H_
#define OBSERVABLES_GYRATIONRADIUS_H_

#include "interfaces/SystemObservable.h"

namespace ba {

class GyrationRadius: public SystemObservable<double> {
public:
	GyrationRadius(bool compute_on_molecules);
	GyrationRadius() = delete;
	virtual ~GyrationRadius();

	void analyse_system(std::shared_ptr<System> system) override;
	void analyse_particle_set(std::shared_ptr<ParticleSet> p_set, const vec3 &box);

protected:
	double _finalised_result() override;

	bool _compute_on_molecules;
};

#ifdef PYTHON_BINDINGS

void export_GyrationRadius(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_GYRATIONRADIUS_H_ */
