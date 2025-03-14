CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

cmake_policy(SET CMP0054 NEW)

OPTION(Debug "Set to ON to compile with debug symbols" OFF)
OPTION(G "Set to ON to compile with optimisations and debug symbols" OFF)
OPTION(COMPILER_EXTENSIONS "Set to ON to enable compiler (most likely GNU) C++ extensions" ON)

PROJECT(baggianalysis CXX)

# if cmake can't find python, nlopts or some of the other libraries add their paths to the following command
SET(CMAKE_FIND_ROOT_PATH)

# The default behaviour is to compile with optimisations
SET(CMAKE_BUILD_TYPE Release)

# build spdlog as a shared lib
add_subdirectory(extern/spdlog)
set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)
include_directories(extern/spdlog/include)

# tell pybind11 to use python3
set(PYBIND11_PYTHON_VERSION 3 CACHE STRING "Python version to use for compiling modules")
ADD_SUBDIRECTORY(extern/pybind11)

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

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ${COMPILER_EXTENSIONS})

ADD_DEFINITIONS(-Wall -Wno-int-in-bool-context)

IF(NOT Debug)
	IF(INTEL)
		ADD_DEFINITIONS(-fast)
	ELSE()
		ADD_DEFINITIONS(-ffast-math)
	ENDIF(INTEL)
ENDIF()

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/extern )

# return the current date as day/month/year
MACRO(CURRENT_DATE result)
    IF(WIN32)
        EXECUTE_PROCESS(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${result})
        string(REGEX REPLACE "(..)/(..)/..(..).*" "\\1/\\2/\\3" ${result} ${${result}})
    ELSEIF(UNIX)
        EXECUTE_PROCESS(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${result})
        string(REGEX REPLACE "(..)/(..)/..(..).*" "\\1/\\2/\\3" ${result} ${${result}})
    ELSE(WIN32)
        MESSAGE(SEND_ERROR "date not implemented")
        SET(${result} 000000)
    ENDIF (WIN32)
ENDMACRO(CURRENT_DATE)

ADD_CUSTOM_TARGET(test_quick
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/TestSuite.py test_list_file.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
    COMMENT "Running build integration tests" VERBATIM
)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

ADD_SUBDIRECTORY(lib)
