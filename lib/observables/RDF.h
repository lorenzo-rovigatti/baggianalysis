/*
 * RDF.h
 *
 *  Created on: 29 apr 2021
 *      Author: lorenzo
 */

#ifndef OBSERVABLES_RDF_H_
#define OBSERVABLES_RDF_H_

#include "interfaces/SystemObservable.h"

namespace ba {

class RDF: public SystemObservable<std::map<double, double>> {
public:
	RDF(double bin_size);
	RDF(double bin_size, double max_value);
	RDF() = delete;
	virtual ~RDF();

	void analyse_system(std::shared_ptr<System> system) override;

protected:
	std::map<double, double> _finalised_result() override;

	double _bin_size = 0.;
	double _max_value = 0.;
	double _max_value_sqr = 0.;

	int _times_called = 0;

	std::vector<long double> _profile;
};

#ifdef PYTHON_BINDINGS

void export_RDF(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_RDF_H_ */
