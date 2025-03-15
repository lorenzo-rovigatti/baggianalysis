cmake_minimum_required(VERSION 3.15)
project(baggianalysis LANGUAGES CXX)

option(Debug "Set to ON to compile with debug symbols" OFF)
option(G "Set to ON to compile with optimisations and debug symbols" OFF)
option(COMPILER_EXTENSIONS "Set to ON to enable compiler (most likely GNU) C++ extensions" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ${COMPILER_EXTENSIONS})

# The default behaviour is to compile with optimisations
SET(CMAKE_BUILD_TYPE Release)

IF(Debug)
	SET(CMAKE_BUILD_TYPE "Debug")
	MESSAGE(STATUS "Compiling in Debug mode")
ENDIF(Debug)

IF(G)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo)
	MESSAGE(STATUS "Compiling in Release+Debug symbols mode")
ENDIF(G)

IF(MOSIX)
	ADD_DEFINITIONS(-DMOSIX)
	MESSAGE(STATUS "Compiling with Mosix support")
ENDIF(MOSIX)

ADD_DEFINITIONS(-Wall -Wno-int-in-bool-context)

set(ba_includes
    ${PROJECT_SOURCE_DIR}/extern
    extern/spdlog/include
)

set(ba_link_libs
    spdlog::spdlog
)

# build spdlog as a shared lib
add_subdirectory(extern/spdlog)
set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)

SET(ba_lib_SOURCES
	src/exporters/BaseExporter.cpp
	src/exporters/Cogli1Exporter.cpp
	src/exporters/LAMMPSDataFileExporter.cpp
	src/exporters/GenericOxDNAExporter.cpp
	src/filters/BaseFilter.cpp
	src/filters/FilterByFunction.cpp
	src/filters/FilterById.cpp
	src/filters/FilterByReducingToCOM.cpp
	src/filters/FilterByType.cpp
	src/filters/FixParticlePath.cpp
	src/filters/MapParticles.cpp
	src/filters/MolecularCOMs.cpp
	src/filters/SubtractCOM.cpp
	src/neighbour_finders/CutoffFinder.cpp
	src/neighbour_finders/FixedNumberFinder.cpp
	src/neighbour_finders/NeighbourFinder.cpp
	src/neighbour_finders/SANNFinder.cpp
	src/observables/ClusterList.cpp
	src/observables/CollectiveISF.cpp
	src/observables/ConvexHull.cpp
	src/observables/FormFactor.cpp
	src/observables/GyrationRadius.cpp
	src/observables/MSD.cpp
	src/observables/ParticleAutocorrelation.cpp
	src/observables/RDF.cpp
	src/observables/SelfISF.cpp
	src/observables/StructureFactor.cpp
	src/parsers/BaseParser.cpp
	src/parsers/OxDNAParser.cpp
	src/parsers/GroParser.cpp
	src/parsers/LJKAParser.cpp
	src/parsers/LAMMPSDataFileParser.cpp
	src/parsers/LAMMPSDumpParser.cpp
	src/parsers/oxDNA_topology_parsers/Default.cpp
	src/parsers/oxDNA_topology_parsers/DetailedPolymer.cpp
	src/parsers/oxDNA_topology_parsers/DNA.cpp
	src/parsers/oxDNA_topology_parsers/NoTopology.cpp
	src/parsers/oxDNA_topology_parsers/TSP.cpp
	src/parsers/PatchyParticlesParser.cpp
	src/particles/Particle.cpp
	src/particles/ParticleSet.cpp
	src/System.cpp
	src/topology/Topology.cpp
	src/topology/TopologyParsers.cpp
	src/trajectories/BaseTrajectory.cpp
	src/trajectories/LazyTrajectory.cpp
	src/trajectories/FullTrajectory.cpp
	src/utils/CellLists.cpp
	src/utils/clusters.cpp
	src/utils/filesystem.cpp
	src/utils/Logger.cpp
	src/utils/Random.cpp
	src/utils/strings.cpp
	src/utils/WaveVectorList.cpp
)

# The lines that follow define the following variables:
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
    src/observables/PoreSize.cpp
)

IF(NLOPT_FOUND)
	ADD_DEFINITIONS(-DNLOPT_FOUND)
	SET(ba_lib_SOURCES
		${ba_lib_SOURCES}
		${with_nlopt_SOURCES}
	)

    set(ba_includes ${ba_includes} ${NLOPT_INCLUDE_DIRS})
    set(ba_link_libs ${ba_link_libs} ${NLOPT_LIBRARIES})
ELSE()
	MESSAGE(WARNING "nlopt not found, the following modules will not be compiled: ${with_nlopt_SOURCES}")
ENDIF()

find_package(GSL)

SET(with_GSL_SOURCES
    src/observables/BondOrderParameters.cpp
)

IF(GSL_FOUND)
	add_definitions(-DGSL_FOUND)
	
	find_package( Boost REQUIRED )
	
	SET(ba_lib_SOURCES
		${ba_lib_SOURCES}
		${with_GSL_SOURCES}
	)

    set(ba_includes ${ba_includes} ${Boost_INCLUDE_DIR})
    set(ba_link_libs ${ba_link_libs} ${GSL_LIBRARIES})
ELSE()
	MESSAGE(WARNING "GSL not found, the following modules will not be compiled: ${with_GSL_SOURCES}")
ENDIF(GSL_FOUND)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
ADD_SUBDIRECTORY(extern/pybind11)

Python_add_library(core MODULE src/bindings.cpp ${ba_lib_SOURCES} WITH_SOABI)
target_link_libraries(core PUBLIC ${ba_link_libs})
include_directories(${ba_includes})

set(PYTHON_SITE_PACKAGES ${Python_SITEARCH})
install(TARGETS core
        LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}/baggianalysis
        ARCHIVE DESTINATION ${PYTHON_SITE_PACKAGES}/baggianalysis)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/baggianalysis/
        DESTINATION ${PYTHON_SITE_PACKAGES}/baggianalysis
        FILES_MATCHING PATTERN "*.py")

add_custom_target(test_quick
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/TestSuite.py test_list_file.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
    COMMENT "Running build integration tests" VERBATIM
)
