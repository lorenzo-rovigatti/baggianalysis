/*
 * FilterById.cpp
 *
 *  Created on: 25 gen 2021
 *      Author: lorenzo
 */

#include "../filters/FilterById.h"

#include "FilterByFunction.h"

#include <algorithm>

namespace ba {

FilterById::FilterById(std::vector<int> ids) :
				BaseFilter() {

	FilterType filter_func = [ids](Particle *p) {
		return std::find(ids.begin(), ids.end(), p->index()) != ids.end();
	};
	_filter = std::make_shared<FilterByFunction>(filter_func);
}

FilterById::~FilterById() {

}

std::shared_ptr<System> FilterById::filter(std::shared_ptr<const System> syst) {
	return _filter->filter(syst);
}

#ifdef PYTHON_BINDINGS

void export_FilterById(py::module &m) {
	py::class_<FilterById, BaseFilter, std::shared_ptr<FilterById>> filter(m, "FilterById", R"pbdoc(
A filter that builds a new system by including only those particles whose index is in the list provided by the user.

As an example, the following snippet creates a filter that will include in the new system only particles with index
1, 13 and 102::

    allowed_ids = [1, 13, 102]
    my_filter = ba.FilterById(allowed_ids)

	)pbdoc");

	filter.def(py::init<std::vector<int>>(), R"pbdoc(
        The constructor takes a list of indexes as its only parameter.

        Parameters
        ----------
        allowed_ids : List(int)
            The list of particle indexes that should be included in the new system.
	)pbdoc");
}

#endif

} /* namespace ashell */
