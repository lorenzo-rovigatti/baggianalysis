/*
 * defs.h
 *
 *  Created on: 08 feb 2018
 *      Author: lorenzo
 */

#ifndef LIB_DEFS_H_
#define LIB_DEFS_H_

#define SQR(x) ((x) * (x))
#define CUB(x) ((x) * (x) * (x))

#include <Eigen/Dense>
#include <Eigen/StdVector>

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>

using vec3 = Eigen::Vector3d;
using mat3 = Eigen::Matrix3d;

using vector_vec3 = std::vector<vec3, Eigen::aligned_allocator<vec3> >;
using vector_scalar = std::vector<double>;

using uint = unsigned int;
using ullint = unsigned long long int;

#endif /* LIB_DEFS_H_ */
