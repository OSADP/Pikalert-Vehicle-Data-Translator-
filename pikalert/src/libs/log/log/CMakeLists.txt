cmake_minimum_required(VERSION 2.8)

project(log)

ADD_LIBRARY(log
    log.cc
    )

include_directories ($ENV{GTG_INC_DIR} $ENV{LOCAL_INC_DIR})

INSTALL(FILES liblog.a
        DESTINATION $ENV{GTG_LIB_DIR}
	)

