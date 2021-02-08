CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

cmake_policy(SET CMP0054 NEW)

OPTION(Python "Enable Python bindings" ON)
OPTION(Debug "Set to ON to compile with debug symbols" OFF)
OPTION(G "Set to ON to compile with optimisations and debug symbols" OFF)

PROJECT(baggianalysis CXX)

# if cmake can't find python, nlopts or some of the other libraries add their paths to the following command
SET(CMAKE_FIND_ROOT_PATH)

# The default behaviour is to compile with optimisations
SET(CMAKE_BUILD_TYPE Release)

# build spdlog as a shared lib
add_subdirectory(extern/spdlog)
set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)

IF(Python)
	ADD_SUBDIRECTORY(extern/pybind11)
	ADD_DEFINITIONS(-DPYTHON_BINDINGS)
	
	INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
	
	MESSAGE(STATUS "Enabling Python bindings")
ENDIF(Python)

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

IF(CMAKE_VERSION VERSION_LESS "3.1")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
ELSE()
	SET(CMAKE_CXX_STANDARD 14)
	SET(CMAKE_CXX_STANDARD_REQUIRED ON)
ENDIF()

if(CMAKE_VERSION VERSION_LESS "2.9")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF()

ADD_DEFINITIONS(-Wall -Wno-int-in-bool-context)

IF(NOT Debug)
	IF(INTEL)
		ADD_DEFINITIONS(-fast)
	ELSE()
		ADD_DEFINITIONS(-ffast-math)
	ENDIF(INTEL)
ENDIF()

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/extern )

# get the current git revision, if git is installed. Avoid warnings if it isn't
FIND_PACKAGE(Git)
IF(Git_FOUND)
	EXECUTE_PROCESS(COMMAND "git rev-parse --short HEAD" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE revision OUTPUT_STRIP_TRAILING_WHITESPACE)
ELSE()
	SET(revision GIT_NOT_FOUND)
ENDIF()

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

CURRENT_DATE(today)
ADD_DEFINITIONS(-D'GIT_VERSION="${revision}"' -D'BUILD_TIME="${today}"')

ADD_CUSTOM_TARGET(test_quick
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/TestSuite.py test_list_file.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
    COMMENT "Running build integration tests" VERBATIM
)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(exec)
