SET(ba_lib_SOURCES
	filters/BaseFilter.cpp
	filters/FilterByReducingToCOM.cpp
	filters/FilterByType.cpp
	filters/SubtractCOM.cpp
	observables/MSD.cpp
	parsers/BaseParser.cpp
	parsers/OxDNAParser.cpp
	Particles.cpp
	System.cpp
	Trajectory.cpp
)

ADD_LIBRARY(_baggianalysis SHARED ${ba_lib_SOURCES})
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
                ${CMAKE_CURRENT_BINARY_DIR})
                