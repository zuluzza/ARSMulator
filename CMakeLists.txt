# Tested with only version 3.23.1, likely works with older versions as well
cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 11)

project(
  ARSMulator
  VERSION 0.1
  LANGUAGES CXX)

add_subdirectory(src)
add_subdirectory(test test/build)
