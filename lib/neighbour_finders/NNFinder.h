/*
 * NNFinder.h
 *
 *  Created on: Jul 30, 2019
 *      Author: lorenzo
 */

#ifndef NEIGHBOUR_FINDERS_NNFINDER_H_
#define NEIGHBOUR_FINDERS_NNFINDER_H_

#include "../particles/Particle.h"

namespace ba {

class NNFinder {
public:
	NNFinder();
	virtual ~NNFinder();

	virtual void set_neighbours(std::vector<std::shared_ptr<Particle>>) = 0;
};

} /* namespace ba */

#endif /* NEIGHBOUR_FINDERS_NNFINDER_H_ */
