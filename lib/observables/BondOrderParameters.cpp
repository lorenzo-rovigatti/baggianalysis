/*
 * BondOrderParameters.cpp
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#include "BondOrderParameters.h"

#include <boost/math/special_functions/spherical_harmonic.hpp>

namespace ba {

BondOrderParameters::BondOrderParameters(std::set<int> orders_to_compute) :
				_orders_to_compute(orders_to_compute) {

}

BondOrderParameters::~BondOrderParameters() {

}

std::vector<vector_scalar> BondOrderParameters::compute(std::shared_ptr<System> frame) {
	std::vector<vector_scalar> results;

	for(auto p : frame->particles()) {
		results.emplace_back(_particle_bops(p, frame->box));
	}

	return results;
}

vector_scalar BondOrderParameters::_particle_bops(std::shared_ptr<Particle> p, const vec3 &box) {
	vector_scalar result;

	for(auto q : p->neighbours()) {

	}

	return result;
}

#ifdef PYTHON_BINDINGS

void export_BondOrderParameters(py::module &m) {
	py::class_<BondOrderParameters, std::shared_ptr<BondOrderParameters>> parser(m, "BondOrderParameters");

	parser
		.def(py::init<std::set<int>>())
		.def("compute", &BondOrderParameters::compute);
}

#endif

} /* namespace ba */
