INCLUDE_DIRECTORIES(
  include
  ${CMAKE_SOURCE_DIR}/src/common_headers
  ${CMAKE_SOURCE_DIR}/libs/libutils/include
  )

ADD_EXECUTABLE(client src/main.c)

TARGET_LINK_LIBRARIES(client
  libutils
  pthread
  )

add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:client> ${CMAKE_SOURCE_DIR}/bin/
)
