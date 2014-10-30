//==============================================================================
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//==============================================================================

/**
 *
 *  @file vdt_probe_data_qc.hh
 *
 *
 *  @class vdt_probe_data_qc
 *
 *   Simple class for quality control check of probe data
 */

#ifndef _VDT_PROBE_DATA_QC_HH
#define _VDT_PROBE_DATA_QC_HH

// Include files
#include <string>
#include <vector>

#include "vdt4/vdt_climatology_file_reader.hh"
#include "vdt4/vdt_history_check.hh"
#include "vdt4/vdt_probe_message.hh"
#include "vdt4/vdt_probe_message_qc.hh"
#include "vdt4/vdt_probe_message_qc_statistics.hh"
#include "qc_check.hh"
#include "config_reader.hh"

using namespace std;

/**
 * @class vdt_probe_data_qc
 *
 * This class performs standard checks, history checks, and concluding checks.
 *
 * The standard checks include the Anticipated Range Test, the Climate Range Test,
 * the Data Filtering Test, and the Spatial Test. The history checks include the
 * Persistence Test and the Time Step Test.  The concluding checks include the
 * Model Analysis Test, the Neighboring Vehicle Test, the Standard Deviation Test,
 * and the Combined Algorithm Test.
 *
 * The Anticipated Range Test detects readings that fall outside the anticipated realistic range of sensor
 * hardware specifications or theoretical limits (i.e., a maximum and minimum value). If the observation
 * value is greater than or equal to the minimum, and less than or equal to the maximum, the sensor
 * reading passes this quality check. If the sensor reading value is less than the minimum or greater than
 * the maximum, the sensor reading does not pass. This test is useful in identifying observations that are
 * likely not possible on the given sensor.  It is not possible to know with precision the ART bounds 
 * for some vehicle instruments because the OEMs do not release the information, so we cannot know with 
 * certainty the actual limits for every sensor on the road. Additionally, due to privacy / anonymity 
 * issues, even if we knew the ranges for sensors, we will not know which sensors are being used for some
 * vehicles.
 *
 * The Climate Range Test detects sensor readings that fall outside predetermined climate range values.
 * We used Clarus climatology data which are created by running a set of historical observational data through a 
 * common model, thus ensuring that the output data are consistent over time. The reanalysis also
 * ensures that data are available in every time period at every grid point. Bounds for the CRT test were
 * determined by computing monthly minimum and maximum values over a 2.5 degree x 2.5 degree fixed 
 * latitude-longitude grid. The appropriate climate maximum and minimum values used for the test are 
 * determined by the month of the sensor reading date and the latitude / longitude region in which the
 * sensor reading location falls. If the sensor reading value is greater than or equal to the climate minimum, 
 * or equal to the climate minimum, and less than or equal to the climate maximum, the sensor reading
 * passes the climate range quality check. If the sensor reading value is less than the climate minimum
 * or greater than the climate maximum, the sensor reading does not pass the sensor climate range
 * quality check.
 *
 * The Data Filtering Test is used to identify data that may be obviously in error, such as latitude values
 * greater than 90 degrees N.
 *
 * The Spaital Tests are more thoroughly documented in the vdt_spatial_check.hh file in the vdt library code.
 *
 */

class vdt_probe_data_qc
{
public:

  /** Constructor */
  vdt_probe_data_qc(const config_reader &cfg_reader_arg, const vdt_climatology_file_reader *climatology_data_ptr, vdt_probe_message_datasets *probe_message_datasets);

  /**
   * @brief perform standard qc checks on probe message input
   */
  void perform_standard_checks(time_t begin_time, time_t end_time, vector<vdt_probe_message_qc> &probe_messages);

  /**
   * @brief perform history qc checks on probe message input
   */
  void perform_history_checks(time_t begin_time, time_t end_time, const vector<vdt_probe_message_qc> &history_msgs, vector<vdt_probe_message_qc> &messages);

  /**
   * @brief perform concluding qc checks such as neighboring vehicle and combined checks
   */
  void perform_concluding_checks(time_t begin_time, time_t end_time, const vector<vdt_probe_message_qc_statistics> &seg_stats, const vector<int> &seg_indices, unordered_map<int, int> &seg_index_offset, vector<vdt_probe_message_qc> &messages);


private:
  vdt_probe_data_qc & operator=(const vdt_probe_data_qc &vdt_probe_data_qc);
  vdt_probe_data_qc(const vdt_probe_data_qc &vdt_probe_data_qc);
  const config_reader &cfg_reader;
  const vdt_climatology_file_reader *climate_data;
  vdt_probe_message_datasets *datasets;
};
#endif /* vdt_PROBE_DATA_QC_HH */
