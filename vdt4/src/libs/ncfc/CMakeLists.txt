cmake_minimum_required(VERSION 2.8)


set (CMAKE_VERBOSE_MAKEFILE "ON")
set (CMAKE_CXX_FLAGS "-Wall -g -O")

project(ncfc)
add_subdirectory(ncfc)
add_subdirectory(include/ncfc)

