
//==============================================================================
//
//   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_road_segment_data.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2013-08-13 21:55:03 $
//
//==============================================================================

/**
 *
 * @file vdt_road_segment_data.hh
 *
 * @class vdt_road_segment_data
 *
 *  vdt road segment data class
 */

#ifndef VDT_ROAD_SEGMENT_DATA_HH
#define VDT_ROAD_SEGMENT_DATA_HH


#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "vdt_road_segment.hh"
#include "vdt_point.hh"

using std::string;
using std::unordered_map;
using std::vector;

#define FI_FILL_FLOAT 9.9692099683868690e+36f

class field_info
{
public:
  field_info(const vector<string> &names, const vector<string> &units);

  vector<string> field_names;
  vector<string> field_units;
  unordered_map<string, int> field_map;
};

/**
 * @class vdt_road_segment
 *
 *   Represents a road segment plus associated data. The road segment is defined by:
 *   two pairs of latitude, longitude - the begin and end points
 *   a unique identifier, name, highway symbol, etc.
 *
 *   Associated data can be temperature, barometric pressure, etc.
 */
class vdt_road_segment_data
{
public:

  /** @brief constructor */
  vdt_road_segment_data(const field_info &field_info_arg, double obs_time_arg, int number_probe_messages_arg, const vdt_road_segment &road_segment_arg);

  /** @brief destructor */
  ~vdt_road_segment_data();


  /** @brief stream insertion operator */
  friend ostream& operator<<(ostream &ostr, const vdt_road_segment_data &road_segment);

  /** @brief print_header */
  int print_header(FILE *fp);
  
  /** @brief print_values */
  int print_values(FILE *fp, int case_number);

  /** @brief print_aux_id_field print the auxiliary road segment id, the date and the field value */
  int print_aux_id_field(FILE *fp, int field_number);

  /**
   * @brief get observation time
   * @return  observation time
   */
  inline double get_obs_time() const
  {
    return obs_time;
  }

  /**
   * @brief get field value based on supplied field number
   * @return  field value
   */
  inline float get_field(int field_number) const
  {
    return field_values[field_number];
  }

  /**
   * @brief get field value based on supplied field number
   * @return  field value
   */
  inline void set_field(int field_number, float field_value)
  {
    field_values[field_number] = field_value;
  }

  /** @brief field_info_vec vector containing field information */
  const field_info &field_info_obj;

  /** @brief obs_time observation time */
  double obs_time;

  /** @brief number_probe_messages number of probe messages associated with this segment */
  int number_probe_messages;

  /** @brief road data field values */
  vector<float> field_values;

  /** @brief road_segment road segment information */
  vdt_road_segment road_segment;
};

#endif /* VDT_ROAD_SEGMENT_DATA_HH */
