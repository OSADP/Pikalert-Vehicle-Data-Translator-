//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright �2013, UCAR. This product includes quality
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
#include "vdt_const.hh"
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

typedef std::shared_ptr<vdt_history_check> HISTORY_CHECK_PTR;

template<typename T>
class vdt_persist_check : public vdt_history_check
{
protected:
  std::string fieldname_;

  virtual std::string create_out_key() const
  {
    return fieldname_ + "_persist_passed";
  }

public:
  vdt_persist_check(const std::string& fieldname, time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg), fieldname_(fieldname)
  {
  }

  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const
  {
    vdt_probe_message_qc &start_probe_message = probe_message[start];
    vdt_probe_message_qc &offset_probe_message = probe_message[offset];

    if (start_probe_message.getattr<T>(fieldname_) == vdt_const::FILL_VALUE)
    {
      start_probe_message.setattr(create_out_key(), static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
      return true;
    }

    if (offset == start)
    {
      start_probe_message.setattr(create_out_key(), static_cast<int>(vdt_probe_message_qc::PASSED));
      return true;
    }
    else if (offset == start+1)
    {
      // Initialize air_temp_persist_passed to PASSED
      start_probe_message.setattr(create_out_key(), static_cast<int>(vdt_probe_message_qc::PASSED));
    }

    if (start_probe_message.get_obs_time() - offset_probe_message.get_obs_time()  < sensor_minutes * 60)
    {
      if (start_probe_message.getattr<T>(fieldname_) == offset_probe_message.getattr<T>(fieldname_))
      {
	// The temperature values are identical so far so the test
	// currently fails but may not be complete
	start_probe_message.setattr(create_out_key(), static_cast<int>(vdt_probe_message_qc::FAILED));
	return false;
      }
      else
      {
	// The temperature values are different so the history test passes and is complete
	start_probe_message.setattr(create_out_key(), static_cast<int>(vdt_probe_message_qc::PASSED));
	return true;
      }
    }
    else
    {
      // The obs times are not sufficiently close
      return true;
    }
  }
};

template<typename T>
class vdt_step_check : public vdt_history_check
{
protected:
  std::string fieldname_;

  virtual std::string create_out_key() const
  {
    return fieldname_ + "_step_passed";
  }

public:
  vdt_step_check(const std::string& fieldname, time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg, float sensor_rate_magnitude) : vdt_history_check(begin_time_arg, end_time_arg, sensor_minutes_arg, sensor_rate_magnitude), fieldname_(fieldname)
  {
  }

  bool operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const
  {
    vdt_probe_message_qc &start_probe_message = probe_message[start];
    vdt_probe_message_qc &offset_probe_message = probe_message[offset];

    if (start_probe_message.getattr<T>(fieldname_) == vdt_const::FILL_VALUE)
    {
      // test cannot be applied
      start_probe_message.setattr<int>(create_out_key(), static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
      return true;
    }

    if (offset == start)
    {
      start_probe_message.setattr<int>(create_out_key(), static_cast<int>(vdt_probe_message_qc::PASSED));
      return true;
    }
    else if (offset == start+1)
    {
      // Initialize air_temp_step_passed to PASSED
      start_probe_message.setattr<int>(create_out_key(), static_cast<int>(vdt_probe_message_qc::PASSED));
    }

    if (offset_probe_message.getattr<T>(fieldname_) == vdt_const::FILL_VALUE)
    {
      // Step check cannot be applied in this case. The test may not be complete so we return false.
      return false;
    }

    unsigned int sensor_seconds = sensor_minutes * 60;
    if (start_probe_message.get_obs_time() - offset_probe_message.get_obs_time()  < sensor_seconds)
    {
      if (fabs(start_probe_message.getattr<T>(fieldname_) - offset_probe_message.getattr<T>(fieldname_)) > sensor_rate_magnitude)
      {
	// The temperature values are not within step test tolerance so test fails
	// and is complete
	start_probe_message.setattr(create_out_key(), static_cast<int>(vdt_probe_message_qc::FAILED));
	return true;
      }
      else
      {
	// The temperature values are within magnitude so the step test passes but the test may not be complete
	start_probe_message.setattr(create_out_key(), static_cast<int>(vdt_probe_message_qc::PASSED));
	return false;
      }
    }
    else
    {
      // Time difference now exceeds maximum so test can no longer be applied
      return true;
    }
  }
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
void vdt_run_history_check(const vector<HISTORY_CHECK_PTR> &history_check, const vector<vdt_probe_message_qc> &current_probe_message, const unordered_map< string, vector<vdt_probe_message_qc> > &probe_message_history_map, time_t begin_time, time_t end_time, vector<vdt_probe_message_qc> &out_probe_message);

#endif /* VDT_HISTORY_CHECK_HH */
