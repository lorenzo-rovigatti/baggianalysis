SET(ba_lib_SOURCES
	filters/BaseFilter.cpp
	filters/FilterByReducingToCOM.cpp
	filters/FilterByType.cpp
	filters/FixParticlePath.cpp
	filters/SubtractCOM.cpp
	observables/MSD.cpp
	parsers/BaseParser.cpp
	parsers/GroParser.cpp
	parsers/LJKAParser.cpp
	parsers/OxDNAParser.cpp
	Particles.cpp
	System.cpp
	trajectories/BaseTrajectory.cpp
	trajectories/LazyTrajectory.cpp
	trajectories/FullTrajectory.cpp
	utils/filesystem.cpp
)

IF(Python)
	ADD_LIBRARY(_baggianalysis STATIC ${ba_lib_SOURCES})
	SET_TARGET_PROPERTIES(_baggianalysis PROPERTIES POSITION_INDEPENDENT_CODE ON)
	pybind11_add_module(baggianalysis SHARED python_bindings.cpp)
	TARGET_LINK_LIBRARIES(baggianalysis PRIVATE _baggianalysis)
	
	// the library should be in a folder with the same name to make it easier to use Python's distutils
	SET_TARGET_PROPERTIES(baggianalysis 
		PROPERTIES 
			SUFFIX ".so"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/baggianalysis"
	)
	
	// copy the files required to install the library with Python's distutils
	CONFIGURE_FILE(python/__init__.py
		${CMAKE_CURRENT_BINARY_DIR}/baggianalysis/__init__.py
	)

	SET(SETUP_PY "${CMAKE_CURRENT_BINARY_DIR}/baggianalysis/setup.py")
	CONFIGURE_FILE(python/setup.py
		${SETUP_PY}
	)

	// setup the make install target to run setup.py (in user mode, no root permissions required)
	FIND_PROGRAM(PYTHON "python3")
	INSTALL(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install --user)")
ELSE(Python)
	ADD_LIBRARY(_baggianalysis SHARED ${ba_lib_SOURCES})
ENDIF(Python)

TARGET_LINK_LIBRARIES(_baggianalysis ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

SET_TARGET_PROPERTIES(_baggianalysis PROPERTIES PREFIX "")

# by default, libraries have extension ".dylib" on Mac OS X, but these are
# not recognized by python as modules
IF(APPLE)
    SET_TARGET_PROPERTIES(_baggianalysis PROPERTIES SUFFIX ".so")
ENDIF(APPLE)

ADD_CUSTOM_COMMAND(
        TARGET _baggianalysis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/tests/test.py
                ${CMAKE_CURRENT_BINARY_DIR}
)
