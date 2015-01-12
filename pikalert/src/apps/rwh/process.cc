//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/process.cc
 *
 */

// Include files

#include <log/log.hh>
#include <libconfig.h++>
#include <stdio.h>
#include <stdlib.h>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "vdt4/vdt_road_segment_file_reader.hh"
#include "vdt4/vdt_probe_message_qc_statistics.hh"

#include "rwx/rwx_vector_collection.hh"
#include "rwx/rwx_cdf_field_reader.hh"
#include "rwx/rwx_cdf_field_writer.hh"
#include "rwx/rwx_road_segment_assessment.hh"

#include "process.hh"
#include "site_vars_fcst_data.hh"

using namespace rwx;
using namespace boost::filesystem;
using namespace std;
using namespace libconfig;
using boost::format;

extern Log *Logg;
extern int Debug_level;

process::~process()
{
}

int process::run()
{
  int ret;
  size_t i, j, s, v;
  
  time_t begin_time = args.begin_time;
  string begin_time_string = args.begin_time_string;
  time_t end_time = args.end_time;
  string end_time_string = args.end_time_string;
  string fcst_road_seg_site_list_file = args.fcst_road_seg_site_list_file;
  string vdt_road_seg_file = args.vdt_road_seg_file;
  string output_file = args.output_file;
  
  Logg->write_time(1, "Info: generating road weather hazard data from %s to %s\n", begin_time_string.c_str(), end_time_string.c_str());
  
  // Initialize utility object for reading site-lists and fcst files
  stat_utils = new StatUtils(Logg);
  ncfc_utils = new NcfcUtils(Logg);
  
  // Read configuration file
  Logg->write_time(1, "Info: reading config file %s\n", args.cfg_file.c_str());
  config_reader cfg_reader(args.cfg_file);
  if (cfg_reader.error != string(""))
    {
      Logg->write_time("Error: configuration file error %s\n", cfg_reader.error.c_str());
      error = string("configuration file error ") + cfg_reader.error;
      return(1);
    }
    
  // Get the site information from the site_list file
  vector <site *> sites;
  Logg->write_time(1, "Info. getting site list info from %s.\n", (char *)(fcst_road_seg_site_list_file).c_str());
  ret = stat_utils->get_site_info((char *)(fcst_road_seg_site_list_file).c_str(), &sites);
  if (ret != 0)
    {
      Logg->write_time("Error: in get_site_info for %s.\n", (char *)(fcst_road_seg_site_list_file).c_str());
      return(ret);
    }
  
  // Read the VDT road segment file (create vdt_road_segment_file_reader object)
  vector<vdt_road_segment> segs; 
  Logg->write_time(1, "Info: calling vdt_road_segment_file_reader(%s)\n", vdt_road_seg_file.c_str());
  vdt_road_segment_file_reader seg_reader = vdt_road_segment_file_reader(vdt_road_seg_file.c_str());
  
  // Get the road-segments
  ret = seg_reader.get_road_segments(segs);
  if(ret != 0)
    {
      Logg->write_time("Error: get_road_segments failed for file: %s\n", vdt_road_seg_file.c_str());
      return(ret);
    }
  //printf("segs.size(): %ld\n", segs.size());
  
  // Get the rwis sites
  ret = seg_reader.get_rwis_sites(segs);
  if(ret != 0)
    {
      Logg->write_time("Error: get_rwis_sites failed for file: %s\n", vdt_road_seg_file.c_str());
      return(ret);
    }
  //printf("segs.size(): %ld\n", segs.size());
  
  
  // Get the segment ids, aux(rwfs) ids and lat/lon mid-points
  vector<int> seg_ids;
  vector<string> aux_ids;
  
  for(i = 0; i < segs.size(); i++)
    {
      seg_ids.push_back(segs[i].get_id());
      aux_ids.push_back(segs[i].get_aux_id());
    }  
  
  // For debugging only
  for(i = 0; i < seg_ids.size(); i++)
    Logg->write_time(4, "Info: i: %d, seg_id: %d, aux_id: %s\n", i, seg_ids[i], aux_ids[i].c_str());
  
  
  // Create a map between the vdt segment-list and the forecast site list index
  unordered_map<int, int> seg_site_map;
  for(i = 0; i < aux_ids.size(); i++)
    {
      int found = 0;
      for(j = 0; j < sites.size(); j++)
	{
	  if( strcmp(aux_ids[i].c_str(), sites[j]->icao_id) == 0 )
	    {
	      Logg->write_time(4, "Info: seg_ids[%ld]: %d aux_ids[%ld]: %s, sites[%ld]->icao_id: %s\n", i, seg_ids[i], i, aux_ids[i].c_str(), j, sites[j]->icao_id);
	      found = 1;
	      seg_site_map[i] = j;
	      break;
	    }
	} // end loop over sites
    } // end loop over segments: aux_ids
  
  /*
  // For debugging only
  for(i = 0; i < seg_ids.size(); i++)
    {
      if(seg_site_map.find(i) != seg_site_map.end())
	printf("seg_ids[%ld]: %d, seg_site_map[%ld]: %d\n", i, seg_ids[i], i, seg_site_map[i]);
    }
  */

  
  // Read in vdt segment statistics file if we have it
  // and store data in vector of vdt_prob_message_qc_statistics objects
  //
  vector<vdt_probe_message_qc_statistics> seg_stats;
  unordered_map<int, int> seg_coll_ind_map;
  if(args.vdt_seg_stats_file != "")
    {      
      Logg->write_time(1, "Info: reading vdt_seg_stats_file %s\n", args.vdt_seg_stats_file.c_str());
      
      // Open seg stats file
      rwx_cdf_field_reader seg_stats_cdf_file(args.vdt_seg_stats_file);
      if (seg_stats_cdf_file.error() != string(""))
	{
	  Logg->write_time("Error: rwx cdf field reader error %s\n", seg_stats_cdf_file.error().c_str());
	  return -1;
	}
      
      // Read double fields
      vector<string> double_field_names;
      vector< vector<double> > double_field_vector;
      vector<string> double_units;
      vector<double> double_missing;
      string error;

      //Logg->write_time("Info: getting double fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.double_field_names, double_field_vector, double_units, double_missing, error);

      vector<string> float_field_names;
      vector< vector<float> > float_field_vector;
      vector<string> float_units;
      vector<float> float_missing;

      //Logg->write_time("Info: getting float fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.float_field_names, float_field_vector, float_units, float_missing, error);

      vector<string> int_field_names;
      vector< vector<int> > int_field_vector;
      vector<string> int_units;
      vector<int> int_missing;

      //Logg->write_time("Info: getting int fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.int_field_names, int_field_vector, int_units, int_missing, error);

      vector<string> short_field_names;
      vector< vector<short> > short_field_vector;
      vector<string> short_units;
      vector<short> short_missing;

      //Logg->write_time("Info: getting short fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.short_field_names, short_field_vector, short_units, short_missing, error);

      vector<string> char_field_names;
      vector< vector<char> > char_field_vector;
      vector<size_t> char_field_size;

      //Logg->write_time("Info: getting character fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.char_field_names, char_field_vector, char_field_size, error);

      Logg->write_time(1, "Info: formulating vector collection for vdt_seg_stats_file\n");
      rwx_vector_collection collection(cfg_reader.input_type_index_map, char_field_vector, char_field_size, short_field_vector, int_field_vector, float_field_vector, double_field_vector);
      if (collection.error != "")
	{
	  Logg->write_time("Error: collection error occurred\n");
	  return 1;
	}
      
      // Get the segment indicies in the seg-stats file
      vector<int> seg_indicies;
      Logg->write_time(1, "Info. getting seg-id indicies from vdt_seg_stats_file\n");
      ret = get_vdt_seg_indicies(collection, seg_ids, seg_indicies);
      if(ret != 0)
	{
	  Logg->write_time("Error: in get_vdt_seg_indicies\n");
	  return 1;
	} 
      
      // Create map between seg-ids and collection vector index
      for(i = 0; i < seg_ids.size(); i++)
	{
	  //printf("seg_ids[%d]: %d, seg_indicies[%d]: %d\n", i, seg_ids[i], i, seg_indicies[i]);
	  if(seg_indicies[i] != MISSING)
	    seg_coll_ind_map[i] = seg_indicies[i];
	}
      
      // Augment vdt_probe_message_qc_statistic object with seg-stats data (collection)
      Logg->write_time(1, "Info. augmenting probe-message-qc-stats with vdt segment collection data\n");
      ret = augment_statistics_with_collection(collection, seg_stats);
      if(ret != 0)
	{
	  Logg->write_time("Error: in augment_statistics_with_collection\n");
	  return 1;
	} 
    }
  
  // Get the hazard data for all vdt segments
  //
  vector<hazard_out *> vdt_hazard_data;
  vdt_hazard_data.clear();
  if(seg_stats.size() > 0)
    {
      // Determine segment statistics valid_time (end_time in seg-stats file)
      double valid_time = MISSING;
      i = 0;
      while(valid_time == MISSING && i < seg_stats.size())
	{
	  valid_time = seg_stats[i].end_time;
	  i++;
	}
      //printf("valid_time: %d\n", (int)valid_time);
      
      // Get hazard data only if we have a non-missing valid_time
      if(valid_time != MISSING)
	{
	  // Cross-check valid_time with the command line begin-time
	  double d_begin_time = (double)begin_time;
	  double begin_diff = d_begin_time - valid_time;
	  float begin_hour_diff = begin_diff / 3600;
	  //printf("d_begin_time: %d, valid_time: %d, begin_hour_diff: %f\n", (int)d_begin_time, (int)valid_time, begin_hour_diff);
	  
	  // Get hazard data only if the difference between the begin-time and the vdt valid-time
	  // is basically positive (vdt data not in future) and within 1 hour
	  // allow 10 minute buffer for end-time into future
	  if(begin_hour_diff > -0.2667 && begin_hour_diff <= 1)
	    {
	      hazard_out *this_hazard_data = new hazard_out;
	      Logg->write_time(1, "Info. getting vdt hazard data\n");
	      ret = get_hazard_data(d_begin_time, valid_time, seg_stats, this_hazard_data);
	      if(ret == 0)
		vdt_hazard_data.push_back(this_hazard_data);
	      else
		Logg->write_time("Warning: get_hazard_data failed for vdt seg-stats data, valid_time: %d, skipping.\n", (int)valid_time);
	    }
	  else
	    Logg->write_time("Warning: did not get hazard data for vdt seg-stats data because the valid_time (end_time): %d is either in the future or too old, skipping.\n", (int)valid_time);
	}
      else
	Logg->write_time("Warning: could not get a valid_time from vdt seg-stats data, skipping.\n");
    }
  

  // Read in the road_wx_fcst data if we have it
  //
  ncfc_io *road_wx_fcst_data = new ncfc_io;
  road_wx_fcst_data->is_valid = 0;
  if (args.road_wx_fcst_file != "")
    {
      Logg->write_time(1, "Info: getting road_wx_fcst data from %s.\n", args.road_wx_fcst_file.c_str());
      ret = ncfc_utils->get_file_data((char *)(args.road_wx_fcst_file).c_str(), road_wx_fcst_data);
      if(ret != 0)
	{
	  Logg->write_time("Error: in get_file_data for %s.\n", (char *)(args.road_wx_fcst_file).c_str());
	  return(ret);
	}
      else // Get site list indicies 
	{
	  ret = get_fcst_site_indicies(road_wx_fcst_data, sites);
	  if(ret != 0)
	    {
	      Logg->write_time("Error: in get_fcst_site_indicies for %s.\n", (char *)(args.road_wx_fcst_file).c_str());
	      return(ret);
	    } 
	}
    }
  //printf("road_wx_fcst_data->is_valid: %d\n", road_wx_fcst_data->is_valid);
  
  // Read in the road_cond_fcst data if we have it
  //
  ncfc_io *road_cond_fcst_data = new ncfc_io;
  road_cond_fcst_data->is_valid = 0;
  if (args.road_cond_fcst_file != "")
    {
      Logg->write_time(1, "Info: getting road_cond_fcst data from %s.\n", args.road_cond_fcst_file.c_str());
      ret = ncfc_utils->get_file_data((char *)(args.road_cond_fcst_file).c_str(), road_cond_fcst_data);
      if(ret != 0)
	{
	  Logg->write_time("Error: in get_file_data for %s.\n", (char *)(args.road_cond_fcst_file).c_str());
	  return(ret);
	}
      else // Get site list indicies 
	{
	  ret = get_fcst_site_indicies(road_cond_fcst_data, sites);
	  if(ret != 0)
	    {
	      Logg->write_time("Error: in get_fcst_site_indicies for %s.\n", (char *)(args.road_cond_fcst_file).c_str());
	      return(ret);
	    } 
	}
    }
  //printf("road_cond_fcst_data->is_valid: %d\n", road_cond_fcst_data->is_valid);
  

  // Store all the forecast-input variables we need for assessment
  // and consolidate the forecast data into one vector
  //
  vector<ncfc_io *> all_fcst_data;
  vector< vector<string> > fcst_field_names;
  int fcst_data_flag = 0;
  if(road_wx_fcst_data->is_valid)
    {
      fcst_data_flag = 1;
      all_fcst_data.push_back(road_wx_fcst_data);
      vector<string> these_field_names;
      for(v = 0; v < cfg_reader.rdwx_field_names.size(); v++)
	{
	  //printf("rdwx_field_names[%d]: %s\n", v, cfg_reader.rdwx_field_names[v].c_str());
	  these_field_names.push_back(cfg_reader.rdwx_field_names[v]);
	}
      fcst_field_names.push_back(these_field_names);
    }
  
  if(road_cond_fcst_data->is_valid)
    {
      fcst_data_flag = 1;
      all_fcst_data.push_back(road_cond_fcst_data);
      vector<string> these_field_names;
      for(v = 0; v < cfg_reader.rcst_field_names.size(); v++)
	{
	  //printf("rcst_field_names[%d]: %s\n", v, cfg_reader.rcst_field_names[v].c_str());
	  these_field_names.push_back(cfg_reader.rcst_field_names[v]);
	}
      fcst_field_names.push_back(these_field_names);
    }
  
  // Calculate number of forecast days into the future based on the difference
  // between the end_time and begin_time
  // Add extra day to account for fcst-hours in last day
  //
  double rnd_begin_time = (double)( ((int)(begin_time/3600)) * 3600 );
  double rnd_end_time = (double)( ((int)(end_time/3600)) * 3600 );
  double end_begin_diff = rnd_end_time - rnd_begin_time; 
  int max_lead_time = end_begin_diff / 3600;
  int max_days = max_lead_time / 24;
  int day_rem = max_lead_time % 24;
  if(day_rem > 0)
    max_days = max_days + 2;
  else
    max_days = max_days + 1;
  
  // Get site-specific forecast data for forecast fields for assessment
  //
  site_vars_fcst_data site_fcst(Logg);
  Logg->write_time(1, "Info. getting site fcst data.\n");
  ret = site_fcst.get_fcst_data(begin_time, sites, fcst_field_names, all_fcst_data, max_days, max_lead_time);
  if(ret != 0){
    Logg->write_time("Error: in get_fcst_data.\n");
    return(ret);
  }
  
  // Loop over forecast lead-times. For each lead-time (valid-time) loop over
  // all sites (segments) and augment vdt_probe_message_qc_statistic object with site-fcst data.
  // Then get hazard data for all segments 
  //
  double d_begin_time = (double)begin_time;
  double valid_time;
  valid_time = rnd_begin_time;
  
  // Create vector of forecast hazard data for all lead-times
  vector<hazard_out *> fcst_hazard_data;
  fcst_hazard_data.clear();
  
  if(fcst_data_flag)
    {     
      Logg->write_time(1, "Info. augmenting probe-message-qc-stats with forecast data and getting forecast hazard data\n", (int)valid_time);
      while(valid_time <= rnd_end_time)
	{
	  vector<vdt_probe_message_qc_statistics> fcst_seg_stats;
	  fcst_seg_stats.clear();
	  Logg->write_time(2, "Info. augmenting probe-message-qc-stats with forecast data for valid_time: %d.\n", (int)valid_time);
	  ret = augment_statistics_with_fcst(valid_time, site_fcst, seg_ids, seg_site_map, fcst_seg_stats);
	  if(ret != 0)
	    {
	      Logg->write_time("Error: in augment_statistics_with_fcst\n");
	      return 1;
	    } 
      
	  // Get the hazard data for all segments for the given lead-time
	  if(fcst_seg_stats.size() > 0)
	    {
	      hazard_out *this_hazard_data = new hazard_out;
	      Logg->write_time(2, "Info. getting forecast hazard data for valid_time: %d\n", (int)valid_time);
	      ret = get_hazard_data(d_begin_time, valid_time, fcst_seg_stats, this_hazard_data);
	      if(ret != 0)
		{
		  Logg->write_time("Warning: get_hazard_data failed for forecast seg-stats data valid_time: %d\n", (int)valid_time);
		  valid_time = valid_time + 3600;
		  continue;
		}
	      fcst_hazard_data.push_back(this_hazard_data);
	    }
      
	  valid_time = valid_time + 3600;

	} // end while loop over valid_times
    }
  
  // Write all the hazard data to the output file
  Logg->write_time(1, "Info. writng hazard data to output file\n");
  ret = write_hazard_file(seg_ids, seg_coll_ind_map, seg_site_map, vdt_hazard_data, fcst_hazard_data, cfg_reader.rwh_cdl_file, output_file);
  if(ret != 0)
    {
      Logg->write_time("Error: write_hazard_file failed.\n");
      return 1;
    }

  // Clean-up
  
  sites.clear();
  segs.clear();
  seg_ids.clear();
  aux_ids.clear();
  
  seg_site_map.clear();
  seg_coll_ind_map.clear();
  
  seg_stats.clear();
  vdt_hazard_data.clear();

  ncfc_utils->clean_ncfc_io(road_wx_fcst_data);
  ncfc_utils->clean_ncfc_io(road_cond_fcst_data);
  
  all_fcst_data.clear();
  fcst_field_names.clear();
  
  fcst_hazard_data.clear();

  return 0;
}


int process::write_hazard_file(const vector<int> &seg_ids, unordered_map<int, int> &seg_coll_ind_map, unordered_map<int, int> &seg_site_map, const vector<hazard_out *> &vdt_hazard_data, const vector<hazard_out *> &fcst_hazard_data, string cdl_file, string output_file)
{
  size_t i,j;
  
  // Open output file
  rwx_cdf_field_writer cdf_file(cdl_file, output_file);
  
  // Get dimensions of rec_num
  size_t out_vector_size = 0;
  const std::multimap<string, netCDF::NcDim> &dim_map = cdf_file.get_dimension_map();
  for(auto itr = dim_map.begin(); itr != dim_map.end(); itr++)
    {
      //printf("dimension_name: %s, dimension_size: %ld\n", itr->first.c_str(), itr->second.getSize());
      if(strcmp(itr->first.c_str(), "rec_num") == 0)
	out_vector_size = itr->second.getSize();
    }
  
  //printf("out_vector_size: %ld\n", out_vector_size);
  if(out_vector_size == 0)
    {
      Logg->write_time("Error: Could not get out_vector_size from cdl rec_num\n.");
      return 1;
    }
  
  // Setup and initialize output vectors
  vector<double> time_nominal;
  vector<int> road_segment_id;
  vector<int> num_probe_msg;
  vector<short> precip_type;
  vector<float> precip_type_conf;
  vector<short> precip_intensity;
  vector<float> precip_intensity_conf;
  vector<short> pavement_condition;
  vector<float> pavement_condition_conf;
  vector<short> visibility;
  vector<float> visibility_conf;
  vector<short> pavement_slickness;
  
  for(i = 0; i < out_vector_size; i++)
    {
      time_nominal.push_back(MISSING);
      road_segment_id.push_back(MISSING);
      num_probe_msg.push_back(MISSING);
      precip_type.push_back(MISSING);
      precip_type_conf.push_back(MISSING);
      precip_intensity.push_back(MISSING);
      precip_intensity_conf.push_back(MISSING);
      pavement_condition.push_back(MISSING);
      pavement_condition_conf.push_back(MISSING);
      visibility.push_back(MISSING);
      visibility_conf.push_back(MISSING);
      pavement_slickness.push_back(MISSING);
    }
  
  // Copy hazard data to output vectors
  //
  size_t rec_num_ind = 0;
  
  // Do VDT data
  double last_vdt_valid_time = MISSING;
  Logg->write_time(2, "Info: copying vdt hazard data to output vectors starting with rec_num_ind: %ld\n", rec_num_ind);
  for(i = 0; i < vdt_hazard_data.size(); i++)
    {
      if(rec_num_ind >= out_vector_size)
	{
	  Logg->write_time("Warning: vdt_hazard_data exceeds out_vector_size, truncating output\n");
	  break;
	}
      
      for(j = 0; j < seg_ids.size(); j++)
	{
	  if(seg_coll_ind_map.find(j) != seg_coll_ind_map.end())
	    {
	      int s = seg_coll_ind_map[j];
	      
	      Logg->write_time(2, "Info: rec_num_ind: %ld, vdt_hazard_data[%ld]->valid_time: %d, seg_id[%ld]: %d, num_probe_msg: %d, precip_type: %d, precip_type_conf: %.2f, precip_intensity: %d, precip_intensity_conf: %.2f, pavement_condition: %d, pavement_condition_conf: %.2f, visibility: %d, visibility_conf: %.2f, pavement_slickness: %d\n", rec_num_ind, i, (int)vdt_hazard_data[i]->valid_time, j, vdt_hazard_data[i]->seg_id[s], vdt_hazard_data[i]->num_probe_msg[s], vdt_hazard_data[i]->precip_type[s], vdt_hazard_data[i]->precip_type_conf[s], vdt_hazard_data[i]->precip_intensity[s], vdt_hazard_data[i]->precip_intensity_conf[s], vdt_hazard_data[i]->pavement_condition[s], vdt_hazard_data[i]->pavement_condition_conf[s], vdt_hazard_data[i]->visibility[s], vdt_hazard_data[i]->visibility_conf[s], vdt_hazard_data[i]->pavement_slickness[s]);
	      
	      time_nominal[rec_num_ind] = vdt_hazard_data[i]->valid_time;
	      road_segment_id[rec_num_ind] = vdt_hazard_data[i]->seg_id[s];
	      num_probe_msg[rec_num_ind] = vdt_hazard_data[i]->num_probe_msg[s];
	      precip_type[rec_num_ind] = vdt_hazard_data[i]->precip_type[s];
	      precip_type_conf[rec_num_ind] = vdt_hazard_data[i]->precip_type_conf[s];
	      precip_intensity[rec_num_ind] = vdt_hazard_data[i]->precip_intensity[s];
	      precip_intensity_conf[rec_num_ind] = vdt_hazard_data[i]->precip_intensity_conf[s];
	      pavement_condition[rec_num_ind] = vdt_hazard_data[i]->pavement_condition[s];
	      pavement_condition_conf[rec_num_ind] = vdt_hazard_data[i]->pavement_condition_conf[s];
	      visibility[rec_num_ind] = vdt_hazard_data[i]->visibility[s];
	      visibility_conf[rec_num_ind] = vdt_hazard_data[i]->visibility_conf[s];
	      pavement_slickness[rec_num_ind] = vdt_hazard_data[i]->pavement_slickness[s];
	      
	      last_vdt_valid_time = vdt_hazard_data[i]->valid_time;
	      rec_num_ind++;
	    }
	} // end loop over seg_ids
    } // end loop over vdt_hazard_data
  
  // Do forecast data
  Logg->write_time(2, "Info: last_vdt_valid_time: %d\n", (int)last_vdt_valid_time);
  Logg->write_time(2, "Info: copying forecast hazard data to output vectors starting with rec_num_ind: %ld\n", rec_num_ind);
  for(i = 0; i < fcst_hazard_data.size(); i++)
    {
      if(rec_num_ind >= out_vector_size)
	{
	  Logg->write_time("Warning: fcst_hazard_data exceeds out_vector_size, truncating output\n");
	  break;
	}
      
      for(j = 0; j < seg_ids.size(); j++)
	{
	  // Only output forecast data that is past the last vdt valid time
	  if(seg_site_map.find(j) != seg_site_map.end() && fcst_hazard_data[i]->valid_time > last_vdt_valid_time)
	    {
	      int s = seg_site_map[j];
	      
	      Logg->write_time(2, "Info: rec_num_ind: %ld, fcst_hazard_data[%ld]->valid_time: %d, seg_id[%ld]: %d, num_probe_msg: %d, precip_type: %d, precip_type_conf: %.2f, precip_intensity: %d, precip_intensity_conf: %.2f, pavement_condition: %d, pavement_condition_conf: %.2f, visibility: %d, visibility_conf: %.2f, pavement_slickness: %d\n", rec_num_ind, i, (int)fcst_hazard_data[i]->valid_time, j, fcst_hazard_data[i]->seg_id[s], fcst_hazard_data[i]->num_probe_msg[s], fcst_hazard_data[i]->precip_type[s], fcst_hazard_data[i]->precip_type_conf[s], fcst_hazard_data[i]->precip_intensity[s], fcst_hazard_data[i]->precip_intensity_conf[s], fcst_hazard_data[i]->pavement_condition[s], fcst_hazard_data[i]->pavement_condition_conf[s], fcst_hazard_data[i]->visibility[s], fcst_hazard_data[i]->visibility_conf[s], fcst_hazard_data[i]->pavement_slickness[s]);
	      
	      time_nominal[rec_num_ind] = fcst_hazard_data[i]->valid_time;
	      road_segment_id[rec_num_ind] = fcst_hazard_data[i]->seg_id[s];
	      num_probe_msg[rec_num_ind] = fcst_hazard_data[i]->num_probe_msg[s];
	      precip_type[rec_num_ind] = fcst_hazard_data[i]->precip_type[s];
	      precip_type_conf[rec_num_ind] = fcst_hazard_data[i]->precip_type_conf[s];
	      precip_intensity[rec_num_ind] = fcst_hazard_data[i]->precip_intensity[s];
	      precip_intensity_conf[rec_num_ind] = fcst_hazard_data[i]->precip_intensity_conf[s];
	      pavement_condition[rec_num_ind] = fcst_hazard_data[i]->pavement_condition[s];
	      pavement_condition_conf[rec_num_ind] = fcst_hazard_data[i]->pavement_condition_conf[s];
	      visibility[rec_num_ind] = fcst_hazard_data[i]->visibility[s];
	      visibility_conf[rec_num_ind] = fcst_hazard_data[i]->visibility_conf[s];
	      pavement_slickness[rec_num_ind] = fcst_hazard_data[i]->pavement_slickness[s];
	      
	      rec_num_ind++;
	    }

	} // end loop over seg_ids
    } // end loop over fcst_hazard_data
  

  // Write data to output file
  string error;
  
  cdf_file.put_field("time_nominal", time_nominal, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("road_segment_id", road_segment_id, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("num_probe_msg", num_probe_msg, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("precip_type", precip_type, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("precip_type_confidence", precip_type_conf, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("precip_intensity", precip_intensity, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("precip_intensity_confidence", precip_intensity_conf, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("pavement_condition", pavement_condition, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("pavement_condition_confidence", pavement_condition_conf, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("visibility", visibility, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("visibility_confidence", visibility_conf, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  cdf_file.put_field("pavement_slickness", pavement_slickness, error);
  if(error != "")
    Logg->write_time("Error: %s\n", error.c_str());
  
  return 0;
}


int process::get_hazard_data(double begin_time, double valid_time, vector<vdt_probe_message_qc_statistics> &seg_stats, hazard_out *hazard_data)
{
  size_t i;

  double valid_diff = valid_time - begin_time;
  float valid_hour_diff = valid_diff / 3600;
  //printf("valid_hour_diff: %f\n", valid_hour_diff);
  
  // Setup values to degrade the forecast confidence with lead time
  float fcst_conf;
  if(valid_hour_diff > 0 && valid_hour_diff <= 6)
    fcst_conf = 0.90;
  else if(valid_hour_diff > 6 && valid_hour_diff <= 24)
    fcst_conf = 0.80;
  else if(valid_hour_diff > 24 && valid_hour_diff <= 48)
    fcst_conf = 0.60;
  else if(valid_hour_diff > 48 && valid_hour_diff <= 72)
    fcst_conf = 0.40;
  else if(valid_hour_diff > 72 && valid_hour_diff <= 96)
    fcst_conf = 0.20;
  else
    fcst_conf = 0.10;
  
  // Store valid time in hazard data struct
  hazard_data->valid_time = valid_time;
  
  for(i = 0; i < seg_stats.size(); i++)
    {
      Logg->write_time(3, "Info: working on hazard assessment for valid_time: %d, seg_id: %d\n", (int)valid_time, seg_stats[i].id);
      
      hazard_data->seg_id.push_back(seg_stats[i].id);
      hazard_data->num_probe_msg.push_back(seg_stats[i].num_msg);
      
      rwx_road_segment_assessment seg_assmnt;
      seg_assmnt.perform_assessment(seg_stats[i]);
      
      int precip_type = seg_assmnt.get_precip_type();
      string precip_type_str = seg_assmnt.get_precip_type_str(precip_type);
      float precip_type_conf = seg_assmnt.get_precip_type_confidence();
      
      int precip_intensity = seg_assmnt.get_precip_intensity();
      string precip_intensity_str = seg_assmnt.get_precip_intensity_str(precip_intensity);
      float precip_intensity_conf = seg_assmnt.get_precip_intensity_confidence();
      
      int pavement_condition = seg_assmnt.get_pavement_condition();
      string pavement_condition_str = seg_assmnt.get_pavement_condition_str(pavement_condition);
      float pavement_condition_conf = seg_assmnt.get_pavement_condition_confidence();
      
      int visibility = seg_assmnt.get_visibility();
      string visibility_str = seg_assmnt.get_visibility_str(visibility);
      float visibility_conf = seg_assmnt.get_visibility_confidence();
      
      bool pavement_slickness = seg_assmnt.get_pavement_slickness();
      
      // Forecast confidence values are set to 1.0 for all times in the rwx_road_segment_assessment library
      // So we need to degrade the forecast confidence with lead time
      if(valid_hour_diff > 0)
	{
	  Logg->write_time(3, "Info: degrading forecast confidence to: %.2f for lead-time: %.2f\n", fcst_conf, valid_hour_diff);
	  precip_type_conf = fcst_conf;
	  precip_intensity_conf = fcst_conf;
	  pavement_condition_conf = fcst_conf;
	  visibility_conf = fcst_conf;
	}
      
      Logg->write_time(3, "Info: precip_type_str: %s, precip_type_conf: %.2f, precip_intensity_str: %s, precip_intensity_conf: %.2f, pavement_condition_str: %s, pavement_condition_conf: %.2f, visibility_str: %s, visibility_conf: %.2f, pavement_slickness: %d\n", precip_type_str.c_str(), precip_type_conf, precip_intensity_str.c_str(), precip_intensity_conf, pavement_condition_str.c_str(), pavement_condition_conf, visibility_str.c_str(), visibility_conf, pavement_slickness);
      
      // Store the value in the hazard data struct
      //
      hazard_data->precip_type.push_back(precip_type);
      hazard_data->precip_intensity.push_back(precip_intensity);
      hazard_data->pavement_condition.push_back(pavement_condition);
      hazard_data->visibility.push_back(visibility);
      hazard_data->pavement_slickness.push_back(pavement_slickness);
      
      hazard_data->precip_type_conf.push_back(precip_type_conf);
      hazard_data->precip_intensity_conf.push_back(precip_intensity_conf);
      hazard_data->pavement_condition_conf.push_back(pavement_condition_conf);
      hazard_data->visibility_conf.push_back(visibility_conf);
   
    } // end loop over seg_stats
  
  return 0;
}


int process::augment_statistics_with_fcst(double valid_time, site_vars_fcst_data &site_data, const vector<int> &seg_ids, unordered_map<int, int> &seg_site_map, vector<vdt_probe_message_qc_statistics> &seg_stats)
{
  size_t i;
  string input_field;
  float value;
  
  for(i = 0; i < seg_ids.size(); i++)
    {
      // Get the associated forecast site-list index for the give segment-id
      // Skip the site if we dont' have it
      int s;
      if(seg_site_map.find(i) != seg_site_map.end())
	{
	  s = seg_site_map[i];
	}
      else
	{
	  //Logg->write_time("Warning: no forecast site index for seg_ids[%ld]: %d, skipping.\n", i, seg_ids[i]);
	  continue; 
	}
      
      Logg->write_time(3, "Info: i: %d, site_data.site_vars_data[%d]: icao_id: %s\n", i, s, site_data.site_vars_data[s]->icao_id.c_str());
      Logg->write_time(3, "  valid_time: %d\n", (int)valid_time);
      
      vdt_probe_message_qc_statistics this_seg;
      this_seg.id = seg_ids[i];
      this_seg.num_msg = 0;
      
      input_field = "T";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_air_temp = value;
      
      input_field = "wind_speed";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_wind_speed = value;
      
      input_field = "precip_type";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_precip_type = value;
      
      input_field = "precip_rate";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_precip_rate = value;
      
      input_field = "rh";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_rh = value;
      
      input_field = "visibility";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_visibility = value;
      
      // Currently don't have prob_fog in the logicast rdwx file
      //input_field = "prob_fog";
      //value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      //Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      //this_seg.fcst_prob_fog = value;
      
      input_field = "road_T";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_road_temp = value;
      
      input_field = "phase_type";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_road_water_phase_type = value;
      
      // Store seg-stats values for this segment in the seg_stats vector
      seg_stats.push_back(this_seg);
      
    } // end loop over sites
  
  return 0;
}


int process::augment_statistics_with_collection(const rwx_vector_collection &vector_collection, vector<vdt_probe_message_qc_statistics> &seg_stats)
{
  size_t i;
  string input_field;
  string value;
  
  for(i = 0; i < vector_collection.size; i++)
    {
      vdt_probe_message_qc_statistics this_seg;
      
      Logg->write_time(3, "Info: segment: i: %ld\n", i);

      input_field = "id";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.id = atoi(value.c_str());
      
      input_field = "begin_time";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.begin_time = (time_t)(atoi(value.c_str()));
      
      input_field = "end_time";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.end_time = (time_t)(atoi(value.c_str()));
      
      input_field = "total_num_msg";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_msg = atoi(value.c_str());
      
      // Fields for precip_type_hazard

      input_field = "air_temp_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.air_temp_mean = atof(value.c_str());
      
      input_field = "air_temp2_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.air_temp2_mean = atof(value.c_str());
      
      input_field = "nss_air_temp_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.nss_air_temp_mean = atof(value.c_str());
      
      input_field = "model_air_temp";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.model_air_temp = atof(value.c_str());
      
      // Fields for precip_intensity_hazard
      
      input_field = "radar_ref";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.radar_ref = atof(value.c_str());
      
      input_field = "num_wipers_off";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_wipers_off = atoi(value.c_str());
      
      input_field = "num_wipers_intermittent";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_wipers_intermittent = atoi(value.c_str());
      
      input_field = "num_wipers_low";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_wipers_low = atoi(value.c_str());
      
      input_field = "num_wipers_high";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_wipers_high = atoi(value.c_str());

      input_field = "speed_ratio";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.speed_ratio = atof(value.c_str());
      
      // Fields for pavement_condition_hazard and slickness
      
      input_field = "surface_temp_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.surface_temp_mean = atof(value.c_str());
      
      input_field = "num_msg_valid_abs";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_msg_valid_abs = atoi(value.c_str());
      
      input_field = "num_abs_off";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_abs_off = atoi(value.c_str());
      
      input_field = "num_abs_on";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_abs_on = atoi(value.c_str());
      
      input_field = "num_abs_engaged";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_abs_engaged = atoi(value.c_str());
      
      input_field = "num_msg_valid_trac";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_msg_valid_trac = atoi(value.c_str());
      
      input_field = "num_trac_off";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_trac_off = atoi(value.c_str());
      
      input_field = "num_trac_on";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_trac_on = atoi(value.c_str());
      
      input_field = "num_trac_engaged";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_trac_engaged = atoi(value.c_str());
      
      input_field = "num_msg_valid_stab";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_msg_valid_stab = atoi(value.c_str());
      
      input_field = "num_stab_off";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_stab_off = atoi(value.c_str());
      
      input_field = "num_stab_on";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_stab_on = atoi(value.c_str());
      
      input_field = "num_stab_engaged";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_stab_engaged = atoi(value.c_str());
            
      input_field = "hoz_accel_lat_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.lat_accel_mean = atof(value.c_str());
      
      input_field = "hoz_accel_lon_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.lon_accel_mean = atof(value.c_str());
            
      input_field = "steering_angle_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.steering_angle_mean = atof(value.c_str());

      input_field = "yaw_min";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_min = atof(value.c_str());
      
      input_field = "yaw_max";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_max = atof(value.c_str());
      
      input_field = "yaw_median";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_median = atof(value.c_str());
      
      input_field = "yaw_iqr25";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_iqr25 = atof(value.c_str());
      
      input_field = "yaw_iqr75";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_iqr75 = atof(value.c_str());
      
      // Additional fields for visibility
      
      input_field = "dew_temp_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.dew_temp_mean = atof(value.c_str());
      
      input_field = "nss_dew_temp_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.nss_dew_temp_mean = atof(value.c_str());
      
      input_field = "nss_wind_speed_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.nss_wind_speed_mean = atof(value.c_str());
      
      input_field = "nss_prevail_vis_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.nss_prevail_vis_mean = atof(value.c_str());
      
      input_field = "num_msg_valid_lights";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_msg_valid_lights = atoi(value.c_str());
      
      input_field = "num_lights_off";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_lights_off = atoi(value.c_str());
      
      input_field = "num_lights_fog";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_lights_fog = atoi(value.c_str());
      
      input_field = "num_lights_high_beam";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.num_lights_high_beam = atoi(value.c_str());
      
      // Extra fields that are not currently being used
      
      input_field = "radar_precip_flag";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.radar_precip_flag = (short)(atoi(value.c_str()));
      
      input_field = "radar_precip_type";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.radar_precip_type = (short)(atoi(value.c_str()));
      
      // Store data for this segment in the seg_stats vector
      seg_stats.push_back(this_seg);
      
      
    } // end loop over vector collection
  
  return 0;
}


int process::get_fcst_site_indicies(ncfc_io *data, const vector<site *> &sites)
{
  unsigned int i,j;
  size_t s;
  

  // Get variable index
  i = ncfc_utils->get_var_index(data, "site_list");
  
  if(i == data->var_names.size())
    {
      Logg->write_time("Error: site_list variable not found in ncfc_io data\n");
      return(1);
    }
  
  // Create a map of the dicast-id's and their associated indicies
  unordered_map<int, int> id_map;
  for(j=0; j < data->var_sizes[i]; j++)
    {
      int dicast_id = ((int *)data->var_ptrs[i])[j];
      if(dicast_id != NC_FILL_INT && dicast_id != -99999)
	{
	  //printf("dicast_id: %d, ind(j): %d\n", dicast_id, j);
	  id_map[dicast_id] = j;
	}
    }
  
  // Get site-list indicies from the map and store for each file
  for(s = 0; s < sites.size(); s++)
    {
      int site_ind;
      if(id_map.find(sites[s]->dicast_id) != id_map.end())
	site_ind = id_map[sites[s]->dicast_id];
      else
	site_ind = MISSING;
      
      //printf("sites[%ld]->dicast_id: %d, site_ind: %d\n", s, sites[s]->dicast_id, site_ind);  
      data->site_indicies.push_back(site_ind);
    }
  
  return 0;
}


int process::get_vdt_seg_indicies(const rwx_vector_collection &vector_collection, const vector<int> &seg_ids, vector<int> &seg_indicies)
{
  size_t i,s;
  string input_field;
  string value;
  
  // Create a map of id's and their associated indicies
  //
  unordered_map<int, int> id_map;
  
  for(i = 0; i < vector_collection.size; i++)
    {
      input_field = "id";
      value = get_collection_value(vector_collection, input_field, i); 
      //printf("value: %s\n", value.c_str());
      
      if(value != "")
	{
	  int id = atoi(value.c_str());
	  //printf("id: %d, ind(i): %ld\n", id, i);
	  id_map[id] = i;
	}
    } // end loop over vector_collection
  
  // Get segment indicies from the map and store for each file
  for(s = 0; s < seg_ids.size(); s++)
    {
      int site_ind;
      if(id_map.find(seg_ids[s]) != id_map.end())
	site_ind = id_map[seg_ids[s]];
      else
	site_ind = MISSING;
      
      //printf("seg_ids[%ld]: %d, site_ind: %d\n", s, seg_ids[s], site_ind);  
      seg_indicies.push_back(site_ind);
    }
  
  return 0;
}


string process::get_collection_value(const rwx_vector_collection &vector_collection, string field_name, int field_index)
{
  int ret;
  string value = "";
  
  ret = vector_collection.get_value(field_name, field_index, value);
  if (ret != 0)
    Logg->write_time("Warning: vector_collection.get_value failed for field_name: %s\n", field_name.c_str());
 
  return value;
}
