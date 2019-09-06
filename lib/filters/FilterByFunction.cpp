/*
 * LambdaFilter.cpp
 *
 *  Created on: Sep 6, 2019
 *      Author: lorenzo
 */

#include "FilterByFunction.h"

namespace ba {

FilterByFunction::FilterByFunction(FilterType f) :
				BaseFilter(),
				_filter_function(f) {

}

FilterByFunction::~FilterByFunction() {

}

std::shared_ptr<System> FilterByFunction::filter(std::shared_ptr<const System> syst) {
	std::shared_ptr<System> new_syst(syst->empty_copy());

	for(auto p : syst->particles()) {
		if(_filter_function(p.get())) {
			std::shared_ptr<Particle> new_particle(new Particle(*p.get()));
			new_syst->add_particle(new_particle);
		}
	}

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_FilterByFunction(py::module &m) {
	pybind11::class_<FilterByFunction, BaseFilter, std::shared_ptr<FilterByFunction>> filter(m, "FilterByFunction");

	filter
		.def(py::init<FilterType>())
		.def("filter", &FilterByFunction::filter);
}

#endif

} /* namespace ba */
