# top level CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(load_balancer)

option(DEBUG_WORKER_LOAD "Debug worker load" ON)
option(DEBUG_CLIENT_WAIT_TIME "Debug client wait time" OFF)

if(DEBUG_CLIENT_WAIT_TIME)
  message(STATUS "DEBUG_CLIENT_WAIT_TIME")
  add_definitions(-DDEBUG_CLIENT_WAIT_TIME)
  add_definitions(-DDEBUG_DISABLE_LOG)
  set(DEBUG_WORKER_LOAD OFF)
endif()

if(DEBUG_WORKER_LOAD)
  message(STATUS "DEBUG_WORKER_LOAD")
  add_definitions(-DDEBUG_WORKER_LOAD)
endif()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(libs)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
