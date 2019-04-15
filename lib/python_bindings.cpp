/*
 * python_bindings.cpp
 *
 *  Created on: Mar 9, 2019
 *      Author: lorenzo
 */

#include "parsers/GroParser.h"
#include "parsers/LJKAParser.h"
#include "parsers/OxDNAParser.h"

#include "filters/SubtractCOM.h"
#include "filters/FilterByReducingToCOM.h"
#include "filters/FilterByType.h"
#include "filters/FixParticlePath.h"

#include "trajectories/LazyTrajectory.h"

#include "observables/MSD.h"

#include "python_defs.h"

PYBIND11_MODULE(baggianalysis, m) {
	// utility classes
	ba::export_System(m);
	ba::export_Particles(m);

	// parsers
	ba::export_BaseParser(m);
	ba::export_GroParser(m);
	ba::export_LJKAParser(m);
	ba::export_OxDNAParser(m);

	// filters
	ba::export_BaseFilter(m);
	ba::export_SubtractCOM(m);
	ba::export_FilterByReducingToCOM(m);
	ba::export_FilterByType(m);
	ba::export_FixParticlePath(m);

	// trajectories
	ba::export_BaseTrajectory(m);
	ba::export_LazyTrajectory(m);

	// observables
	ba::export_MSD(m);
}
