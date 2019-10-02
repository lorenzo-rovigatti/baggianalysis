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

	filter
		.def(py::init<>())
		.def("filter", &BaseFilter::filter);
}

#endif

} /* namespace ashell */
