/*
 * FilterByType.h
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#ifndef FILTERS_FILTERBYTYPE_H_
#define FILTERS_FILTERBYTYPE_H_

#include "../filters/BaseFilter.h"

namespace ba {

class FilterByType: public ba::BaseFilter {
public:
	FilterByType(std::vector<particle_type> allowed_types);
	virtual ~FilterByType();

	virtual std::shared_ptr<System> filter(std::shared_ptr<System> syst) override;

protected:
	std::vector<particle_type> _allowed_types;
};

} /* namespace ashell */

#endif /* FILTERS_FILTERBYTYPE_H_ */
