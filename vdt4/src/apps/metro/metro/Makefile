###########################################################################
#
# Makefile for metro lib. This C and FORTRAN code was copied from the METRo
# distribution and placed here to be built as a stand-alone library for
# direct use by other applications, such as roadcond_mdss. The function
# of interest is Do_Metro() in macadam.c.
#
# METRo version: 3.2.6
#
# Jim Cowie/RAL/Jan 2013
#
###########################################################################


include $(RAP_MAKE_INC_DIR)/rap_make_macros

LOC_INCLUDES = -I../include/metro
LOC_CFLAGS =

LOC_FFLAGS = -fdefault-integer-8

TARGET_FILE = ../libmetro.a
MODULE_TYPE = library

HDRS =	\
	../include/metro/global.h	\
	../include/metro/macadam.h	\
	../include/metro/number.h


C_SRCS = \
	global.c	\
	macadam.c

F_SRCS = \
	array2matrix.f	\
	balanc.f	\
	constPhys.f	\
	coupla.f	\
	flxsurfz.f	\
	grille.f	\
	initial.f	\
	lib_gen.f	\
	lib_therm.f

#
# general targets
#

include $(RAP_MAKE_INC_DIR)/rap_make_lib_module_targets


#
# local targets
#
depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.
