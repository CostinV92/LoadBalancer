INCLUDE_DIRECTORIES(
  include
  ${CMAKE_SOURCE_DIR}/src/common_headers
  ${CMAKE_SOURCE_DIR}/libs/libheap/include
  ${CMAKE_SOURCE_DIR}/libs/liblist/include
  ${CMAKE_SOURCE_DIR}/libs/libutils/include
  )

ADD_EXECUTABLE(load_balancer
  src/main.c
  src/connections.c
  src/client_listener.c
  src/worker_listener.c
  )

option(DEBUG_WORKER_LOAD "Debug worker load" ON)
option(DEBUG_CLIENT_WAIT_TIME "Debug client wait time" OFF)

if(DEBUG_CLIENT_WAIT_TIME)
  message(STATUS "DEBUG_CLIENT_WAIT_TIME ON")
  message(STATUS "LOGGING OFF")
  add_definitions(-DDEBUG_CLIENT_WAIT_TIME)
  add_definitions(-DDEBUG_DISABLE_LOG)
  set(DEBUG_WORKER_LOAD OFF)
endif()

if(DEBUG_WORKER_LOAD)
  message(STATUS "DEBUG_WORKER_LOAD")
  add_definitions(-DDEBUG_WORKER_LOAD)
endif()


TARGET_LINK_LIBRARIES(load_balancer
  libheap
  liblist
  libutils
  )

add_custom_command(TARGET load_balancer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:load_balancer> ${CMAKE_SOURCE_DIR}/test_bed/load_balancer/
)
