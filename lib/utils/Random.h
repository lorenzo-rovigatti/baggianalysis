/*
 * Random.h
 *
 *  Created on: Jul 18, 2019
 *      Author: lorenzo
 */

#ifndef UTILS_RANDOM_H_
#define UTILS_RANDOM_H_

#define BARANDOM (ba::Random::instance())

#include "../defs.h"

#include <random>

namespace ba {

class Random {
public:
	Random(const Random&) = delete;
	Random& operator=(const Random &) = delete;
	Random(Random &&) = delete;
	Random &operator=(Random) = delete;
	virtual ~Random();

	static Random &instance();

	void reset_seed(unsigned int);
	double uniform();
	vec3 random_vector_on_sphere();

private:
	Random();

	std::mt19937 _rng;
};

} /* namespace ba */

#endif /* UTILS_RANDOM_H_ */
