/*
 * SubtractCOM.cpp
 *
 *  Created on: 30 gen 2019
 *      Author: lorenzo
 */

#include "SubtractCOM.h"

using namespace std;

namespace ba {

SubtractCOM::SubtractCOM() {

}

SubtractCOM::~SubtractCOM() {

}

shared_ptr<System> SubtractCOM::filter(std::shared_ptr<const System> syst) {
	shared_ptr<System> new_syst(syst->empty_copy());

	vec3 com = syst->com();
	vec3 v_avg = syst->average_velocity();

	for(uint i = 0; i < syst->particles.N(); i++) {
		new_syst->particles.types().push_back(syst->particles.types()[i]);
		new_syst->particles.positions().push_back(syst->particles.positions()[i] - com);
		new_syst->particles.velocities().push_back(syst->particles.velocities()[i] - v_avg);
	}

	return new_syst;
}

} /* namespace ba */
