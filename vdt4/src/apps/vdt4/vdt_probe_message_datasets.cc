//==============================================================================
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//==============================================================================

#include "vdt_probe_message_datasets.hh"
#include "vdt4/vdt_metar_file_reader.hh"
#include "vdt4/vdt_rwis_file_reader.hh"
#include "rwx/rwx_rtma_dataset.hh"
#include "vdt4/vdt_road_segment_file_reader.hh"
#include "vdt4/vdt_const.hh"
#include "vdt4/stat_tools.hh"

extern Log *Logg;

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


vdt_probe_message_datasets::vdt_probe_message_datasets(const config_reader *cfg,
						       string radar_file_pattern,
						       string metar_file,
						       string rwis_file,
						       string rtma_file,
						       string cloud_class_file,
						       Log* logg) : cfg_reader(cfg)
{
  radar = NULL;
  rtma = NULL;
  cloud_class = NULL;
  this->logg = logg;

  if (radar_file_pattern != "")
    {
      Logg->write_time_info("calling vdt_nssl_tiled_radar_dataset(%s)\n", radar_file_pattern.c_str());
      radar = new vdt_nssl_tiled_radar_dataset(radar_file_pattern.c_str(), logg);
    }
  if (metar_file != "")
    {
      Logg->write_time_info("calling vdt_metar_file_reader(%s)\n", metar_file.c_str());
      vdt_metar_file_reader* metar_reader = new vdt_metar_file_reader(metar_file.c_str());
      metar_reader->get_metars(obs);
      delete metar_reader;
    }
  if (rwis_file != "")
    {
      Logg->write_time_info("calling vdt_rwis_file_reader(%s)\n", rwis_file.c_str());
      vdt_rwis_file_reader* rwis_reader = new vdt_rwis_file_reader(rwis_file.c_str());
      rwis_reader->get_rwis(obs);
      delete rwis_reader;
    }
  if (rtma_file != "")
    {
      Logg->write_time_info("calling rwx_rtma_dataset(%s)\n", rtma_file.c_str());
      rtma = new rwx_rtma_dataset(rtma_file.c_str(), cfg->rtma_proj);
      if (rtma->error != "")
	{
	  Logg->write_time_warning("failed reading rtma file, error: %s\n", rtma->error.c_str());
	  rtma = NULL;
	}
    }
  if (cloud_class_file != "")
    {
      Logg->write_time_info("vdt_cloud_mask_dataset(%s) not currently supported\n", cloud_class_file.c_str());
      //Logg->write_time_info("calling vdt_cloud_mask_dataset(%s)\n", cloud_class_file.c_str());
      //cloud_class = new vdt_cloud_mask_dataset(cloud_class_file.c_str());
    }

  Logg->write_time_info("creating surface observation nearest neighbor object using %ld surface obs\n", surface_station_latitude.size());

  vdt_point::get_points_lle(obs, surface_station_latitude, surface_station_longitude, surface_station_elevation);
  surface_obs_nearest_nbr = new vdt_nearest_nbr(surface_station_latitude, surface_station_longitude);
}

vdt_probe_message_datasets::~vdt_probe_message_datasets()
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

void vdt_probe_message_datasets::get_point_datasets(double obs_time, const vector<vdt_point> &vdt_points, vector<vdt_dataset> &datasets)
{
  for (size_t i = 0; i < vdt_points.size(); i++)
    {
      vdt_dataset ds = get_vdt_dataset(obs_time, vdt_points[i]);
      datasets.push_back(ds);
    }
}

void vdt_probe_message_datasets::update_probe_messages(const config_reader& cfg, const vector<vdt_probe_message>& in_msgs, vector<vdt_probe_message_qc> &out_msgs)
{
  for (size_t i = 0; i < in_msgs.size(); i++)
    {
      vdt_probe_message msg = in_msgs[i];
      vdt_probe_message_qc qc_msg(msg);
      double obs_time = msg.get_obs_time();
      vdt_dataset ds = get_vdt_dataset(obs_time,  msg.get_point());
      qc_msg.set_radar_ref(ds.radar_ref);
      qc_msg.set_model_dewpoint(ds.model_dewpoint);
      qc_msg.set_model_air_temp(ds.model_air_temp);
      qc_msg.set_model_bar_press(ds.model_bar_press);
      qc_msg.set_cloud_mask(ds.cloud_mask);
      qc_msg.set_nss_air_temp_mean(ds.nss_air_temp_mean);
      qc_msg.set_nss_bar_press_mean(ds.nss_bar_press_mean);
      qc_msg.set_nss_prevail_vis_mean(ds.nss_prevail_vis_mean);
      qc_msg.set_nss_dew_temp_mean(ds.nss_dew_temp_mean);
      qc_msg.set_nss_wind_speed_mean(ds.nss_wind_speed_mean);
      qc_msg.set_nss_wind_dir_mean(ds.nss_wind_dir_mean);
      qc_msg.set_nss_hourly_precip_mean(ds.nss_hourly_precip_mean);
      out_msgs.push_back(qc_msg);
    }
}

vdt_dataset vdt_probe_message_datasets::get_vdt_dataset(double obs_time, const vdt_point &target)
{
  float ref = vdt_const::FILL_VALUE;
  short radar_precip_flag = vdt_const::FILL_VALUE;
  short radar_precip_type = vdt_const::FILL_VALUE;
  float model_dewpoint = vdt_const::FILL_VALUE;
  float model_temp = vdt_const::FILL_VALUE;
  float model_press = vdt_const::FILL_VALUE;
  short cloud = vdt_const::SHORT_FILL_VALUE;
  double lat = target.get_latitude();
  double lon = target.get_longitude();

  // Notice how the time is not used for the gridded information
  if (radar != NULL)
    {
      radar->get_reflectivity(lat, lon, ref);
    }
  if (rtma != NULL)
    {
      rtma->get_dewpoint_celsius(lat, lon, model_dewpoint);
      rtma->get_temperature_celsius(lat, lon, model_temp);
      rtma->get_pressure_mb(lat, lon, model_press);
    }
  /*
  if (cloud_class != NULL)
    {
      cloud_class->get_cloud(lat, lon, cloud);
    }
  */

  vector<vdt_surface_observation> near;

  get_near_surface_obs(target, obs_time, cfg_reader->nst_search_radius, cfg_reader->nst_time_range, near);
  float nss_air_temp_mean = vdt_surface_observation::get_avg_air_temp(near);
  float nss_press_mean = vdt_surface_observation::get_avg_press(near);
  float nss_vis_mean = vdt_surface_observation::get_avg_vis(near);
  float nss_dew_temp_mean = vdt_surface_observation::get_avg_dew_temp(near);
  float nss_wind_speed_mean = vdt_surface_observation::get_avg_wind_speed(near);
  float nss_wind_dir_mean = vdt_surface_observation::get_avg_wind_dir(near);
  float nss_hourly_precip_mean = vdt_surface_observation::get_avg_hourly_precip(near);
  vdt_dataset ds;
  ds.model_dewpoint = model_dewpoint;
  ds.model_air_temp = model_temp;
  ds.model_bar_press = model_press;
  ds.radar_ref = ref;
  ds.radar_precip_flag = radar_precip_flag;
  ds.radar_precip_type = radar_precip_type;

  ds.cloud_mask = cloud;
  ds.nss_air_temp_mean = nss_air_temp_mean;
  ds.nss_bar_press_mean = nss_press_mean;
  ds.nss_prevail_vis_mean = nss_vis_mean;
  ds.nss_dew_temp_mean = nss_dew_temp_mean;
  ds.nss_wind_speed_mean = nss_wind_speed_mean;
  ds.nss_wind_dir_mean = nss_wind_dir_mean;
  ds.nss_hourly_precip_mean = nss_hourly_precip_mean;
  return ds;
}

void vdt_probe_message_datasets::get_near_surface_obs(const vdt_point &target, double obs_time, float search_range_km, int time_range_secs, vector<vdt_surface_observation> &near)
{
  // convert km to equator degrees
  float degrees = search_range_km * KM_TO_DEGREES;

  int num_neighbor = 25;
  double target_lat = target.get_latitude();
  double target_lon = target.get_longitude();
  double sq_radius = degrees * degrees;
  vector<int> indices;
  vector<double> distances;
  double eps = 0;
  int ret = surface_obs_nearest_nbr->get_nearest_pts(eps, target_lat, target_lon, num_neighbor, sq_radius, indices, distances);

  if (ret < 0)
    {
      // This will happen only if num_neighbor <= 0 or there were no nearest neighbor points
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
