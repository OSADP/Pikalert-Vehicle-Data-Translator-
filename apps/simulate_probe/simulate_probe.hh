//==============================================================================
//
//   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file simulate_probe.hh
 *
 * @class Simulate_Probe
 *
 *  <file/class description>
 *  
 * @date 12/16/14
 */

#ifndef SIMULATE_PROBE_HH
#define SIMULATE_PROBE_HH

#include <vector>
#include "vdt4/vdt_generic_message.hh"
#include "vdt4/vdt_road_segment_data.hh"

using std::vector;

/**
 * @brief simulate_probe_road_segment_data
 *
 * Simulate probe messages based on road segment data
 *
 * @param[in] number of probe messages for road segment
 * @param[in] number of meteorological fields
 * @param[in] standard_deviation standard deviation specified for each met variable such as Air temperature : 0.5 degrees C
 * @param[in] road_segment_data
 * @param[in] standard deviation information
 * @param[out] probe_messages
 */
void simulate_probe_road_segment_data(size_t number_probe_messages, size_t number_fields, const vector<float> &standard_deviation_vec, const vdt_road_segment_data &road_segment_data, vdt_road_segment_data &output_road_segment_data, vector<vdt_generic_message> &probe_messages);



#endif /* SIMULATE_PROBE_HH */
