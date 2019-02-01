/*
 * System.h
 *
 *  Created on: 25 gen 2019
 *      Author: lorenzo
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "Particles.h"

namespace ba {

class System {
public:
	System();
	virtual ~System();

	std::shared_ptr<System> empty_copy() const;

	vec3 com() const;
	vec3 average_velocity() const;

public:
	ullint time = 0;
	Particles particles;
};

} /* namespace ba */

#endif /* SYSTEM_H_ */
