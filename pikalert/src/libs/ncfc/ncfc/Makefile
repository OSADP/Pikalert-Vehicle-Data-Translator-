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

TARGET_FILE = ../libncfc.a
MODULE_TYPE = library

HDRS = ../include/ncfc/ncfc.hh

CPPC_SRCS = \
	cdfinput.cc	\
	get_dims.cc	\
	get_unlim.cc	\
	get_variables.cc	\
	get_var_info.cc		\
	get_var_list.cc	\
	make_cdf.cc	\
	put_variables.cc	\
	var_input.cc	\
	var_output.cc

#
# general targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_lib_module_targets


#
# local targets
#

depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.
