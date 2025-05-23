/*
 * FilterByReducingToCOM.h
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#ifndef FILTERS_FILTERBYREDUCINGTOCOM_H_
#define FILTERS_FILTERBYREDUCINGTOCOM_H_

#include "../filters/BaseFilter.h"

namespace ba {

class FilterByReducingToCOM: public BaseFilter {
public:
	FilterByReducingToCOM();
	virtual ~FilterByReducingToCOM();

	virtual std::shared_ptr<System> filter(std::shared_ptr<System> syst) override;
};

void export_FilterByReducingToCOM(py::module &m);

} /* namespace ba */

#endif /* FILTERS_FILTERBYREDUCINGTOCOM_H_ */
