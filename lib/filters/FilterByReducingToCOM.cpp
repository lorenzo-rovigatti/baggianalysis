/*
 * FilterByReducingToCOM.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "../filters/FilterByReducingToCOM.h"

using namespace std;

namespace ba {

FilterByReducingToCOM::FilterByReducingToCOM() {

}

FilterByReducingToCOM::~FilterByReducingToCOM() {

}

shared_ptr<System> FilterByReducingToCOM::filter(std::shared_ptr<const System> syst) {
	shared_ptr<System> new_syst(syst->empty_copy());

	shared_ptr<Particle> new_particle(new Particle(DEFAULT_PARTICLE_TYPE, syst->com(), syst->average_velocity()));
	new_syst->add_particle(new_particle);

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_FilterByReducingToCOM(py::module &m) {
	py::class_<FilterByReducingToCOM, BaseFilter, std::shared_ptr<FilterByReducingToCOM>> filter(m, "FilterByReducingToCOM");

	filter
		.def(py::init<>())
		.def("filter", &FilterByReducingToCOM::filter);
}

#endif

} /* namespace ba */
