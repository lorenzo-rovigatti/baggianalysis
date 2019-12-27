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
	static std::uniform_real_distribution<double> dist(0., 1.);
	return dist(_rng);
}

vec3 Random::random_vector_on_sphere() {
	double ransq = 1.;
	double ran1, ran2;

	while(ransq >= 1) {
		ran1 = 1. - 2. * uniform();
		ran2 = 1. - 2. * uniform();
		ransq = ran1 * ran1 + ran2 * ran2;
	}

	double ranh = 2. * sqrt(1. - ransq);
	return vec3(ran1 * ranh, ran2 * ranh, 1. - 2. * ransq);
}

} /* namespace ba */
