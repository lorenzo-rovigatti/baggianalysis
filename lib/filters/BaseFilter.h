/*
 * BaseFilter.h
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#ifndef FILTERS_BASEFILTER_H_
#define FILTERS_BASEFILTER_H_

#include "../System.h"

namespace ba {

class BaseFilter {
public:
	BaseFilter();
	virtual ~BaseFilter();

	/**
	 * @brief Build a new System by applying some procedure to an already existing System.
	 *
	 * @param syst the System the filter will be applied to
	 * @return a copy of the System, filtered according to some procedure
	 */
	virtual std::shared_ptr<System> filter(std::shared_ptr<const System> syst) = 0;
};

#ifdef PYTHON_BINDINGS

/**
 * @brief Trampoline class for BaseFilter.
 */
class PyBaseFilter : public BaseFilter {
public:
	using BaseFilter::BaseFilter;

	std::shared_ptr<System> filter(std::shared_ptr<const System> syst) override {
		PYBIND11_OVERLOAD_PURE(
				std::shared_ptr<System>,
				BaseFilter,
				filter,
				syst
		);

		// suppress warnings
		return std::shared_ptr<System>(std::make_shared<System>());
	}
};

void export_BaseFilter(py::module &m);

#endif

} /* namespace ashell */

#endif /* FILTERS_BASEFILTER_H_ */
