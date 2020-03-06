/*
 * python_bindings.cpp
 *
 *  Created on: Mar 9, 2019
 *      Author: lorenzo
 */

#include "../exporters/BaseExporter.h"
#include "../exporters/Cogli1Exporter.h"
#include "../exporters/LAMMPSDataFileExporter.h"

#include "../filters/FilterByFunction.h"
#include "../filters/FilterByReducingToCOM.h"
#include "../filters/FilterByType.h"
#include "../filters/FixParticlePath.h"
#include "../filters/SubtractCOM.h"

#include "../neighbour_finders/CutoffFinder.h"
#include "../neighbour_finders/FixedNumberFinder.h"
#include "../neighbour_finders/NeighbourFinder.h"
#include "../neighbour_finders/SANNFinder.h"

#include "../observables/BondOrderParameters.h"
#include "../observables/ConvexHull.h"
#include "../observables/FormFactor.h"
#include "../observables/MSD.h"

#ifdef NLOPT_FOUND
#include "../observables/PoreSize.h"
#endif

#include "../parsers/GenericOxDNAParser.h"
#include "../parsers/GroParser.h"
#include "../parsers/LAMMPSDataFileParser.h"
#include "../parsers/LAMMPSDumpParser.h"
#include "../parsers/LJKAParser.h"

#include "../topology/Topology.h"
#include "../topology/TopologyParsers.h"

#include "../trajectories/FullTrajectory.h"
#include "../trajectories/LazyTrajectory.h"

#include "../python_defs.h"

PYBIND11_MODULE(core, m) {
	m.doc() = R"pbdoc(
        The core module
        ===============

        This module contains the Python bindings of the c++ classes

        Particles and sets of particles
        -------------------------------

        .. currentmodule:: baggianalysis.core

        .. autosummary::
           :nosignatures:

           Particle
           ParticleSet
           System

        Topology
        --------

        .. autosummary::
           :nosignatures:

           Topology
           parse_microgel_bondfile
           parse_LAMMPS_topology

        Parsers
        -------

        .. autosummary::
           :nosignatures:
           
           BaseParser
           GenericOxDNAParser
           GroParser
           LAMMPSDataFileParser
           LAMMPSDumpParser
           LJKAParser

        Exporters
        ---------

        .. autosummary::
           :nosignatures:
           
           BaseExporter
           Cogli1Exporter
           LAMMPSDataFileExporter

        Filters
        -------

        .. autosummary::
           :nosignatures:

           BaseFilter
           FilterByFunction
           FilterByReducingToCOM
           FilterByType
           FixParticlePath
           SubtractCOM

        Trajectories
        ------------

        .. autosummary::
           :nosignatures:

           BaseTrajectory
           FullTrajectory
           LazyTrajectory

        Observables
        -----------

        .. autosummary::
           :nosignatures:

           BondOrderParameters
           ConvexHull
           FormFactor
           PoreSize
           MSD

        Neighbour finders
        -----------------

        .. autosummary::
           :nosignatures:

           NeighbourFinder
           FixedNumberFinder
           CutoffFinder
           SANNFinder

    )pbdoc";

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
	ba::export_LAMMPSDataFileParser(m);
	ba::export_LAMMPSDumpParser(m);
	ba::export_LJKAParser(m);

	// exporters
	ba::export_BaseExporter(m);
	ba::export_Cogli1Exporter(m);
	ba::export_LAMMPSDataFileExporter(m);

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
	ba::export_ConvexHull(m);
	ba::export_FormFactor(m);
	ba::export_MSD(m);
#ifdef NLOPT_FOUND
	ba::export_PoreSize(m);
#endif

	// neighbour finders
	ba::export_NeighbourFinder(m);
	ba::export_FixedNumberFinder(m);
	ba::export_CutoffFinder(m);
	ba::export_SANNFinder(m);
}
