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
			std::shared_ptr<Particle> new_particle(std::make_shared<Particle>(*p.get()));
			new_syst->add_particle(new_particle);
		}
	}

	return new_syst;
}

#ifdef PYTHON_BINDINGS

void export_FilterByFunction(py::module &m) {
	py::class_<FilterByFunction, BaseFilter, std::shared_ptr<FilterByFunction>> filter(m, "FilterByFunction", R"pbdoc(
        Filter that uses a user-provided callable to choose which particles will be included in the new system
	)pbdoc");

	filter.def(py::init<FilterType>(), R"pbdoc(
        The constructor takes as a parameter a callable that will be used to decided which particles will be included in the new system.

        Parameters
        ----------
        function : callable
            A callable that takes a particle and returns True if the particle should be included in the new system, False otherwise.
	)pbdoc");
}

#endif

} /* namespace ba */
