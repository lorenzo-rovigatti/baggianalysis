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

	virtual std::shared_ptr<System> filter(std::shared_ptr<System> syst) override;

private:
	std::vector<std::vector<int>> _id_lists;
};

void export_MapParticles(py::module &m);

} /* namespace ba */

#endif /* FILTERS_MAPPARTICLES_H_ */
