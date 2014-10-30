cmake_minimum_required(VERSION 2.8)

project(Proj4Wrap)

ADD_LIBRARY(Proj4Wrap
    Proj4Wrap.cc)

include_directories ($ENV{GTG_INC_DIR} $ENV{LOCAL_INC_DIR})

INSTALL(FILES libProj4Wrap.a
        DESTINATION $ENV{GTG_LIB_DIR}
	)



