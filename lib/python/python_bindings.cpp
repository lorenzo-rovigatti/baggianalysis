/*
 * python_bindings.cpp
 *
 *  Created on: Mar 9, 2019
 *      Author: lorenzo
 */

#include "../filters/FilterByFunction.h"
#include "../filters/FilterByReducingToCOM.h"
#include "../filters/FilterByType.h"
#include "../filters/FixParticlePath.h"
#include "../filters/SubtractCOM.h"

#include "../neighbour_finders/CutoffFinder.h"
#include "../neighbour_finders/NeighbourFinder.h"
#include "../neighbour_finders/SANNFinder.h"

#include "../observables/BondOrderParameters.h"
#include "../observables/PoreSize.h"
#include "../observables/MSD.h"

#include "../parsers/GenericOxDNAParser.h"
#include "../parsers/GroParser.h"
#include "../parsers/LAMMPSDumpParser.h"
#include "../parsers/LJKAParser.h"

#include "../topology/Topology.h"
#include "../topology/TopologyParsers.h"

#include "../trajectories/FullTrajectory.h"
#include "../trajectories/LazyTrajectory.h"

#include "../python_defs.h"

PYBIND11_MODULE(baggianalysis, m) {
	// utility classes
	ba::export_Particle(m);
	ba::export_ParticleSet(m);
	ba::export_System(m);
	ba::export_Topology(m);
	ba::export_TopologyParsers(m);

	// parsers
	ba::export_BaseParser(m);
	ba::export_GenericOxDNAParser(m);
	ba::export_GroParser(m);
	ba::export_LAMMPSDumpParser(m);
	ba::export_LJKAParser(m);

	// filters
	ba::export_BaseFilter(m);
	ba::export_FilterByFunction(m);
	ba::export_FilterByReducingToCOM(m);
	ba::export_FilterByType(m);
	ba::export_FixParticlePath(m);
	ba::export_SubtractCOM(m);

	// trajectories
	ba::export_BaseTrajectory(m);
	ba::export_FullTrajectory(m);
	ba::export_LazyTrajectory(m);

	// observables
	ba::export_BondOrderParameters(m);
	ba::export_PoreSize(m);
	ba::export_MSD(m);

	// neighbour finders
	ba::export_NeighbourFinder(m);
	ba::export_CutoffFinder(m);
	ba::export_SANNFinder(m);
}
