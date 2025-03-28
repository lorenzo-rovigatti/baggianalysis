/*
 * FixParticlePath.h
 *
 *  Created on: 01 feb 2019
 *      Author: lorenzo
 */

#ifndef FILTERS_FIXPARTICLEPATH_H_
#define FILTERS_FIXPARTICLEPATH_H_

#include "BaseFilter.h"

namespace ba {

using particle_shift = glm::ivec3;

class FixParticlePath: public BaseFilter {
public:
	FixParticlePath();
	virtual ~FixParticlePath();

	virtual std::shared_ptr<System> filter(std::shared_ptr<System> syst) override;

protected:
	std::shared_ptr<System> _previous_frame = nullptr;
	std::vector<particle_shift> _shifts;
};

void export_FixParticlePath(py::module &m);

} /* namespace ba */

#endif /* FILTERS_FIXPARTICLEPATH_H_ */
