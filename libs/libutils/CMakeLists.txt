INCLUDE_DIRECTORIES(
  include
  ${CMAKE_SOURCE_DIR}/src/common_headers
  )

ADD_LIBRARY(libutils
  src/libutils.c
  )
