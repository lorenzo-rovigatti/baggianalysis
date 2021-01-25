/*
 * FilterByType.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "../filters/FilterByType.h"

#include "FilterByFunction.h"

#include <algorithm>

namespace ba {

FilterByType::FilterByType(std::vector<particle_type> allowed_types) :
				BaseFilter() {

	FilterType filter_func = [allowed_types](Particle *p) {
		return std::find(allowed_types.begin(), allowed_types.end(), p->type()) != allowed_types.end();
	};
	_filter = std::make_shared<FilterByFunction>(filter_func);
}

FilterByType::~FilterByType() {

}

std::shared_ptr<System> FilterByType::filter(std::shared_ptr<const System> syst) {
	return _filter->filter(syst);
}

#ifdef PYTHON_BINDINGS

void export_FilterByType(py::module &m) {
	py::class_<FilterByType, BaseFilter, std::shared_ptr<FilterByType>> filter(m, "FilterByType", R"pbdoc(
A filter that builds a new system by including only those particles whose type is in the list provided by the user.

As an example, the following snippet creates a filter that will include in the new system only particles of type
"0" and "3"::

    allowed_types = ["0", "3"]
    my_filter = ba.FilterByType(allowed_types)

	)pbdoc");

	filter.def(py::init<std::vector<particle_type>>(), R"pbdoc(
        The constructor takes a list of particle types as its only parameter.

        Parameters
        ----------
        allowed_types : List(str)
            The list of particle types that should be included in the new system.
	)pbdoc");
}

#endif

} /* namespace ashell */
