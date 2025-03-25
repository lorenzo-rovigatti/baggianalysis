/*
 * FilterById.h
 *
 *  Created on: 25 gen 2021
 *      Author: lorenzo
 */

#ifndef FILTERS_FILTERBYID_H_
#define FILTERS_FILTERBYID_H_

#include "BaseFilter.h"

namespace ba {

class FilterByFunction;

class FilterById: public ba::BaseFilter {
public:
	FilterById(std::vector<int> ids);
	virtual ~FilterById();

	virtual std::shared_ptr<System> filter(std::shared_ptr<System> syst) override;

protected:
	std::shared_ptr<FilterByFunction> _filter;
};

void export_FilterById(py::module &m);

} /* namespace ashell */

#endif /* FILTERS_FILTERBYID_H_ */
