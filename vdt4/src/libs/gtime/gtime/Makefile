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

LOC_INCLUDES = -I../include
LOC_CFLAGS = -g
LOC_CPPC_FLAGS = -g 

TARGET_FILE = ../libgtime.a
MODULE_TYPE = library

HDRS = \
	../include/gtime/gtime.h	\
	../include/gtime/calc_sun.hh


CPPC_SRCS = \
	calc_sun.cc	\
	gtime.cc

#
# general targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_lib_module_targets

#
# local targets
#
test_gtime: test_gtime.o
	$(CPPC) $(LOC_CPPC_FLAGS) test_gtime.o ../libgtime.a -o test_gtime	

test_gtime1.o: test_gtime1.cc
	$(CPPC) -c $(LOC_CPPC_FLAGS) test_gtime1.cc

test_gtime1: test_gtime1.o
	$(CPPC) $(LOC_CPPC_FLAGS) test_gtime1.o ../libgtime.a -o test_gtime1

depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.
