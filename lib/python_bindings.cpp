/*
 * python_bindings.cpp
 *
 *  Created on: Mar 9, 2019
 *      Author: lorenzo
 */

#include "parsers/OxDNAParser.h"
#include "trajectories/LazyTrajectory.h"

#include "python_defs.h"

PYBIND11_MODULE(baggianalysis, m) {
	// we also bind the STL containers we will be using (see https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html)
//	py::bind_vector<vector_vec3>(m, "vector_vec3");
//	py::bind_vector<vector_scalar>(m, "vector_scalar");

	ba::export_System(m);
	ba::export_Particles(m);

	ba::export_BaseParser(m);
	ba::export_OxDNAParser(m);

	ba::export_BaseTrajectory(m);
	ba::export_LazyTrajectory(m);
}
