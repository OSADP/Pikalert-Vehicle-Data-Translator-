#/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
#/* ** Copyright UCAR (c) 1992 - 2010 */
#/* ** University Corporation for Atmospheric Research(UCAR) */
#/* ** National Center for Atmospheric Research(NCAR) */
#/* ** Research Applications Laboratory(RAL) */
#/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
#/* ** 2010/10/7 23:12:33 */
#/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
cmake_minimum_required(VERSION 2.8)

project(ncf)

ADD_LIBRARY(ncf
    att_as_string.cc
    finite.cc
    cdltonc.cc
    cdf2unix_time.cc
    gtime.cc
    find_day_start.cc
    write_var.cc
    get_dim_size.cc
    get_var.cc
    get_var_attr.cc
    get_var_fill_val.cc
    put_var_attr.cc
    put_var.cc
    get_attr.cc
    put_attr.cc
    )

include_directories ($ENV{GTG_INC_DIR} $ENV{LOCAL_INC_DIR} $ENV{NETCDF_INC_DIR} "../include")

INSTALL(FILES libncf.a
        DESTINATION $ENV{GTG_LIB_DIR}
	)
