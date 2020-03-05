SET(ba_lib_SOURCES
	exporters/BaseExporter.cpp
	exporters/Cogli1Exporter.cpp
	exporters/LAMMPSDataFileExporter.cpp
	filters/BaseFilter.cpp
	filters/FilterByFunction.cpp
	filters/FilterByReducingToCOM.cpp
	filters/FilterByType.cpp
	filters/FixParticlePath.cpp
	filters/SubtractCOM.cpp
	neighbour_finders/CutoffFinder.cpp
	neighbour_finders/NeighbourFinder.cpp
	neighbour_finders/SANNFinder.cpp
	observables/BondOrderParameters.cpp
	observables/ConvexHull.cpp
	observables/FormFactor.cpp
	observables/MSD.cpp
	parsers/BaseParser.cpp
	parsers/GenericOxDNAParser.cpp
	parsers/GroParser.cpp
	parsers/LJKAParser.cpp
	parsers/LAMMPSDataFileParser.cpp
	parsers/LAMMPSDumpParser.cpp
	particles/Particle.cpp
	particles/ParticleSet.cpp
	System.cpp
	topology/Topology.cpp
	topology/TopologyParsers.cpp
	trajectories/BaseTrajectory.cpp
	trajectories/LazyTrajectory.cpp
	trajectories/FullTrajectory.cpp
	utils/CellLists.cpp
	utils/filesystem.cpp
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

IF(Python)
	ADD_LIBRARY(_baggianalysis STATIC ${ba_lib_SOURCES})
	TARGET_LINK_LIBRARIES(_baggianalysis ${PYTHON_LIBRARIES})
	SET_TARGET_PROPERTIES(_baggianalysis PROPERTIES POSITION_INDEPENDENT_CODE ON)
	pybind11_add_module(core SHARED python/python_bindings.cpp)
	TARGET_LINK_LIBRARIES(core PRIVATE _baggianalysis)
	
	# the library should be in a folder with the same name to make it easier to use Python's distutils
	SET_TARGET_PROPERTIES(core 
		PROPERTIES 
			SUFFIX ".so"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/baggianalysis"
	)
	
	# copy the files required to install the library with Python's distutils
	CONFIGURE_FILE(python/__init__.py
		${CMAKE_CURRENT_BINARY_DIR}/baggianalysis/__init__.py
	)

	SET(SETUP_PY "${CMAKE_CURRENT_BINARY_DIR}/baggianalysis/setup.py")
	CONFIGURE_FILE(python/setup.py
		${SETUP_PY}
	)

	# setup the make install target to run setup.py (in user mode, no root permissions required)
	FIND_PROGRAM(PYTHON "python3")
	INSTALL(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install --user)")
ELSE(Python)
	ADD_LIBRARY(_baggianalysis SHARED ${ba_lib_SOURCES})
ENDIF(Python)

TARGET_LINK_LIBRARIES(_baggianalysis ${Boost_LIBRARIES})

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
