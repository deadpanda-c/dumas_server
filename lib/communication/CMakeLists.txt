add_library(communication SHARED
  src/Communication.cpp
)

target_include_directories(communication PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
