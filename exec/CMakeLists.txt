# BEGIN TO COMPILE STUFF
IF(CMAKE_BUILD_TYPE MATCHES Debug)
	SET(exe_name baggianalysis_debug)
ELSE()
	SET(exe_name baggianalysis)
ENDIF()

SET(ba_SOURCES
	main.cpp
)

ADD_EXECUTABLE(${exe_name} ${ba_SOURCES})
TARGET_LINK_LIBRARIES(${exe_name} _baggianalysis)
