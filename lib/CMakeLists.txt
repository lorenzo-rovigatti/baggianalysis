SET(ba_lib_SOURCES
	exporters/BaseExporter.cpp
	exporters/Cogli1Exporter.cpp
	exporters/LAMMPSDataFileExporter.cpp
	exporters/GenericOxDNAExporter.cpp
	filters/BaseFilter.cpp
	filters/FilterByFunction.cpp
	filters/FilterById.cpp
	filters/FilterByReducingToCOM.cpp
	filters/FilterByType.cpp
	filters/FixParticlePath.cpp
	filters/MapParticles.cpp
	filters/MolecularCOMs.cpp
	filters/SubtractCOM.cpp
	neighbour_finders/CutoffFinder.cpp
	neighbour_finders/FixedNumberFinder.cpp
	neighbour_finders/NeighbourFinder.cpp
	neighbour_finders/SANNFinder.cpp
	observables/ClusterList.cpp
	observables/ConvexHull.cpp
	observables/FormFactor.cpp
	observables/MSD.cpp
	observables/RDF.cpp
	observables/StructureFactor.cpp
	parsers/BaseParser.cpp
	parsers/OxDNAParser.cpp
	parsers/GroParser.cpp
	parsers/LJKAParser.cpp
	parsers/LAMMPSDataFileParser.cpp
	parsers/LAMMPSDumpParser.cpp
	parsers/oxDNA_topology_parsers/Default.cpp
	parsers/oxDNA_topology_parsers/TSP.cpp
	parsers/PatchyParticlesParser.cpp
	particles/Particle.cpp
	particles/ParticleSet.cpp
	System.cpp
	topology/Topology.cpp
	topology/TopologyParsers.cpp
	trajectories/BaseTrajectory.cpp
	trajectories/LazyTrajectory.cpp
	trajectories/FullTrajectory.cpp
	utils/CellLists.cpp
	utils/clusters.cpp
	utils/filesystem.cpp
	utils/Logger.cpp
	utils/Random.cpp
	utils/strings.cpp
)

# The lines that follow the following variables:
#   NLOPT_FOUND
#   NLOPT_INCLUDE_DIRS
#   NLOPT_LIBRARIES
#   NLOPT_DEFINITIONS
#   NLOPT_VERSION
#
# and the following targets:
#   NLOPT::nlopt

FIND_PACKAGE(PkgConfig QUIET)

# Check to see if pkgconfig is installed.
PKG_CHECK_MODULES(PC_NLOPT nlopt QUIET)

# Definitions
SET(NLOPT_DEFINITIONS ${PC_NLOPT_CFLAGS_OTHER})

# Include directories
FIND_PATH(NLOPT_INCLUDE_DIRS
    NAMES nlopt.h
    HINTS ${PC_NLOPT_INCLUDEDIR}
    PATHS "${CMAKE_INSTALL_PREFIX}/include")

# Libraries
FIND_LIBRARY(NLOPT_LIBRARIES
    NAMES nlopt nlopt_cxx
    HINTS ${PC_NLOPT_LIBDIR})

# Version
SET(NLOPT_VERSION ${PC_NLOPT_VERSION})

# Set (NAME)_FOUND if all the variables and the version are satisfied.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NLOPT
    FAIL_MESSAGE  DEFAULT_MSG
    REQUIRED_VARS NLOPT_INCLUDE_DIRS NLOPT_LIBRARIES
    VERSION_VAR   NLOPT_VERSION)

SET(with_nlopt_SOURCES
	observables/PoreSize.cpp
)

IF(NLOPT_FOUND)
	ADD_DEFINITIONS(-DNLOPT_FOUND)
	SET(ba_lib_SOURCES
		${ba_lib_SOURCES}
		${with_nlopt_SOURCES}
	)
ELSE()
	MESSAGE(WARNING "nlopt not found, the following modules will not be compiled: ${with_nlopt_SOURCES}")
ENDIF()

find_package(GSL)

SET(with_GSL_SOURCES
	observables/BondOrderParameters.cpp
)

IF(GSL_FOUND)
	add_definitions(-DGSL_FOUND)
	
	find_package( Boost REQUIRED )
	include_directories( ${Boost_INCLUDE_DIR} )
	
	SET(ba_lib_SOURCES
		${ba_lib_SOURCES}
		${with_GSL_SOURCES}
	)
ELSE()
	MESSAGE(WARNING "GSL not found, the following modules will not be compiled: ${with_GSL_SOURCES}")
ENDIF(GSL_FOUND)

add_library(_baggianalysis STATIC ${ba_lib_SOURCES})

IF(Python)
	TARGET_LINK_LIBRARIES(_baggianalysis ${PYTHON_LIBRARIES})
	SET_TARGET_PROPERTIES(_baggianalysis PROPERTIES POSITION_INDEPENDENT_CODE ON)
	pybind11_add_module(core SHARED python/bindings.cpp)
	TARGET_LINK_LIBRARIES(core PRIVATE _baggianalysis)
	
	set(BA_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/python/baggianalysis")
	
	# the library should be in a folder with the same name to make it easier to use Python's distutils
	SET_TARGET_PROPERTIES(core 
		PROPERTIES 
			SUFFIX ".so"
			LIBRARY_OUTPUT_DIRECTORY ${BA_OUTPUT_DIR}
	)
	
	# copy the files required to install the library with Python's setuptools
	CONFIGURE_FILE(python/__init__.py
		${BA_OUTPUT_DIR}/__init__.py
	)
	
	CONFIGURE_FILE(python/utils.py
		${BA_OUTPUT_DIR}/utils.py
	)

	CONFIGURE_FILE(python/make_install_setup.py
		${BA_OUTPUT_DIR}/setup.py
	)

	# setup the make install target to run make_install_setup.py (in user mode, no root permissions required)
	INSTALL(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pip install ${BA_OUTPUT_DIR})")
	
ENDIF(Python)

TARGET_LINK_LIBRARIES(_baggianalysis spdlog::spdlog)
IF(GSL_FOUND)
	TARGET_LINK_LIBRARIES(_baggianalysis ${GSL_LIBRARIES})
ENDIF(GSL_FOUND)

IF(NLOPT_FOUND)
	INCLUDE_DIRECTORIES( ${NLOPT_INCLUDE_DIRS} )
	TARGET_LINK_LIBRARIES(_baggianalysis ${NLOPT_LIBRARIES})
ENDIF()

SET_TARGET_PROPERTIES(_baggianalysis PROPERTIES PREFIX "")

# by default, libraries have extension ".dylib" on Mac OS X, but these are
# not recognized by python as modules
IF(APPLE)
    SET_TARGET_PROPERTIES(core PROPERTIES SUFFIX ".so")
ENDIF(APPLE)

ADD_CUSTOM_COMMAND(
        TARGET _baggianalysis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/tests
                ${CMAKE_CURRENT_BINARY_DIR}
)
