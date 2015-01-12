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

TARGET_FILE = ../libnxncf.a
MODULE_TYPE = library

HDRS = ../include/ncf/defs.hh

CPPC_SRCS = \
	ncf_fcst_output.cc     \
	ncf_site.cc     \
	get_sites.cc	\
	write_creation_time.cc	\
	model_id.cc		\
	create_index_array.cc

#
# general targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_lib_module_targets


#
# local targets
#

test_ncf: test_ncf.o test_ncf_add.o
	$(CPPC) $(LOC_CPPC_CFLAGS) test_ncf.o test_ncf_add.o ../libncf.a -o test_ncf

depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.
