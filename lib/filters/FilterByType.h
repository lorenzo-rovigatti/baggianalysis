/*
 * FilterByType.h
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#ifndef FILTERS_FILTERBYTYPE_H_
#define FILTERS_FILTERBYTYPE_H_

#include "BaseFilter.h"

namespace ba {

class FilterByFunction;

class FilterByType: public ba::BaseFilter {
public:
	FilterByType(std::vector<particle_type> allowed_types);
	virtual ~FilterByType();

	virtual std::shared_ptr<System> filter(std::shared_ptr<const System> syst) override;

protected:
	std::shared_ptr<FilterByFunction> _filter;
};

#ifdef PYTHON_BINDINGS

void export_FilterByType(py::module &m);

#endif

} /* namespace ashell */

#endif /* FILTERS_FILTERBYTYPE_H_ */
