//==============================================================================
//
//   (c) Copyright, 2008 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: stat_tools.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2013-04-22 22:34:49 $
//
//==============================================================================

/**
 *
 * @file stat_tools.hh
 *
 * @class ST_stats_container
 *
 *  Statistical functions and classes for vectors of data.
 *
 * @date 2/15/08
 *
 */

#ifndef STAT_TOOLS_HH
#define STAT_TOOLS_HH


#include <algorithm>
#include <numeric>
#include <map>
#include "math.h"
#include <stdexcept>
#include <string>
#include <vector>

using std::accumulate;
using std::map;
using std::string;
using std::vector;

extern char *ST_stat_list_cstring[];
extern map<string, int> ST_stat_map;
void ST_init_stat_map();

template <typename T>
double ST_mean(const vector<T> &array)
{
  if (array.empty())
    throw std::length_error("ST_mean(): empty iterator");

  double sum = accumulate(array.begin(), array.end(), (double)0.0);
  return sum/array.size();
}


template <typename T>
double ST_variance(const vector<T> &array)
{
  if (array.empty())
    throw std::length_error("ST_variance(): empty iterator");

  double mean = ST_mean(array);
  double sum = 0.0;

  for (int i=0; i<(int)array.size(); i++)
    {
      double deviation = array[i] - mean;
      sum += deviation * deviation;
    }

  return sum/array.size();
}

template <typename T>
double ST_standard_deviation(const vector<T> &array)
{
  if (array.empty())
    throw std::length_error("ST_mean(): empty iterator");

  return sqrt(ST_variance(array));
}


#ifdef NOTNOW
template <typename T>
double ST_mean(const T &begin, const T &end)
{
  if (begin == end)
    throw std::length_error("ST_mean(): empty iterator");
  double sum = accumulate(begin, end, (double)0.0);
  return sum/(end-begin);
}

template <typename T>
void ST_median1(const T &begin, const T &end)
{
  if (begin == end)
    throw std::length_error("ST_median(): empty array");

  vector<T> sort_array;
  copy(begin, end, back_inserter(sort_array));
  //  nth_element(sort_array.begin(), sort_array.begin() + sort_array.size()/2, sort_array.end());
  //  return *(sort_array.begin() + sort_array.size()/2);
  //  return *begin;
}
#endif

template <typename T>
T ST_median(const vector<T> &array)
{
  if (array.empty())
    throw std::length_error("ST_median(): empty array");

  vector<T> sort_array;
  copy(array.begin(), array.end(), back_inserter(sort_array));
  nth_element(sort_array.begin(), sort_array.begin() + sort_array.size()/2, sort_array.end());
  return *(sort_array.begin() + sort_array.size()/2);
}

/**
 *  <brief description>
 *
 *  @param [in,out] array -- array of basic numeric type
 *  @param [out] min_max_median -- minimum, maximum and median values of array are stored in this vector as doubles
 *  @pre array must be non-empty or std::length_error exception is thrown
 *  @note input array is sorted on return from this function
 *  @return void
 */
template <typename T>
void ST_min_max_median(vector<T> &array, vector<T> &min_max_median)
{
  if (array.empty())
    throw std::length_error("ST_min_max_median(): empty array");

  // Sorts the input array
  sort(array.begin(), array.end());
  min_max_median.push_back(*array.begin());
  min_max_median.push_back(*(array.end() - 1));
  min_max_median.push_back(*(array.begin() + array.size()/2));
  return;
}

// Sort values in place and return quartile locations
template <typename T>
int ST_quartiles(vector<T> &values, vector<size_t> &quartile_locations)
{
  int median_index = 0;
  int low_quartile_index = 0 ;
  int high_quartile_index = 0;

  size_t num_values = values.size();
  if (num_values == 0)
    {
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

  quartile_locations.push_back(low_quartile_index);
  quartile_locations.push_back(median_index);
  quartile_locations.push_back(high_quartile_index);
  return 0;
}

template <typename T>
void ST_histogram(const vector<T> &array, double min, double max, int number_bins, vector<int> &histogram)
{
  if (array.empty())
    throw std::length_error("ST_histogram(): empty array");

  histogram.clear();
  int size = array.size();
  double range = max - min;
  if (range == 0)
    {
      histogram.push_back(size);
    }
  else
    {
      histogram.resize(number_bins, 0);
      double factor = number_bins / range;

      for (int i=0; i<size; i++)
        {
          if (array[i] == max)
            {
              histogram[number_bins-1]++;
            }
          else
            {
              histogram[(int)((array[i] - min) * factor)]++;
            }
        }
    }

  return;
}

template <typename T>
void ST_min_max(vector<T> array, T *min, T *max)
{

  *min = array[0];
  *max = array[0];
  for (int i=1; i<(int)array.size(); i++)
    {
      if (array[i] < *min)
        *min = array[i];
      if (array[i] > *max)
        *max = array[i];
    }

  return;
}


/// Stats_container class
template<typename T>
class ST_stats_container
{
public:
  /// Constructors
  ST_stats_container();
  ST_stats_container(vector<T> &array);
  ST_stats_container(vector<T> &array, const vector<string> &stats_config);
  ST_stats_container(vector<T> &array, int number_bins);
  ST_stats_container(vector<T> &array, double min_param, double max_param, int number_bins);
  ST_stats_container(const ST_stats_container &stats_container);

  vector<int> histogram;	// Histogram of values between minimum and maximum
  T max;			// Maximum
  T mean;			// Mean
  T median;			// Median
  T min;			// Minimum
  T standard_deviation;		// Standard deviation
  T variance;			// Variance
  T median_abs_deviation;	// Median absolute deviation
  vector<T> stats;		// Accumulated statistics
};


template<typename T>
ST_stats_container<T>::ST_stats_container(const ST_stats_container &stats_container)
{
}

template<typename T>
ST_stats_container<T>::ST_stats_container()
{
}

template<typename T>
ST_stats_container<T>::ST_stats_container(vector<T> &array)
{
  if (array.size() == 0)
    return;

  ST_min_max<T>(array, &min, &max);
  mean = ST_mean<T>(array);
  median = ST_median<T>(array);
  variance = ST_variance<T>(array);
  standard_deviation = ST_standard_deviation<T>(array);
  stats.resize(ST_stat_map["ST_MAX_INDICES"], 0);
  stats[ST_stat_map["ST_COUNT"]] = array.size();
  stats[ST_stat_map["ST_MAX"]] = max;
  stats[ST_stat_map["ST_MEAN"]] = mean;
  stats[ST_stat_map["ST_MEDIAN"]] = median;
  stats[ST_stat_map["ST_MIN"]] = min;
  stats[ST_stat_map["ST_STANDARD_DEVIATION"]] = standard_deviation;
  stats[ST_stat_map["ST_VARIANCE"]] = variance;
  return;
}

template<typename T>
ST_stats_container<T>::ST_stats_container(vector<T> &array, int number_bins)
{
  if (array.size() == 0)
    return;

  ST_min_max<T>(array, &min, &max);
  mean = ST_mean<T>(array);
  median = ST_median<T>(array);
  variance = ST_variance<T>(array);
  standard_deviation = ST_standard_deviation<T>(array);
  ST_histogram<T>(array, min, max, number_bins, histogram);
  stats.resize(ST_stat_map["ST_MAX_INDICES"] + number_bins, 0);
  stats[ST_stat_map["ST_COUNT"]] = array.size();
  stats[ST_stat_map["ST_MAX"]] = max;
  stats[ST_stat_map["ST_MEAN"]] = mean;
  stats[ST_stat_map["ST_MEDIAN"]] = median;
  stats[ST_stat_map["ST_MIN"]] = min;
  stats[ST_stat_map["ST_STANDARD_DEVIATION"]] = standard_deviation;
  stats[ST_stat_map["ST_VARIANCE"]] = variance;
  for (int i=0; i<(int)histogram.size(); i++)
    {
      stats[ST_stat_map["ST_HISTOGRAM"] + i] = static_cast<T>(histogram[i]);
    }

  return;
}

template<typename T>
ST_stats_container<T>::ST_stats_container(vector<T> &array, double min_param, double max_param, int number_bins)
{
  if (array.size() == 0)
    return;

  ST_min_max<T>(array, &min, &max);
  mean = ST_mean<T>(array);
  median = ST_median<T>(array);
  variance = ST_variance<T>(array);
  standard_deviation = ST_standard_deviation<T>(array);
  ST_histogram<T>(array, min_param, max_param, number_bins, histogram);
  stats.resize(ST_stat_map["ST_MAX_INDICES"] + number_bins, 0);
  stats[ST_stat_map["ST_COUNT"]] = array.size();
  stats[ST_stat_map["ST_MAX"]] = max;
  stats[ST_stat_map["ST_MEAN"]] = mean;
  stats[ST_stat_map["ST_MEDIAN"]] = median;
  stats[ST_stat_map["ST_MIN"]] = min;
  stats[ST_stat_map["ST_STANDARD_DEVIATION"]] = standard_deviation;
  stats[ST_stat_map["ST_VARIANCE"]] = variance;
  for (int i=0; i<(int)histogram.size(); i++)
    {
      stats[ST_stat_map["ST_HISTOGRAM"] + i] = static_cast<T>(histogram[i]);
    }

  return;
}

#ifdef NOTNOW
template<typename T>
ST_stats_container<T>::ST_stats_container(vector<T> &array, const vector<string> &stats_config)
{
  if (array.size() == 0)
    return;

  ST_min_max<T>(array, &min, &max);
  mean = ST_mean<T>(array);

  vector<string>::const_iterator pos;
  for (pos=stats_config.begin(); pos!=stats_config.end(); ++pos)
    {
      if *pos
    }

ST_histogram<T>(array, min, max, number_bins, histogram);
  stats.push_back(max);
  stats.push_back(mean);
  stats.push_back(min);
  for (int i=0; i<(int)histogram.size(); i++)
    {
      stats.push_back(static_cast<T>(histogram[i]));
    }

  return;
}
#endif

#endif /* STAT_TOOLS_HH */
