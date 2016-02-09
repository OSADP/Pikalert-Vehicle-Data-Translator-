#!/usr/bin/env python
#
# File containing standard definitions for various things
#

# File suffixes or extensions
ADDS_SUFFIX = "bin"
ASC_SUFFIX = "asc"
BUFR_SUFFIX = "bufr"
EPL_SUFFIX = "epl"
GINI_SUFFIX = "gini"
GRB_SUFFIX = "grb"
GRB2_SUFFIX = "grb2"
GZIP_SUFFIX = "gz"
HDF_SUFFIX = "h5"
NC_SUFFIX  = "nc"
NETCDF_SUFFIX = "nc"
PKL_SUFFIX = "pkl"
PYL_SUFFIX = "pyl"

# File format names - used for naming subdirs
Ascii = "ascii"
Grib = "grib"
Grib2 = "grib2"
Hdf = "hdf"
Mdv = "mdv"
Netcdf = "netcdf"

# Time period names - used for naming subdirs
Daily = "daily"
History = "history"
Hourly = "hourly"
Init = "init"

# Index file information
Index_base = "index"
Index_period = 24
Index_wait = 0
Index_out_base = "index_out"
Index_prior_days = 1

# Forecast time variables
Fcst_hours = [0,1,2,3,4,5,6,7,8,9,10,11,12]
Fcst_int_fcst_tol = 3600
Fcst_int_ver_tol = 3600
Fcst_int_num_days = 2

# Names of things
DM_base = "DM"
Fcst_gen_base = "fcst_gen"
Fcst_listing_base = "fcst_listing"
Hrrr_base = "hrrr"
Lamp_base = "LAMP"
Metar_base = "METAR"
Metar_interp_base = "metar_interp"
Model_base = "model"
Ncv_base = "NCV"
Ncv_fcst_base = "ncvfcst"
Ruc_base = "RUC"
Ruc_base_raw = "ruc"
Ruc_ta_base = "ruc_ta"
Ruc_ta_op_base = "ruc_ta_op"
Rr_base_raw = "WRF-RR"
Rr_base = "wrf-rr"
Gini_base = ""
Gini2nc_out_base = "ch_combine"
Persist_base = "PERSIST"
Projection_out_base = "proj"
Cloud_mask_base = "cloud"
Obs_paths_base = "obs_paths"
Score_fcst_base = "score_fcst"
Score_history_base = "score_history"
Site_cont_file_base = "Site_cont"
Site_fcst_file_base = "Site_fcst"
Site_fcst_int_base = "Site_fcst_int"

Station_name_id = "stn_name"           # name of station id array in forecast and contingency files

# Satellite 
sat_channels = ["11","3_9"]
blend_time_window = 45*60  # in seconds
Max_sat_files_to_process = 4

# Output field list for Analysis, forecast products
Var_list = ["CEILING", "VISIBILITY", "FLIGHTCAT", "CEILINGCONF", "VISIBILITYCONF", "FLIGHTCATCONF"]
Fcst_var_list = ["CEILING", "VISIBILITY", "FLIGHTCAT"]


# Name schema information. This is a hash table set up so that you can
# get at the schema formats and patterns if you know the file base name
# (the key).
#
Name_schema_formats =   {
                        DM_base         : "%B.%D.i%I.f%F.%S",
                        Fcst_gen_base : "%B.%D.i%I.f%F.%S",
                        Fcst_listing_base : "%B.%D.i%I.f%F.%S",
                        Hrrr_base : "%B.%D.i%I.f%F.%S",
                        Lamp_base       : "%B.%D.i%I.f%F.%S",
                        Metar_base      : "%B.%D.i%I.f%F.%S",
                        Metar_interp_base : "%B.%D.%I%F.%S",
                        Model_base      : "%B.%D.i%I.f%F.%S",
                        Ncv_base        : "%D%I_%B_%F.%S",
                        Ncv_fcst_base   : "%B.%D.i%I.f%F.%S",
                        Obs_paths_base  : "%B.%D.i%I.f%F.%S",
                        Persist_base    : "%B.%D.i%I.f%F.%S",
                        Rr_base         : "%B.%D.i%I.f%F.%S",
                        Rr_base_raw     : "%D_i%I_f%F_%B.%S",
                        Ruc_base        : "%B.%D.i%I.f%F.%S",
                        Ruc_base_raw    : "%B.%D.i%I.f%F.%S",
                        Ruc_ta_base     : "%B.%D.i%I.f%F.%S",
                        Ruc_ta_op_base  : "%B.%D.i%I.f%F.%S",
                        Score_fcst_base : "%B.%D.i%I.f%F.%S",
                        Score_history_base : "%B.%D.i%I.f%F.%S",
                        Site_cont_file_base : "%B.%D.i%I.f%F.%S",
                        Site_fcst_file_base : "%B.%D.i%I.f%F.%S",
                        Site_fcst_int_base : "%B.%D.i%I.f%F.%S"
                        }


Name_schema_patterns =  {
                        DM_base + ASC_SUFFIX  : [DM_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        DM_base + HDF_SUFFIX : [DM_base, "YYYYMMDD", "HH", "HH", HDF_SUFFIX],
                        DM_base + NC_SUFFIX  : [DM_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Fcst_gen_base + NC_SUFFIX  : [Fcst_gen_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Fcst_listing_base + ASC_SUFFIX  : [Fcst_listing_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        Hrrr_base        : [Hrrr_base, "YYYYMMDD", "HH", "HH", GRB2_SUFFIX],
                        Lamp_base + ASC_SUFFIX  : [Lamp_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        Lamp_base + HDF_SUFFIX : [Lamp_base, "YYYYMMDD", "HH", "HH", HDF_SUFFIX],
                        Lamp_base + NC_SUFFIX  : [Lamp_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Metar_base + HDF_SUFFIX : [Metar_base, "YYYYMMDD", "HH", "HH", HDF_SUFFIX],
                        Metar_base + NC_SUFFIX  : [Metar_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Metar_base + ASC_SUFFIX  : [Metar_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        Metar_interp_base : [Metar_interp_base, "YYYYMMDD", "HHMM", "", NC_SUFFIX],
                        Model_base      : ["", "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Ncv_base        : ["YYYYMMDD", "HHMM", Ncv_base, "H", GRB_SUFFIX],
                        Ncv_fcst_base   : [Ncv_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Obs_paths_base      : [Obs_paths_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        Persist_base + ASC_SUFFIX  : [Persist_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        Persist_base + NC_SUFFIX  : [Persist_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Ruc_base + ASC_SUFFIX : [Ruc_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        Ruc_base + HDF_SUFFIX : [Ruc_base, "YYYYMMDD", "HH", "HH", HDF_SUFFIX],
                        Ruc_base + NC_SUFFIX : [Ruc_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Ruc_base_raw    : [Ruc_base_raw, "YYYYMMDD", "HH", "HH", GRB2_SUFFIX],
                        Ruc_ta_base     : [Ruc_ta_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Rr_base         : [Rr_base, "YYYYMMDD", "HH", "HH", GRB2_SUFFIX],
                        Rr_base_raw    : ["YYYYMMDD", "HH", "HHH", Rr_base_raw, GRB2_SUFFIX],
                        Ruc_ta_op_base  : [Ruc_ta_op_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Score_fcst_base      : [Score_fcst_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Score_history_base + ASC_SUFFIX  : [Score_history_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        Site_cont_file_base + ASC_SUFFIX  : [Site_cont_file_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        Site_cont_file_base + NC_SUFFIX   : [Site_cont_file_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Site_fcst_file_base + ASC_SUFFIX  : [Site_fcst_file_base, "YYYYMMDD", "HH", "HH", ASC_SUFFIX],
                        Site_fcst_file_base + NC_SUFFIX   : [Site_fcst_file_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Site_fcst_int_base + NC_SUFFIX    : [Site_fcst_int_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX],
                        Site_fcst_int_base + ASC_SUFFIX   : [Site_fcst_int_base, "YYYYMMDD", "HH", "HH", NC_SUFFIX]
                        }
