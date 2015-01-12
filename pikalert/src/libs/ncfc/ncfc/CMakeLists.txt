cmake_minimum_required(VERSION 2.8)

project(ncfc)

ADD_LIBRARY(ncfc
    var_input.cc
    var_output.cc
    get_unlim.cc
    get_variables.cc
    get_var_info.cc
    get_var_list.cc
    put_variables.cc
    cdfinput.cc
    make_cdf.cc
    get_dims.cc
    )

include_directories ($ENV{GTG_INC_DIR} $ENV{LOCAL_INC_DIR} $ENV{NETCDF_INC_DIR})

INSTALL(FILES libncfc.a
        DESTINATION $ENV{GTG_LIB_DIR}
	)

