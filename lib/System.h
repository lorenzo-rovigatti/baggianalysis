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

	Particles particles;

protected:

};

} /* namespace ba */

#endif /* SYSTEM_H_ */
