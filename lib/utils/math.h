/*
 * math.h
 *
 *  Created on: Jun 21, 2023
 *      Author: lorenzo
 */

#ifndef UTILS_MATH_H_
#define UTILS_MATH_H_

namespace ba {

namespace utils {

// see https://stackoverflow.com/a/28050328/5140209 and https://github.com/AZHenley/cosine/pull/2/files#diff-d44422df098eaa7b32304c90397ee2994e1e0370a03c57b236de4afe0612ce21
template<typename T>
inline T cos(T x) noexcept {
	constexpr T tp = 1. / (2. * M_PI);
	x *= tp;
	x -= T(.25) + std::floor(x + T(.25));
	x *= T(16.) * (std::abs(x) - T(.5));
	x += T(.225) * x * (std::abs(x) - T(1.));
	return x;
}

template<typename T>
inline T sin(T x) noexcept {
	constexpr T tp = -1. / (2. * M_PI);
	x *= tp;
	x -= std::floor(x + T(.5));
	x *= T(16.) * (std::abs(x) - T(.5));
	x += T(.225) * x * (std::abs(x) - T(1.));
	return x;
}

}

}

#endif /* UTILS_MATH_H_ */
