//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: vdt_history_check.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2014/04/08 17:02:32 $
//
//==============================================================================

/**
 * @file vdt_history_check.hh
 *
 * @class vdt_history_check
 *
 */

#ifndef VDT_HISTORY_CHECK_HH
#define VDT_HISTORY_CHECK_HH

#include <string>
#include <unordered_map>
#include <vector>

#include "vdt_probe_message_qc.hh"
using std::string;
using std::unordered_map;
using std::vector;

/**
 * @class vdt_history_check
 * supports the persistence and time step checks which require that the sensors be tracked through time
 */
class vdt_history_check
{
public:
  vdt_history_check()
  {
    printf("calling history base class constructor\n");
  }

  /** @brief constructor for persistence check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   */
  vdt_history_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg);

  /** @brief constructor for step check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   * @param[in] sensor_rate_magnitude_arg check that the magnitude of a step change is < sensor_rate_magnitude_arg
   */
  vdt_history_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg, float sensor_rate_magnitude_arg);

  virtual bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;

  virtual ~vdt_history_check();

  time_t begin_time;
  time_t end_time;
  unsigned int sensor_minutes;  // for both persistence check and step check
  float sensor_rate_magnitude;	// for step check
};

/**
 * @class vdt_bar_press_persist_check  
 * performs barometric pressure persistence check
 */
class vdt_bar_press_persist_check : public vdt_history_check
{
public:
  /** @brief constructor for barometric pressure persistence check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   */
  vdt_bar_press_persist_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg){}


  /** @brief Performs history check on barometric pressure values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_air_temp_persist_check
 * performs air temperature history check
 */
class vdt_air_temp_persist_check : public vdt_history_check
{
public:
  vdt_air_temp_persist_check()
  {
    printf("calling air temp persist check constructor\n");
  }

  /** @brief constructor for air temperature persistence check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   */
  vdt_air_temp_persist_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg){printf("\n");}

  /** @brief performs history check on temperature values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_air_temp2_persist_check
 * performs air temperature2 history check
 */
class vdt_air_temp2_persist_check : public vdt_history_check
{
public:
  vdt_air_temp2_persist_check()
  {
    printf("calling air temp2 persist check constructor\n");
  }

  /** @brief constructor for air temperature2 persistence check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   */
  vdt_air_temp2_persist_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg){printf("\n");}

  /** @brief performs history check on temperature values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_dew_temp_persist_check
 * performs dew temperature history check
 */
class vdt_dew_temp_persist_check : public vdt_history_check
{
public:
  vdt_dew_temp_persist_check()
  {
    printf("calling dew temp persist check constructor\n");
  }

  /** @brief constructor for barometric pressure persistence check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   */
  vdt_dew_temp_persist_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg){printf("\n");}

  /** @brief performs history check on temperature values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_surface_temp_persist_check
 * performs surface temperature history check
 */
class vdt_surface_temp_persist_check : public vdt_history_check
{
public:
  vdt_surface_temp_persist_check()
  {
    printf("calling surface temp persist check constructor\n");
  }

  /** @brief constructor for barometric pressure persistence check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   */
  vdt_surface_temp_persist_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg){}

  /** @brief performs history check on temperature values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_bar_press_step_check
 * performs barometric pressure step check
 */
class vdt_bar_press_step_check : public vdt_history_check
{
public:
  /** @brief constructor for barometric pressure step check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   * @param[in] sensor_rate_magnitude check that the magnitude of a step change is < sensor_rate_magnitude_arg
   */
  vdt_bar_press_step_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg, float sensor_rate_magnitude) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg, sensor_rate_magnitude){}

  /** @brief Performs step check on barometric pressure values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_air_temp_step_check 
 * air temperature step check
 */
class vdt_air_temp_step_check : public vdt_history_check
{
public:
  /** @brief constructor for temperature step check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   * @param[in] sensor_rate_magnitude check that the magnitude of a step change is < sensor_rate_magnitude_arg
   */
  vdt_air_temp_step_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg, float sensor_rate_magnitude) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg, sensor_rate_magnitude){}

  /** @brief Performs step check on temperature values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_air_temp2_step_check 
 * air temperature2 step check
 */
class vdt_air_temp2_step_check : public vdt_history_check
{
public:
  /** @brief constructor for temperature step check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   * @param[in] sensor_rate_magnitude check that the magnitude of a step change is < sensor_rate_magnitude_arg
   */
  vdt_air_temp2_step_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg, float sensor_rate_magnitude) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg, sensor_rate_magnitude){}

  /** @brief Performs step check on temperature values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_dew_temp_step_check 
 * dew temperature step check
 */
class vdt_dew_temp_step_check : public vdt_history_check
{
public:
  /** @brief constructor for temperature step check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   * @param[in] sensor_rate_magnitude check that the magnitude of a step change is < sensor_rate_magnitude_arg
   */
  vdt_dew_temp_step_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg, float sensor_rate_magnitude) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg, sensor_rate_magnitude){}

  /** @brief Performs step check on temperature values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_surface_temp_step_check
 * surface temperature step check
 */
class vdt_surface_temp_step_check : public vdt_history_check
{
public:
  /** @brief constructor for temperature step check
   * @param[in] begin_time_arg only check observations that have times >= begin_time_arg
   * @param[in] end_time_arg only check observations that have times < end_time_arg
   * @param[in] sensor_minutes_arg check consecutive observations whose time difference is < sensor_minutes_arg
   * @param[in] sensor_rate_magnitude check that the magnitude of a step change is < sensor_rate_magnitude_arg
   */
  vdt_surface_temp_step_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg, float sensor_rate_magnitude) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg, sensor_rate_magnitude){}

  /** @brief Performs step check on temperature values */
  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const;
};

/**
 * @class vdt_history_sensor_time_period
 * contains variables for the history sensor time period
 */
class vdt_history_sensor_time_period
{
  unsigned temperature_minutes;
  unsigned air_pressure_minutes;
};

/**
 * @class vdt_history_sensor_time_check
 * contains variables for the history sensor time check
 */
class vdt_history_sensor_time_check
{
  bool temperature_check;
  bool air_pressure_check;
};

/** @brief Perform history check on sensor values of probe messages having the same vehicle identifier
 *
 * @param[in] history_check vector of history checks to perform such as air temperature and pressure history checks. Includes sensor time periods for history checks.
 * @param[in] current_probe_message vector of the most recent probe messages
 * @param[in] probe_message_history_map map of vehicle identifiers to probe messages having those identifiers
 * @param[in] begin_time beginning time for time window of current probe messages
 * @param[in] end_time ending time for time window of current probe messages
 * @param[out] out_probe_message vector of current probe messages with history test qc information
 */
void vdt_run_history_check(const vector<vdt_history_check *> &history_check, const vector<vdt_probe_message_qc> &current_probe_message, const unordered_map< string, vector<vdt_probe_message_qc> > &probe_message_history_map, time_t begin_time, time_t end_time, vector<vdt_probe_message_qc> &out_probe_message);

#endif /* VDT_HISTORY_CHECK_HH */
