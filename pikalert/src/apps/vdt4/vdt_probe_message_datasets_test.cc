//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: vdt_probe_message_datasets_test.cc,v $
//       Version: $Revision: 1.7 $  Dated: $Date: 2014/04/11 20:36:38 $
//
//==============================================================================

/**
 * @file vdt_probe_message_datasets_test.cc
 */

// Include files 
#include <fstream>
#include "vdt4/vdt_point.hh"
#include "vdt_test_generator.hh"
#include "vdt_probe_message_datasets_test.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

void vdt_probe_message_datasets_test::get_near_surface_obs(const vdt_point &target, double obstime, float search_range_km, int time_range_secs, vector<vdt_surface_observation> &near)
{
  vector<vdt_surface_observation> surface_obs_vec;
  vdt_surface_observation surface_obs;
  vdt_point point;

  if (obstime == vdt_test_generator::obs_time)
    {

      double dist = vdt_point::great_circle_distance(target.latitude, target.longitude, vdt_test_generator::boulder_lat, vdt_test_generator::boulder_lon);
      if (dist < 50)
	{
	  // push 2 locations in boulder to pass barnes test
	  point.set_latitude(vdt_test_generator::boulder_lat);
	  point.set_longitude(vdt_test_generator::boulder_lon);
	  point.set_elevation(vdt_test_generator::boulder_elev);
	  surface_obs.set_point(point);
	  surface_obs.set_air_temp(vdt_test_generator::boulder_barnes_temp);
	  surface_obs.set_surface_temp(vdt_test_generator::boulder_barnes_temp);
	  surface_obs.set_press(vdt_test_generator::boulder_barnes_bar_press);
	  near.push_back(surface_obs);
	  near.push_back(surface_obs);
	}
      else
	{
	  dist = vdt_point::great_circle_distance(target.latitude, target.longitude, vdt_test_generator::glenwood_lat, vdt_test_generator::glenwood_lon);
	  if (dist < 50)
	    {
	      // push 5 locations at glenwood to pass iqr test

	      point.set_latitude(vdt_test_generator::glenwood_lat);
	      point.set_longitude(vdt_test_generator::glenwood_lon);
	      point.set_elevation(vdt_test_generator::glenwood_elev);
	      surface_obs.set_point(point);
	      surface_obs.set_air_temp(vdt_test_generator::glenwood_spatial_temp);
	      surface_obs.set_surface_temp(vdt_test_generator::glenwood_spatial_temp);
	      surface_obs.set_dew_temp(vdt_test_generator::glenwood_spatial_dew_temp);
	      surface_obs.set_press(vdt_test_generator::glenwood_spatial_bar_press);
	      near.push_back(surface_obs);
	      near.push_back(surface_obs);
	      near.push_back(surface_obs);
	      near.push_back(surface_obs);
	      near.push_back(surface_obs);
	    }
	}
    }

  return;
}

void vdt_probe_message_datasets_test::update_probe_messages(const config_reader& cfg, const vector<vdt_probe_message>& in_msgs, vector<vdt_probe_message_qc> &out_msgs)
{
  //printf("\nBelow are the probe message header and the probe messages. You may wish to copy and paste these into a spreadsheet application for easier inspection. \n\n");
  std::fstream fspm;
  fspm.open("test_probe_message.csv", std::fstream::out);

  for (size_t i = 0; i < in_msgs.size(); i++)
    {
      vdt_probe_message msg = in_msgs[i];
      vdt_probe_message_qc qc_msg(msg);
      vdt_probe_message msg1 = qc_msg;
      if (i == 0)
	{
	  //std::cout << msg1.probe_message_hdr() << std::endl;
	  fspm << msg1.probe_message_hdr() << std::endl;
	}
      fspm << msg1.probe_message_string() << std::endl;
      
      //std::cout << msg1.probe_message_string() << std::endl;
      //std::cout << "pmdt: " << msg1.probe_message_string() << std::endl;
      // double obs_time = msg.get_obs_time();
      qc_msg.set_model_air_temp(55);
      qc_msg.set_model_dewpoint(40);
      qc_msg.set_model_bar_press(835);
      qc_msg.set_nss_air_temp_mean(0);
      qc_msg.set_nss_bar_press_mean(0);
      out_msgs.push_back(qc_msg);
    }
  std::cout << "\n";
}
