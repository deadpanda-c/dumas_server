cmake_minimum_required(VERSION 3.22)

project(dumas)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(include)
add_subdirectory(lib/communication)

add_executable(dumas_server ${SOURCES})
target_link_libraries(dumas_server communication)


