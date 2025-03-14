/*
 * MolecularCOMs.h
 *
 *  Created on: 1 apr 2021
 *      Author: lorenzo
 */

#ifndef FILTERS_MOLECULARCOMS_H_
#define FILTERS_MOLECULARCOMS_H_

#include "BaseFilter.h"

namespace ba {

class MolecularCOMs: public BaseFilter {
public:
	MolecularCOMs();
	virtual ~MolecularCOMs();

	std::shared_ptr<System> filter(std::shared_ptr<System> syst) override;
};

void export_MolecularCOMs(py::module &m);

} /* namespace ba */

#endif /* FILTERS_MOLECULARCOMS_H_ */
