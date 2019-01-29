/*
 * System.cpp
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#include "System.h"

namespace ba {

System::System() {

}

System::~System() {

}

vec3 System::com() {
	vec3 com(0., 0., 0.);
	for(auto &pos : particles.positions) {
		com += pos;
	}
	com /= particles.positions.size();
	return com;
}

} /* namespace ba */
