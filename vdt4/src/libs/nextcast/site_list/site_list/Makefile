###########################################################################
#
# Makefile for site list module
#
# Gerry Wiener, RAP, NCAR, Boulder, Co, USA, 80307
#
# June 1999
#
###########################################################################

include $(RAP_MAKE_INC_DIR)/rap_make_macros

LOC_INCLUDES = $(NETCDF_INCS) -I../include
LOC_CFLAGS =
LOC_CPPC_FLAGS = 

TARGET_FILE = ../libsite_list.a
MODULE_TYPE = library

HDRS = \
	$(RAP_INC_DIR)/site_list/site_list.hh \
	$(RAP_INC_DIR)/site_list/SiteList.hh \
	$(RAP_INC_DIR)/site_list/SiteLoc.hh

CPPC_SRCS = \
	site_list.cc \
	SiteList.cc \
	SiteLoc.cc

#
# general targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_lib_module_targets

#
# local targets
#

test_site_list: test_site_list.o
	$(CPPC) $(LOC_CPPC_CFLAGS) test_site_list.o ../libsite_list.a -o test_site_list

depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.
