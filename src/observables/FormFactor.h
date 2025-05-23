/*
 * FormFactor.h
 *
 *  Created on: 20 dic 2019
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_FORMFACTOR_H_
#define OBSERVABLES_FORMFACTOR_H_

#include "interfaces/SystemObservable.h"

namespace ba {

class FormFactor: public SystemObservable<std::map<double, double>> {
public:
	FormFactor(vector_scalar q_modules, int q_repetitions, bool compute_on_molecules);
	FormFactor() = delete;
	virtual ~FormFactor();

	void clear_b_factors();
	void set_b_factors(vector_scalar b_factors);

	void analyse_system(std::shared_ptr<System> system) override;
	void analyse_particle_set(std::shared_ptr<ParticleSet> p_set);

protected:
	std::map<double, double> _finalised_result() override;

	vector_scalar _q_modules;
	int _q_repetitions;
	bool _compute_on_molecules;
	vector_scalar _b_factors;
};

void export_FormFactor(py::module &m);

} /* namespace ba */

#endif /* OBSERVABLES_FORMFACTOR_H_ */
