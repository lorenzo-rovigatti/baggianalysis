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
	new_syst->particles.types().push_back(DEFAULT_PARTICLE_TYPE);
	new_syst->particles.positions().push_back(syst->com());
	new_syst->particles.velocities().push_back(syst->average_velocity());

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_FilterByReducingToCOM(py::module &m) {
	pybind11::class_<FilterByReducingToCOM, BaseFilter, std::shared_ptr<FilterByReducingToCOM>> filter(m, "FilterByReducingToCOM");

	filter
		.def(py::init<>())
		.def("filter", &FilterByReducingToCOM::filter);
}

#endif

} /* namespace ba */
