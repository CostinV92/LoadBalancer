INCLUDE_DIRECTORIES(
  include
  ${CMAKE_SOURCE_DIR}/src/common_headers
  ${CMAKE_SOURCE_DIR}/libs/libutils/include
  )

ADD_EXECUTABLE(worker
  src/main.c
  src/registration.c
  src/work.c
  )

TARGET_LINK_LIBRARIES(worker
  libutils
  pthread
  )

add_custom_command(TARGET worker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:worker> ${CMAKE_SOURCE_DIR}/bin/
)
