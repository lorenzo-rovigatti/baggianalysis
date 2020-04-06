/*
 * BaseFilter.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "../filters/BaseFilter.h"

namespace ba {

BaseFilter::BaseFilter() {

}

BaseFilter::~BaseFilter() {

}

#ifdef PYTHON_BINDINGS

void export_BaseFilter(py::module &m) {
	py::class_<BaseFilter, PyBaseFilter, std::shared_ptr<BaseFilter>> filter(m, "BaseFilter");

	filter.def(py::init<>());
	filter.def("filter", &BaseFilter::filter, py::arg("system"), R"pbdoc(
        Returns a copy of the given system, filtered according to some criterion. This method should be overridden by
        child classes.

        Parameters
        ----------
        system : :class:`System`
            The input system.

        Returns
        -------
        :class:`System`
            A system that has the same timestep and box size of the input one but different particles, depending on the type of filter applied.
	)pbdoc");
}

#endif

} /* namespace ashell */
