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
	ba::export_System(m);
	ba::export_Particles(m);

	ba::export_BaseParser(m);
	ba::export_OxDNAParser(m);

	ba::export_BaseTrajectory(m);
	ba::export_LazyTrajectory(m);
}
