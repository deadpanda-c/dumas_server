cmake_minimum_required(VERSION 4.3)
add_library(communication SHARED
  src/Communication.cpp
)

target_include_directories(communication PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(communication_tests
    tests/CommunicationTests.cpp
  )
  target_link_libraries(communication_tests PRIVATE communication GTest::gtest_main)
  target_include_directories(communication_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

  include(GoogleTest)
  gtest_discover_tests(communication_tests)

  add_custom_target(tests_run
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS communication_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()
