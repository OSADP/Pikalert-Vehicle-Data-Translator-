cmake_minimum_required(VERSION 2.8)


set (CMAKE_VERBOSE_MAKEFILE "ON")
set (CMAKE_CXX_FLAGS "-Wall -g -O")

project(log)
add_subdirectory(log)
add_subdirectory(include/log)

