# Tested with versions 3.23.1 and 3.16.3, likely works with older versions as well
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 11)

enable_testing()

project(
  ARSMulator
  VERSION 0.1
  LANGUAGES CXX)

add_subdirectory(src src/build)
add_subdirectory(test test/build)
