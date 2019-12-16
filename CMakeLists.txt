cmake_minimum_required(VERSION 3.2)

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
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/dmlockfree_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/dmlockfree_config.h)

ModuleImport("dmlog" "thirdparty/dmlog")
LibImport("dmlockfree" "src/dmlockfree_module")
ExeImport("test" "dmlockfree;dmlog")

#ADD_CUSTOM_COMMAND(
#TARGET dmlockfreetest
#PRE_BUILD
#COMMAND ${CMAKE_COMMAND} -E copy_if_different
#${CMAKE_CURRENT_SOURCE_DIR}/config/test.csv
#$<TARGET_FILE_DIR:dmlockfreetest>
#)

#INSTALL(DIRECTORY include/ DESTINATION include)
#INSTALL(TARGETS dmlockfree
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib)

#SET(DMLOCKFREE_LIBS "dmlockfree")
#MARK_AS_ADVANCED(${DMLOCKFREE_LIBS})