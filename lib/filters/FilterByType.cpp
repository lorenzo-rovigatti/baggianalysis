/*
 * FilterByType.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "../filters/FilterByType.h"

#include <algorithm>

using namespace std;

namespace ba {

FilterByType::FilterByType(std::vector<particle_type> allowed_types) :
				BaseFilter(),
				_allowed_types(allowed_types) {

}

FilterByType::~FilterByType() {

}

shared_ptr<System> FilterByType::filter(std::shared_ptr<System> syst) {
	shared_ptr<System> new_syst(syst->empty_copy());

	for(uint i = 0; i < syst->particles.N(); i++) {
		particle_type p_type = syst->particles.types[i];
		if(find(_allowed_types.begin(), _allowed_types.end(), p_type) != _allowed_types.end()) {
			new_syst->particles.types.push_back(p_type);
			new_syst->particles.positions.push_back(syst->particles.positions[i]);
			new_syst->particles.velocities.push_back(syst->particles.velocities[i]);
		}
	}

	return new_syst;
}

} /* namespace ashell */
