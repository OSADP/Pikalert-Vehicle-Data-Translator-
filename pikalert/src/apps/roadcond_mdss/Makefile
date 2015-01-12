
###########################################################################
#
# Makefile for int_fcst module
#
# Gerry Wiener, RAP, NCAR, Boulder, Co, USA, 80307
#
# June 1999
#
###########################################################################


include $(RAP_MAKE_INC_DIR)/rap_make_macros

LOC_INCLUDES = $(NETCDF_INCS)
LOC_CPPC_CFLAGS =
LOC_LDFLAGS = $(NETCDF_LIBS)
LOC_LIBS = -lnetcdf_c++ -lnetcdf -lsite_list -lnxncf -lncf -llog -lgtime \
	-lhdf5_hl -lhdf5 -lz -lsz -lmetro -lgfortran -ldl

TARGET_FILE = roadcond_mdss
MODULE_TYPE = progcpp

HDRS = \
	parse_traffic_layer_config.hh \
	SiteLayerTraffic.hh \
	Obs.hh \
	WxFcst.hh \
	layer_info.hh \
	Utilites.hh \
	RoadCond.hh


CPPC_SRCS = 	\
	bridge.cc \
	calc_mobility.cc \
	calc_local_time.cc \
	calc_road_temp.cc \
	ChemConc.cc \
	main.cc	\
	CharacterizeStorm.cc \
	CalcChemRate.cc \
	Utilities.cc \
	metro_direct.cc \
	metro_file_io.cc \
	parse_traffic_layer_config.cc \
	parse_treatment_list.cc \
	read_last_rc_file.cc \
	read_layer.cc \
	read_traffic.cc \
	read_obs.cc \
	read_wx.cc \
	road_cond.cc \
	RulesOfPractice.cc \
	SiteLayerTraffic.cc \
	snow_density.cc \
	store_road_cond.cc

include $(RAP_MAKE_INC_DIR)/rap_make_targets

# local targets

depend: depend_generic

# DO NOT DELETE THIS LINE -- make depend depends on it.

