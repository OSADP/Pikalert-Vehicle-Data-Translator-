
#include "vdt_probe_message_qc_file_reader.hh"

const char* vdt_probe_message_qc_file_reader::REC_NUM = "rec_num";
const char* vdt_probe_message_qc_file_reader::VEHICLE_ID_LEN = "vehicle_id_len";
const char* vdt_probe_message_qc_file_reader::ABS = "abs";
const char* vdt_probe_message_qc_file_reader::ABS_RANGE_QC_PASSED = "abs_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::BRAKE_BOOST = "brake_boost";
const char* vdt_probe_message_qc_file_reader::BRAKE_BOOST_RANGE_QC_PASSED = "brake_boost_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::BRAKE_STATUS = "brake_status";
const char* vdt_probe_message_qc_file_reader::BRAKE_STATUS_RANGE_QC_PASSED = "brake_status_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::CLOUD_MASK = "cloud_mask";
const char* vdt_probe_message_qc_file_reader::ELEVATION = "elevation";
const char* vdt_probe_message_qc_file_reader::HEADING = "heading";
const char* vdt_probe_message_qc_file_reader::HEADING_RANGE_QC_PASSED = "heading_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::HUMIDITY = "humidity";
const char* vdt_probe_message_qc_file_reader::LAT_ACCEL = "hoz_accel_lat";
const char* vdt_probe_message_qc_file_reader::LAT_ACCEL_RANGE_QC_PASSED = "hoz_accel_lat_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::LON_ACCEL = "hoz_accel_long";
const char* vdt_probe_message_qc_file_reader::LON_ACCEL_RANGE_QC_PASSED = "hoz_accel_long_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::LATITUDE = "latitude";
const char* vdt_probe_message_qc_file_reader::LATITUDE_DFT_PASSED = "latitude_dft_passed";
const char* vdt_probe_message_qc_file_reader::LONGITUDE = "longitude";
const char* vdt_probe_message_qc_file_reader::LONGITUDE_DFT_PASSED = "longitude_dft_passed";
const char* vdt_probe_message_qc_file_reader::LIGHTS = "lights";
const char* vdt_probe_message_qc_file_reader::LIGHTS_RANGE_QC_PASSED = "lights_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::MODEL_AIR_TEMP = "model_air_temp";
const char* vdt_probe_message_qc_file_reader::MODEL_DEWPOINT = "model_dew_temp";
const char* vdt_probe_message_qc_file_reader::MODEL_BAR_PRESS = "model_bar_pressure";
const char* vdt_probe_message_qc_file_reader::NSS_AIR_TEMP_MEAN = "nss_air_temp_mean";
const char* vdt_probe_message_qc_file_reader::NSS_BAR_PRESS_MEAN = "nss_bar_pressure_mean";
const char* vdt_probe_message_qc_file_reader::NSS_DEW_TEMP_MEAN = "nss_dew_temp_mean";
const char* vdt_probe_message_qc_file_reader::NSS_HOURLY_PRECIP_MEAN = "nss_hourly_precip_mean";
const char* vdt_probe_message_qc_file_reader::NSS_PREVAIL_VIS_MEAN = "nss_prevail_vis_mean";
const char* vdt_probe_message_qc_file_reader::NSS_WIND_DIR_MEAN = "nss_wind_dir_mean";
const char* vdt_probe_message_qc_file_reader::NSS_WIND_SPEED_MEAN = "nss_wind_speed_mean";
const char* vdt_probe_message_qc_file_reader::OBS_TIME = "obs_time";
const char* vdt_probe_message_qc_file_reader::RADAR_REF = "radar_ref";
const char* vdt_probe_message_qc_file_reader::RADAR_PRECIP_FLAG = "radar_precip_flag";
const char* vdt_probe_message_qc_file_reader::RADAR_PRECIP_TYPE = "radar_precip_type";
const char* vdt_probe_message_qc_file_reader::REC_TIME = "rec_time";
const char* vdt_probe_message_qc_file_reader::ROAD_SEGMENT_ID = "road_segment_id";
const char* vdt_probe_message_qc_file_reader::SPEED = "speed";
const char* vdt_probe_message_qc_file_reader::SPEED_RANGE_QC_PASSED = "speed_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::STABILITY_CONTROL = "stab";
const char* vdt_probe_message_qc_file_reader::STABILITY_CONTROL_RANGE_QC_PASSED = "stab_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::STEERING_ANGLE = "steering_angle";
const char* vdt_probe_message_qc_file_reader::STEERING_ANGLE_RANGE_QC_PASSED = "steering_angle_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::STEERING_RATE = "steering_rate";
const char* vdt_probe_message_qc_file_reader::STEERING_RATE_RANGE_QC_PASSED = "steering_rate_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::TRACTION_CONTROL = "trac";
const char* vdt_probe_message_qc_file_reader::TRACTION_CONTROL_RANGE_QC_PASSED = "trac_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::VEHICLE_ID = "vehicle_id";
const char* vdt_probe_message_qc_file_reader::WIPER_STATUS = "wiper_status";
const char* vdt_probe_message_qc_file_reader::WIPER_STATUS_RANGE_QC_PASSED = "wiper_status_range_qc_passed";
const char* vdt_probe_message_qc_file_reader::YAW_RATE = "yaw_rate";
const char* vdt_probe_message_qc_file_reader::YAW_RATE_RANGE_QC_PASSED = "yaw_rate_range_qc_passed";

vdt_probe_message_qc_file_reader::~vdt_probe_message_qc_file_reader()
{
}

int vdt_probe_message_qc_file_reader::get_probe_messages(const FIELD_NAME_TYPE_VEC& qc_fields, vector<vdt_probe_message_qc>& msgs)
{
  int recnum = nc->get_dim(REC_NUM)->size();
  int vehicleidlen = nc->get_dim(VEHICLE_ID_LEN)->size();

  msgs.reserve(recnum);

  std::unique_ptr<NcError> ncErr(new NcError(NcError::silent_nonfatal));

  NcVar* abs_var = nc->get_var(ABS);
  NcVar* abs_range_qc_passed_var = nc->get_var(ABS_RANGE_QC_PASSED);
  NcVar* brake_boost_var = nc->get_var(BRAKE_BOOST);
  NcVar* brake_boost_range_qc_passed_var = nc->get_var(BRAKE_BOOST_RANGE_QC_PASSED);
  NcVar* brake_status_var = nc->get_var(BRAKE_STATUS);
  NcVar* brake_status_range_qc_passed_var = nc->get_var(BRAKE_STATUS_RANGE_QC_PASSED);
  NcVar* cloud_mask_var = nc->get_var(CLOUD_MASK);
  NcVar* elev_var = nc->get_var(ELEVATION);
  NcVar* head_var = nc->get_var(HEADING);
  NcVar* head_range_qc_passed_var = nc->get_var(HEADING_RANGE_QC_PASSED);
  NcVar* humidity_var = nc->get_var(HUMIDITY);
  NcVar* lat_accel_var = nc->get_var(LAT_ACCEL);
  NcVar* lat_accel_range_qc_passed_var = nc->get_var(LAT_ACCEL_RANGE_QC_PASSED);
  NcVar* lon_accel_var = nc->get_var(LON_ACCEL);
  NcVar* lon_accel_range_qc_passed_var = nc->get_var(LON_ACCEL_RANGE_QC_PASSED);
  NcVar* lat_var = nc->get_var(LATITUDE);
  NcVar* lat_dft_passed_var = nc->get_var(LATITUDE_DFT_PASSED);
  NcVar* lon_var = nc->get_var(LONGITUDE);
  NcVar* lon_dft_passed_var = nc->get_var(LONGITUDE_DFT_PASSED);
  NcVar* lights_var = nc->get_var(LIGHTS);
  NcVar* lights_range_qc_passed_var = nc->get_var(LIGHTS_RANGE_QC_PASSED);
  NcVar* model_air_temp_var = nc->get_var(MODEL_AIR_TEMP);
  NcVar* model_dewpoint_var = nc->get_var(MODEL_DEWPOINT);
  NcVar* model_bar_press_var = nc->get_var(MODEL_BAR_PRESS);
  NcVar* nss_air_temp_mean_var = nc->get_var(NSS_AIR_TEMP_MEAN);
  NcVar* nss_bar_press_mean_var = nc->get_var(NSS_BAR_PRESS_MEAN);
  NcVar* nss_dew_temp_mean_var = nc->get_var(NSS_DEW_TEMP_MEAN);
  NcVar* nss_hourly_precip_mean_var = nc->get_var(NSS_HOURLY_PRECIP_MEAN);
  NcVar* nss_prevail_vis_mean_var = nc->get_var(NSS_PREVAIL_VIS_MEAN);
  NcVar* nss_wind_dir_mean_var = nc->get_var(NSS_WIND_DIR_MEAN);
  NcVar* nss_wind_speed_mean_var = nc->get_var(NSS_WIND_SPEED_MEAN);
  NcVar* obs_time_var = nc->get_var(OBS_TIME);
  NcVar* radar_ref_var = nc->get_var(RADAR_REF);
  NcVar* radar_precip_flag_var = nc->get_var(RADAR_PRECIP_FLAG);
  NcVar* radar_precip_type_var = nc->get_var(RADAR_PRECIP_TYPE);
  NcVar* rec_time_var = nc->get_var(REC_TIME);
  NcVar* road_segment_id_var = nc->get_var(ROAD_SEGMENT_ID);
  NcVar* speed_var = nc->get_var(SPEED);
  NcVar* speed_range_qc_passed_var = nc->get_var(SPEED_RANGE_QC_PASSED);
  NcVar* stab_var = nc->get_var(STABILITY_CONTROL);
  NcVar* stab_range_qc_passed_var = nc->get_var(STABILITY_CONTROL_RANGE_QC_PASSED);
  NcVar* steering_angle_var = nc->get_var(STEERING_ANGLE);
  NcVar* steering_angle_range_qc_passed_var = nc->get_var(STEERING_ANGLE_RANGE_QC_PASSED);
  NcVar* steering_rate_var = nc->get_var(STEERING_RATE);
  NcVar* steering_rate_range_qc_passed_var = nc->get_var(STEERING_ANGLE_RANGE_QC_PASSED);
  NcVar* trac_var = nc->get_var(TRACTION_CONTROL);
  NcVar* trac_range_qc_passed_var = nc->get_var(TRACTION_CONTROL_RANGE_QC_PASSED);
  NcVar* vehicle_id_var = nc->get_var(VEHICLE_ID);
  NcVar* wiper_status_var = nc->get_var(WIPER_STATUS);
  NcVar* wiper_status_range_qc_passed_var = nc->get_var(WIPER_STATUS_RANGE_QC_PASSED);
  NcVar* yaw_var = nc->get_var(YAW_RATE);
  NcVar* yaw_rate_range_qc_passed_var = nc->get_var(YAW_RATE_RANGE_QC_PASSED);

  std::map<std::string, NcVar*> field_var_map;
  std::map<std::string, pika_type_id> field_type_map;
  //Handle dynamic fields
  for(const auto& var_pair : qc_fields)
  {
    const std::string& name = var_pair.first;
    NcVar* nc_var = nc->get_var(name.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(name,nc_var));
      field_type_map.insert(std::make_pair(name, var_pair.second));
    }

    const std::string qc_passed = name + "_range_qc_passed";
    nc_var = nc->get_var(qc_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(qc_passed,nc_var));
      field_type_map.insert(std::make_pair(qc_passed, INT_TYPE));
    }

    const std::string crt_passed = name + "_crt_passed";
    nc_var = nc->get_var(crt_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(crt_passed,nc_var));
      field_type_map.insert(std::make_pair(crt_passed, INT_TYPE));
    }
 
    const std::string nvt_passed = name + "_nvt_passed";
    nc_var = nc->get_var(nvt_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(nvt_passed,nc_var));
      field_type_map.insert(std::make_pair(nvt_passed, INT_TYPE));
    }

    const std::string cat_passed = name + "_cat_passed";
    nc_var = nc->get_var(cat_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(cat_passed,nc_var));
      field_type_map.insert(std::make_pair(cat_passed, INT_TYPE));
    }

    const std::string sdt_passed = name + "_sdt_passed";
    nc_var = nc->get_var(sdt_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(sdt_passed,nc_var));
      field_type_map.insert(std::make_pair(sdt_passed, INT_TYPE));
    }

    const std::string persist_passed = name + "_persist_passed";
    nc_var = nc->get_var(persist_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(persist_passed,nc_var));
      field_type_map.insert(std::make_pair(persist_passed, INT_TYPE));
    }

    const std::string barnes_passed = name + "_spatial_barnes_passed";
    nc_var = nc->get_var(barnes_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(barnes_passed,nc_var));
      field_type_map.insert(std::make_pair(barnes_passed, INT_TYPE));
    }

    const std::string iqr_passed = name + "_spatial_iqr_passed";
    nc_var = nc->get_var(iqr_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(iqr_passed,nc_var));
      field_type_map.insert(std::make_pair(iqr_passed, INT_TYPE));
    }

    const std::string step_passed = name + "_step_passed";
    nc_var = nc->get_var(step_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(step_passed,nc_var));
      field_type_map.insert(std::make_pair(step_passed, INT_TYPE));
    }

    const std::string mat_passed = name + "_mat_passed";
    nc_var = nc->get_var(mat_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(mat_passed,nc_var));
      field_type_map.insert(std::make_pair(mat_passed, INT_TYPE));
    }

    const std::string nst_passed = name + "_nst_passed";
    nc_var = nc->get_var(nst_passed.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(nst_passed,nc_var));
      field_type_map.insert(std::make_pair(nst_passed, INT_TYPE));
    }

    const std::string cat_confidence = name + "_cat_confidence";
    nc_var = nc->get_var(cat_confidence.c_str());
    if(nc_var)
    {
      field_var_map.insert(std::make_pair(cat_confidence,nc_var));
      field_type_map.insert(std::make_pair(cat_confidence, INT_TYPE));
    }
  } //End Handle dynamic fields

  short abs;
  short abs_range_qc_passed;
  short brake_boost;
  short brake_boost_range_qc_passed;
  short brake_status;
  short brake_status_range_qc_passed;
  short cloud_mask;
  float elev;
  float heading;
  short heading_range_qc_passed;
  float humidity;
  float lat_accel;
  short lat_accel_range_qc_passed;
  float lon_accel;
  short lon_accel_range_qc_passed;
  double lat;
  short lat_dft_passed;
  double lon;
  short lon_dft_passed;
  short lights;
  short lights_range_qc_passed;
  float model_air_temp;
  float model_dewpoint;
  float model_bar_press;
  float nss_air_temp_mean;
  float nss_bar_press_mean;
  float nss_dew_temp_mean;
  float nss_hourly_precip_mean;
  float nss_prevail_vis_mean;
  float nss_wind_dir_mean;
  float nss_wind_speed_mean;
  double obs_time;
  float radar_ref;
  short radar_precip_flag;
  short radar_precip_type;
  double rec_time;
  int road_segment_id;
  float speed;
  short speed_range_qc_passed;
  short stab;
  short stab_range_qc_passed;
  float steering_angle;
  short steering_angle_range_qc_passed;
  short steering_rate;
  short steering_rate_range_qc_passed;
  short trac;
  short trac_range_qc_passed;
  char* vehicle_id = new char[vehicleidlen];
  short wiper_status;
  short wiper_status_range_qc_passed;
  float yaw_rate;
  short yaw_rate_range_qc_passed;
  
  //For dynamic fields
  int ival;
  double dval;

  const vdt_probe_message_qc default_msg(qc_fields);

  for (int i = 0; i < recnum; i++)
    {
      abs_var->set_cur(i);
      abs_range_qc_passed_var->set_cur(i);
      brake_boost_var->set_cur(i);
      brake_boost_range_qc_passed_var->set_cur(i);
      brake_status_var->set_cur(i);
      brake_status_range_qc_passed_var->set_cur(i);
      cloud_mask_var->set_cur(i);
      elev_var->set_cur(i);
      head_var->set_cur(i);
      head_range_qc_passed_var->set_cur(i);
      humidity_var->set_cur(i);      
      lat_accel_var->set_cur(i);
      lon_accel_range_qc_passed_var->set_cur(i);     
      lon_accel_var->set_cur(i);
      lon_accel_range_qc_passed_var->set_cur(i);
      lat_var->set_cur(i);
      lat_dft_passed_var->set_cur(i);
      lon_var->set_cur(i);
      lon_dft_passed_var->set_cur(i);
      lights_var->set_cur(i);
      lights_range_qc_passed_var->set_cur(i);
      model_air_temp_var->set_cur(i);
      model_dewpoint_var->set_cur(i);
      model_bar_press_var->set_cur(i);
      nss_air_temp_mean_var->set_cur(i);
      nss_bar_press_mean_var->set_cur(i);
      nss_dew_temp_mean_var->set_cur(i);
      nss_hourly_precip_mean_var->set_cur(i);
      nss_prevail_vis_mean_var->set_cur(i);
      nss_wind_dir_mean_var->set_cur(i);
      nss_wind_speed_mean_var->set_cur(i);
      obs_time_var->set_cur(i);
      radar_ref_var->set_cur(i);
      radar_precip_flag_var->set_cur(i);
      radar_precip_type_var->set_cur(i);
      rec_time_var->set_cur(i);
      road_segment_id_var->set_cur(i);
      speed_var->set_cur(i);
      speed_range_qc_passed_var->set_cur(i);
      stab_var->set_cur(i);
      stab_range_qc_passed_var->set_cur(i);
      steering_angle_var->set_cur(i);
      steering_angle_range_qc_passed_var->set_cur(i);
      steering_rate_var->set_cur(i);
      steering_rate_range_qc_passed_var->set_cur(i);
      trac_var->set_cur(i);
      trac_range_qc_passed_var->set_cur(i);
      vehicle_id_var->set_cur(i);
      wiper_status_var->set_cur(i);
      wiper_status_range_qc_passed_var->set_cur(i);
      yaw_var->set_cur(i);
      yaw_rate_range_qc_passed_var->set_cur(i);

      abs_var->get(&abs,1);
      abs_range_qc_passed_var->get(&abs_range_qc_passed,1);
      brake_boost_var->get(&brake_boost,1);
      brake_boost_range_qc_passed_var->get(&brake_boost_range_qc_passed,1);
      brake_status_var->get(&brake_status,1);
      brake_status_range_qc_passed_var->get(&brake_status_range_qc_passed,1);
      cloud_mask_var->get(&cloud_mask,1);
      elev_var->get(&elev,1);
      head_var->get(&heading,1);
      head_range_qc_passed_var->get(&heading_range_qc_passed,1);
      humidity_var->get(&humidity,1);
      lat_accel_var->get(&lat_accel,1);
      lat_accel_range_qc_passed_var->get(&lat_accel_range_qc_passed,1);
      lon_accel_var->get(&lon_accel,1);
      lon_accel_range_qc_passed_var->get(&lon_accel_range_qc_passed,1);
      lat_var->get(&lat,1);
      lat_dft_passed_var->get(&lat_dft_passed,1);
      lon_var->get(&lon,1);
      lon_dft_passed_var->get(&lon_dft_passed,1);
      lights_var->get(&lights,1);
      lights_range_qc_passed_var->get(&lights_range_qc_passed,1);
      model_air_temp_var->get(&model_air_temp,1);
      model_dewpoint_var->get(&model_dewpoint,1);
      model_bar_press_var->get(&model_bar_press,1);
      nss_air_temp_mean_var->get(&nss_air_temp_mean,1);
      nss_bar_press_mean_var->get(&nss_bar_press_mean,1);
      nss_dew_temp_mean_var->get(&nss_dew_temp_mean,1);
      nss_hourly_precip_mean_var->get(&nss_hourly_precip_mean,1);
      nss_prevail_vis_mean_var->get(&nss_prevail_vis_mean,1);
      nss_wind_dir_mean_var->get(&nss_wind_dir_mean,1);
      nss_wind_speed_mean_var->get(&nss_wind_speed_mean,1);
      obs_time_var->get(&obs_time,1);
      radar_ref_var->get(&radar_ref,1);
      radar_precip_flag_var->get(&radar_precip_flag,1);
      radar_precip_type_var->get(&radar_precip_type,1);
      rec_time_var->get(&rec_time,1);
      road_segment_id_var->get(&road_segment_id,1);
      speed_var->get(&speed,1);
      speed_range_qc_passed_var->get(&speed_range_qc_passed,1);
      stab_var->get(&stab,1);
      stab_range_qc_passed_var->get(&stab_range_qc_passed,1);
      steering_angle_var->get(&steering_angle,1);
      steering_angle_range_qc_passed_var->get(&steering_angle_range_qc_passed,1);
      steering_rate_var->get(&steering_rate,1);
      steering_rate_range_qc_passed_var->get(&steering_rate_range_qc_passed,1);
      trac_var->get(&trac,1);
      trac_range_qc_passed_var->get(&trac_range_qc_passed,1);
      vehicle_id_var->get(vehicle_id,1,vehicleidlen);
      wiper_status_var->get(&wiper_status,1);
      wiper_status_range_qc_passed_var->get(&wiper_status_range_qc_passed,1);
      yaw_var->get(&yaw_rate,1);
      yaw_rate_range_qc_passed_var->get(&yaw_rate_range_qc_passed,1);

      vdt_probe_message_qc msg = default_msg;
      vdt_point pt;

      pt.set_latitude(lat);
      pt.set_longitude(lon);
      pt.set_elevation(elev);
      msg.set_point(pt);

      msg.set_abs(abs);
      msg.set_abs_range_qc_passed(abs_range_qc_passed);
      msg.set_brake_boost(brake_boost);
      msg.set_brake_boost_range_qc_passed(brake_boost_range_qc_passed);
      msg.set_brake_status(brake_status);
      msg.set_brake_status_range_qc_passed(brake_status_range_qc_passed);
      msg.set_heading(heading);
      msg.set_heading_range_qc_passed(heading_range_qc_passed);
      msg.set_hoz_accel_lat(lat_accel);
      msg.set_hoz_accel_lat_range_qc_passed(lat_accel_range_qc_passed);
      msg.set_hoz_accel_long(lon_accel);
      msg.set_hoz_accel_long_range_qc_passed(lon_accel_range_qc_passed);
      msg.set_lights(lights);
      msg.set_lights_range_qc_passed(lights_range_qc_passed);
      msg.set_latitude_dft_passed(lat_dft_passed);
      msg.set_longitude_dft_passed(lon_dft_passed);
      msg.set_model_air_temp(model_air_temp);
      msg.set_model_dewpoint(model_dewpoint);
      msg.set_model_bar_press(model_bar_press);
      msg.set_nss_air_temp_mean(nss_air_temp_mean);
      msg.set_nss_bar_press_mean(nss_bar_press_mean);
      msg.set_nss_dew_temp_mean(nss_dew_temp_mean);
      msg.set_nss_hourly_precip_mean(nss_hourly_precip_mean);
      msg.set_nss_prevail_vis_mean(nss_prevail_vis_mean);
      msg.set_nss_wind_dir_mean(nss_wind_dir_mean);
      msg.set_nss_wind_speed_mean(nss_wind_speed_mean);
      msg.set_obs_time(obs_time);
      msg.set_radar_ref(radar_ref);
      msg.set_radar_precip_flag(radar_precip_flag);
      msg.set_radar_precip_type(radar_precip_type);
      msg.set_rec_time(rec_time);
      msg.set_road_segment_id(road_segment_id);
      msg.set_speed(speed);
      msg.set_speed_range_qc_passed(speed_range_qc_passed);
      msg.set_stab(stab);
      msg.set_stab_range_qc_passed(stab_range_qc_passed);
      msg.set_steering_angle(steering_angle);
      msg.set_steering_angle_range_qc_passed(steering_angle_range_qc_passed);
      msg.set_steering_rate(steering_rate);
      msg.set_steering_rate_range_qc_passed(steering_rate_range_qc_passed);
      msg.set_trac(trac);
      msg.set_trac_range_qc_passed(trac_range_qc_passed);
      string vid = string(vehicle_id);
      msg.set_vehicle_id(vid);
      msg.set_wipers_front_status(wiper_status);
      msg.set_wipers_front_status_range_qc_passed(wiper_status_range_qc_passed);
      msg.set_yaw_rate(yaw_rate);
      msg.set_yaw_range_qc_passed(yaw_rate_range_qc_passed);

      //Handle dynamic fields
      for(const auto& elem : field_var_map)
      {
	const std::string& name = elem.first;
	NcVar* nc_var = elem.second;
	nc_var->set_cur(i);

	//Extract as type to boost variant
	switch(field_type_map.at(name))
	{
	  case INT_TYPE:
	  {
	    nc_var->get(&ival,1);
	    msg.setattr(name, ival);
	    break;
	  }
	  case DOUBLE_TYPE:
	  {
	    nc_var->get(&dval,1);
	    msg.setattr(name, dval);
	    break;
	  }
	  default:
	  {
	    throw VDT_EXCEPTION("Invalid variable type.");
	  }
	}
      } //End Handle dynamic fields
      //msg.dump();

      msgs.push_back(msg);
    }
  delete [] vehicle_id;
  return 0;
}
