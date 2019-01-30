/*
 * FilterByReducingToCOM.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "../filters/FilterByReducingToCOM.h"

using namespace std;

namespace ba {

FilterByReducingToCOM::FilterByReducingToCOM() {

}

FilterByReducingToCOM::~FilterByReducingToCOM() {

}

shared_ptr<System> FilterByReducingToCOM::filter(std::shared_ptr<System> syst) {
	shared_ptr<System> new_syst(syst->empty_copy());
	new_syst->particles.types.push_back(DEFAULT_PARTICLE_TYPE);
	new_syst->particles.positions.push_back(syst->com());
	new_syst->particles.velocities.push_back(syst->average_velocity());

	return new_syst;
}

} /* namespace ba */
