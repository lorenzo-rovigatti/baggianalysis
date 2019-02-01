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

	virtual std::shared_ptr<System> filter(std::shared_ptr<const System> syst) = 0;
};

} /* namespace ashell */

#endif /* FILTERS_BASEFILTER_H_ */
