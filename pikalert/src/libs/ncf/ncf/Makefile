# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2010 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** 2010/10/7 23:12:33 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
###########################################################################
#
# Makefile for ncf module
#
# Gerry Wiener, RAP, NCAR, Boulder, Co, USA, 80307
#
# June 1999
#
###########################################################################


include $(RAP_MAKE_INC_DIR)/rap_make_macros

LOC_INCLUDES = $(NETCDF_INCS) -I../include 
LOC_CFLAGS =  

TARGET_FILE = ../libncf.a
MODULE_TYPE = library

HDRS = ../include/ncf/ncf.hh

CPPC_SRCS = \
	att_as_string.cc	\
	finite.cc		\
	cdltonc.cc		\
	cdf2unix_time.cc	\
	gtime.cc		\
	find_day_start.cc	\
	write_var.cc		\
	get_dim_size.cc		\
	get_var.cc		\
	get_var_attr.cc		\
	get_var_fill_val.cc 	\
	put_var_attr.cc		\
	put_var.cc		\
	get_attr.cc		\
	put_attr.cc		

#
# general targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_lib_module_targets


#
# local targets
#

test_ncf: test_ncf.o test_ncf_add.o
	$(CPPC) $(LOC_CPPC_CFLAGS) test_ncf.o test_ncf_add.o ../libncf.a -o test_ncf

ncf_input_example: ncf_input_example.o
	$(CPPC) $(LOC_CPPC_CFLAGS) ncf_input_example.o ../libncf.a -lnetcdf  -lnetcdf_c++ -o ncf_input_example

ncf_output_example: ncf_output_example.o
	$(CPPC) $(LOC_CPPC_CFLAGS) ncf_output_example.o ../libncf.a -lnetcdf  -lnetcdf_c++ -o ncf_output_example

depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.
