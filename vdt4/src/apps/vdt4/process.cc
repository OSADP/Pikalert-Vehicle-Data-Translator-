//==============================================================================
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
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
#include <fstream>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "process.hh"
#include "vdt_probe_data_qc.hh"
#include "vdt4/vdt_climatology_file_reader.hh"
#include "vdt4/vdt_grid.hh"
#include "vdt4/vdt_history_check.hh"
#include "vdt4/vdt_nearest_nbr.hh"
#include "vdt4/vdt_probe_message_history.hh"
#include "vdt4/vdt_probe_message_file_reader.hh"
#include "vdt4/vdt_probe_message_qc_file_reader.hh"
#include "vdt4/vdt_probe_message_qc_statistics.hh"
#include "vdt4/vdt_probe_message_qc_file_writer.hh"
#include "vdt4/vdt_probe_message_qc_statistics_file_writer.hh"
#include "vdt4/vdt_road_segment.hh"
#include "vdt4/vdt_road_segment_file_reader.hh"
#include "vdt_probe_message_datasets.hh"
#include "vdt_probe_message_datasets_test.hh"
#include "vdt_test_generator.hh"

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
  time_t begin_time = args.begin_time;
  string begin_time_string = args.begin_time_string;
  time_t end_time = args.end_time;
  string end_time_string = args.end_time_string;

  // Read configuration file
  Logg->write_time("Info: reading config file %s\n", args.cfg_file.c_str());

  config_reader cfg_reader(args.cfg_file);
  if (cfg_reader.error != string(""))
    {
      Logg->write_time("Error: configuration file error %s\n", cfg_reader.error.c_str());
      error = str(format("configuration file error %1%") % cfg_reader.error);
      return -1;
    }

  vdt_climatology_file_reader *climate_data = 0;
  vdt_probe_message_datasets *ds = 0;
  vector<vdt_probe_message_qc> history_msgs;
  vector<vdt_probe_message> msgs;
  vector<vdt_probe_message_qc> qc_msgs;
  vector<vdt_probe_message> raw_msgs;
  vector<vdt_road_segment> segs;
  vector<vdt_road_segment> rwis_sites;
  vector<int> seg_ids;
  vector<int> rwis_site_ids;
 

  Logg->write_time_info("calling vdt_climatology_file_reader on climatology file %s\n", cfg_reader.climatology_file.c_str());
  climate_data = new vdt_climatology_file_reader(cfg_reader.climatology_file);
  if (climate_data->error() != "")
    {
      Logg->write_time_warning("failed in vdt_climatology_file_reader constructor: %s\n", climate_data->error().c_str());
    }

  if (args.test)
    {
      Logg->write_time_info("initializing test messages\n");
      vdt_test_generator vdt_test_generator(cfg_reader);
      vdt_test_generator.get_probe_messages(msgs);
      vdt_test_generator.get_road_segments(segs);
      begin_time = vdt_test_generator::begin_time;
      begin_time_string = vdt_test_generator::begin_time_string;
      end_time = vdt_test_generator::end_time;
      end_time_string = vdt_test_generator::end_time_string;
      ds = new vdt_probe_message_datasets_test();

      Logg->write_time_info("updating probe messages\n");
      ds->update_probe_messages(cfg_reader, msgs, qc_msgs);

      vdt_test_generator.get_history_probe_messages(history_msgs);
    }
  else
    {
      // Read probe messages into vector container
      Logg->write_time_info("reading probe message file %s\n", args.probe_message_file.c_str());

      /*
	New reading approach using vector collection 
	rwx_vector_collection_nc probe_message_reader(args.probe_message_file.c_str(), cfg_reader.probe_message_field_names);
	if (probe_message_reader.error != string(""))
	{
	Logg->write_time_error("ingest error for %s: %s\n", args.probe_message_file.c_str(), probe_message_reader.error.c_str());
	return -1;
	}
      */

      vdt_probe_message_file_reader probe_rdr(args.probe_message_file.c_str());
      if (probe_rdr.get_error() != "")
	{
	  Logg->write_time_info("no probe message file: %s\n", probe_rdr.get_error().c_str());
	  Logg->write_time_info("performing weather processing without probe messages\n");
	}
      else
	{
	  probe_rdr.get_probe_messages(raw_msgs);
	}

      Logg->write_time_info("filtering out messages with missing or bad latitude/longitude or obs time that is not relevant to the specified begin_time, end_time interval\n");
      size_t bad_msg_count = 0;
      for (size_t i=0; i<raw_msgs.size(); i++)
	{
	  double raw_lat = raw_msgs[i].get_latitude();
	  double raw_lon = raw_msgs[i].get_longitude();
	  time_t obs_time = raw_msgs[i].get_obs_time();
	  if (vdt_point::check_lat_lon(raw_lat, raw_lon) && obs_time != vdt_const::FILL_VALUE)
	    {
	      time_t begin_time_delta = begin_time - obs_time;
	      time_t end_time_delta = obs_time - end_time;
	      if (begin_time_delta < 0 && (-begin_time_delta < cfg_reader.max_begin_time_delta))
		{
		  msgs.push_back(raw_msgs[i]);
		}
	      else if (obs_time < end_time)
		{
		  msgs.push_back(raw_msgs[i]);
		}
	      else if (end_time_delta < cfg_reader.max_end_time_delta)
		{
		  msgs.push_back(raw_msgs[i]);
		}
	      else
		{
		  bad_msg_count += 1;
		}
	    }
	  else
	    {
	      bad_msg_count += 1;
	    }
	}

      Logg->write_time_info("removed %ld out of %ld messages\n", bad_msg_count, raw_msgs.size());

      // Log optional input files
      if (args.radar_file_pattern != "")
	{
	  Logg->write_time_info("using %s for radar_file_pattern\n", args.radar_file_pattern.c_str());
	}

      if (args.metar_file != "")
	{
	  Logg->write_time_info("using %s for metar file\n", args.metar_file.c_str());
	}

      if (args.rwis_file != "")
	{
	  Logg->write_time_info("using %s for rwis file\n", args.rwis_file.c_str());
	}

      if (args.rtma_file != "")
	{
	  Logg->write_time_info("using %s for rtma file\n", args.rtma_file.c_str());
	}

      if (args.cloud_class_file != "")
	{
	  Logg->write_time_info("using %s for cloud class file\n", args.cloud_class_file.c_str());
	}

      // Read in optional input files
      Logg->write_time_info("constructing vdt_probe_message_datasets from optional input files\n");
      ds = new vdt_probe_message_datasets(&cfg_reader,
					  args.radar_file_pattern,
					  args.metar_file,
					  args.rwis_file,
					  args.rtma_file,
					  args.cloud_class_file,
					  Logg);

      Logg->write_time_info("using %s for road segments\n", args.road_seg_file.c_str());

      if (boost::filesystem::exists(args.road_seg_file))
	{
	  Logg->write_time_info("calling vdt_road_segment_file_reader(%s)\n", args.road_seg_file.c_str());
	  vdt_road_segment_file_reader seg_reader = vdt_road_segment_file_reader(args.road_seg_file.c_str());
	  seg_reader.get_road_segments(segs);
	  seg_reader.get_rwis_sites(rwis_sites);
	}

      // Fill in known data for each probe message using probe message datasets
      // This information is used in later qc processing
      Logg->write_time_info("calling update_probe_messages\n");
      ds->update_probe_messages(cfg_reader, msgs, qc_msgs);
    }


  // Extract latitudes, longitudes and elevations for each probe message
  vector<double> qc_msg_latitude;
  vector<double> qc_msg_longitude;
  vector<double> qc_msg_elevation;
  vector<int> qc_msg_segment_indices;
  vector<double> qc_msg_segment_distances;
  vector<int> qc_msg_grid_cell_indices;
  vector<double> qc_msg_grid_cell_distances;
  vdt_point::get_points_lle(qc_msgs, qc_msg_latitude, qc_msg_longitude, qc_msg_elevation);

  // Extract latitudes, longitudes and elevations for each road segment
  vector<double> road_seg_latitude;
  vector<double> road_seg_longitude;
  vector<double> road_seg_elevation;
  vdt_point::get_points_lle(segs, road_seg_latitude, road_seg_longitude, road_seg_elevation);
#ifdef NOTNOW
  for (size_t i=0; i<road_seg_latitude.size(); i++)
    {
      printf("road seg lat-lon %lf %lf\n", road_seg_latitude[i], road_seg_longitude[i]);
    }
#endif

  // Establish seg index map mapping offset to segment id
  unordered_map<int, int> seg_id_map;
  unordered_map<int, int> inverse_seg_id_map;
  //printf("segs size %ld\n", segs.size());
  for (size_t i=0; i<segs.size(); i++)
    {
      int id = segs[i].get_id();
      seg_id_map[i] = id;
      inverse_seg_id_map[id] = i;
      vdt_point mid_pt = segs[i].get_mid_point();
      //printf("seg: offset, id, mid lat, mid lon:  %ld, %d, %f, %f\n", i, seg_id_map[i], mid_pt.get_latitude(), mid_pt.get_longitude());
      seg_ids.push_back(segs[i].get_id());
    }
  
  // Establish kd tree for road segment latitude, longitude midpoints
  if (segs.size() == 0)
    {
      Logg->write_time_error("no road segments\n");
      return -1;
    }

  Logg->write_time_info("constructing road segment kdtree using %ld segments\n", segs.size());
  vdt_nearest_nbr road_segment_nearest_nbr(road_seg_latitude, road_seg_longitude);

  // Find probe message qc indices that belong to each road segment.
  // Note that qc_msg_indices[i] is the index of the nearest road segment to qc
  // probe msg, qc_msgs[i]
  double eps = 0; 		// get exact nearest neighbors
  road_segment_nearest_nbr.get_nearest_indices(eps, qc_msg_latitude, qc_msg_longitude, qc_msg_segment_indices, qc_msg_segment_distances);

  // Store road segment offset in probe message
  for (size_t i=0; i<qc_msgs.size(); i++)
    {
      if (qc_msg_segment_distances[i] < cfg_reader.max_dist_to_segment * cfg_reader.max_dist_to_segment)
	{
	  qc_msgs[i].set_road_segment_id(seg_id_map[qc_msg_segment_indices[i]]);
	  //printf("index %ld, distance good %lf, segment id %d, surface temp %f, sourcelat, sourcelon, seglat, seglon %lf %lf %lf %lf\n", i, qc_msg_segment_distances[i], qc_msgs[i].get_road_segment_id(), qc_msgs[i].surface_temp, qc_msg_latitude[i], qc_msg_longitude[i], road_seg_latitude[qc_msg_segment_indices[i]], road_seg_longitude[qc_msg_segment_indices[i]]);
	  if (qc_msg_segment_indices[i] == 13)
	    {
	      //std::cout << qc_msgs[i].short_probe_message_hdr() << std::endl;
	      //std::cout << qc_msgs[i].short_probe_message_string() << std::endl;
	    }
	}
      else
	{
	  //if (qc_msg_segment_distances[i] < 0.5)
	  //printf("distance too large %lf, segment index %d\n", qc_msg_segment_distances[i], qc_msg_segment_indices[i]);
	  ;
	}
    }

  // Construct map mapping segment index to the probe message indices associated with that segment
  unordered_map<int, vdt_index_distance> seg_index_map;
  vdt_nearest_nbr::get_inverse_indices(cfg_reader.max_dist_to_segment, qc_msg_segment_indices, qc_msg_segment_distances, seg_index_map);

  // Find probe message qc indices that belong to each grid cell
  // Note that the underlying grid currently used is the climatology grid
  vdt_grid grid(climate_data->get_latitude_size(), climate_data->get_longitude_size(), climate_data->get_lower_left_latitude(), climate_data->get_lower_left_longitude(), climate_data->get_upper_right_latitude(), climate_data->get_upper_right_longitude());
  grid.get_nearest_indices(qc_msg_latitude, qc_msg_longitude, qc_msg_grid_cell_indices, qc_msg_grid_cell_distances);
  unordered_map<int, int> cell_id_map;
  for (int i=0; i<grid.get_grid_size(); i++)
    {
      cell_id_map[i] = i;
    }

  unordered_map<int, vdt_index_distance> cell_index_map;
  vdt_nearest_nbr::get_inverse_indices(cfg_reader.max_dist_to_cell, qc_msg_grid_cell_indices, qc_msg_grid_cell_distances, cell_index_map);
  
  // Construct qc manager
  Logg->write_time_info("constructing qc manager for running qc checks\n");
  vdt_probe_data_qc qc_manager(cfg_reader, climate_data, ds);

  // Perform standard qc checks
  Logg->write_time_info("performing standard qc checks\n");
  qc_manager.perform_standard_checks(begin_time, end_time, qc_msgs);

  // If a history probe message file was provided, read in the history messages
  if (args.history_probe_message_file != string(""))
    {
      Logg->write_time_info("reading historic probe messages for persistence and step checks from %s\n", args.history_probe_message_file.c_str());

      vdt_probe_message_qc_file_reader history_probe_rdr(args.history_probe_message_file.c_str());
      string history_error = history_probe_rdr.get_error();
      if (history_error != "")
	{
	  Logg->write_time_error("history file read error: %s\n", error.c_str());
	}
      else
	{
	  history_probe_rdr.get_probe_messages(history_msgs);
	}
    }

  if (history_msgs.size() > 0)
    {
      // If historic messages are  provided, perform persistence
      // and step qc checks.
      qc_manager.perform_history_checks(begin_time, end_time, history_msgs, qc_msgs);
    }
  else
    {
      Logg->write_time_info("no historic probe messages were provided\n");
    }

#ifdef NOTNOW
  if (true)
    {
      std::cout << "segment index map:" << std::endl;

      for (auto itr = seg_index_map.begin(); itr != seg_index_map.end(); ++itr)
	{
	  const vdt_index_distance index_distance = itr->second;
	  std::cout << index_distance;
	}
    }
#endif

  // set up combined algorithm check
  Logg->write_time_info("performing combined algorithm check\n");
  vdt_combined_check combined_check(cfg_reader, climate_data, ds);
  for (unsigned int i=0; i<qc_msgs.size(); i++)
    {
      combined_check(qc_msgs[i]);
    }

  // Generate road segment statistics for the given begin time value
  Logg->write_time_info("generating probe message road segment statistics for time %s\n", begin_time_string.c_str());
  vector<vdt_probe_message_qc_statistics> seg_stats;
  unordered_map<int, int> seg_index_offset_map;
  vdt_probe_message_qc_statistics::get_probe_message_qc_statistics(begin_time, end_time, seg_index_map, qc_msgs, seg_id_map, seg_stats, seg_index_offset_map);

  // Optionally add road segments not associated with vehicles to seg_stats vector
  if (args.add_non_associated_segments)
    {
      Logg->write_time_info("adding non associated road segments. Number of road segments with probe messages: %ld\n", seg_stats.size());
      vdt_probe_message_qc_statistics::add_non_associated_ids(begin_time, end_time, seg_ids, seg_stats);
      Logg->write_time_info("number of road segments after adding non associated segments: %ld\n", seg_stats.size());
    }

  // Generate grid cell statistics for the given begin time value on request
  Logg->write_time_info("generating probe message grid cell statistics for time %s\n", begin_time_string.c_str());
  vector<vdt_probe_message_qc_statistics> cell_stats;
  unordered_map<int, int> cell_index_offset_map;
  if (args.grid_cell_statistics_out_file != string(""))
    {
      vdt_probe_message_qc_statistics::get_probe_message_qc_statistics(begin_time, end_time, cell_index_map, qc_msgs, cell_id_map, cell_stats, cell_index_offset_map);
    }
 
  // Perform concluding qc checks on road segments
  // These are dependent on the calculation of grid cell statistics 
  // or in the case of the combined qc check, must be performed after
  // all other qc checks
  Logg->write_time_info("performing concluding qc checks\n");
  qc_manager.perform_concluding_checks(begin_time, end_time, seg_stats, qc_msg_segment_indices, seg_index_offset_map, qc_msgs);

  // Get road segment dataset information
  vector<vdt_dataset> datasets; 

  if (!args.test)
    {
      Logg->write_time_info("getting road segment datasets for %ld segments at time %s\n", seg_stats.size(), begin_time_string.c_str());

      // Get the road segment points for the segments in seg_stats. This
      // will typically be a proper subset of all road segments since
      // there may be road segments that do not have probe messages.
      vector<vdt_point> road_seg_points;
      for (size_t i=0; i<seg_stats.size(); i++)
      {
	int id = seg_stats[i].id;
	auto itr = inverse_seg_id_map.find(id);
	if (itr == inverse_seg_id_map.end())
	  {
	    Logg->write_time_error("id %d was not in inverse_seg_id_map", id);
	    return -1;
	  }

	size_t offset = itr->second;
	road_seg_points.push_back(segs[offset].get_point());
	//printf("lat lon for %d: %lf, %lf\n", id, segs[offset].get_point().get_latitude(), segs[offset].get_point().get_longitude());
      }

      ds->get_point_datasets(begin_time, road_seg_points, datasets);

      Logg->write_time_info("augmenting road segment statistics with datasets\n", begin_time_string.c_str());
      string error;
      int ret = vdt_probe_message_qc_statistics::augment_statistics_vector_with_datasets(datasets, seg_stats, error);
      if (ret < 0)
      {
	Logg->write_time("Error: element in seg_stats refers to a dataset that is not in the datasets vector: %s\n", error.c_str());
	return -1;
      }
    }

  // Get rwis_site dataset information
  vector<vdt_dataset> rwis_site_datasets; 
  vector<vdt_probe_message_qc_statistics> rwis_site_stats;
  
  if (!args.test) 
  {
    // Establish rwis site index map mapping offset to rwis site id
    unordered_map<int, int> rwis_site_id_map;
    unordered_map<int, int> inverse_rwis_site_id_map;
    //printf("rwis_sites size %ld\n", rwis_sites.size());
    for (size_t i=0; i<rwis_sites.size(); i++)
    {
      int id = rwis_sites[i].get_id();
      rwis_site_id_map[i] = id;
      inverse_rwis_site_id_map[id] = i;
      vdt_point mid_pt = rwis_sites[i].get_mid_point();
      //printf("rwis_site: offset, id, mid lat, mid lon:  %ld, %d, %f, %f\n", i, rwis_site_id_map[i], mid_pt.get_latitude(), mid_pt.get_longitude());
      rwis_site_ids.push_back(rwis_sites[i].get_id());
    }
    
    if (rwis_sites.size() == 0)
    {
      Logg->write_time_error("no rwis sites\n");
      return -1;
    }
    
    // Optionally add rwis sites not associated with vehicles to rwis_site_stats vector
    if (args.add_non_associated_segments)
    {
      Logg->write_time_info("adding non associated rwis_sites. Number of rwis_sites with probe messages: %ld\n", rwis_site_stats.size());
      vdt_probe_message_qc_statistics::add_non_associated_ids(begin_time, end_time, rwis_site_ids, rwis_site_stats);
      Logg->write_time_info("number of rwis sites after adding non associated segments: %ld\n", rwis_site_stats.size());
    }
 

//  if (!args.test)
//    {
      Logg->write_time_info("getting rwis site datasets for %ld rwis sites at time %s\n", rwis_site_stats.size(), begin_time_string.c_str());

      // Get the rwis site points for the rwis sites in rwis_site_stats. 
      vector<vdt_point> rwis_site_points;
      for (size_t i=0; i<rwis_site_stats.size(); i++)
      {
	int id = rwis_site_stats[i].id;
	auto itr = inverse_rwis_site_id_map.find(id);
	if (itr == inverse_rwis_site_id_map.end())
	  {
	    Logg->write_time_error("id %d was not in inverse_rwis_site_id_map", id);
	    return -1;
	  }

	size_t offset = itr->second;
	rwis_site_points.push_back(rwis_sites[offset].get_point());
	//printf("lat lon for %d: %lf, %lf\n", id, rwis_sites[offset].get_point().get_latitude(), rwis_sites[offset].get_point().get_longitude());
      }

      ds->get_point_datasets(begin_time, rwis_site_points, rwis_site_datasets);

      Logg->write_time_info("augmenting rwis site statistics with rwis datasets\n", begin_time_string.c_str());
      string error;
      int ret = vdt_probe_message_qc_statistics::augment_statistics_vector_with_datasets(rwis_site_datasets, rwis_site_stats, error);
      if (ret < 0)
      {
	Logg->write_time("Error: element in rwis_site_stats refers to a dataset that is not in the rwis datasets vector: %s\n", error.c_str());
	return -1;
      }

      Logg->write_time_info("appending rwis_site_stats to seg_stats\n");
      seg_stats.insert(seg_stats.end(), rwis_site_stats.begin(), rwis_site_stats.end());
//    }
  
  }

  if (args.test)
    {
      printf("\n");
      printf("number of qc probe messages: %ld\n", qc_msgs.size());
      printf("number of road segments: %ld\n", segs.size());
      printf("number of road segment statistics: %ld\n", seg_stats.size());
      printf("\n");

      //std::cout << vdt_probe_message_qc::short_probe_message_hdr() << std::endl;

      std::fstream fs;
      fs.open("test_probe_message_qc.csv", std::fstream::out);

      fs << vdt_probe_message_qc::probe_message_hdr() << std::endl;
      for (size_t i=0; i<qc_msgs.size(); i++)
	{
	  //std::cout << qc_msgs[i].short_probe_message_string() << std::endl;
	  fs << qc_msgs[i].probe_message_string() << std::endl;
	}

      std::fstream fss;
      fss.open("test_probe_message_qc_statistics.csv", std::fstream::out);

      fss << vdt_probe_message_qc_statistics::probe_message_statistics_hdr() << std::endl;
      for (size_t i=0; i<seg_stats.size(); i++)
	{
	  fss << seg_stats[i].probe_message_statistics_string() << std::endl;
	}
    }
  else
    {
      // Write output probe message qc file
      if (args.probe_message_qc_output)
	{
	  Logg->write_time_info("writing %d quality controlled probe messages to %s using %s cdl file\n", qc_msgs.size(), args.probe_message_qc_out_file.c_str(), cfg_reader.probe_message_qc_cdl_file.c_str());
	  vdt_probe_message_qc_file_writer wrt(args.probe_message_qc_out_file.c_str(), cfg_reader.probe_message_qc_cdl_file.c_str());
	  wrt.write_probe_messages(qc_msgs);
	}

      // Write output road segments statistics file 
      if (seg_stats.size() > 0)
	{
	  Logg->write_time_info("writing %d segment stats to %s\n", seg_stats.size(), args.probe_message_qc_statistics_out_file.c_str());
	  vdt_probe_message_qc_statistics_file_writer seg_writer(args.probe_message_qc_statistics_out_file.c_str(), cfg_reader.probe_message_qc_statistics_cdl_file.c_str());
	  seg_writer.write_probe_message_qc_statistics(seg_stats);
	}
      else
	{
	  Logg->write_time_info("no segment statistics since no segment data found\n");
	}

      if (args.grid_cell_statistics_out_file != string("") && cell_stats.size() > 0)
	{
	  Logg->write_time_info("writing %d grid cell stats to %s\n", cell_stats.size(), args.grid_cell_statistics_out_file.c_str());
	  vdt_probe_message_qc_statistics_file_writer cell_writer(args.grid_cell_statistics_out_file.c_str(), cfg_reader.probe_message_qc_statistics_cdl_file.c_str());
	  cell_writer.write_probe_message_qc_statistics(cell_stats);
	}
      else if (!cell_stats.size() > 0)
	{
	  Logg->write_time_info("no grid cell statistics since no grid cell data found.\n");
	}
    }

  delete ds;

  return 0;
}
