/*
 * StructureFactor.h
 *
 *  Created on: 30 jul 2020
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_STRUCTUREFACTOR_H_
#define OBSERVABLES_STRUCTUREFACTOR_H_

#include "interfaces/SystemObservable.h"

namespace ba {

struct DestructuredStructureFactor {
	vector_scalar q_modules;
	std::vector<std::vector<vector_scalar>> cos_contribs;
	std::vector<std::vector<vector_scalar>> sin_contribs;
};

class StructureFactor: public SystemObservable<std::map<double, double>> {
public:
	StructureFactor(double largest_q, uint max_n_realisations, double max_delta_q);
	StructureFactor() = delete;
	virtual ~StructureFactor();

	void clear_b_factors();
	void set_b_factors(vector_scalar b_factors);

	DestructuredStructureFactor destructured_from_system(std::shared_ptr<System> system);

	void analyse_system(std::shared_ptr<System> system) override;

protected:
	std::map<double, double> _finalised_result() override;

	void _init_qs(std::shared_ptr<System> system);

	double _largest_q;
	uint _max_n_realisations;
	double _max_delta_q;

	std::map<double, std::vector<vec3>> _q_vectors;
	vec3 _last_box = vec3(0., 0., 0.);
	vector_scalar _b_factors;

	int _times_called = 0;
};

#ifdef PYTHON_BINDINGS

void export_StructureFactor(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_STRUCTUREFACTOR_H_ */
