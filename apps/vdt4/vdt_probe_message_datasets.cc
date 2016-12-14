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

//#define DEBUG 1

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
						       string radar_file_pattern_cref,
						       string radar_file_pattern_dual_pol_hc,
						       string radar_file_pattern_dual_pol_hr,
						       float radar_rad,
						       string metar_file,
						       string rwis_file,
						       string rtma_file,
						       string cloud_class_file,
						       bool is_alaska,
						       bool old_radar,
						       Log* logg) : cfg_reader(cfg)
{
  alaska = false;
  radar = NULL;
  radar_cref = NULL;
  radar_dual_pol_hc = NULL;
  radar_dual_pol_hr = NULL;  
  rtma = NULL;
  cloud_class = NULL;
  this->logg = logg;

  if (is_alaska)
    alaska = true;
  
  if (radar_file_pattern != "")
    { 
      if (old_radar)
      {
	Logg->write_time_info("Warning: using old MRMS radar format\n");
	Logg->write_time_info("calling vdt_nssl_tiled_radar_dataset(%s)\n", radar_file_pattern.c_str());
	radar = new vdt_nssl_tiled_radar_dataset(radar_file_pattern.c_str(), logg);
      }
      else
      {
	Logg->write_time_info("calling vdt_nssl_radar_dataset2(%s)\n", radar_file_pattern.c_str());
	vdt_nssl_radar_dataset2* radar_ds2 = new vdt_nssl_radar_dataset2(radar_file_pattern.c_str(), logg);
	//Provides a print out of the reflectivity at the 4 bounding corners
	radar_ds2->corner_tests();
	radar = radar_ds2;
      }
    }
  if (radar_file_pattern_cref != "")
    { 
      Logg->write_time_info("calling vdt_nssl_radar_dataset2(%s)\n", radar_file_pattern_cref.c_str());
      vdt_nssl_radar_dataset2* radar_ds2 = new vdt_nssl_radar_dataset2(radar_file_pattern_cref.c_str(), logg);
      //Provides a print out of the reflectivity at the 4 bounding corners
      radar_ds2->corner_tests();
      radar_cref = radar_ds2;
    }
  if (radar_file_pattern_dual_pol_hc != "")
    { 
      Logg->write_time_info("calling vdt_dual_pol_tiled_radar_dataset(%s)\n", radar_file_pattern_dual_pol_hc.c_str());
      vdt_dual_pol_tiled_radar_dataset* radar_ds3 = new vdt_dual_pol_tiled_radar_dataset(radar_file_pattern_dual_pol_hc.c_str(), 1, radar_rad, logg);
      //Provides a print out of the reflectivity at the 4 bounding corners
      radar_ds3->corner_tests();
      radar_dual_pol_hc = radar_ds3;
    }

  if (radar_file_pattern_dual_pol_hr != "")
    { 
      Logg->write_time_info("calling vdt_dual_pol_tiled_radar_dataset3(%s)\n", radar_file_pattern_dual_pol_hr.c_str());
      vdt_dual_pol_tiled_radar_dataset* radar_ds3 = new vdt_dual_pol_tiled_radar_dataset(radar_file_pattern_dual_pol_hr.c_str(), 0, radar_rad, logg);
      //Provides a print out of the reflectivity at the 4 bounding corners
      radar_ds3->corner_tests();
      radar_dual_pol_hr = radar_ds3;
    }
  
  if (metar_file != "")
    {
      Logg->write_time_info("calling vdt_metar_file_reader(%s)\n", metar_file.c_str());
      vdt_metar_file_reader* metar_reader = new vdt_metar_file_reader(metar_file.c_str());
      metar_reader->get_metars(obs);
      Logg->write_time_info("read %ld metar surface obs\n", obs.size());
      delete metar_reader;
    }
  if (rwis_file != "")
    {
      Logg->write_time_info("calling vdt_rwis_file_reader(%s)\n", rwis_file.c_str());
      vdt_rwis_file_reader* rwis_reader = new vdt_rwis_file_reader(rwis_file.c_str());
      rwis_reader->get_rwis(obs);
      if (rwis_reader->get_error() != "")
      {
	Logg->write_time_warning("%s\n", rwis_reader->get_error().c_str());
      }
      Logg->write_time_info("read %d rwis surface obs\n", rwis_reader->get_num_rwis_obs());
      delete rwis_reader;
    }
  if (rtma_file != "")
    {
      if (alaska)
	{
	  Logg->write_time_info("constructing rwx_ak_rtma_dataset(%s)\n", rtma_file.c_str());
	  
	  ak_rtma = new rwx_ak_rtma_dataset(rtma_file.c_str(), cfg->ak_rtma_proj_info);
	  if (ak_rtma->error != "")
	    {
	      Logg->write_time_warning("failed reading ak_rtma file, error: %s\n", ak_rtma->error.c_str());
	      ak_rtma = NULL;
	    }
	}
      else
	{
	  Logg->write_time_info("constructing rwx_rtma_dataset(%s)\n", rtma_file.c_str());
	  rtma = new rwx_rtma_dataset(rtma_file.c_str(), cfg->rtma_proj);
	  if (rtma->error != "")
	    {
	      Logg->write_time_warning("failed reading rtma file, error: %s\n", rtma->error.c_str());
	      rtma = NULL;
	    }
	}
    }
  if (cloud_class_file != "")
    {
      Logg->write_time_info("vdt_cloud_mask_dataset(%s) not currently supported\n", cloud_class_file.c_str());
      //Logg->write_time_info("calling vdt_cloud_mask_dataset(%s)\n", cloud_class_file.c_str());
      //cloud_class = new vdt_cloud_mask_dataset(cloud_class_file.c_str());
    }

  vdt_point::get_points_lle(obs, surface_station_latitude, surface_station_longitude, surface_station_elevation);

  Logg->write_time_info("creating surface observation nearest neighbor object using %ld surface obs\n", surface_station_latitude.size());
  surface_obs_nearest_nbr = new vdt_nearest_nbr(surface_station_latitude, surface_station_longitude);
}

vdt_probe_message_datasets::~vdt_probe_message_datasets()
{
  if (radar != NULL)
    {
      delete radar;
      radar = NULL;
    }
  if (radar_cref != NULL)
    {
      delete radar_cref;
      radar_cref = NULL;
    }
  if (radar_dual_pol_hc != NULL)
    {
      delete radar_dual_pol_hc;
      radar_dual_pol_hc = NULL;
    }
  if (radar_dual_pol_hr != NULL)
    {
      delete radar_dual_pol_hr;
      radar_dual_pol_hr = NULL;
    }
  
  if (rtma != NULL)
    {
      delete rtma;
      rtma = NULL;
    }

  if (ak_rtma != NULL)
    {
      delete ak_rtma;
      ak_rtma = NULL;
    }

  delete surface_obs_nearest_nbr;
  surface_obs_nearest_nbr = NULL;
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
  FIELD_NAME_TYPE_VEC qc_fields;
  filter_vars_by_test(cfg, var_test::MET_FIELD_KEY, qc_fields);
  
  for (size_t i = 0; i < in_msgs.size(); i++)
    {
      const vdt_probe_message& msg = in_msgs[i];
      vdt_probe_message_qc qc_msg(msg, qc_fields);
      double obs_time = msg.get_obs_time();
      vdt_dataset ds = get_vdt_dataset(obs_time,  msg.get_point());
      qc_msg.set_radar_ref(ds.radar_ref);
      qc_msg.set_radar_cref(ds.radar_cref);
      qc_msg.set_radar_dual_pol_hc(ds.radar_dual_pol_hc);
      qc_msg.set_radar_dual_pol_hr(ds.radar_dual_pol_hr);            
      qc_msg.set_model_dewpoint(ds.model_dewpoint);
      qc_msg.set_model_air_temp(ds.model_air_temp);
      qc_msg.set_model_bar_press(ds.model_bar_press);
      qc_msg.set_cloud_mask(ds.cloud_mask);
      qc_msg.set_nss_air_temp_mean(ds.nss_air_temp_mean);
      qc_msg.set_nss_surface_temp_mean(ds.nss_surface_temp_mean);
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
  float cref = vdt_const::FILL_VALUE;
  float dual_pol_hc_val = vdt_const::FILL_VALUE;
  float dual_pol_hr_val = vdt_const::FILL_VALUE;    
  short radar_precip_flag = vdt_const::FILL_VALUE;
  short radar_precip_type = vdt_const::FILL_VALUE;
  float model_dewpoint = vdt_const::FILL_VALUE;
  float model_temp = vdt_const::FILL_VALUE;
  float model_press = vdt_const::FILL_VALUE;
  short cloud = vdt_const::SHORT_FILL_VALUE;
  double lat = target.get_latitude();
  double lon = target.get_longitude();
  int xcoord;
  int ycoord;

  // Notice how the time is not used for the gridded information
  if (radar != NULL)
    {
      radar->get_reflectivity(lat, lon, ref, xcoord, ycoord);
#ifdef DEBUG
      if(ref != vdt_const::FILL_VALUE)
      {
	Logg->write_time("Debug: lon:%.2f,lat:%.2f,x:%d,y:%d,ref:%.2f\n",lon,lat,xcoord,ycoord,ref);
      }
#endif
    }
  if (radar_cref != NULL)
    {
      radar_cref->get_reflectivity(lat, lon, cref, xcoord, ycoord);
#ifdef DEBUG
      if(cref != vdt_const::FILL_VALUE)
      {
	Logg->write_time("Debug: lon:%.2f,lat:%.2f,x:%d,y:%d,cref:%.2f\n",lon,lat,xcoord,ycoord,cref);
      }
#endif
    }

    if (radar_dual_pol_hc != NULL)
    {
      radar_dual_pol_hc->get_reflectivity(lat, lon, dual_pol_hc_val, xcoord, ycoord);
      //dual_pol_hc_val = 10.0;
#ifdef DEBUG
      if(dual_pol_hc_val != vdt_const::FILL_VALUE)
      {
	Logg->write_time("Debug: lon:%.2f,lat:%.2f,x:%d,y:%d,dual_pol_hc ref:%.2f\n",lon,lat,xcoord,ycoord,dual_pol_hc_val);
      }
#endif
    }

    if (radar_dual_pol_hr != NULL)
    {
      radar_dual_pol_hr->get_reflectivity(lat, lon, dual_pol_hr_val, xcoord, ycoord);
      //dual_pol_hr_val = 10.0;
#ifdef DEBUG
      if(dual_pol_hr_val != vdt_const::FILL_VALUE)
      {
	Logg->write_time("Debug: lon:%.2f,lat:%.2f,x:%d,y:%d,dual_pol_hr ref:%.2f\n",lon,lat,xcoord,ycoord,dual_pol_hr_val);
      }
#endif
    }
    
if (rtma != NULL)
    {
      rtma->get_dewpoint_celsius(lat, lon, model_dewpoint);
      rtma->get_temperature_celsius(lat, lon, model_temp);
      rtma->get_pressure_mb(lat, lon, model_press);
    }
  else if (ak_rtma != NULL)
    {
      ak_rtma->get_dewpoint_celsius(lat, lon, model_dewpoint);
      ak_rtma->get_temperature_celsius(lat, lon, model_temp);
      ak_rtma->get_pressure_mb(lat, lon, model_press);
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
  float nss_surface_temp_mean = vdt_surface_observation::get_avg_surface_temp(near);
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
  ds.radar_cref = cref;
  ds.radar_dual_pol_hc = dual_pol_hc_val;
  ds.radar_dual_pol_hr = dual_pol_hr_val;  
  ds.radar_precip_flag = radar_precip_flag;
  ds.radar_precip_type = radar_precip_type;

  ds.cloud_mask = cloud;
  ds.nss_air_temp_mean = nss_air_temp_mean;
  ds.nss_surface_temp_mean = nss_surface_temp_mean;
  ds.nss_bar_press_mean = nss_press_mean;
  ds.nss_prevail_vis_mean = nss_vis_mean;
  ds.nss_dew_temp_mean = nss_dew_temp_mean;
  ds.nss_wind_speed_mean = nss_wind_speed_mean;
  ds.nss_wind_dir_mean = nss_wind_dir_mean;
  ds.nss_hourly_precip_mean = nss_hourly_precip_mean;

  if(near.size() > 0)
  {
    //near[0] should be the nearest neighbor
    ds.pres_wx = near[0].get_pres_wx();
    ds.wx = near[0].get_wx();
    ds.road_state_1 = near[0].get_road_state_1();
  }

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
