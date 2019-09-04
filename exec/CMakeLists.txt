# BEGIN TO COMPILE STUFF
SET(exe_name test_feature)

SET(ba_SOURCES
	test_feature.cpp
)

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/lib )

ADD_EXECUTABLE(${exe_name} ${ba_SOURCES})
TARGET_LINK_LIBRARIES(${exe_name} _baggianalysis)
