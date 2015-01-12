//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: vdt_spatial_check.cc,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2014/04/11 20:34:06 $
//
//==============================================================================

/**
 * @file vdt_spatial_check.cc
 *
 */

// Include files
#include <algorithm>
#include <cmath>
#include <math.h>
#include "vdt_const.hh"
#include "vdt_point.hh"
#include "vdt_probe_message_qc.hh"
#include "vdt_spatial_check.hh"
#include "stat_tools.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables

// Functions

vdt_spatial_check_barnes::vdt_spatial_check_barnes(size_t barnes_min_size_arg, size_t barnes_max_sd_arg) : barnes_min_size(barnes_min_size_arg), barnes_max_sd(barnes_max_sd_arg)
{
}

void vdt_spatial_check_barnes::calc_distances(const vdt_point &target, const vector<vdt_point> &near, vector<float> &distances)
{
  for (size_t i=0; i<near.size(); i++)
    {
      distances.push_back(vdt_point::great_circle_distance(target.get_latitude(), target.get_longitude(), near[i].get_latitude(), near[i].get_longitude()));
    }
}

int vdt_spatial_check_barnes::perform_barnes(vector<float> &values, vector<float> &distances, float target_reading)
{
  size_t num_values = values.size();
  if (target_reading == vdt_const::FILL_VALUE || num_values < barnes_min_size || values.size() != distances.size())
    return vdt_probe_message_qc::NOT_APPLICABLE;

  float deviation = ST_standard_deviation(values);

  float weighted_sum = 0;
  float total_weights = 0;
  for (size_t i=0; i<distances.size(); i++)
    {
      float difference = values[i] - target_reading;
      float quotient = distances[i]/deviation;
      float weight = exp(- (difference * difference) / (2 * quotient * quotient));
      weighted_sum += weight * values[i];
      total_weights += weight;
    }

  float weighted_mean = weighted_sum / total_weights;

  if (fabs(weighted_mean - target_reading) > barnes_max_sd * deviation)
    {
      // test failed
      return vdt_probe_message_qc::FAILED;
    }
  else
    {
      return vdt_probe_message_qc::PASSED;
    }
}

int vdt_spatial_check_iqr::iqr_median(vector<float> &values, float *iqr25, float *iqr75, float *median)
{
  int median_index = 0;
  int low_quartile_index = 0 ;
  int high_quartile_index = 0;

  size_t num_values = values.size();
  if (num_values == 0)
    {
      *iqr25 = vdt_const::FILL_VALUE;
      *iqr75 = vdt_const::FILL_VALUE;
      *median = vdt_const::FILL_VALUE;
      return -1;
    }

  // Sort values
  std::sort(values.begin(), values.end());

  // Pick the 25 and 75 percentile elements
  median_index = static_cast<int>(rint(num_values * 0.50));
  low_quartile_index = static_cast<int>(rint(num_values * 0.25));
  high_quartile_index = static_cast<int>(rint(num_values * 0.75));

  if (num_values == 1)
    {
      median_index = 0;
      low_quartile_index = 0;
      high_quartile_index = 0;
    }
  else
    {
      median_index -= 1;
      low_quartile_index -= 1;
      high_quartile_index -= 1;
    }

  *iqr25 = values[low_quartile_index];
  *iqr75 = values[high_quartile_index];

  if (num_values % 2 == 0)
    {
      *median = (values[median_index] + values[median_index+1])/2.0;
    }
  else
    {
      *median = values[median_index];
    }

  return 0;
}

int vdt_spatial_check_iqr::perform_iqr(vector<float> &values, float target_reading) const
{
  size_t num_values = values.size();
  //printf("#target reading: %f\n", target_reading);
  if (target_reading == vdt_const::FILL_VALUE || num_values < min_iqr_size || num_values < 4)
    {
      return vdt_probe_message_qc::NOT_APPLICABLE;
    }

  float iqr25;
  float iqr75;
  float median;
  iqr_median(values, &iqr25, &iqr75, &median);
  float iqr = iqr75 - iqr25;

  float compare = multiplier * unbiased_scale * iqr;
  float upper_bound = compare < min_tolerance ? min_tolerance : compare;

  //printf("#median %f values.size() %d, upper_bound %f\n", median, values.size(), upper_bound);

  if (fabs(median - target_reading) > upper_bound)
    {
      return vdt_probe_message_qc::FAILED;
    }
  else
    {
      return vdt_probe_message_qc::PASSED;
    }
}

int vdt_spatial_check_iqr::perform_iqr(size_t num_values, float iqr25, float iqr75, float median, float target_reading) const
{
  if (target_reading == vdt_const::FILL_VALUE || num_values < min_iqr_size || num_values < 4)
    {
      return vdt_probe_message_qc::NOT_APPLICABLE;
    }

  float iqr = iqr75 - iqr25;
  float compare = multiplier * unbiased_scale * iqr;
  float upper_bound = compare < min_tolerance ? min_tolerance : compare;

  if (fabs(median - target_reading) > upper_bound)
    {
      return vdt_probe_message_qc::FAILED;
    }
  else
    {
      return vdt_probe_message_qc::PASSED;
    }
}

const float vdt_spatial_check_iqr::air_temperature_tolerance_degC = 3.5;
const float vdt_spatial_check_iqr::air_pressure_tolerance_mb = 7.5;
const float vdt_spatial_check_iqr::standard_multiplier = 3.0;
const float vdt_spatial_check_iqr::relative_humidity_multiplier = 2.5;
const size_t vdt_spatial_check_iqr::min_iqr_size = 5;
const float vdt_spatial_check_iqr::unbiased_scale = 0.7413;
