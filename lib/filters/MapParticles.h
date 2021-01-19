/*
 * MapParticles.h
 *
 *  Created on: Jan 19, 2021
 *      Author: lorenzo
 */

#ifndef FILTERS_MAPPARTICLES_H_
#define FILTERS_MAPPARTICLES_H_

#include "BaseFilter.h"

namespace ba {

class MapParticles: public virtual BaseFilter {
public:
	MapParticles(std::vector<std::vector<int>> id_lists);
	virtual ~MapParticles();

	virtual std::shared_ptr<System> filter(std::shared_ptr<const System> syst) override;

private:
	std::vector<std::vector<int>> _id_lists;
};

#ifdef PYTHON_BINDINGS

void export_MapParticles(py::module &m);

#endif

} /* namespace ba */

#endif /* FILTERS_MAPPARTICLES_H_ */
