cmake_minimum_required(VERSION 3.8)

PROJECT(dmlockfree)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

SET(DMLOCKFREE_VERSION_MAJOR "1")
SET(DMLOCKFREE_VERSION_MINOR "0")
SET(DMLOCKFREE_VERSION_PATCH "1")
SET(DMLOCKFREE_VERSION "${DMLOCKFREE_VERSION_MAJOR}.${DMLOCKFREE_VERSION_MINOR}.${DMLOCKFREE_VERSION_PATCH}")

MESSAGE(STATUS "VERSION: ${DMLOCKFREE_VERSION}")

OPTION(USE_DMLOCKFREE "use dmlockfree" OFF)

IF(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpermissive")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
ENDIF(UNIX)

ModuleImport("dmtest" "thirdparty/dmtest")
ModuleImport("dmlog" "thirdparty/dmlog")
ExeImport("test" "dmtest")
