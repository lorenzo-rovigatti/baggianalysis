/*
 * SubtractCOM.h
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#ifndef FILTERS_SUBTRACTCOM_H_
#define FILTERS_SUBTRACTCOM_H_

#include "BaseFilter.h"

namespace ba {

class SubtractCOM: public BaseFilter {
public:
	SubtractCOM();
	virtual ~SubtractCOM();

	virtual std::shared_ptr<System> filter(std::shared_ptr<const System> syst) override;
};

#ifdef PYTHON_BINDINGS

void export_SubtractCOM(py::module &m);

#endif

} /* namespace ba */

#endif /* FILTERS_SUBTRACTCOM_H_ */
