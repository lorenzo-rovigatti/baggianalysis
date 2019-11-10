/*
 * python_defs.h
 *
 *  Created on: Mar 9, 2019
 *      Author: lorenzo
 */

#ifndef PYTHON_DEFS_H_
#define PYTHON_DEFS_H_

#ifdef PYTHON_BINDINGS

#include <pybind11/include/pybind11/pybind11.h>
#include <pybind11/include/pybind11/stl.h>
#include <pybind11/include/pybind11/functional.h>

#include "python/python_glm.h"

namespace py = pybind11;

#endif

#endif /* PYTHON_DEFS_H_ */
