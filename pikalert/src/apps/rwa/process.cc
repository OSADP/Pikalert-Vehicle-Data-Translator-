//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//==============================================================================

/**
 *
 * @file process.cc
 *
 */

// Include files
#include <algorithm>
#include <log/log.hh>
#include <libconfig.h++>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include "json_writer.hh"
#include "obs_stats_record.hh"
#include "road_segment_map.hh"
#include "rwx/rwx_vector_collection_nc.hh"
#include "rwh_record.hh"
#include "site_alert.hh"
#include "site_info.hh"
#include "treatment_record.hh"

#include "process.hh"

using namespace rwx;
using namespace std;
using namespace libconfig;

extern Log *Logg;

process::~process()
{
}

int process::run()
{
  Logg->write_time_info("reading config file %s\n", args.cfg_file.c_str());

  config_reader cfg_reader(args.cfg_file);
  if (cfg_reader.error != string(""))
    {
      Logg->write_time_error("configuration file error %s\n", cfg_reader.error.c_str());
      error = string("configuration file error") + cfg_reader.error;
      return -1;
    }

  site_info *site_info_ptr = 0;
  if (args.site_list_file != "")
    {
      Logg->write_time_info("reading site list file %s\n", args.site_list_file.c_str());
      site_info_ptr = new site_info(args.site_list_file);
    }

  Logg->write_time_info("reading road segment file %s\n", cfg_reader.road_segment_file.c_str());
  rwx_vector_collection_nc road_segment_collection(cfg_reader.road_segment_file, cfg_reader.road_segment_field_names);
  if (road_segment_collection.error != string(""))
    {
      Logg->write_time_error("ingest error for %s: %s\n", cfg_reader.road_segment_file.c_str(), road_segment_collection.error.c_str());
      return -1;
    }

  Logg->write_time_info("creating map mapping road segment numbers to road segment info %s\n", cfg_reader.road_segment_file.c_str());
  std::unordered_map<int, road_segment_info> road_segment_map;
  int ret = create_road_segment_map(cfg_reader.road_segment_na, road_segment_collection, road_segment_map, error);

  Logg->write_time_info("reading rwh input file %s\n", args.rwh_input_file.c_str());
  rwx_vector_collection_nc rwh_collection(args.rwh_input_file, cfg_reader.rwh_field_names);
  if (rwh_collection.error != string(""))
    {
      Logg->write_time("Error: ingest error for %s: %s\n", args.rwh_input_file.c_str(), rwh_collection.error.c_str());
      return -1;
    }

  Logg->write_time_info("creating rwh record map mapping site indices to rwh alert records\n");
  unordered_map<int, rwh_record> rwh_record_map;
  ret = create_rwh_record_map(cfg_reader.road_segment_na, rwh_collection, rwh_record_map, error);
  if (ret != 0)
    {
      Logg->write_time_error("failed creating rwh record map: %s\n", error.c_str());
      return -1;
    }
  
  const vector<double> *rwh_time_field;

  rwh_collection.get_vector(cfg_reader.rwh_time_field_name, &rwh_time_field);
  time_t obs_time = (*rwh_time_field)[0];
  Logg->write_time_info("observation time %ld\n", obs_time);

  rwx_vector_collection_nc *treatment_collection = 0;
  time_t treatment_forecast_time = 0;
  unordered_map<int, treatment_record> treatment_record_map;

  if (args.treatment_input_file == "")
    {
      Logg->write_time_info("no treatment file specified\n");
    }
  else
    {
      // read treatment file
      if (boost::filesystem::exists(args.treatment_input_file))
	{
	  Logg->write_time_info("reading treatment file %s\n", args.treatment_input_file.c_str());

	  treatment_collection = new rwx_vector_collection_nc(args.treatment_input_file, cfg_reader.treatment_field_names);
	  if (treatment_collection->error != string(""))
	    {
	      Logg->write_time_warning("ingest error for %s: %s\n", args.treatment_input_file.c_str(), treatment_collection->error.c_str());
	      delete treatment_collection;
	      treatment_collection = 0;
	    }
	  else
	    {
	      const vector<double> *treatment_forecast_time_vec;
	      int ret = treatment_collection->get_vector(cfg_reader.treatment_forecast_time_field_name, &treatment_forecast_time_vec);
	      if (ret < 0)
		{
		  Logg->write_time_warning("treatment file error %s: %s\n", args.treatment_input_file.c_str(), treatment_collection->error.c_str());
		}
	      treatment_forecast_time = (*treatment_forecast_time_vec)[0];

	      Logg->write_time_info("creating treatment record map mapping site indices to treatment records\n");
	      ret = create_treatment_record_map(cfg_reader.road_segment_na, treatment_collection, treatment_record_map, error);
	      if (ret != 0)
		{
		  Logg->write_time_error("failed creating treatment record map: %s\n", error.c_str());
		  return -1;
		}
	    }
	}
      else
	{
	  Logg->write_time_warning("treatment file %s does not exist\n", args.treatment_input_file.c_str());
	}
    }


  // Do obs stats processing if requested
  unordered_map<int, obs_stats_record> obs_stats_record_map;
  rwx_vector_collection_nc *obs_stats_collection = 0;

  if (args.obs_stats_input_file == "")
    {
      Logg->write_time_info("no observation statistics file specified\n");
    }
  else
    {
      // read observation statistics file (aka seg stats file)
      if (boost::filesystem::exists(args.obs_stats_input_file))
	{
	  Logg->write_time_info("reading observation statistics file %s\n", args.obs_stats_input_file.c_str());

	  obs_stats_collection = new rwx_vector_collection_nc(args.obs_stats_input_file, cfg_reader.obs_stats_field_names);
	  if (obs_stats_collection->error != string(""))
	    {
	      Logg->write_time_warning("ingest error for %s: %s\n", args.obs_stats_input_file.c_str(), obs_stats_collection->error.c_str());
	      delete obs_stats_collection;
	      obs_stats_collection = 0;
	    }
	  else
	    {
	      Logg->write_time_info("creating obs stats record map mapping site indices to their observation statistics\n");

	      ret = create_obs_stats_record_map(cfg_reader.road_segment_na, obs_stats_collection, obs_stats_record_map, error);

	      if (ret != 0)
		{
		  Logg->write_time_error("failed creating obs stats record map: %s\n", error.c_str());
		  return -1;
		}
	    }
	}
      else
	{
	  Logg->write_time_warning("observation statistics file %s does not exist\n", args.obs_stats_input_file.c_str());
	}
    }

  // establish alert conditions
  const alert_conditions_class alert_conditions = *cfg_reader.alert_conditions;

  string date_time_string = args.date_time_string;
  if (date_time_string[8] == '.')
    {
      date_time_string = date_time_string.substr(0, 8) + date_time_string.substr(9, 4);
    }

  struct tm tms;
  if (NULL == strptime(date_time_string.c_str(), "%Y%m%d%H%M", &tms))
    {
      Logg->write_time_error("could not convert time string %s to time structure\n", date_time_string.c_str());
      return -1;
    }

  vector<site_alert_class> site_alerts(rwh_record_map.size());

  size_t ct = 0;
  for (auto rwh_itr=rwh_record_map.begin(); rwh_itr!=rwh_record_map.end(); ++rwh_itr)
    {
      int road_site_num = rwh_itr->first;

      // determine observed and forecast alert conditions for the site
      alert_conditions.create_rwh_record_alerts(rwh_itr->second, site_alerts[ct].alert_time_series);

      if (ct == 0)
	{
	  Logg->write_time_info("Length of site alert time series[%ld] = %ld\n", ct, site_alerts[ct].alert_time_series.size());
	}

      // Look up additional information in road_segment_map for site alert

      auto itr = road_segment_map.find(road_site_num);
      if (itr == road_segment_map.end())
	{
	  Logg->write_time_error("could not find site number in road_segment_map: %d\n", road_site_num);
	  return -1;
	}

      site_alerts[ct].is_rwis_site = false;
      site_alerts[ct].is_wx_obs_site = false;
      site_alerts[ct].is_road_cond_site = false;

      if (itr->second.point_type)
	{
	  site_alerts[ct].is_rwis_site = true;
	}
      else
	{
	  site_alerts[ct].is_road_cond_site = true;
	}
      site_alerts[ct].lat = itr->second.mid_point_latitude;
      site_alerts[ct].lon = itr->second.mid_point_longitude;
      site_alerts[ct].site_id = itr->second.aux_id;
      site_alerts[ct].road_segment_number = road_site_num;
      site_alerts[ct].set_time_alert_codes(obs_time);

      // get generic forecast id
      auto generic_id_itr = site_info_ptr->fcst_site_map.find(site_alerts[ct].site_id);
      if (generic_id_itr == site_info_ptr->fcst_site_map.end())
	{
	  Logg->write_time_error("could not find site id %s in fcst_site_map\n", site_alerts[ct].site_id.c_str());
	  return -1;
	}

      site_alerts[ct].site_num = generic_id_itr->second.id;
      site_alerts[ct].description = generic_id_itr->second.long_name;

      //printf("iterating record map, site num: %d, mid lat %f, mid lon %f\n", itr->first, site_alerts[ct].lat, site_alerts[ct].lon);

      // incorporate treatment information into alert_time_series for this site

      if (treatment_collection != 0)
	{
	  incorporate_treatments(ct, treatment_record_map, site_alerts);

	  //printf("looking for site id %s, len %ld\n", site_alerts[ct].site_id.c_str(), site_info_ptr->fcst_site_map.size());

	  //printf("found site id %s, %d\n", site_alerts[ct].site_id.c_str(), treatment_id_itr->second);
	}

      ct += 1;
    }

  Logg->write_time_info("outputting json alerts file: %s\n", args.alerts_output_file.c_str());

  ret = alerts_json_writer(date_time_string, cfg_reader, args.alerts_output_file, site_alerts, error);
  if (ret != 0)
    {
      Logg->write_time_error("alerts_json_writer failure:  %s\n", error.c_str());
      return -1;
    }

  Logg->write_time_info("outputting json plots file: %s\n", args.plots_output_file.c_str());

  ret = plots_json_writer(date_time_string, args.plots_output_file, site_alerts, error);
  if (ret != 0)
    {
      Logg->write_time_error("plots_json_writer failure:  %s\n", error.c_str());
      return -1;
    }

  if (obs_stats_record_map.size() > 0)
    {
      Logg->write_time_info("outputting json road segment observations file: %s\n", args.obs_stats_output_file.c_str());
      ret = obs_stats_json_writer(date_time_string, args.obs_stats_output_file, obs_stats_record_map, road_segment_map, site_info_ptr, error);
      if (ret != 0)
	{
	  Logg->write_time_error("obs_stats_json_writer failure:  %s\n", error.c_str());
	  return -1;
	}
    }

  Logg->write_time_info("outputting json maw file: %s\n", args.maw_output_file.c_str());

  ret = maw_json_writer(date_time_string, args.maw_output_file, site_alerts, error);
  if (ret != 0)
    {
      Logg->write_time_error("maw_json_writer failure:  %s\n", error.c_str());
      return -1;
    }

  delete site_info_ptr;
  delete obs_stats_collection;
  delete treatment_collection;

  return ret;
}
