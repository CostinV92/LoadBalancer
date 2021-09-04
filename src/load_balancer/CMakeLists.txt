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
