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
#include "../observables/CollectiveISF.h"
#include "../observables/ConvexHull.h"
#include "../observables/FormFactor.h"
#include "../observables/GyrationRadius.h"
#include "../observables/MSD.h"
#include "../observables/ParticleAutocorrelation.h"
#include "../observables/RDF.h"
#include "../observables/StructureFactor.h"
#include "../observables/SelfISF.h"
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

#include "../utils/CellLists.h"
#include "../utils/Logger.h"
#include "../utils/WaveVectorList.h"

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
	ba::export_CollectiveISF(m);
	ba::export_ConvexHull(m);
	ba::export_FormFactor(m);
	ba::export_GyrationRadius(m);
	ba::export_MSD(m);
	ba::export_ParticleAutocorrelation(m);
#ifdef NLOPT_FOUND
	ba::export_PoreSize(m);
#endif
	ba::export_RDF(m);
	ba::export_SelfISF(m);
	ba::export_StructureFactor(m);

	// neighbour finders
	ba::export_NeighbourFinder(m);
	ba::export_FixedNumberFinder(m);
	ba::export_CutoffFinder(m);
	ba::export_SANNFinder(m);

	ba::export_CellLists(m);
	ba::export_WaveVectorList(m);

	// Here we bind Logger's set_logging_mode static method directly, since we can't do it 
	// in the Logger class because of the way includes are (badlyt) organised in the project
	py::enum_<ba::Logger::LogMode>(m, "LogMode")
        .value("STDERR", ba::Logger::LogMode::STDERR)
        .value("FILE", ba::Logger::LogMode::FILE)
        .value("SILENT", ba::Logger::LogMode::SILENT)
        .export_values();  // Allows direct access without `LogMode.` prefix

    // Bind the Logger class (only static methods, as it is not instantiable)
    m.def("set_logging_mode", &ba::Logger::set_logging_mode, py::arg("mode"), py::arg("file_path") = "ba_log.txt",
		"Set the logging mode (STDERR, FILE, or SILENT) and optionally specify a file path (used only if the FILE mode is chosen).");
}
