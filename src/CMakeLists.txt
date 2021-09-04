option(DEBUG_CLIENT_WAIT_TIME "Debug client wait time" OFF)

if(DEBUG_CLIENT_WAIT_TIME)
  message(STATUS "DEBUG_CLIENT_WAIT_TIME ON")
  message(STATUS "LOGGING OFF")
  add_definitions(-DDEBUG_CLIENT_WAIT_TIME)
  add_definitions(-DDEBUG_DISABLE_LOG)
  set(DEBUG_WORKER_LOAD OFF)
endif()

ADD_SUBDIRECTORY(client)
ADD_SUBDIRECTORY(worker)
ADD_SUBDIRECTORY(load_balancer)
