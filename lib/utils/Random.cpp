/*
 * Random.cpp
 *
 *  Created on: Jul 18, 2019
 *      Author: lorenzo
 */

#include "Random.h"

namespace ba {

Random::Random() {
	// deterministic runs for debugging purposes, use dev() to "randomly" initialise the rng
	_rng = std::mt19937(std::random_device{}());
}

Random::~Random() {

}

Random &Random::instance() {
	static Random random;
	return random;
}

void Random::reset_seed(unsigned int seed) {
	_rng = std::mt19937(seed);
}

double Random::uniform() {
	static std::uniform_int_distribution<int> dist(0., 1.);
	return dist(_rng);
}

} /* namespace ba */
