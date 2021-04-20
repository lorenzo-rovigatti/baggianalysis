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

#define DEFAULT_PARTICLE_TYPE "0"
#define DEFAULT_LINK_TYPE "1"

#ifndef PYTHON_BINDINGS
#define PYBIND11_EXPORT
#endif

#include "utils/Logger.h"

#include <spdlog/fmt/fmt.h>

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/common.hpp>
#include <glm/geometric.hpp>

#include <memory>

#include <iostream>

using vec3 = glm::dvec3;

using vector_vec3 = std::vector<vec3>;
using vector_scalar = std::vector<double>;

using uint = unsigned int;
using ullint = unsigned long long int;

using particle_type = std::string;

#endif /* LIB_DEFS_H_ */
