/*
 * python_bindings.cpp
 *
 *  Created on: Mar 9, 2019
 *      Author: lorenzo
 */

#include "../exporters/BaseExporter.h"
#include "../exporters/Cogli1Exporter.h"
#include "../exporters/LAMMPSDataFileExporter.h"
#include "../exporters/GenericOxDNAExporter.h"

#include "../filters/FilterByFunction.h"
#include "../filters/FilterById.h"
#include "../filters/FilterByReducingToCOM.h"
#include "../filters/FilterByType.h"
#include "../filters/FixParticlePath.h"
#include "../filters/MapParticles.h"
#include "../filters/MolecularCOMs.h"
#include "../filters/SubtractCOM.h"

#include "../neighbour_finders/CutoffFinder.h"
#include "../neighbour_finders/FixedNumberFinder.h"
#include "../neighbour_finders/NeighbourFinder.h"
#include "../neighbour_finders/SANNFinder.h"

#ifdef GSL_FOUND
#include "../observables/BondOrderParameters.h"
#endif
#include "../observables/ClusterList.h"
#include "../observables/ConvexHull.h"
#include "../observables/FormFactor.h"
#include "../observables/MSD.h"
#include "../observables/RDF.h"
#include "../observables/StructureFactor.h"
#ifdef NLOPT_FOUND
#include "../observables/PoreSize.h"
#endif

#include "../parsers/OxDNAParser.h"
#include "../parsers/GroParser.h"
#include "../parsers/LAMMPSDataFileParser.h"
#include "../parsers/LAMMPSDumpParser.h"
#include "../parsers/LJKAParser.h"
#include "../parsers/PatchyParticlesParser.h"

#include "../topology/Topology.h"
#include "../topology/TopologyParsers.h"

#include "../trajectories/FullTrajectory.h"
#include "../trajectories/LazyTrajectory.h"

#include "../python_defs.h"

PYBIND11_MODULE(core, m) {
	// utility classes
	ba::export_Particle(m);
	ba::export_ParticleSet(m);
	ba::export_System(m);
	ba::export_Topology(m);
	ba::export_TopologyParsers(m);

	// parsers
	ba::export_BaseParser(m);
	ba::export_OxDNAParser(m);
	ba::export_GroParser(m);
	ba::export_LAMMPSDataFileParser(m);
	ba::export_LAMMPSDumpParser(m);
	ba::export_LJKAParser(m);
	ba::export_PatchyParticlesParser(m);

	// exporters
	ba::export_BaseExporter(m);
	ba::export_Cogli1Exporter(m);
	ba::export_LAMMPSDataFileExporter(m);
	ba::export_GenericOxDNAExporter(m);

	// filters
	ba::export_BaseFilter(m);
	ba::export_FilterByFunction(m);
	ba::export_FilterById(m);
	ba::export_FilterByReducingToCOM(m);
	ba::export_FilterByType(m);
	ba::export_FixParticlePath(m);
	ba::export_MapParticles(m);
	ba::export_MolecularCOMs(m);
	ba::export_SubtractCOM(m);

	// trajectories
	ba::export_BaseTrajectory(m);
	ba::export_FullTrajectory(m);
	ba::export_LazyTrajectory(m);

	// observables
#ifdef GSL_FOUND
	ba::export_BondOrderParameters(m);
#endif
	ba::export_ClusterList(m);
	ba::export_ConvexHull(m);
	ba::export_FormFactor(m);
	ba::export_MSD(m);
#ifdef NLOPT_FOUND
	ba::export_PoreSize(m);
#endif
	ba::export_RDF(m);
	ba::export_StructureFactor(m);

	// neighbour finders
	ba::export_NeighbourFinder(m);
	ba::export_FixedNumberFinder(m);
	ba::export_CutoffFinder(m);
	ba::export_SANNFinder(m);
}
