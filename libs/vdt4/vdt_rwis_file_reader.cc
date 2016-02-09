
#include "vdt_rwis_file_reader.hh"

const char* vdt_rwis_file_reader::REC_NUM = "recNum";
const char* vdt_rwis_file_reader::STN_NAME_LEN = "maxNameLength";

const char* vdt_rwis_file_reader::STATION_NAME = "stationName";
const char* vdt_rwis_file_reader::LATITUDE = "latitude";
const char* vdt_rwis_file_reader::LONGITUDE = "longitude";
const char* vdt_rwis_file_reader::ELEVATION = "elevation";
const char* vdt_rwis_file_reader::OBS_TIME = "observationTime";
const char* vdt_rwis_file_reader::AIR_TEMP = "temperature";
const char* vdt_rwis_file_reader::AIR_TEMP_DD = "temperatureDD";
const char* vdt_rwis_file_reader::SURFACE_TEMP = "roadTemperature1";
const char* vdt_rwis_file_reader::PRESS = "stationPressure";
const char* vdt_rwis_file_reader::PRESS_DD = "stationPressureDD";
const char* vdt_rwis_file_reader::DEW_TEMP = "dewpoint";
const char* vdt_rwis_file_reader::DEW_TEMP_DD = "dewpointDD";
const char* vdt_rwis_file_reader::HOURLY_PRECIP = "precipAccum";
const char* vdt_rwis_file_reader::HOURLY_PRECIP_DD = "precipAccumDD";
const char* vdt_rwis_file_reader::PREVAIL_VIS = "visibility";
const char* vdt_rwis_file_reader::PREVAIL_VIS_DD = "visibilityDD";
const char* vdt_rwis_file_reader::WIND_DIR = "windDir";
const char* vdt_rwis_file_reader::WIND_DIR_DD = "windDirDD";
const char* vdt_rwis_file_reader::WIND_SPEED = "windSpeed";
const char* vdt_rwis_file_reader::WIND_SPEED_DD = "windSpeedDD";
const char* vdt_rwis_file_reader::WIND_GUST = "windGust";
const char* vdt_rwis_file_reader::WIND_GUST_DD = "windGustDD";

const char* vdt_rwis_file_reader::MAX_WEATHER_LEN = "maxWeatherLen";
const char* vdt_rwis_file_reader::PRES_WX = "presWx";

const char* vdt_rwis_file_reader::FILL_ATTR = "_FillValue";

//#define QC_DEBUG 1

bool passedQC(char c)
{
  //For speed sake
  return c == 'S' || c == 'V' || c == 'K' || c == 'k' || c == 'G';
}

vdt_rwis_file_reader::~vdt_rwis_file_reader()
{
}

int vdt_rwis_file_reader::get_rwis(vector<vdt_surface_observation>& obs)
{
  if (nc == NULL)
    {
      return 1;
    }

  int recnum = nc->get_dim(REC_NUM)->size();
  num_rwis_obs = recnum;
  int stnlen = nc->get_dim(STN_NAME_LEN)->size();
  NcVar* stn_name_var = nc->get_var(STATION_NAME);
  NcVar* lat_var = nc->get_var(LATITUDE);
  NcVar* lon_var = nc->get_var(LONGITUDE);
  NcVar* elev_var = nc->get_var(ELEVATION);
  NcVar* obstime_var = nc->get_var(OBS_TIME);
  NcVar* temp_var = nc->get_var(AIR_TEMP);
  NcVar* temp_qc_var = nc->get_var(AIR_TEMP_DD);
  float temp_fill = temp_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* surface_temp_var = nc->get_var(SURFACE_TEMP);
  float surface_temp_fill = surface_temp_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* press_var = nc->get_var(PRESS);
  NcVar* press_qc_var = nc->get_var(PRESS_DD);
  float press_fill = press_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* dewp_var = nc->get_var(DEW_TEMP);
  NcVar* dewp_qc_var = nc->get_var(DEW_TEMP_DD);
  float dewp_fill = dewp_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* precip_hr_var = nc->get_var(HOURLY_PRECIP);
  NcVar* precip_hr_qc_var = nc->get_var(HOURLY_PRECIP_DD);
  float precip_hr_fill = precip_hr_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* vis_var = nc->get_var(PREVAIL_VIS);
  NcVar* vis_qc_var = nc->get_var(PREVAIL_VIS_DD);
  float vis_fill = vis_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* dir_var = nc->get_var(WIND_DIR);
  NcVar* dir_qc_var = nc->get_var(WIND_DIR_DD);
  float dir_fill = dir_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* spd_var = nc->get_var(WIND_SPEED);
  NcVar* spd_qc_var = nc->get_var(WIND_SPEED_DD);
  float spd_fill = spd_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* gust_var = nc->get_var(WIND_GUST);
  NcVar* gust_qc_var = nc->get_var(WIND_GUST_DD);
  float gust_fill = gust_var->get_att(FILL_ATTR)->values()->as_float(0);
  int weatherlen = nc->get_dim(MAX_WEATHER_LEN)->size();
  NcVar* pres_wx_var = nc->get_var(PRES_WX);

  char* stn_name = new char[stnlen];
  char* pres_wx = new char[weatherlen];
  for (int i = 0; i < recnum; i++)
    {
      stn_name_var->set_cur(i);
      lat_var->set_cur(i);
      lon_var->set_cur(i);
      elev_var->set_cur(i);
      obstime_var->set_cur(i);
      temp_var->set_cur(i);
      temp_qc_var->set_cur(i);
      surface_temp_var->set_cur(i);
      press_var->set_cur(i);
      press_qc_var->set_cur(i);
      dewp_var->set_cur(i);
      dewp_qc_var->set_cur(i);
      precip_hr_var->set_cur(i);
      precip_hr_qc_var->set_cur(i);
      vis_var->set_cur(i);
      vis_qc_var->set_cur(i);
      dir_var->set_cur(i);
      dir_qc_var->set_cur(i);
      spd_var->set_cur(i);
      spd_qc_var->set_cur(i);
      gust_var->set_cur(i);
      gust_qc_var->set_cur(i);
      pres_wx_var->set_cur(i);

      double obstime;
      float lat,lon,elev;
      float temp,surface_temp,dewp,press;
      float precip_hr;
      float vis;
      float spd,dir,gust;

      stn_name_var->get(stn_name,1,stnlen);
      lat_var->get(&lat,1);
      lon_var->get(&lon,1);
      elev_var->get(&elev,1);
      obstime_var->get(&obstime,1);
      temp_var->get(&temp,1);
      surface_temp_var->get(&surface_temp,1);
      dewp_var->get(&dewp,1);
      press_var->get(&press,1);
      precip_hr_var->get(&precip_hr,1);
      vis_var->get(&vis,1);
      dir_var->get(&dir,1);
      spd_var->get(&spd,1);
      gust_var->get(&gust,1);
      pres_wx_var->get(pres_wx,1,weatherlen);

      char qc_tmp;
      if(temp_qc_var->get(&qc_tmp,1) && !passedQC(qc_tmp))
      {
	temp = vdt_surface_observation::FILL_VALUE;
        #ifdef QC_DEBUG
	error += "RWIS temperature value failed QC. Index " + std::to_string(i) + " QC value = " + qc_tmp  + "\n";
        #endif
      }

      if(press_qc_var->get(&qc_tmp,1) && !passedQC(qc_tmp))
      {
	press = vdt_surface_observation::FILL_VALUE;
        #ifdef QC_DEBUG
	error += "RWIS pressure value failed QC. Index " + std::to_string(i) + " QC value = " + qc_tmp  + "\n";
        #endif
      }

      if(dewp_qc_var->get(&qc_tmp,1) && !passedQC(qc_tmp))
      {
	dewp = vdt_surface_observation::FILL_VALUE;
	#ifdef QC_DEBUG
	error += "RWIS dew point value failed QC. Index " + std::to_string(i) + " QC value = " + qc_tmp  + "\n";
	#endif
      }
      
      if(precip_hr_qc_var->get(&qc_tmp,1) && !passedQC(qc_tmp))
      {
	precip_hr = vdt_surface_observation::FILL_VALUE;
	#ifdef QC_DEBUG
	error += "RWIS precipitation value failed QC. Index " + std::to_string(i) + " QC value = " + qc_tmp  + "\n";
	#endif
      }

      if(vis_qc_var->get(&qc_tmp,1) && !passedQC(qc_tmp))
      {
	vis = vdt_surface_observation::FILL_VALUE;
	#ifdef QC_DEBUG
	error += "RWIS visibility value failed QC. Index " + std::to_string(i) + " QC value = " + qc_tmp  + "\n";
	#endif
      }
         
      if(dir_qc_var->get(&qc_tmp,1) && !passedQC(qc_tmp))
      {
	dir = vdt_surface_observation::FILL_VALUE;
	#ifdef QC_DEBUG
	error += "RWIS wind direction value failed QC. Index " + std::to_string(i) + " QC value = " + qc_tmp  + "\n";
	#endif
      }
      
      if(spd_qc_var->get(&qc_tmp,1) && !passedQC(qc_tmp))
      {
	spd = vdt_surface_observation::FILL_VALUE;
	#ifdef QC_DEBUG
	error += "RWIS wind speed value failed QC. Index " + std::to_string(i) + " QC value = " + qc_tmp  + "\n";
	#endif
      }
          
      if(gust_qc_var->get(&qc_tmp,1) && !passedQC(qc_tmp))
      {
	gust = vdt_surface_observation::FILL_VALUE;
	#ifdef QC_DEBUG
	error += "RWIS wind gust value failed QC. Index " + std::to_string(i) + " QC value = " + qc_tmp  + "\n";
	#endif
      }

      vdt_surface_observation::set_fill(temp,temp_fill);
      vdt_surface_observation::set_fill(surface_temp,surface_temp_fill);
      vdt_surface_observation::set_fill(dewp,dewp_fill);
      vdt_surface_observation::set_fill(press,press_fill);
      vdt_surface_observation::set_fill(precip_hr,precip_hr_fill);
      vdt_surface_observation::set_fill(vis,vis_fill);
      vdt_surface_observation::set_fill(spd,spd_fill);
      vdt_surface_observation::set_fill(dir,dir_fill);
      vdt_surface_observation::set_fill(gust,gust_fill);

      if (vis != vdt_surface_observation::FILL_VALUE)
        {
          vis = vis / 1000.0; //convert from m to km
        }
      if (temp != vdt_surface_observation::FILL_VALUE)
        {
          temp = vdt_surface_observation::k_to_c(temp);
        }
      if (dewp != vdt_surface_observation::FILL_VALUE)
        {
          dewp = vdt_surface_observation::k_to_c(dewp);
        }
      if (surface_temp != vdt_surface_observation::FILL_VALUE)
        {
          surface_temp = vdt_surface_observation::k_to_c(surface_temp);
        }
      if (press != vdt_surface_observation::FILL_VALUE)
        {
          press = vdt_surface_observation::pa_to_mb(press);
        }

      vdt_surface_observation o;
      vdt_point pt;
      pt.set_latitude(lat);
      pt.set_longitude(lon);
      pt.set_elevation(elev);
      o.set_time_obs(obstime);
      o.set_point(pt);
      o.set_station_name(string(stn_name));
      o.set_air_temp(temp);
      o.set_surface_temp(surface_temp);
      o.set_dew_temp(dewp);
      o.set_press(press);
      o.set_hourly_precip(precip_hr);
      o.set_prevail_vis(vis);
      o.set_wind_speed(spd);
      o.set_wind_dir(dir);
      o.set_wind_gust(gust);
      o.set_pres_wx(pres_wx);

      obs.push_back(o);
    }
  delete [] stn_name;
  delete [] pres_wx;
  return 0;
}
