/*
 * CutoffFinder.h
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#ifndef NEIGHBOUR_FINDERS_CUTOFFFINDER_H_
#define NEIGHBOUR_FINDERS_CUTOFFFINDER_H_

#include "NNFinder.h"

namespace ba {

class CutoffFinder: public NNFinder {
public:
	CutoffFinder();
	virtual ~CutoffFinder();

	virtual void set_neighbours(std::vector<std::shared_ptr<Particle>> finder);
};

} /* namespace ba */

#endif /* NEIGHBOUR_FINDERS_CUTOFFFINDER_H_ */
