/*
 * LambdaFilter.h
 *
 *  Created on: Sep 6, 2019
 *      Author: lorenzo
 */

#ifndef FILTERS_FILTERBYFUNCTION_H_
#define FILTERS_FILTERBYFUNCTION_H_

#include "BaseFilter.h"

namespace ba {

using FilterType = std::function<bool(Particle *)>;

class FilterByFunction: public BaseFilter {
public:
	FilterByFunction(FilterType);
	virtual ~FilterByFunction();

	virtual std::shared_ptr<System> filter(std::shared_ptr<const System> syst) override;

private:
	FilterType _filter_function;
};

#ifdef PYTHON_BINDINGS

void export_FilterByFunction(py::module &m);

#endif

} /* namespace ba */

#endif /* FILTERS_FILTERBYFUNCTION_H_ */
