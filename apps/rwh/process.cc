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

#include <fstream>
#include <log/log.hh>
#include <libconfig.h++>
#include <stdio.h>
#include <stdlib.h>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "vdt4/vdt_road_segment_file_reader.hh"
#include "rwx/vdt_probe_message_qc_statistics.hh"

#include "rwx/rwx_vector_collection.hh"
#include "rwx/rwx_cdf_field_reader.hh"
#include "rwx/rwx_cdf_field_writer.hh"
#include "rwx/rwx_road_segment_assessment.hh"

#include "process.hh"
#include "rwh_record.hh"
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
  size_t i, j, s, f, v, t;
  
  time_t begin_time = args.begin_time;
  string begin_time_string = args.begin_time_string;
  time_t end_time = args.end_time;
  string end_time_string = args.end_time_string;
  string fcst_road_seg_site_list_file = args.fcst_road_seg_site_list_file;
  string vdt_road_seg_file = args.vdt_road_seg_file;
  string output_file = args.output_file;

  double d_begin_time = (double)begin_time;
  
  Logg->write_time("Info: generating road weather hazard data from %s to %s\n", begin_time_string.c_str(), end_time_string.c_str());
  //printf("d_begin_time: %d\n", (int)d_begin_time);
  
  // Initialize utility object for reading site-lists and fcst files
  stat_utils = new StatUtils(Logg);
  ncfc_utils = new NcfcUtils(Logg);
  
  // Read configuration file
  Logg->write_time("Info: reading config file %s\n", args.cfg_file.c_str());
  config_reader cfg_reader(args.cfg_file);
  if (cfg_reader.error != string(""))
    {
      Logg->write_time("Error: configuration file error %s\n", cfg_reader.error.c_str());
      error = string("configuration file error ") + cfg_reader.error;
      return(1);
    }
  
  // Get the site information from the site_list file
  vector <site *> sites;
  Logg->write_time("Info. getting site list info from %s.\n", (char *)(fcst_road_seg_site_list_file).c_str());
  ret = stat_utils->get_site_info((char *)(fcst_road_seg_site_list_file).c_str(), &sites);
  if (ret != 0)
    {
      Logg->write_time("Error: in get_site_info for %s.\n", (char *)(fcst_road_seg_site_list_file).c_str());
      return(ret);
    }
  
  // Read the VDT road segment file (create vdt_road_segment_file_reader object)
  vector<vdt_road_segment> segs; 
  Logg->write_time("Info: calling vdt_road_segment_file_reader(%s)\n", vdt_road_seg_file.c_str());

  vdt_road_segment_file_reader seg_reader = vdt_road_segment_file_reader(vdt_road_seg_file.c_str());
  
  // Get the road-segments
  segs = seg_reader.get_road_segments();
  // ret = seg_reader.get_road_segments(segs);
  // if(ret != 0)
  //   {
  //     Logg->write_time("Error: get_road_segments failed for file: %s\n", vdt_road_seg_file.c_str());
  //     return(ret);
  //   }
  //printf("segs.size(): %ld\n", segs.size());
  
  // Get the rwis sites
  const std::vector<vdt_road_segment>& rwis_segs = seg_reader.get_rwis_sites();
  std::copy(rwis_segs.begin(), rwis_segs.end(), std::back_inserter(segs));

  // ret = seg_reader.get_rwis_sites(segs);
  // if(ret != 0)
  //   {
  //     Logg->write_time("Error: get_rwis_sites failed for file: %s\n", vdt_road_seg_file.c_str());
  //     return(ret);
  //   }
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
  
  
  // For debugging only
  /*
  for(i = 0; i < seg_ids.size(); i++)
    {
      if(seg_site_map.find(i) != seg_site_map.end())
	printf("seg_ids[%ld]: %d, seg_site_map[%ld]: %d\n", i, seg_ids[i], i, seg_site_map[i]);
    }
  */

  // Read in previouis rwh config file (contains a list of previous rwh files)
  // Store data in a vector of structs.
  //
  vector<prev_rwh *> prev_rwh_data;
  prev_rwh_data.clear();
  
  if(args.prev_rwh_conf_file != "")
    {
      string line;
      vector<string> lines;

      Logg->write_time("Info: reading prev_rwh_conf_file %s\n", args.prev_rwh_conf_file.c_str());
      
      // Open and read the prev_rwh_conf_file. Ignore comment/blank lines
      ifstream pf(args.prev_rwh_conf_file.c_str());
      if (!pf)
	{
	  Logg->write_time("Error: Could not open %s.\n", args.prev_rwh_conf_file.c_str());
	  return 1;
	}
      
      lines.clear();
      while(!getline(pf, line).eof())
	if (line[0] != '#' && line.size() > 0)
	  lines.push_back(line);
      
      pf.close();
      Logg->write_time(1, "Info: read %d lines from prev_rwh_conf_file %s\n", lines.size(), args.prev_rwh_conf_file.c_str());

      for(i = 0; i < lines.size(); i++)
	{
	  Logg->write_time(2, "Info reading rwh input file %s\n", lines[i].c_str());
	  rwx_vector_collection_nc rwh_collection(lines[i], cfg_reader.rwh_field_names);
	  if (rwh_collection.error != string(""))
	    {
              Logg->write_time("Error: ingest error for %s: %s\n", lines[i].c_str(), rwh_collection.error.c_str());
	      return -1;
	    }

	  prev_rwh *this_prev_rwh_data = new prev_rwh;
	  this_prev_rwh_data->rwh_record_map.clear();
	  
	  int road_segment_na = (int)MISSING;
	  string error;
	  Logg->write_time(2, "Info: creating rwh record map: mapping site indices, to rwh record struct\n");
	  ret = create_rwh_record_map(road_segment_na, rwh_collection, this_prev_rwh_data->rwh_record_map, error);
	  if (ret != 0)
            {
	      Logg->write_time("Error: failed creating rwh record map: %s\n", error.c_str());
	      return -1;
	    }

	  Logg->write_time(2, "Info: getting all observation times (time_nominal) from the rwh file.\n");
	  string rwh_time_field_name = "time_nominal";
	  rwh_collection.get_vector(rwh_time_field_name, &this_prev_rwh_data->rwh_time_field);
	  time_t obs_time = (*this_prev_rwh_data->rwh_time_field)[0];
	  //Logg->write_time(2, "Info: first observation time %ld\n", obs_time);
	  
	  // Store this files data in a vector of structs
	  prev_rwh_data.push_back(this_prev_rwh_data);
	  
	} // end loop over lines
    }

  // Determine latest, meaningful precip-type, intensity and time from the previous rwh files
  vector<rwh_existing_hazards *> rwh_eh;
  rwh_eh.clear();
  if(prev_rwh_data.size() > 0)
    {
      Logg->write_time(1, "Info. determining existing hazards from previous rwh files\n");
      ret = determine_prev_rwh_hazard(d_begin_time, seg_ids, prev_rwh_data, rwh_eh);
      if(ret != 0)
	{
	  Logg->write_time("Error: in determine_prev_rwh_hazards\n");
	  return 1;
	} 
    }
  
  // Read in vdt segment statistics file if we have it
  // and store data in vector of vdt_prob_message_qc_statistics objects
  //
  vector<vdt_probe_message_qc_statistics> seg_stats;
  unordered_map<int, int> seg_coll_ind_map;
  if(args.vdt_seg_stats_file != "")
    {      
      Logg->write_time("Info: reading vdt_seg_stats_file %s\n", args.vdt_seg_stats_file.c_str());
      
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

      Logg->write_time(1, "Info: getting double fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.double_field_names, double_field_vector, double_units, double_missing, error);

      vector<string> float_field_names;
      vector< vector<float> > float_field_vector;
      vector<string> float_units;
      vector<float> float_missing;

      Logg->write_time(1, "Info: getting float fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.float_field_names, float_field_vector, float_units, float_missing, error);

      vector<string> int_field_names;
      vector< vector<int> > int_field_vector;
      vector<string> int_units;
      vector<int> int_missing;

      Logg->write_time(1, "Info: getting int fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.int_field_names, int_field_vector, int_units, int_missing, error);

      vector<string> short_field_names;
      vector< vector<short> > short_field_vector;
      vector<string> short_units;
      vector<short> short_missing;

      Logg->write_time(1, "Info: getting short fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.short_field_names, short_field_vector, short_units, short_missing, error);

      vector<string> char_field_names;
      vector< vector<char> > char_field_vector;
      vector<size_t> char_field_size;
      
      Logg->write_time(1, "Info: getting character fields\n");
      ret = seg_stats_cdf_file.get_fields(cfg_reader.char_field_names, char_field_vector, char_field_size, error);

      Logg->write_time("Info: formulating vector collection for vdt_seg_stats_file\n");
      rwx_vector_collection collection(cfg_reader.input_type_index_map, char_field_vector, char_field_size, short_field_vector, int_field_vector, float_field_vector, double_field_vector);
      if (collection.error != "")
	{
	  Logg->write_time("Error: collection error occurred: %s\n", collection.error.c_str());
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
      Logg->write_time("Info. augmenting probe-message-qc-stats with vdt segment collection data\n");
      ret = augment_statistics_with_collection(collection, rwh_eh, seg_stats);
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
	  double begin_diff = d_begin_time - valid_time;
	  float begin_hour_diff = begin_diff / 3600;
	  //printf("d_begin_time: %d, valid_time: %d, begin_hour_diff: %f\n", (int)d_begin_time, (int)valid_time, begin_hour_diff);
	  
	  // Get hazard data only if the difference between the begin-time and the vdt valid-time
	  // is basically positive (vdt data not in future) and within 1 hour
	  // allow 10 minute buffer for end-time into future
	  if(begin_hour_diff > -0.2667 && begin_hour_diff <= 1)
	    {
	      hazard_out *this_hazard_data = new hazard_out;
	      Logg->write_time("Info. getting vdt hazard data\n");
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
      Logg->write_time("Info: getting road_wx_fcst data from %s.\n", args.road_wx_fcst_file.c_str());
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
      Logg->write_time("Info: getting road_cond_fcst data from %s.\n", args.road_cond_fcst_file.c_str());
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
  Logg->write_time("Info. getting site fcst data.\n");
  ret = site_fcst.get_fcst_data(begin_time, sites, fcst_field_names, all_fcst_data, max_days, max_lead_time);
  if(ret != 0){
    Logg->write_time("Error: in get_fcst_data.\n");
    return(ret);
  }
  
  // Loop over forecast lead-times. For each lead-time (valid-time) loop over
  // all sites (segments) and augment vdt_probe_message_qc_statistic object with site-fcst data.
  // Then get hazard data for all segments 
  //
  double valid_time;
  valid_time = rnd_begin_time;
  
  // Create vector of forecast hazard data for all lead-times
  vector<hazard_out *> fcst_hazard_data;
  fcst_hazard_data.clear();
  
  if(fcst_data_flag)
    {     
      Logg->write_time("Info. augmenting probe-message-qc-stats with forecast data and getting forecast hazard data\n", (int)valid_time);
      while(valid_time <= rnd_end_time)
	{
	  vector<vdt_probe_message_qc_statistics> fcst_seg_stats;
	  fcst_seg_stats.clear();
	  Logg->write_time(1, "Info. augmenting probe-message-qc-stats with forecast data for valid_time: %d.\n", (int)valid_time);
	  ret = augment_statistics_with_fcst(valid_time, site_fcst, seg_ids, seg_site_map, rwh_eh, fcst_seg_stats);
	  if(ret != 0)
	    {
	      Logg->write_time("Error: in augment_statistics_with_fcst\n");
	      return 1;
	    } 
      
	  // Get the hazard data for all segments for the given lead-time
	  if(fcst_seg_stats.size() > 0)
	    {
	      hazard_out *this_hazard_data = new hazard_out;
	      Logg->write_time(1, "Info. getting forecast hazard data for valid_time: %d\n", (int)valid_time);
	      ret = get_hazard_data(d_begin_time, valid_time, fcst_seg_stats, this_hazard_data);
	      if(ret != 0)
		{
		  Logg->write_time("Warning: get_hazard_data failed for forecast seg-stats data valid_time: %d\n", (int)valid_time);
		  valid_time = valid_time + 3600;
		  continue;
		}

	      // For debugging only
	      /*
	      for(i = 0; i < this_hazard_data->seg_id.size(); i++)
		printf("this_hazard_data->seg_id[%ld]->seg_id: %d\n", i, this_hazard_data->seg_id[i]);
	      
	      printf("end this file.\n" );
	      */
	      
	      fcst_hazard_data.push_back(this_hazard_data);
	    }
      
	  valid_time = valid_time + 3600;

	} // end while loop over valid_times
    }
  
  // Write all the hazard data to the output file
  Logg->write_time("Info. writng hazard data to output file\n");
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

  prev_rwh_data.clear();
  rwh_eh.clear();
  
  seg_stats.clear();
  vdt_hazard_data.clear();

  ncfc_utils->clean_ncfc_io(road_wx_fcst_data);
  ncfc_utils->clean_ncfc_io(road_cond_fcst_data);
  
  all_fcst_data.clear();
  fcst_field_names.clear();
  
  fcst_hazard_data.clear();

  return 0;
}


int process::determine_prev_rwh_hazard(double begin_time, const vector<int> &seg_ids, const vector<prev_rwh *> &prev_rwh_data, vector<rwh_existing_hazards *> &rwh_eh)
{
  int f; // have to use a signed integer for looping backwards over the files
  size_t s, t;
  
  // For debugging only
  //
  for(s = 0; s < seg_ids.size(); s++)
    {
      rwh_existing_hazards *this_rwh_eh = new rwh_existing_hazards;
      this_rwh_eh->id = seg_ids[s];
      this_rwh_eh->prev_precip_type = 0;
      this_rwh_eh->prev_precip_intensity = 0;
      this_rwh_eh->prev_precip_time = MISSING;

      int prev_precip_intensity = 0;
      int prev_precip_type = 0;
      double prev_precip_time = MISSING;
      
      // Loop over files (start from latest file to oldest) to look for most recent hazards
      // This assumes the latest rwh file is listed last in the prev_rwh_conf_file
      for(f = prev_rwh_data.size()-1; f >= 0; f--)
	{
	  if( prev_rwh_data[f]->rwh_record_map.find(seg_ids[s]) != prev_rwh_data[f]->rwh_record_map.end() )
	    {
	      //printf("  f: %d, s: %ld, prev_rwh_data[%d]->rwh_record_map[%d].site_num: %d\n", f, s, f, seg_ids[s], prev_rwh_data[f]->rwh_record_map[seg_ids[s]].site_num);
	      
	      for(t = 0; t < 1; t++) // only need the first time (vdt-time) from each rwh file. 
		{
		  //printf("    rwh_record_map[]: info_vec[%ld]: time_nominal: %d, precip_type: %d, precip_intensity: %d\n", t, (int)prev_rwh_data[f]->rwh_record_map[seg_ids[s]].info_vec[t].time_nominal, prev_rwh_data[f]->rwh_record_map[seg_ids[s]].info_vec[t].precip_type, prev_rwh_data[f]->rwh_record_map[seg_ids[s]].info_vec[t].precip_intensity);	  
		  
		  // Look for latest, non-zero precip-intensity
		  if(prev_rwh_data[f]->rwh_record_map[seg_ids[s]].info_vec[t].precip_intensity != NC_FILL_INT && prev_rwh_data[f]->rwh_record_map[seg_ids[s]].info_vec[t].precip_intensity > 0)
		    {
		      //printf("   SETTING previous precip-intensity and precip-type\n");
		      prev_precip_intensity = prev_rwh_data[f]->rwh_record_map[seg_ids[s]].info_vec[t].precip_intensity;
		      prev_precip_type = prev_rwh_data[f]->rwh_record_map[seg_ids[s]].info_vec[t].precip_type;
		      prev_precip_time = prev_rwh_data[f]->rwh_record_map[seg_ids[s]].info_vec[t].time_nominal;
		    }
		  
		} // end loop over times
	    }
	  else
	    {
	      Logg->write_time(3, "Warning: seg_ids[%ld]: %d, not found in file f: %d\n", s, seg_ids[s], f);
	    }

	  if(prev_precip_intensity != 0)
	    break;
	  
	} // end while loop over files
      
      Logg->write_time(3, "Info: for seg_ids[%ld]: %d, prev_precip_intensity: %d, prev_precip_type: %d, prev_precip_time: %d\n", s, seg_ids[s], prev_precip_intensity, prev_precip_type, (int)prev_precip_time);

      // Stove values in struct
      this_rwh_eh->prev_precip_type = prev_precip_type;
      this_rwh_eh->prev_precip_intensity = prev_precip_intensity;
      this_rwh_eh->prev_precip_time = prev_precip_time;
      
      // Push to vector
      rwh_eh.push_back(this_rwh_eh);
      
    } // end loop over sites (seg-ids)
    
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

      // Initialize site index in the fcst_hazard_data[i] vectors of data
      // Those vectors just contain data for the forecast sites (based on command line site-list)
      // The forecast sites could be a subset of all the vdt segments
      int s = 0;
      
      // Loop over all segments, get data for sites that we have data for (based on command line site-list
      //
      for(j = 0; j < seg_ids.size(); j++)
	{
	  // Only output forecast data that is past the last vdt valid time
	  if(seg_site_map.find(j) != seg_site_map.end() && fcst_hazard_data[i]->valid_time > last_vdt_valid_time)
	    {
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
	      s++; // iterate through the sites we have data for
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


int process::augment_statistics_with_fcst(double valid_time, site_vars_fcst_data &site_data, const vector<int> &seg_ids, unordered_map<int, int> &seg_site_map, const vector<rwh_existing_hazards *> &rwh_eh, vector<vdt_probe_message_qc_statistics> &seg_stats)
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
      Logg->write_time(3, "  seg_id: %d\n", this_seg.id);
      
      input_field = "T";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_air_temp = value;
      
      input_field = "dewpt";
      value = site_data.get_value(valid_time, input_field, site_data.site_vars_data[s]);
      Logg->write_time(3, "  input_field: %s, value: %.2f\n", input_field.c_str(), value);
      this_seg.fcst_dew_temp = value;
      
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

      // Fields that are set based on previous rwh files diagnosis
      //
      if(rwh_eh.size() > 0)
	{
	  if(this_seg.id == rwh_eh[i]->id)
	    {
	      double prev_precip_time_diff;
	      if(rwh_eh[i]->prev_precip_time != MISSING)
		prev_precip_time_diff = valid_time - rwh_eh[i]->prev_precip_time;
	      else
		prev_precip_time_diff = MISSING;

	      if(prev_precip_time_diff < 0)
		prev_precip_time_diff = MISSING;
	      
	      this_seg.prev_precip_type = rwh_eh[i]->prev_precip_type;
	      this_seg.prev_precip_intensity = rwh_eh[i]->prev_precip_intensity;
	      this_seg.prev_precip_time_gap = prev_precip_time_diff;
	      Logg->write_time(3, "  this_seg.prev_precip_type: %d, this_seg.prev_precip_intensity: %d, this_seg.prev_precip_time_gap: %d\n", this_seg.prev_precip_type, this_seg.prev_precip_intensity, (int)this_seg.prev_precip_time_gap);
	    }
	  else
	    {
	      this_seg.prev_precip_type = 0;
	      this_seg.prev_precip_intensity = 0;
	      this_seg.prev_precip_time_gap = MISSING;
	      Logg->write_time(3, "  Warning: in augment-stats-with-fcst: this_seg.id: %d DOES NOT MATCH rwh_eh[i]->id: %d setting previous rwh fields to missing.\n", this_seg.id, rwh_eh[i]->id);
	      Logg->write_time(3, "  this_seg.prev_precip_type: %d, this_seg.prev_precip_intensity: %d, this_seg.prev_precip_time_gap: %d\n", this_seg.prev_precip_type, this_seg.prev_precip_intensity, (int)this_seg.prev_precip_time_gap);
	    }
	}
      else // rwh_eh is empty
	{
	  this_seg.prev_precip_type = 0;
	  this_seg.prev_precip_intensity = 0;
	  this_seg.prev_precip_time_gap = MISSING;
	  Logg->write_time(3, "  Inof: in augment-stats-with-fcst: rwh_eh is empty, setting previous rwh fields to missing.\n");
	  Logg->write_time(3, "  this_seg.prev_precip_type: %d, this_seg.prev_precip_intensity: %d, this_seg.prev_precip_time_gap: %d\n", this_seg.prev_precip_type, this_seg.prev_precip_intensity, (int)this_seg.prev_precip_time_gap);
	}
      
      // Store seg-stats values for this segment in the seg_stats vector
      seg_stats.push_back(this_seg);
      
    } // end loop over sites
  
  return 0;
}


int process::augment_statistics_with_collection(const rwx_vector_collection &vector_collection, const vector<rwh_existing_hazards *> &rwh_eh, vector<vdt_probe_message_qc_statistics> &seg_stats)
{
  size_t i;
  string input_field;
  string value;
  int ret = 0;
  
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
      
      input_field = "radar_cref";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.radar_cref = atof(value.c_str());
      
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
      
      input_field = "nss_surface_temp_mean";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.nss_surface_temp_mean = atof(value.c_str());
      
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

      input_field = "yaw_rate_min";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_min = atof(value.c_str());
      
      input_field = "yaw_rate_max";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_max = atof(value.c_str());
      
      input_field = "yaw_rate_median";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_median = atof(value.c_str());
      
      input_field = "yaw_rate_iqr25";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_iqr25 = atof(value.c_str());
      
      input_field = "yaw_rate_iqr75";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      this_seg.yaw_iqr75 = atof(value.c_str());

      // Wx indication from RWIS or other non-metar stations
      input_field = "pres_wx";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      
      // Decode the RWIS wx indication
      int pres_wx_precip_type;
      int pres_wx_precip_intensity;
      int pres_wx_visibility;
      ret = decode_station_wx(value, pres_wx_precip_type, pres_wx_precip_intensity, pres_wx_visibility);
      if(ret != 0)
	{
	  Logg->write_time("Error: in decode_station_wx() for value: %s\n", value.c_str());
	  pres_wx_precip_type = MISSING;
	  pres_wx_precip_intensity = MISSING;
	  pres_wx_visibility =  MISSING;
	}
      
      // Wx indication from METAR
      input_field = "wx";
      value = get_collection_value(vector_collection, input_field, i);
      Logg->write_time(3, "  input_field: %s, value: %s\n", input_field.c_str(), value.c_str());
      
      // Decode the METAR wx indication
      int wx_precip_type;
      int wx_precip_intensity;
      int wx_visibility;
      ret = decode_station_wx(value, wx_precip_type, wx_precip_intensity, wx_visibility);
      if(ret != 0)
	{
	  Logg->write_time("Error: in decode_station_wx() for value: %s\n", value.c_str());
	  wx_precip_type = MISSING;
	  wx_precip_intensity = MISSING;
	  wx_visibility =  MISSING;
	}
      
      // Use the METAR wx indication if we have it, if not use RWIS wx indication
      int use_metar_flag = 0;
      if(wx_precip_type != MISSING || wx_precip_intensity != MISSING || wx_visibility != MISSING)  
	use_metar_flag = 1;
      
      if(use_metar_flag)
	{
	  Logg->write_time(3, "  using METAR wx report: wx_precip_type: %d, wx_precip_intensity: %d, wx_visibility: %d\n", wx_precip_type, wx_precip_intensity, wx_visibility);
	  this_seg.wx_precip_type = wx_precip_type;
	  this_seg.wx_precip_intensity = wx_precip_intensity;
	  this_seg.wx_visibility = wx_visibility;
	}
      else
	{
	  Logg->write_time(3, "  using RWIS pres-wx report: pres_wx_precip_type: %d, pres_wx_precip_intensity: %d, pres_wx_visibility: %d\n", pres_wx_precip_type, pres_wx_precip_intensity, pres_wx_visibility);
	  this_seg.wx_precip_type = pres_wx_precip_type;
	  this_seg.wx_precip_intensity = pres_wx_precip_intensity;
	  this_seg.wx_visibility = pres_wx_visibility;
	}
      
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

      // Fields that are set based on previous rwh files diagnosis
      //
      //printf("in augment-stats-with-coll: rwh_eh.size(): %ld\n", rwh_eh.size());
      if(rwh_eh.size() > 0)
	{
	  if(this_seg.id == rwh_eh[i]->id)
	    {
	      double prev_precip_time_diff;
	      if(rwh_eh[i]->prev_precip_time != MISSING)
		prev_precip_time_diff = (double)this_seg.end_time - rwh_eh[i]->prev_precip_time;
	      else
		prev_precip_time_diff = MISSING;
	      
	      if(prev_precip_time_diff < 0)
		prev_precip_time_diff = MISSING;
	      
	      this_seg.prev_precip_type = rwh_eh[i]->prev_precip_type;
	      this_seg.prev_precip_intensity = rwh_eh[i]->prev_precip_intensity;
	      this_seg.prev_precip_time_gap = prev_precip_time_diff;
	      Logg->write_time(3, "  this_seg.prev_precip_type: %d, this_seg.prev_precip_intensity: %d, this_seg.prev_precip_time_gap: %d\n", this_seg.prev_precip_type, this_seg.prev_precip_intensity, (int)this_seg.prev_precip_time_gap);
	    }
	  else
	    {
	      this_seg.prev_precip_type = 0;
	      this_seg.prev_precip_intensity = 0;
	      this_seg.prev_precip_time_gap = MISSING;
	      Logg->write_time(3, "  Warning: in augment-stats-with-coll: this_seg.id: %d DOES NOT MATCH rwh_eh[i]->id: %d, setting previous rwh fields to missing.\n", this_seg.id, rwh_eh[i]->id);
	      Logg->write_time(3, "  this_seg.prev_precip_type: %d, this_seg.prev_precip_intensity: %d, this_seg.prev_precip_time_gap: %d\n", this_seg.prev_precip_type, this_seg.prev_precip_intensity, (int)this_seg.prev_precip_time_gap);
	    }
	}
      else // rwh_eh is empty
	{
	  this_seg.prev_precip_type = 0;
	  this_seg.prev_precip_intensity = 0;
	  this_seg.prev_precip_time_gap = MISSING;
	  Logg->write_time(3, "  Info: in augment-stats-with-coll: rwh_eh is empty, setting previous rwh fields to missing.\n");
	  Logg->write_time(3, "  this_seg.prev_precip_type: %d, this_seg.prev_precip_intensity: %d, this_seg.prev_precip_time_gap: %d\n", this_seg.prev_precip_type, this_seg.prev_precip_intensity, (int)this_seg.prev_precip_time_gap);
	}
      
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


int process::decode_station_wx(string wx_str, int &wx_precip_type, int &wx_precip_intensity, int &wx_visibility)
{
  
  if(wx_str == "")
    {
      wx_precip_type = MISSING;
      wx_precip_intensity = MISSING;
      wx_visibility = MISSING;
    }
  else
    {
      //cout << "wx_str[0] = " << wx_str[0] << endl;

      // Look for precip-intensity indication first
      // Note, that (-) is light precip (+) is heavy precip, no indication is moderate precip
      //
      size_t found_heavy = wx_str.find_first_of("+");
      size_t found_light = wx_str.find_first_of("-");
      if(found_heavy !=string::npos)
	{
	  //printf("found heavy\n");
	  wx_precip_intensity = rwx_road_segment_assessment::HEAVY_PRECIP;
	}
      else if(found_light !=string::npos)
	{
	  //printf("found light\n");
	   wx_precip_intensity = rwx_road_segment_assessment::LIGHT_PRECIP;
	}
      else
	{
	  //printf("found moderate\n");
	  wx_precip_intensity = rwx_road_segment_assessment::MODERATE_PRECIP;
	}

      // Snow categories
      size_t found_sn = wx_str.find("SN");
      size_t found_sg = wx_str.find("SG");

      // Ice categories
      size_t found_ic = wx_str.find("IC");
      size_t found_pl = wx_str.find("PL");

      // Rain categories
      size_t found_ra = wx_str.find("RA");
      size_t found_dz = wx_str.find("DZ");

      // Unknown category (usually a mix of rain/snow)
      size_t found_up = wx_str.find("UP");

      // Determine precip-type
      // Note for Rain and Snow the intensity is usually indicated above (with - or +)
      // For some of the other categories, intensity is not given, so we just set the intensity to light
      if(found_sn !=string::npos)
	{
	  //printf("found snow\n");
	  wx_precip_type = rwx_road_segment_assessment::SNOW;
	}
      else if(found_sg !=string::npos)
	{
	  //printf("found snow grains\n");
	  wx_precip_type = rwx_road_segment_assessment::SNOW;
	  wx_precip_intensity = rwx_road_segment_assessment::LIGHT_PRECIP;
	}
      else if(found_ic !=string::npos || found_pl !=string::npos || found_up !=string::npos)
	{
	  //printf("found ice, ice-pellets or unknown-precip\n");
	  wx_precip_type = rwx_road_segment_assessment::MIX;
	  wx_precip_intensity = rwx_road_segment_assessment::LIGHT_PRECIP;
	}
      else if(found_ra !=string::npos)
	{
	  //printf("found rain\n");
	  wx_precip_type = rwx_road_segment_assessment::RAIN;
	}
      else if(found_dz !=string::npos)
	{
	  //printf("found drizzle\n");
	  wx_precip_type = rwx_road_segment_assessment::RAIN;
	  wx_precip_intensity = rwx_road_segment_assessment::LIGHT_PRECIP;
	}
      else
	{
	  wx_precip_type = MISSING;
	  wx_precip_intensity = MISSING;
	}
      
      // Now look for Freezing (FZ) descriptor which sometimes indicates
      // freezing rain, so will set the precip-type to MIX
      // If intensity is not set, set it to LIGHT
      size_t found_fz = wx_str.find("FZ");
      if(found_fz !=string::npos)
	{
	  //printf("found freezing precip\n");
	  wx_precip_type = rwx_road_segment_assessment::MIX;
	  if(wx_precip_intensity == MISSING)
	    wx_precip_intensity = rwx_road_segment_assessment::LIGHT_PRECIP;
	}
      
      // Now look for visibility (obscuration) indications for fog, haze, dust and smoke
      //
      size_t found_fg = wx_str.find("FG");
      size_t found_hz = wx_str.find("HZ");
      size_t found_du = wx_str.find("DU");
      size_t found_fu = wx_str.find("FU");

      //cout << "found_fg = " << found_fg << endl;
      //cout << "found_hz = " << found_hz << endl;
      //cout << "found_du = " << found_du << endl;
      //cout << "found_fu = " << found_fu << endl;
      
      if(found_fg !=string::npos)
	{
	  //printf("found fog\n");
	  wx_visibility = rwx_road_segment_assessment::VIS_FOG;
	}
      else if(found_hz !=string::npos)
	{
	  //printf("found haze\n");
	  wx_visibility = rwx_road_segment_assessment::VIS_HAZE; 
	}
      else if(found_du !=string::npos)
	{
	  //printf("found dust\n");
	  wx_visibility = rwx_road_segment_assessment::VIS_DUST; 
	}
      else if(found_fu !=string::npos)
	{
	  //printf("found smoke\n");
	  wx_visibility = rwx_road_segment_assessment::VIS_SMOKE; 
	}
      else
	{
	  wx_visibility = MISSING;
	}
    }
  
  return 0;
}


