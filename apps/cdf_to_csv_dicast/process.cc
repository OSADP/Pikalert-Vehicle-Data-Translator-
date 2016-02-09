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
#include <log/log.hh>
#include <libconfig.h++>
#include <stdio.h>
#include <stdlib.h>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "vdt4/vdt_road_segment_file_reader.hh"
#include "rwx/rwx_vector_collection.hh"
#include "rwx/rwx_cdf_field_reader.hh"

#include "process.hh"
#include "csv_writer.hh"

using namespace rwx;
using namespace std;
using namespace libconfig;

extern Log *Logg;

process::~process()
{
}

int process::run()
{
  size_t i,j,k;
  int ret;

  string fcst_road_seg_site_list_file = args.fcst_road_seg_site_list_file;
  string vdt_road_seg_file = args.vdt_road_seg_file;

  // Initialize utility object for reading site-lists and fcst files
  stat_utils = new StatUtils(Logg);

  
  // Read configuration file
  Logg->write_time("Info: reading config file %s\n", args.cfg_file.c_str());

  config_reader cfg_reader(args.cfg_file);
  if (cfg_reader.error != string(""))
    {
      Logg->write_time("Error: configuration file error %s\n", cfg_reader.error.c_str());
      error = string("configuration file error") + cfg_reader.error;
      return -1;
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

  
  // Read in the VDT road segment info file
  vector<vdt_road_segment> segs; 
  Logg->write_time("Info: calling vdt_road_segment_file_reader(%s)\n", vdt_road_seg_file.c_str());
  vdt_road_segment_file_reader seg_reader = vdt_road_segment_file_reader(vdt_road_seg_file.c_str());
  ret = seg_reader.get_road_segments(segs);
  if(ret != 0)
    {
      Logg->write_time("Error: unable to read the vdt road segment file: %s\n", vdt_road_seg_file.c_str());
      return(ret);
    }
  //printf("segs.size(): %d\n", (int)segs.size());
  

  // Get the segment ids, aux(rwfs) ids and lat/lon mid-points
  vector<int> seg_ids;
  vector<string> aux_ids;
  vector<vdt_point> road_seg_mid_points;
  
  for(i = 0; i < segs.size(); i++)
    {
      seg_ids.push_back(segs[i].get_id());
      aux_ids.push_back(segs[i].get_aux_id());
      road_seg_mid_points.push_back(segs[i].get_mid_point());
    }  
  // For debugging only
  /*
  for(i = 0; i < road_seg_mid_points.size(); i++)
    {
      printf("i: %ld, seg_id: %d, aux_id: %s, lat: %f, lon: %f\n", i, seg_ids[i], aux_ids[i].c_str(), road_seg_mid_points[i].latitude, road_seg_mid_points[i].longitude);
    }
  */

  
  // Create a map between the vdt segment-list "seg-id" and the forecast site list index
  // using the vdt "aux-id" and the site-list "icao-id" for comparing
  unordered_map<int, int> seg_site_map;
  for(i = 0; i < aux_ids.size(); i++)
    {
      int found = 0;
      for(j = 0; j < sites.size(); j++)
        {
          if( strcmp(aux_ids[i].c_str(), sites[j]->icao_id) == 0 )
            {
              //printf("seg_ids[%d]: %d aux_ids[%d]: %s, sites[%d]->icao_id: %s\n", i, seg_ids[i], i, aux_ids[i].c_str(), j, sites[j]->icao_id);
              found = 1;
              seg_site_map[seg_ids[i]] = j;
              break;
            }
        } // end loop over sites
    } // end loop over segments: aux_ids
  
  // For debugging only
  /*
  for(i = 0; i < seg_ids.size(); i++)
    {
      if(seg_site_map.find(seg_ids[i]) != seg_site_map.end())
        printf("seg_site_map[%d]: %d\n", seg_ids[i], seg_site_map[seg_ids[i]]);
    }
  */
  
  
  Logg->write_time_info("Info: reading input file %s\n", args.input_file.c_str());

  // Open input cdf file
  rwx_cdf_field_reader::rwx_cdf_field_reader cdf_file(args.input_file);

  // Read double fields
  vector<string> double_field_names;
  vector< vector<double> > double_field_vector;
  vector<string> double_units;
  vector<double> double_missing;
  string error;

  Logg->write_time("Info: getting double fields\n");
  ret = cdf_file.get_fields(cfg_reader.double_field_names, double_field_vector, double_units, double_missing, error);
  
  vector<string> float_field_names;
  vector< vector<float> > float_field_vector;
  vector<string> float_units;
  vector<float> float_missing;

  Logg->write_time("Info: getting float fields\n");
  ret = cdf_file.get_fields(cfg_reader.float_field_names, float_field_vector, float_units, float_missing, error);

  vector<string> int_field_names;
  vector< vector<int> > int_field_vector;
  vector<string> int_units;
  vector<int> int_missing;

  Logg->write_time("Info: getting int fields\n");
  ret = cdf_file.get_fields(cfg_reader.int_field_names, int_field_vector, int_units, int_missing, error);

  vector<string> short_field_names;
  vector< vector<short> > short_field_vector;
  vector<string> short_units;
  vector<short> short_missing;

  Logg->write_time("Info: getting short fields\n");
  ret = cdf_file.get_fields(cfg_reader.short_field_names, short_field_vector, short_units, short_missing, error);

  vector<string> char_field_names;
  vector< vector<char> > char_field_vector;
  vector<size_t> char_field_size;

  Logg->write_time("Info: getting character fields\n");
  ret = cdf_file.get_fields(cfg_reader.char_field_names, char_field_vector, char_field_size, error);

  Logg->write_time("Info: formulating collection\n");
  rwx_vector_collection collection(cfg_reader.input_type_index_map, char_field_vector, char_field_size, short_field_vector, int_field_vector, float_field_vector, double_field_vector);
  if (collection.error != "")
    {
      Logg->write_time("Error: collection error occurred\n");
      return 1;
    }


  // Create the csv file
  //
  Logg->write_time("Info: outputting csv file\n");
  csv_writer(args.output_file, sites, seg_site_map, cfg_reader.input_output_csv, collection);

  return 0;
}

