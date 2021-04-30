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
	RDF(double bin_size, std::vector<particle_type> types_1, std::vector<particle_type> types_2);
	RDF(double bin_size, double max_value);
	RDF(double bin_size, double max_value, std::vector<particle_type> types_1, std::vector<particle_type> types_2);
	RDF() = delete;
	virtual ~RDF();

	void analyse_system(std::shared_ptr<System> system) override;

protected:
	bool _include(std::shared_ptr<Particle> p, std::shared_ptr<Particle> q);
	std::map<double, double> _finalised_result() override;

	double _bin_size = 0.;
	double _max_value = 0.;
	double _max_value_sqr = 0.;

	int _times_called = 0;

	std::vector<long double> _profile;
	std::pair<std::vector<particle_type>, std::vector<particle_type>> _allowed_types;
};

#ifdef PYTHON_BINDINGS

void export_RDF(py::module &m);

#endif

} /* namespace ba */

#endif /* OBSERVABLES_RDF_H_ */
