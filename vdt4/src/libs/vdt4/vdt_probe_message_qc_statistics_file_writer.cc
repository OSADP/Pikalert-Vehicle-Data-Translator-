// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_probe_message_qc_statistics_file_writer.hh"

const char* vdt_probe_message_qc_statistics_file_writer::PROBE_MESSAGE_QC_ID = "id";
const char* vdt_probe_message_qc_statistics_file_writer::TOTAL_NUM_MSG = "total_num_msg";

const char* vdt_probe_message_qc_statistics_file_writer::BEGIN_TIME = "begin_time";
const char* vdt_probe_message_qc_statistics_file_writer::END_TIME = "end_time";

const char* vdt_probe_message_qc_statistics_file_writer::MODEL_AIR_TEMP = "model_air_temp";
const char* vdt_probe_message_qc_statistics_file_writer::MODEL_DEWPOINT = "model_dewpoint";
const char* vdt_probe_message_qc_statistics_file_writer::MODEL_BAR_PRESS = "model_bar_press";
const char* vdt_probe_message_qc_statistics_file_writer::RADAR_REF = "radar_ref";
const char* vdt_probe_message_qc_statistics_file_writer::RADAR_PRECIP_FLAG = "radar_precip_flag";
const char* vdt_probe_message_qc_statistics_file_writer::RADAR_PRECIP_TYPE = "radar_precip_type";
const char* vdt_probe_message_qc_statistics_file_writer::CLOUD_MASK = "cloud_mask";
const char* vdt_probe_message_qc_statistics_file_writer::NSS_AIR_TEMP_MEAN = "nss_air_temp_mean";
const char* vdt_probe_message_qc_statistics_file_writer::NSS_BAR_PRESS_MEAN = "nss_bar_press_mean";
const char* vdt_probe_message_qc_statistics_file_writer::NSS_DEW_TEMP_MEAN = "nss_dew_temp_mean";
const char* vdt_probe_message_qc_statistics_file_writer::NSS_WIND_DIR_MEAN = "nss_wind_dir_mean";
const char* vdt_probe_message_qc_statistics_file_writer::NSS_WIND_SPEED_MEAN = "nss_wind_speed_mean";
const char* vdt_probe_message_qc_statistics_file_writer::NSS_HOURLY_PRECIP_MEAN = "nss_hourly_precip_mean";
const char* vdt_probe_message_qc_statistics_file_writer::NSS_PREVAIL_VIS_MEAN = "nss_prevail_vis_mean";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_AIR_TEMP = "num_msg_valid_air_temp";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP_MIN = "air_temp_min";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP_MAX = "air_temp_max";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP_MEAN = "air_temp_mean";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP_MEDIAN = "air_temp_median";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP_VAR = "air_temp_var";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP_STDEV = "air_temp_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP_IQR25 = "air_temp_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP_IQR75 = "air_temp_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_AIR_TEMP2 = "num_msg_valid_air_temp2";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP2_MIN = "air_temp2_min";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP2_MAX = "air_temp2_max";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP2_MEAN = "air_temp2_mean";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP2_MEDIAN = "air_temp2_median";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP2_VAR = "air_temp2_var";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP2_STDEV = "air_temp2_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP2_IQR25 = "air_temp2_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::AIR_TEMP2_IQR75 = "air_temp2_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_BAR_PRESS = "num_msg_valid_bar_press";
const char* vdt_probe_message_qc_statistics_file_writer::BAR_PRESS_MIN = "bar_press_min";
const char* vdt_probe_message_qc_statistics_file_writer::BAR_PRESS_MAX = "bar_press_max";
const char* vdt_probe_message_qc_statistics_file_writer::BAR_PRESS_MEAN = "bar_press_mean";
const char* vdt_probe_message_qc_statistics_file_writer::BAR_PRESS_MEDIAN = "bar_press_median";
const char* vdt_probe_message_qc_statistics_file_writer::BAR_PRESS_VAR = "bar_press_var";
const char* vdt_probe_message_qc_statistics_file_writer::BAR_PRESS_STDEV = "bar_press_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::BAR_PRESS_IQR25 = "bar_press_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::BAR_PRESS_IQR75 = "bar_press_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_DEW_TEMP = "num_msg_valid_dew_temp";
const char* vdt_probe_message_qc_statistics_file_writer::DEW_TEMP_MIN = "dew_temp_min";
const char* vdt_probe_message_qc_statistics_file_writer::DEW_TEMP_MAX = "dew_temp_max";
const char* vdt_probe_message_qc_statistics_file_writer::DEW_TEMP_MEAN = "dew_temp_mean";
const char* vdt_probe_message_qc_statistics_file_writer::DEW_TEMP_MEDIAN = "dew_temp_median";
const char* vdt_probe_message_qc_statistics_file_writer::DEW_TEMP_VAR = "dew_temp_var";
const char* vdt_probe_message_qc_statistics_file_writer::DEW_TEMP_STDEV = "dew_temp_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::DEW_TEMP_IQR25 = "dew_temp_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::DEW_TEMP_IQR75 = "dew_temp_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_HEADING = "num_msg_valid_heading";
const char* vdt_probe_message_qc_statistics_file_writer::HEADING_MIN = "heading_min";
const char* vdt_probe_message_qc_statistics_file_writer::HEADING_MAX = "heading_max";
const char* vdt_probe_message_qc_statistics_file_writer::HEADING_MEAN = "heading_mean";
const char* vdt_probe_message_qc_statistics_file_writer::HEADING_MEDIAN = "heading_median";
const char* vdt_probe_message_qc_statistics_file_writer::HEADING_VAR = "heading_var";
const char* vdt_probe_message_qc_statistics_file_writer::HEADING_STDEV = "heading_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::HEADING_IQR25 = "heading_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::HEADING_IQR75 = "heading_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_LAT_ACCEL = "num_msg_valid_hoz_accel_lat";
const char* vdt_probe_message_qc_statistics_file_writer::LAT_ACCEL_MIN = "hoz_accel_lat_min";
const char* vdt_probe_message_qc_statistics_file_writer::LAT_ACCEL_MAX = "hoz_accel_lat_max";
const char* vdt_probe_message_qc_statistics_file_writer::LAT_ACCEL_MEAN = "hoz_accel_lat_mean";
const char* vdt_probe_message_qc_statistics_file_writer::LAT_ACCEL_MEDIAN = "hoz_accel_lat_median";
const char* vdt_probe_message_qc_statistics_file_writer::LAT_ACCEL_VAR = "hoz_accel_lat_var";
const char* vdt_probe_message_qc_statistics_file_writer::LAT_ACCEL_STDEV = "hoz_accel_lat_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::LAT_ACCEL_IQR25 = "hoz_accel_lat_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::LAT_ACCEL_IQR75 = "hoz_accel_lat_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_LON_ACCEL = "num_msg_valid_hoz_accel_lon";
const char* vdt_probe_message_qc_statistics_file_writer::LON_ACCEL_MIN = "hoz_accel_lon_min";
const char* vdt_probe_message_qc_statistics_file_writer::LON_ACCEL_MAX = "hoz_accel_lon_max";
const char* vdt_probe_message_qc_statistics_file_writer::LON_ACCEL_MEAN = "hoz_accel_lon_mean";
const char* vdt_probe_message_qc_statistics_file_writer::LON_ACCEL_MEDIAN = "hoz_accel_lon_median";
const char* vdt_probe_message_qc_statistics_file_writer::LON_ACCEL_VAR = "hoz_accel_lon_var";
const char* vdt_probe_message_qc_statistics_file_writer::LON_ACCEL_STDEV = "hoz_accel_lon_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::LON_ACCEL_IQR25 = "hoz_accel_lon_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::LON_ACCEL_IQR75 = "hoz_accel_lon_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_YAW = "num_msg_valid_yaw";
const char* vdt_probe_message_qc_statistics_file_writer::YAW_MIN = "yaw_min";
const char* vdt_probe_message_qc_statistics_file_writer::YAW_MAX = "yaw_max";
const char* vdt_probe_message_qc_statistics_file_writer::YAW_MEAN = "yaw_mean";
const char* vdt_probe_message_qc_statistics_file_writer::YAW_MEDIAN = "yaw_median";
const char* vdt_probe_message_qc_statistics_file_writer::YAW_VAR = "yaw_var";
const char* vdt_probe_message_qc_statistics_file_writer::YAW_STDEV = "yaw_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::YAW_IQR25 = "yaw_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::YAW_IQR75 = "yaw_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_STEERING_ANGLE = "num_msg_valid_steering_angle";
const char* vdt_probe_message_qc_statistics_file_writer::STEERING_ANGLE_MIN = "steering_angle_min";
const char* vdt_probe_message_qc_statistics_file_writer::STEERING_ANGLE_MAX = "steering_angle_max";
const char* vdt_probe_message_qc_statistics_file_writer::STEERING_ANGLE_MEAN = "steering_angle_mean";
const char* vdt_probe_message_qc_statistics_file_writer::STEERING_ANGLE_MEDIAN = "steering_angle_median";
const char* vdt_probe_message_qc_statistics_file_writer::STEERING_ANGLE_VAR = "steering_angle_var";
const char* vdt_probe_message_qc_statistics_file_writer::STEERING_ANGLE_STDEV = "steering_angle_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::STEERING_ANGLE_IQR25 = "steering_angle_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::STEERING_ANGLE_IQR75 = "steering_angle_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_LIGHTS = "num_msg_valid_lights";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_OFF = "num_lights_off";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_HAZARD = "num_lights_hazard";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_PARKING = "num_lights_parking";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_FOG = "num_lights_fog";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_DRL = "num_lights_drl";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_AUTOMATIC_CONTROL = "num_lights_automatic_control";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_RIGHT_TURN = "num_lights_right_turn";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_LEFT_TURN = "num_lights_left_turn";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_HIGH_BEAM = "num_lights_high_beam";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_LIGHTS_LOW_BEAM = "num_lights_low_beam";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_SPEED = "num_msg_valid_speed";
const char* vdt_probe_message_qc_statistics_file_writer::SPEED_RATIO = "speed_ratio";
const char* vdt_probe_message_qc_statistics_file_writer::SPEED_MIN = "speed_min";
const char* vdt_probe_message_qc_statistics_file_writer::SPEED_MAX = "speed_max";
const char* vdt_probe_message_qc_statistics_file_writer::SPEED_MEAN = "speed_mean";
const char* vdt_probe_message_qc_statistics_file_writer::SPEED_MEDIAN = "speed_median";
const char* vdt_probe_message_qc_statistics_file_writer::SPEED_VAR = "speed_var";
const char* vdt_probe_message_qc_statistics_file_writer::SPEED_STDEV = "speed_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::SPEED_IQR25 = "speed_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::SPEED_IQR75 = "speed_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_SURFACE_TEMP = "num_msg_valid_surface_temp";
const char* vdt_probe_message_qc_statistics_file_writer::SURFACE_TEMP_MIN = "surface_temp_min";
const char* vdt_probe_message_qc_statistics_file_writer::SURFACE_TEMP_MAX = "surface_temp_max";
const char* vdt_probe_message_qc_statistics_file_writer::SURFACE_TEMP_MEAN = "surface_temp_mean";
const char* vdt_probe_message_qc_statistics_file_writer::SURFACE_TEMP_MEDIAN = "surface_temp_median";
const char* vdt_probe_message_qc_statistics_file_writer::SURFACE_TEMP_VAR = "surface_temp_var";
const char* vdt_probe_message_qc_statistics_file_writer::SURFACE_TEMP_STDEV = "surface_temp_stdev";
const char* vdt_probe_message_qc_statistics_file_writer::SURFACE_TEMP_IQR25 = "surface_temp_iqr25";
const char* vdt_probe_message_qc_statistics_file_writer::SURFACE_TEMP_IQR75 = "surface_temp_iqr75";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_WIPERS = "num_msg_valid_wipers";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_WIPERS_NOT_EQUIPPED = "num_wipers_not_equipped";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_WIPERS_OFF = "num_wipers_off";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_WIPERS_INTERMITTENT = "num_wipers_intermittent";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_WIPERS_LOW = "num_wipers_low";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_WIPERS_HIGH = "num_wipers_high";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_WIPERS_WASHER = "num_wipers_washer";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_WIPERS_AUTOMATIC = "num_wipers_automatic";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_ABS = "num_msg_valid_abs";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_ABS_NOT_EQUIPPED = "num_abs_not_equipped";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_ABS_OFF = "num_abs_off";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_ABS_ON = "num_abs_on";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_ABS_ENGAGED = "num_abs_engaged";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_STAB = "num_msg_valid_stab";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_STAB_NOT_EQUIPPED = "num_stab_not_equipped";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_STAB_OFF = "num_stab_off";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_STAB_ON = "num_stab_on";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_STAB_ENGAGED = "num_stab_engaged";

const char* vdt_probe_message_qc_statistics_file_writer::NUM_MSG_VALID_TRAC = "num_msg_valid_trac";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_TRAC_NOT_EQUIPPED = "num_trac_not_equipped";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_TRAC_OFF = "num_trac_off";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_TRAC_ON = "num_trac_on";
const char* vdt_probe_message_qc_statistics_file_writer::NUM_TRAC_ENGAGED = "num_trac_engaged";

vdt_probe_message_qc_statistics_file_writer::vdt_probe_message_qc_statistics_file_writer(const char* fpath, const char* cdl) : vdt_nc_file_writer(fpath,cdl)
{
}

vdt_probe_message_qc_statistics_file_writer::~vdt_probe_message_qc_statistics_file_writer()
{
}

int vdt_probe_message_qc_statistics_file_writer::write_probe_message_qc_statistics(vector<vdt_probe_message_qc_statistics>& stats)
{
  NcVar* id_var = nc->get_var(PROBE_MESSAGE_QC_ID);
  NcVar* num_msg_var = nc->get_var(TOTAL_NUM_MSG);
  NcVar* begin_time_var = nc->get_var(BEGIN_TIME);
  NcVar* end_time_var = nc->get_var(END_TIME);
  NcVar* model_air_temp_var = nc->get_var(MODEL_AIR_TEMP);
  NcVar* model_dewpoint_var = nc->get_var(MODEL_DEWPOINT);
  NcVar* model_bar_press_var = nc->get_var(MODEL_BAR_PRESS);
  NcVar* radar_ref_var = nc->get_var(RADAR_REF);
  NcVar* radar_precip_flag_var = nc->get_var(RADAR_PRECIP_FLAG);
  NcVar* radar_precip_type_var = nc->get_var(RADAR_PRECIP_TYPE);
  NcVar* cloud_mask_var = nc->get_var(CLOUD_MASK);
  NcVar* nss_air_temp_mean_var = nc->get_var(NSS_AIR_TEMP_MEAN);
  NcVar* nss_bar_press_mean_var = nc->get_var(NSS_BAR_PRESS_MEAN);
  NcVar* nss_dew_temp_mean_var = nc->get_var(NSS_DEW_TEMP_MEAN);
  NcVar* nss_wind_dir_mean_var = nc->get_var(NSS_WIND_DIR_MEAN);
  NcVar* nss_wind_speed_mean_var = nc->get_var(NSS_WIND_SPEED_MEAN);
  NcVar* nss_hourly_precip_mean_var = nc->get_var(NSS_HOURLY_PRECIP_MEAN);
  NcVar* nss_prevail_vis_mean_var = nc->get_var(NSS_PREVAIL_VIS_MEAN);

  NcVar* num_air_temp_var = nc->get_var(NUM_MSG_VALID_AIR_TEMP);
  NcVar* air_temp_min_var = nc->get_var(AIR_TEMP_MIN);
  NcVar* air_temp_max_var = nc->get_var(AIR_TEMP_MAX);
  NcVar* air_temp_mean_var = nc->get_var(AIR_TEMP_MEAN);
  NcVar* air_temp_median_var = nc->get_var(AIR_TEMP_MEDIAN);
  NcVar* air_temp_var_var = nc->get_var(AIR_TEMP_VAR);
  NcVar* air_temp_stdev_var = nc->get_var(AIR_TEMP_STDEV);
  NcVar* air_temp_iqr25_var = nc->get_var(AIR_TEMP_IQR25);
  NcVar* air_temp_iqr75_var = nc->get_var(AIR_TEMP_IQR75);

  NcVar* num_air_temp2_var = nc->get_var(NUM_MSG_VALID_AIR_TEMP2);
  NcVar* air_temp2_min_var = nc->get_var(AIR_TEMP2_MIN);
  NcVar* air_temp2_max_var = nc->get_var(AIR_TEMP2_MAX);
  NcVar* air_temp2_mean_var = nc->get_var(AIR_TEMP2_MEAN);
  NcVar* air_temp2_median_var = nc->get_var(AIR_TEMP2_MEDIAN);
  NcVar* air_temp2_var_var = nc->get_var(AIR_TEMP2_VAR);
  NcVar* air_temp2_stdev_var = nc->get_var(AIR_TEMP2_STDEV);
  NcVar* air_temp2_iqr25_var = nc->get_var(AIR_TEMP2_IQR25);
  NcVar* air_temp2_iqr75_var = nc->get_var(AIR_TEMP2_IQR75);

  NcVar* num_bar_press_var = nc->get_var(NUM_MSG_VALID_BAR_PRESS);
  NcVar* bar_press_min_var = nc->get_var(BAR_PRESS_MIN);
  NcVar* bar_press_max_var = nc->get_var(BAR_PRESS_MAX);
  NcVar* bar_press_mean_var = nc->get_var(BAR_PRESS_MEAN);
  NcVar* bar_press_median_var = nc->get_var(BAR_PRESS_MEDIAN);
  NcVar* bar_press_var_var = nc->get_var(BAR_PRESS_VAR);
  NcVar* bar_press_stdev_var = nc->get_var(BAR_PRESS_STDEV);
  NcVar* bar_press_iqr25_var = nc->get_var(BAR_PRESS_IQR25);
  NcVar* bar_press_iqr75_var = nc->get_var(BAR_PRESS_IQR75);

  NcVar* num_dew_temp_var = nc->get_var(NUM_MSG_VALID_DEW_TEMP);
  NcVar* dew_temp_min_var = nc->get_var(DEW_TEMP_MIN);
  NcVar* dew_temp_max_var = nc->get_var(DEW_TEMP_MAX);
  NcVar* dew_temp_mean_var = nc->get_var(DEW_TEMP_MEAN);
  NcVar* dew_temp_median_var = nc->get_var(DEW_TEMP_MEDIAN);
  NcVar* dew_temp_var_var = nc->get_var(DEW_TEMP_VAR);
  NcVar* dew_temp_stdev_var = nc->get_var(DEW_TEMP_STDEV);
  NcVar* dew_temp_iqr25_var = nc->get_var(DEW_TEMP_IQR25);
  NcVar* dew_temp_iqr75_var = nc->get_var(DEW_TEMP_IQR75);

  NcVar* num_heading_var = nc->get_var(NUM_MSG_VALID_HEADING);
  NcVar* heading_min_var = nc->get_var(HEADING_MIN);
  NcVar* heading_max_var = nc->get_var(HEADING_MAX);
  NcVar* heading_mean_var = nc->get_var(HEADING_MEAN);
  NcVar* heading_median_var = nc->get_var(HEADING_MEDIAN);
  NcVar* heading_var_var = nc->get_var(HEADING_VAR);
  NcVar* heading_stdev_var = nc->get_var(HEADING_STDEV);
  NcVar* heading_iqr25_var = nc->get_var(HEADING_IQR25);
  NcVar* heading_iqr75_var = nc->get_var(HEADING_IQR75);

  NcVar* num_lat_accel_var = nc->get_var(NUM_MSG_VALID_LAT_ACCEL);
  NcVar* lat_accel_min_var = nc->get_var(LAT_ACCEL_MIN);
  NcVar* lat_accel_max_var = nc->get_var(LAT_ACCEL_MAX);
  NcVar* lat_accel_mean_var = nc->get_var(LAT_ACCEL_MEAN);
  NcVar* lat_accel_median_var = nc->get_var(LAT_ACCEL_MEDIAN);
  NcVar* lat_accel_var_var = nc->get_var(LAT_ACCEL_VAR);
  NcVar* lat_accel_stdev_var = nc->get_var(LAT_ACCEL_STDEV);
  NcVar* lat_accel_iqr25_var = nc->get_var(LAT_ACCEL_IQR25);
  NcVar* lat_accel_iqr75_var = nc->get_var(LAT_ACCEL_IQR75);

  NcVar* num_lon_accel_var = nc->get_var(NUM_MSG_VALID_LON_ACCEL);
  NcVar* lon_accel_min_var = nc->get_var(LON_ACCEL_MIN);
  NcVar* lon_accel_max_var = nc->get_var(LON_ACCEL_MAX);
  NcVar* lon_accel_mean_var = nc->get_var(LON_ACCEL_MEAN);
  NcVar* lon_accel_median_var = nc->get_var(LON_ACCEL_MEDIAN);
  NcVar* lon_accel_var_var = nc->get_var(LON_ACCEL_VAR);
  NcVar* lon_accel_stdev_var = nc->get_var(LON_ACCEL_STDEV);
  NcVar* lon_accel_iqr25_var = nc->get_var(LON_ACCEL_IQR25);
  NcVar* lon_accel_iqr75_var = nc->get_var(LON_ACCEL_IQR75);

  NcVar* num_yaw_var = nc->get_var(NUM_MSG_VALID_YAW);
  NcVar* yaw_min_var = nc->get_var(YAW_MIN);
  NcVar* yaw_max_var = nc->get_var(YAW_MAX);
  NcVar* yaw_mean_var = nc->get_var(YAW_MEAN);
  NcVar* yaw_median_var = nc->get_var(YAW_MEDIAN);
  NcVar* yaw_var_var = nc->get_var(YAW_VAR);
  NcVar* yaw_stdev_var = nc->get_var(YAW_STDEV);
  NcVar* yaw_iqr25_var = nc->get_var(YAW_IQR25);
  NcVar* yaw_iqr75_var = nc->get_var(YAW_IQR75);

  NcVar* num_steering_angle_var = nc->get_var(NUM_MSG_VALID_STEERING_ANGLE);
  NcVar* steering_angle_min_var = nc->get_var(STEERING_ANGLE_MIN);
  NcVar* steering_angle_max_var = nc->get_var(STEERING_ANGLE_MAX);
  NcVar* steering_angle_mean_var = nc->get_var(STEERING_ANGLE_MEAN);
  NcVar* steering_angle_median_var = nc->get_var(STEERING_ANGLE_MEDIAN);
  NcVar* steering_angle_var_var = nc->get_var(STEERING_ANGLE_VAR);
  NcVar* steering_angle_stdev_var = nc->get_var(STEERING_ANGLE_STDEV);
  NcVar* steering_angle_iqr25_var = nc->get_var(STEERING_ANGLE_IQR25);
  NcVar* steering_angle_iqr75_var = nc->get_var(STEERING_ANGLE_IQR75);

  NcVar* num_msg_valid_lights_var = nc->get_var(NUM_MSG_VALID_LIGHTS);
  NcVar* num_lights_off_var = nc->get_var(NUM_LIGHTS_OFF);
  NcVar* num_lights_hazard_var = nc->get_var(NUM_LIGHTS_HAZARD);
  NcVar* num_lights_parking_var = nc->get_var(NUM_LIGHTS_PARKING);
  NcVar* num_lights_fog_var = nc->get_var(NUM_LIGHTS_FOG);
  NcVar* num_lights_drl_var = nc->get_var(NUM_LIGHTS_DRL);
  NcVar* num_lights_automatic_control_var = nc->get_var(NUM_LIGHTS_AUTOMATIC_CONTROL);
  NcVar* num_lights_right_turn_var = nc->get_var(NUM_LIGHTS_RIGHT_TURN);
  NcVar* num_lights_left_turn_var = nc->get_var(NUM_LIGHTS_LEFT_TURN);
  NcVar* num_lights_high_beam_var = nc->get_var(NUM_LIGHTS_HIGH_BEAM);
  NcVar* num_lights_low_beam_var = nc->get_var(NUM_LIGHTS_LOW_BEAM);

  NcVar* num_speed_var = nc->get_var(NUM_MSG_VALID_SPEED);
  NcVar* speed_ratio_var = nc->get_var(SPEED_RATIO);
  NcVar* speed_min_var = nc->get_var(SPEED_MIN);
  NcVar* speed_max_var = nc->get_var(SPEED_MAX);
  NcVar* speed_mean_var = nc->get_var(SPEED_MEAN);
  NcVar* speed_median_var = nc->get_var(SPEED_MEDIAN);
  NcVar* speed_var_var = nc->get_var(SPEED_VAR);
  NcVar* speed_stdev_var = nc->get_var(SPEED_STDEV);
  NcVar* speed_iqr25_var = nc->get_var(SPEED_IQR25);
  NcVar* speed_iqr75_var = nc->get_var(SPEED_IQR75);

  NcVar* num_surface_temp_var = nc->get_var(NUM_MSG_VALID_SURFACE_TEMP);
  NcVar* surface_temp_min_var = nc->get_var(SURFACE_TEMP_MIN);
  NcVar* surface_temp_max_var = nc->get_var(SURFACE_TEMP_MAX);
  NcVar* surface_temp_mean_var = nc->get_var(SURFACE_TEMP_MEAN);
  NcVar* surface_temp_median_var = nc->get_var(SURFACE_TEMP_MEDIAN);
  NcVar* surface_temp_var_var = nc->get_var(SURFACE_TEMP_VAR);
  NcVar* surface_temp_stdev_var = nc->get_var(SURFACE_TEMP_STDEV);
  NcVar* surface_temp_iqr25_var = nc->get_var(SURFACE_TEMP_IQR25);
  NcVar* surface_temp_iqr75_var = nc->get_var(SURFACE_TEMP_IQR75);

  NcVar* num_wipers_var = nc->get_var(NUM_MSG_VALID_WIPERS);
  NcVar* num_wipers_ne_var = nc->get_var(NUM_WIPERS_NOT_EQUIPPED);
  NcVar* num_wipers_off_var = nc->get_var(NUM_WIPERS_OFF);
  NcVar* num_wipers_int_var = nc->get_var(NUM_WIPERS_INTERMITTENT);
  NcVar* num_wipers_low_var = nc->get_var(NUM_WIPERS_LOW);
  NcVar* num_wipers_high_var = nc->get_var(NUM_WIPERS_HIGH);
  NcVar* num_wipers_washer_var = nc->get_var(NUM_WIPERS_WASHER);
  NcVar* num_wipers_auto_var = nc->get_var(NUM_WIPERS_AUTOMATIC);

  NcVar* num_msg_valid_abs_var = nc->get_var(NUM_MSG_VALID_ABS);
  NcVar* num_abs_ne_var = nc->get_var(NUM_ABS_NOT_EQUIPPED);
  NcVar* num_abs_off_var = nc->get_var(NUM_ABS_OFF);
  NcVar* num_abs_on_var = nc->get_var(NUM_ABS_ON);
  NcVar* num_abs_engaged_var = nc->get_var(NUM_ABS_ENGAGED);

  NcVar* num_msg_valid_stab_var = nc->get_var(NUM_MSG_VALID_STAB);
  NcVar* num_stab_ne_var = nc->get_var(NUM_STAB_NOT_EQUIPPED);
  NcVar* num_stab_off_var = nc->get_var(NUM_STAB_OFF);
  NcVar* num_stab_on_var = nc->get_var(NUM_STAB_ON);
  NcVar* num_stab_engaged_var = nc->get_var(NUM_STAB_ENGAGED);

  NcVar* num_msg_valid_trac_var = nc->get_var(NUM_MSG_VALID_TRAC);
  NcVar* num_trac_ne_var = nc->get_var(NUM_TRAC_NOT_EQUIPPED);
  NcVar* num_trac_off_var = nc->get_var(NUM_TRAC_OFF);
  NcVar* num_trac_on_var = nc->get_var(NUM_TRAC_ON);
  NcVar* num_trac_engaged_var = nc->get_var(NUM_TRAC_ENGAGED);

  for (size_t i = 0; i < stats.size(); i++)
    {
      vdt_probe_message_qc_statistics stat = stats[i];

      id_var->set_cur(i);
      num_msg_var->set_cur(i);
      begin_time_var->set_cur(i);
      end_time_var->set_cur(i);
      model_air_temp_var->set_cur(i);
      model_dewpoint_var->set_cur(i);
      model_bar_press_var->set_cur(i);
      radar_ref_var->set_cur(i);
      radar_precip_flag_var->set_cur(i);
      radar_precip_type_var->set_cur(i);
      cloud_mask_var->set_cur(i);
      nss_air_temp_mean_var->set_cur(i);
      nss_bar_press_mean_var->set_cur(i);
      nss_dew_temp_mean_var->set_cur(i);
      nss_wind_dir_mean_var->set_cur(i);
      nss_wind_speed_mean_var->set_cur(i);
      nss_hourly_precip_mean_var->set_cur(i);
      nss_prevail_vis_mean_var->set_cur(i);

      num_air_temp_var->set_cur(i);
      air_temp_min_var->set_cur(i);
      air_temp_max_var->set_cur(i);
      air_temp_mean_var->set_cur(i);
      air_temp_median_var->set_cur(i);
      air_temp_var_var->set_cur(i);
      air_temp_stdev_var->set_cur(i);
      air_temp_iqr25_var->set_cur(i);
      air_temp_iqr75_var->set_cur(i);

      num_air_temp2_var->set_cur(i);
      air_temp2_min_var->set_cur(i);
      air_temp2_max_var->set_cur(i);
      air_temp2_mean_var->set_cur(i);
      air_temp2_median_var->set_cur(i);
      air_temp2_var_var->set_cur(i);
      air_temp2_stdev_var->set_cur(i);
      air_temp2_iqr25_var->set_cur(i);
      air_temp2_iqr75_var->set_cur(i);

      num_bar_press_var->set_cur(i);
      bar_press_min_var->set_cur(i);
      bar_press_max_var->set_cur(i);
      bar_press_mean_var->set_cur(i);
      bar_press_median_var->set_cur(i);
      bar_press_var_var->set_cur(i);
      bar_press_stdev_var->set_cur(i);
      bar_press_iqr25_var->set_cur(i);
      bar_press_iqr75_var->set_cur(i);

      num_dew_temp_var->set_cur(i);
      dew_temp_min_var->set_cur(i);
      dew_temp_max_var->set_cur(i);
      dew_temp_mean_var->set_cur(i);
      dew_temp_median_var->set_cur(i);
      dew_temp_var_var->set_cur(i);
      dew_temp_stdev_var->set_cur(i);
      dew_temp_iqr25_var->set_cur(i);
      dew_temp_iqr75_var->set_cur(i);

      num_heading_var->set_cur(i);
      heading_min_var->set_cur(i);
      heading_max_var->set_cur(i);
      heading_mean_var->set_cur(i);
      heading_median_var->set_cur(i);
      heading_var_var->set_cur(i);
      heading_stdev_var->set_cur(i);
      heading_iqr25_var->set_cur(i);
      heading_iqr75_var->set_cur(i);

      num_lat_accel_var->set_cur(i);
      lat_accel_min_var->set_cur(i);
      lat_accel_max_var->set_cur(i);
      lat_accel_mean_var->set_cur(i);
      lat_accel_median_var->set_cur(i);
      lat_accel_var_var->set_cur(i);
      lat_accel_stdev_var->set_cur(i);
      lat_accel_iqr25_var->set_cur(i);
      lat_accel_iqr75_var->set_cur(i);

      num_lon_accel_var->set_cur(i);
      lon_accel_min_var->set_cur(i);
      lon_accel_max_var->set_cur(i);
      lon_accel_mean_var->set_cur(i);
      lon_accel_median_var->set_cur(i);
      lon_accel_var_var->set_cur(i);
      lon_accel_stdev_var->set_cur(i);
      lon_accel_iqr25_var->set_cur(i);
      lon_accel_iqr75_var->set_cur(i);

      num_yaw_var->set_cur(i);
      yaw_min_var->set_cur(i);
      yaw_max_var->set_cur(i);
      yaw_mean_var->set_cur(i);
      yaw_median_var->set_cur(i);
      yaw_var_var->set_cur(i);
      yaw_stdev_var->set_cur(i);
      yaw_iqr25_var->set_cur(i);
      yaw_iqr75_var->set_cur(i);

      num_steering_angle_var->set_cur(i);
      steering_angle_min_var->set_cur(i);
      steering_angle_max_var->set_cur(i);
      steering_angle_mean_var->set_cur(i);
      steering_angle_median_var->set_cur(i);
      steering_angle_var_var->set_cur(i);
      steering_angle_stdev_var->set_cur(i);
      steering_angle_iqr25_var->set_cur(i);
      steering_angle_iqr75_var->set_cur(i);

      num_msg_valid_lights_var->set_cur(i);
      num_lights_off_var->set_cur(i);
      num_lights_hazard_var->set_cur(i);
      num_lights_parking_var->set_cur(i);
      num_lights_fog_var->set_cur(i);
      num_lights_drl_var->set_cur(i);
      num_lights_automatic_control_var->set_cur(i);
      num_lights_right_turn_var->set_cur(i);
      num_lights_left_turn_var->set_cur(i);
      num_lights_high_beam_var->set_cur(i);
      num_lights_low_beam_var->set_cur(i);

      num_speed_var->set_cur(i);
      speed_ratio_var->set_cur(i);
      speed_min_var->set_cur(i);
      speed_max_var->set_cur(i);
      speed_mean_var->set_cur(i);
      speed_median_var->set_cur(i);
      speed_var_var->set_cur(i);
      speed_stdev_var->set_cur(i);
      speed_iqr25_var->set_cur(i);
      speed_iqr75_var->set_cur(i);

      num_surface_temp_var->set_cur(i);
      surface_temp_min_var->set_cur(i);
      surface_temp_max_var->set_cur(i);
      surface_temp_mean_var->set_cur(i);
      surface_temp_median_var->set_cur(i);
      surface_temp_var_var->set_cur(i);
      surface_temp_stdev_var->set_cur(i);
      surface_temp_iqr25_var->set_cur(i);
      surface_temp_iqr75_var->set_cur(i);

      num_wipers_var->set_cur(i);
      num_wipers_ne_var->set_cur(i);
      num_wipers_off_var->set_cur(i);
      num_wipers_int_var->set_cur(i);
      num_wipers_low_var->set_cur(i);
      num_wipers_high_var->set_cur(i);
      num_wipers_washer_var->set_cur(i);
      num_wipers_auto_var->set_cur(i);

      num_msg_valid_abs_var->set_cur(i);
      num_abs_ne_var->set_cur(i);
      num_abs_off_var->set_cur(i);
      num_abs_on_var->set_cur(i);
      num_abs_engaged_var->set_cur(i);

      num_msg_valid_stab_var->set_cur(i);
      num_stab_ne_var->set_cur(i);
      num_stab_off_var->set_cur(i);
      num_stab_on_var->set_cur(i);
      num_stab_engaged_var->set_cur(i);

      num_msg_valid_trac_var->set_cur(i);
      num_trac_ne_var->set_cur(i);
      num_trac_off_var->set_cur(i);
      num_trac_on_var->set_cur(i);
      num_trac_engaged_var->set_cur(i);

      id_var->put(&stat.id,1);
      num_msg_var->put(&stat.num_msg,1);

      begin_time_var->put(&stat.begin_time, 1);
      end_time_var->put(&stat.end_time, 1);

      model_air_temp_var->put(&stat.model_air_temp,1);
      model_dewpoint_var->put(&stat.model_dewpoint,1);
      model_bar_press_var->put(&stat.model_bar_press,1);
      radar_ref_var->put(&stat.radar_ref,1);
      radar_precip_flag_var->put(&stat.radar_precip_flag,1);
      radar_precip_type_var->put(&stat.radar_precip_type,1);
      cloud_mask_var->put(&stat.cloud_mask,1);
      nss_air_temp_mean_var->put(&stat.nss_air_temp_mean,1);
      nss_bar_press_mean_var->put(&stat.nss_bar_press_mean,1);
      nss_dew_temp_mean_var->put(&stat.nss_dew_temp_mean,1);
      nss_wind_dir_mean_var->put(&stat.nss_wind_dir_mean,1);
      nss_wind_speed_mean_var->put(&stat.nss_wind_speed_mean,1);
      nss_hourly_precip_mean_var->put(&stat.nss_hourly_precip_mean,1);
      nss_prevail_vis_mean_var->put(&stat.nss_prevail_vis_mean,1);

      num_air_temp_var->put(&stat.num_msg_valid_air_temp,1);
      air_temp_min_var->put(&stat.air_temp_min,1);
      air_temp_max_var->put(&stat.air_temp_max,1);
      air_temp_mean_var->put(&stat.air_temp_mean,1);
      air_temp_median_var->put(&stat.air_temp_median,1);
      air_temp_var_var->put(&stat.air_temp_var,1);
      air_temp_stdev_var->put(&stat.air_temp_stdev,1);
      air_temp_iqr25_var->put(&stat.air_temp_iqr25,1);
      air_temp_iqr75_var->put(&stat.air_temp_iqr75,1);

      num_air_temp2_var->put(&stat.num_msg_valid_air_temp2,1);
      air_temp2_min_var->put(&stat.air_temp2_min,1);
      air_temp2_max_var->put(&stat.air_temp2_max,1);
      air_temp2_mean_var->put(&stat.air_temp2_mean,1);
      air_temp2_median_var->put(&stat.air_temp2_median,1);
      air_temp2_var_var->put(&stat.air_temp2_var,1);
      air_temp2_stdev_var->put(&stat.air_temp2_stdev,1);
      air_temp2_iqr25_var->put(&stat.air_temp2_iqr25,1);
      air_temp2_iqr75_var->put(&stat.air_temp2_iqr75,1);

      num_bar_press_var->put(&stat.num_msg_valid_bar_press,1);
      bar_press_min_var->put(&stat.bar_press_min,1);
      bar_press_max_var->put(&stat.bar_press_max,1);
      bar_press_mean_var->put(&stat.bar_press_mean,1);
      bar_press_median_var->put(&stat.bar_press_median,1);
      bar_press_var_var->put(&stat.bar_press_var,1);
      bar_press_stdev_var->put(&stat.bar_press_stdev,1);
      bar_press_iqr25_var->put(&stat.bar_press_iqr25,1);
      bar_press_iqr75_var->put(&stat.bar_press_iqr75,1);

      num_dew_temp_var->put(&stat.num_msg_valid_dew_temp,1);
      dew_temp_min_var->put(&stat.dew_temp_min,1);
      dew_temp_max_var->put(&stat.dew_temp_max,1);
      dew_temp_mean_var->put(&stat.dew_temp_mean,1);
      dew_temp_median_var->put(&stat.dew_temp_median,1);
      dew_temp_var_var->put(&stat.dew_temp_var,1);
      dew_temp_stdev_var->put(&stat.dew_temp_stdev,1);
      dew_temp_iqr25_var->put(&stat.dew_temp_iqr25,1);
      dew_temp_iqr75_var->put(&stat.dew_temp_iqr75,1);

      num_heading_var->put(&stat.num_msg_valid_heading,1);
      heading_min_var->put(&stat.heading_min,1);
      heading_max_var->put(&stat.heading_max,1);
      heading_mean_var->put(&stat.heading_mean,1);
      heading_median_var->put(&stat.heading_median,1);
      heading_var_var->put(&stat.heading_var,1);
      heading_stdev_var->put(&stat.heading_stdev,1);
      heading_iqr25_var->put(&stat.heading_iqr25,1);
      heading_iqr75_var->put(&stat.heading_iqr75,1);

      num_lat_accel_var->put(&stat.num_msg_valid_lat_accel,1);
      lat_accel_min_var->put(&stat.lat_accel_min,1);
      lat_accel_max_var->put(&stat.lat_accel_max,1);
      lat_accel_mean_var->put(&stat.lat_accel_mean,1);
      lat_accel_median_var->put(&stat.lat_accel_median,1);
      lat_accel_var_var->put(&stat.lat_accel_var,1);
      lat_accel_stdev_var->put(&stat.lat_accel_stdev,1);
      lat_accel_iqr25_var->put(&stat.lat_accel_iqr25,1);
      lat_accel_iqr75_var->put(&stat.lat_accel_iqr75,1);

      num_lon_accel_var->put(&stat.num_msg_valid_lon_accel,1);
      lon_accel_min_var->put(&stat.lon_accel_min,1);
      lon_accel_max_var->put(&stat.lon_accel_max,1);
      lon_accel_mean_var->put(&stat.lon_accel_mean,1);
      lon_accel_median_var->put(&stat.lon_accel_median,1);
      lon_accel_var_var->put(&stat.lon_accel_var,1);
      lon_accel_stdev_var->put(&stat.lon_accel_stdev,1);
      lon_accel_iqr25_var->put(&stat.lon_accel_iqr25,1);
      lon_accel_iqr75_var->put(&stat.lon_accel_iqr75,1);

      num_yaw_var->put(&stat.num_msg_valid_yaw,1);
      yaw_min_var->put(&stat.yaw_min,1);
      yaw_max_var->put(&stat.yaw_max,1);
      yaw_mean_var->put(&stat.yaw_mean,1);
      yaw_median_var->put(&stat.yaw_median,1);
      yaw_var_var->put(&stat.yaw_var,1);
      yaw_stdev_var->put(&stat.yaw_stdev,1);
      yaw_iqr25_var->put(&stat.yaw_iqr25,1);
      yaw_iqr75_var->put(&stat.yaw_iqr75,1);

      num_steering_angle_var->put(&stat.num_msg_valid_steering_angle,1);
      steering_angle_min_var->put(&stat.steering_angle_min,1);
      steering_angle_max_var->put(&stat.steering_angle_max,1);
      steering_angle_mean_var->put(&stat.steering_angle_mean,1);
      steering_angle_median_var->put(&stat.steering_angle_median,1);
      steering_angle_var_var->put(&stat.steering_angle_var,1);
      steering_angle_stdev_var->put(&stat.steering_angle_stdev,1);
      steering_angle_iqr25_var->put(&stat.steering_angle_iqr25,1);
      steering_angle_iqr75_var->put(&stat.steering_angle_iqr75,1);

      num_msg_valid_lights_var->put(&stat.num_msg_valid_lights,1);
      num_lights_off_var->put(&stat.num_lights_off,1);
      num_lights_hazard_var->put(&stat.num_lights_hazard,1);
      num_lights_parking_var->put(&stat.num_lights_parking,1);
      num_lights_fog_var->put(&stat.num_lights_fog,1);
      num_lights_drl_var->put(&stat.num_lights_drl,1);
      num_lights_automatic_control_var->put(&stat.num_lights_automatic_control,1);
      num_lights_right_turn_var->put(&stat.num_lights_right_turn,1);
      num_lights_left_turn_var->put(&stat.num_lights_left_turn,1);
      num_lights_high_beam_var->put(&stat.num_lights_high_beam,1);
      num_lights_low_beam_var->put(&stat.num_lights_low_beam,1);

      num_speed_var->put(&stat.num_msg_valid_speed,1);
      speed_ratio_var->put(&stat.speed_ratio,1);
      speed_min_var->put(&stat.speed_min,1);
      speed_max_var->put(&stat.speed_max,1);
      speed_mean_var->put(&stat.speed_mean,1);
      speed_median_var->put(&stat.speed_median,1);
      speed_var_var->put(&stat.speed_var,1);
      speed_stdev_var->put(&stat.speed_stdev,1);
      speed_iqr25_var->put(&stat.speed_iqr25,1);
      speed_iqr75_var->put(&stat.speed_iqr75,1);

      num_surface_temp_var->put(&stat.num_msg_valid_surface_temp,1);
      surface_temp_min_var->put(&stat.surface_temp_min,1);
      surface_temp_max_var->put(&stat.surface_temp_max,1);
      surface_temp_mean_var->put(&stat.surface_temp_mean,1);
      surface_temp_median_var->put(&stat.surface_temp_median,1);
      surface_temp_var_var->put(&stat.surface_temp_var,1);
      surface_temp_stdev_var->put(&stat.surface_temp_stdev,1);
      surface_temp_iqr25_var->put(&stat.surface_temp_iqr25,1);
      surface_temp_iqr75_var->put(&stat.surface_temp_iqr75,1);

      num_wipers_var->put(&stat.num_msg_valid_wipers,1);
      num_wipers_ne_var->put(&stat.num_wipers_not_equipped,1);
      num_wipers_off_var->put(&stat.num_wipers_off,1);
      num_wipers_int_var->put(&stat.num_wipers_intermittent,1);
      num_wipers_low_var->put(&stat.num_wipers_low,1);
      num_wipers_high_var->put(&stat.num_wipers_high,1);
      num_wipers_washer_var->put(&stat.num_wipers_washer,1);
      num_wipers_auto_var->put(&stat.num_wipers_automatic,1);

      num_msg_valid_abs_var->put(&stat.num_msg_valid_abs,1);
      num_abs_ne_var->put(&stat.num_abs_not_equipped,1);
      num_abs_off_var->put(&stat.num_abs_off,1);
      num_abs_on_var->put(&stat.num_abs_on,1);
      num_abs_engaged_var->put(&stat.num_abs_engaged,1);

      num_msg_valid_stab_var->put(&stat.num_msg_valid_stab,1);
      num_stab_ne_var->put(&stat.num_stab_not_equipped,1);
      num_stab_off_var->put(&stat.num_stab_off,1);
      num_stab_on_var->put(&stat.num_stab_on,1);
      num_stab_engaged_var->put(&stat.num_stab_engaged,1);

      num_msg_valid_trac_var->put(&stat.num_msg_valid_trac,1);
      num_trac_ne_var->put(&stat.num_trac_not_equipped,1);
      num_trac_off_var->put(&stat.num_trac_off,1);
      num_trac_on_var->put(&stat.num_trac_on,1);
      num_trac_engaged_var->put(&stat.num_trac_engaged,1);

    }
  return 0;
}
