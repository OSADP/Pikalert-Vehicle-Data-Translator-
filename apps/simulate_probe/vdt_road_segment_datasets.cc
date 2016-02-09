//==============================================================================
//
//   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2014/10/04 14:44:18 $
//
//==============================================================================

#include "wiper_status.hh"
#include "vdt_road_segment_datasets.hh"
#include "vdt4/vdt_metar_file_reader.hh"
#include "vdt4/vdt_rwis_file_reader.hh"
#include "rwx/rwx_rtma_dataset.hh"
#include "vdt4/vdt_road_segment_file_reader.hh"
#include "vdt4/vdt_const.hh"
#include "vdt4/stat_tools.hh"

const float KM_TO_DEGREES = 1/111.12;

inline double return_pt(vdt_point p, size_t k )
{
  if (k == 0)
    {
      return p.get_latitude();
    }
  return p.get_longitude();
};

inline double return_obs_pt(vdt_surface_observation o, size_t k )
{
  return return_pt(o.get_point(), k);
};

inline double return_time(vdt_surface_observation o, size_t k )
{
  return o.get_time_obs();
};


int get_field_name_index(const string &field_name, const unordered_map<string, int> &field_name_map)
{
  auto itr = field_name_map.find(field_name);
  if (itr == field_name_map.end())
    {
      return -1;
    }
  else
    {
      return itr->second;
    }
}

vdt_road_segment_datasets::vdt_road_segment_datasets(const config_reader &cfg, const string &radar_file_pattern, const string &metar_file, const string &rwis_file, const string &rtma_file, Log *log_object) : cfg_reader(cfg), logg(log_object)
{
  radar = NULL;
  rtma = NULL;

  if (radar_file_pattern != "")
    {
      logg->write_time_info("reading radar template file: %s\n", radar_file_pattern.c_str());
      radar = new vdt_nssl_tiled_radar_dataset(radar_file_pattern.c_str());
    }
  if (metar_file != "")
    {
      logg->write_time_info("reading metar file: %s\n", metar_file.c_str());
      vdt_metar_file_reader *metar_reader = new vdt_metar_file_reader(metar_file.c_str());
      metar_reader->get_metars(obs);
      delete metar_reader;
    }
  if (rwis_file != "")
    {
      logg->write_time_info("reading rwis file: %s\n", rwis_file.c_str());
      vdt_rwis_file_reader *rwis_reader = new vdt_rwis_file_reader(rwis_file.c_str());
      rwis_reader->get_rwis(obs);
      delete rwis_reader;
    }
  if (rtma_file != "")
    {
      logg->write_time_info("reading rtma file: %s\n", rtma_file.c_str());
      rtma = new rwx_rtma_dataset(rtma_file.c_str(), cfg.rtma_proj);
      if (rtma->error != "")
	{
	  logg->write_time_warning("failed reading rtma file, error: %s\n", rtma->error.c_str());
	  rtma = NULL;
	}
    }

  if (obs.size() == 0)
    {
      logg->write_time_warning("no surface obs\n");
    }
  else
    {
      vdt_point::get_points_lle(obs, surface_station_latitude, surface_station_longitude, surface_station_elevation);
    }

  logg->write_time_info("creating surface observation nearest neighbor object using %ld surface obs\n", surface_station_latitude.size());
  surface_obs_nearest_nbr = new vdt_nearest_nbr(surface_station_latitude, surface_station_longitude);
}

vdt_road_segment_datasets::~vdt_road_segment_datasets()
{
  if (radar != NULL)
    {
      delete radar;
    }
  if (rtma != NULL)
    {
      delete rtma;
    }

  delete surface_obs_nearest_nbr;
}

void vdt_road_segment_datasets::get_point_datasets(double obs_time, const vector<vdt_point> &vdt_points, vector<vdt_dataset> &datasets)
{
  for (size_t i = 0; i < vdt_points.size(); i++)
    {
      vdt_dataset ds = get_vdt_dataset(obs_time, vdt_points[i]);
      datasets.push_back(ds);
    }
}

void vdt_road_segment_datasets::update_road_segments(double obs_time, const Wiper_status *wiper_status, const field_info &field_info_obj, const vector<vdt_road_segment> &in_segments, vector<vdt_road_segment_data> &out_segments)
{
  // Get index offsets of:
  // radar_ref
  // model_dewpoint
  // etc.


  logg->write_time_info("update_road_segments: getting field indices for radar_ref, model_dewpoint and so on\n");

  int lat_index = get_field_name_index("lat", field_info_obj.field_map);
  int lon_index = get_field_name_index("lon", field_info_obj.field_map);
  int radar_x_index = get_field_name_index("radar_x", field_info_obj.field_map);
  int radar_y_index = get_field_name_index("radar_y", field_info_obj.field_map);
  int radar_ref_index = get_field_name_index("radar_ref", field_info_obj.field_map);
  int wiper_status_index = get_field_name_index("wiper_status", field_info_obj.field_map);
  int model_x_index = get_field_name_index("model_x", field_info_obj.field_map);
  int model_y_index = get_field_name_index("model_y", field_info_obj.field_map);
  int model_dewpoint_index = get_field_name_index("model_dewpoint", field_info_obj.field_map);
  int model_air_temp_index = get_field_name_index("model_air_temp", field_info_obj.field_map);
  int model_bar_press_index = get_field_name_index("model_bar_press", field_info_obj.field_map);
  int model_geopotential_height_index = get_field_name_index("model_geopotential_height", field_info_obj.field_map);
  int model_wind_speed_index = get_field_name_index("model_wind_speed", field_info_obj.field_map);
  int model_wind_dir_index = get_field_name_index("model_wind_dir", field_info_obj.field_map);
  int nss_air_temp_mean_index = get_field_name_index("nss_air_temp_mean", field_info_obj.field_map);
  int nss_bar_press_mean_index = get_field_name_index("nss_bar_press_mean", field_info_obj.field_map);
  int nss_prevail_vis_mean_index = get_field_name_index("nss_prevail_vis_mean", field_info_obj.field_map);
  int nss_dew_temp_mean_index = get_field_name_index("nss_dew_temp_mean", field_info_obj.field_map);
  int nss_wind_speed_mean_index = get_field_name_index("nss_wind_speed_mean", field_info_obj.field_map);
  int nss_wind_dir_mean_index = get_field_name_index("nss_wind_dir_mean", field_info_obj.field_map);
  int nss_hourly_precip_mean_index = get_field_name_index("nss_hourly_precip_mean", field_info_obj.field_map);

  logg->write_time_info("update_road_segments: setting fields for %ld road segments\n", in_segments.size());
  for (size_t i = 0; i < in_segments.size(); i++)
    {
      const vdt_road_segment &road_segment = in_segments[i];

      vdt_road_segment_data road_segment_data(field_info_obj, obs_time, 0, road_segment);
      vdt_dataset ds = get_vdt_dataset(obs_time,  road_segment_data.road_segment.get_point());

      road_segment_data.set_field(lat_index, static_cast<float>(ds.lat));
      road_segment_data.set_field(lon_index, static_cast<float>(ds.lon));
      road_segment_data.set_field(radar_x_index, static_cast<float>(ds.radar_x));
      road_segment_data.set_field(radar_y_index, static_cast<float>(ds.radar_y));
      road_segment_data.set_field(radar_ref_index, ds.radar_ref);
      Wiper_status::Wiper_status_values wiper_status_value = wiper_status->find_wiper_status(ds.radar_ref);
      road_segment_data.set_field(wiper_status_index, static_cast<float>(wiper_status_value));
      road_segment_data.set_field(model_x_index, static_cast<float>(ds.model_x));
      road_segment_data.set_field(model_y_index, static_cast<float>(ds.model_y));
      road_segment_data.set_field(model_dewpoint_index, ds.model_dewpoint);
      road_segment_data.set_field(model_air_temp_index, ds.model_air_temp);
      road_segment_data.set_field(model_bar_press_index, ds.model_bar_press);
      road_segment_data.set_field(model_geopotential_height_index, ds.model_geopotential_height);
      road_segment_data.set_field(model_wind_speed_index, ds.model_wind_speed);
      road_segment_data.set_field(model_wind_dir_index, ds.model_wind_dir);
      road_segment_data.set_field(nss_air_temp_mean_index, ds.nss_air_temp_mean);
      road_segment_data.set_field(nss_bar_press_mean_index, ds.nss_bar_press_mean);
      road_segment_data.set_field(nss_prevail_vis_mean_index, ds.nss_prevail_vis_mean);
      road_segment_data.set_field(nss_dew_temp_mean_index, ds.nss_dew_temp_mean);
      road_segment_data.set_field(nss_wind_speed_mean_index, ds.nss_wind_speed_mean);
      road_segment_data.set_field(nss_wind_dir_mean_index, ds.nss_wind_dir_mean);
      road_segment_data.set_field(nss_hourly_precip_mean_index, ds.nss_hourly_precip_mean);

      out_segments.push_back(road_segment_data);
    }
}

vdt_dataset vdt_road_segment_datasets::get_vdt_dataset(double obs_time, const vdt_point &target)
{
  float ref = vdt_const::FILL_VALUE;
  short radar_precip_flag = vdt_const::FILL_VALUE;
  short radar_precip_type = vdt_const::FILL_VALUE;
  float model_dewpoint = vdt_const::FILL_VALUE;
  float model_temp = vdt_const::FILL_VALUE;
  float model_press = vdt_const::FILL_VALUE;
  float model_wind_dir = vdt_const::FILL_VALUE;
  float model_wind_speed = vdt_const::FILL_VALUE;
  float model_geopotential_height = vdt_const::FILL_VALUE;

  double lat = target.get_latitude();
  double lon = target.get_longitude();
  vdt_dataset ds;
  
  ds.lat = lat;
  ds.lon = lon;

  // Notice how the time is not used for the gridded information
  if (radar != NULL)
    {
      int ret = radar->get_reflectivity(lat, lon, ref, ds.radar_x, ds.radar_y);
      if (ret != 0)
	{
	  ref = vdt_const::FILL_VALUE;
	  ds.radar_x = vdt_const::FILL_VALUE;
	  ds.radar_y = vdt_const::FILL_VALUE;
	}

      ds.radar_ref = ref;
      ds.radar_precip_flag = radar_precip_flag;
      ds.radar_precip_type = radar_precip_type;
    }

  if (rtma != NULL)
    {
      size_t offset;
      double xc;
      double yc;
      int ret = rtma->get_coord_offset(lat, lon, &xc, &yc, &offset);

      if (ret == 0)
	{
	  rtma->get_dewpoint_celsius(offset, model_dewpoint);
	  rtma->get_temperature_celsius(offset, model_temp);
	  rtma->get_pressure_mb(offset, model_press);
	  rtma->get_wind_dir_deg(offset, model_wind_dir);
	  rtma->get_wind_speed_ms(offset, model_wind_speed);
	  rtma->get_geopotential_height(offset, model_geopotential_height);

	  ds.model_x = xc;
	  ds.model_y = yc;
	  ds.model_dewpoint = model_dewpoint;
	  ds.model_air_temp = model_temp;
	  ds.model_bar_press = model_press;
	  ds.model_wind_speed = model_wind_speed;
	  ds.model_wind_dir = model_wind_dir;
	  ds.model_geopotential_height = model_geopotential_height;
	}
    }

  vector<vdt_surface_observation> near;

  get_near_surface_obs(target, obs_time, cfg_reader.nst_search_radius, cfg_reader.nst_time_range, near);
  float nss_air_temp_mean = vdt_surface_observation::get_avg_air_temp(near);
  float nss_press_mean = vdt_surface_observation::get_avg_press(near);
  float nss_vis_mean = vdt_surface_observation::get_avg_vis(near);
  float nss_dew_temp_mean = vdt_surface_observation::get_avg_dew_temp(near);
  float nss_wind_speed_mean = vdt_surface_observation::get_avg_wind_speed(near);
  float nss_wind_dir_mean = vdt_surface_observation::get_avg_wind_dir(near);
  float nss_hourly_precip_mean = vdt_surface_observation::get_avg_hourly_precip(near);

  ds.nss_air_temp_mean = nss_air_temp_mean;
  ds.nss_bar_press_mean = nss_press_mean;
  ds.nss_prevail_vis_mean = nss_vis_mean;
  ds.nss_dew_temp_mean = nss_dew_temp_mean;
  ds.nss_wind_speed_mean = nss_wind_speed_mean;
  ds.nss_wind_dir_mean = nss_wind_dir_mean;
  ds.nss_hourly_precip_mean = nss_hourly_precip_mean;
  return ds;
}

void vdt_road_segment_datasets::get_near_surface_obs(const vdt_point &target, double obs_time, float search_range_km, int time_range_secs, vector<vdt_surface_observation> &near)
{
  // convert km to equator degrees
  float degrees = search_range_km * KM_TO_DEGREES;

  int max_num_neighbor = 25;
  double target_lat = target.get_latitude();
  double target_lon = target.get_longitude();
  double sq_radius = degrees * degrees;
  vector<int> indices;
  vector<double> distances;
  double eps = 0;
  int ret = surface_obs_nearest_nbr->get_nearest_pts(eps, target_lat, target_lon, max_num_neighbor, sq_radius, indices, distances);

  if (ret < 0)
    {
      // This will happen only if max_num_neighbor <= 0 or there were no nearest neighbor points
      return;
    }

  // Now calculate great circle distances and compare against upper bound
  for (size_t i=0; i<indices.size(); i++)
    {
      int offset = indices[i];
      double dist = vdt_point::great_circle_distance(target_lat, target_lon, surface_station_latitude[offset], surface_station_longitude[offset]);

      if (dist <= search_range_km)
	{
	  //printf("fabs(obs[i].get_time_obs() - obs_time), time_range_secs: %f, %d\n", fabs(obs[i].get_time_obs() - obs_time), time_range_secs);
	  if (fabs(obs[offset].get_time_obs() - obs_time) < time_range_secs)
	    {
	      //printf("obs[i].get_air_temp(): %f\n", obs[offset].get_air_temp());
	      near.push_back(obs[offset]);
	    }
	}
    }

  return;
}
