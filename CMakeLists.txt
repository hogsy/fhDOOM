cmake_minimum_required (VERSION 3.2.0)
project (fhDOOM)

OPTION(ID_ALLOW_TOOLS "Compile Tools" ON)
OPTION(ID_ALLOW_MAYA "Compile Maya Tools" ON)
OPTION(ID_UNICODE "Use unicode version of WIN32 API" OFF)
OPTION(ID_ENFORCE32BIT "Build 32bit on 64bit linux platform" OFF)

include(setup.cmake)

include_directories(neo/renderer)
include_directories(neo/glew)
add_subdirectory (neo/idlib)
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  add_subdirectory (neo/curl)
  add_subdirectory (neo/TypeInfo)
  if(ID_ALLOW_MAYA)
    add_subdirectory (neo/maya2md5)
  endif()
ENDIF()  

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release/base/ )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/release/base/ )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/release/base/ )

add_subdirectory (neo/game)
add_subdirectory (neo/d3xp)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release/ )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/release/ )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/release/ )

add_subdirectory (neo)
