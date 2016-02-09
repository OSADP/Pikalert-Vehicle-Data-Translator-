//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: vdt_spatial_check.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2014-02-21 18:13:00 $
//
//==============================================================================

/**
 * @file vdt_spatial_check.hh
 *
 * @class vdt_spatial_check
 *
 */

#ifndef VDT_SPATIAL_CHECK_HH
#define VDT_SPATIAL_CHECK_HH

#include <vector>

using std::vector;

/**
 * @class vdt_spatial_check_iqr
 *
 * The IQR spatial test is a method for checking whether a sensor
 * reading is consistent with its neighboring sensor readings. It
 * detects sensor readings that differ by more than a predefined
 * threshold from an expected value within a neighborhood of the
 * target sensor reading.
 *     
 * In general, across large regions, the atmosphere is quite homogeneous
 * spatially. However, there are times and places such as in complex
 * terrain or near frontal systems where close sensor readings are quite
 * different but realistic. As such, the spatial test has some
 * limitations.
 *     
 *     A target sensor reading does not pass the IQR test when
 *     |Ze - Zo| > max(M * 0.7413 * IQR, minToleranceBound)
 *     where
 *     Ze = Median of neighbors
 *     Zo = Target sensor reading
 *     M = Multiplier value: The value is 3 for all fields, except Relative Humidity, which is 2.5
 *
 *     IQR = Interquartile range: The difference between the .25 and .75
 *     percentiles of the neighbors. The coefficient 0.7413 makes the IQR an
 *     unbiased estimate of the true standard deviation
 *       minToleranceBound  = A fixed value set for each field that bounds the minimum
 *     acceptable spread between the target sensor value and the estimate
 *     
 *     In order to guarantee adequate spatial variation between neighboring
 *     sensors, various tolerance bounds (minToleranceBound) are used for
 *     different weather parameter fields. These bounds are adjustable, so
 *     that they can be tuned further as necessary.
 *     
 *     The minToleranceBound values for each weather parameter field are as follows:
 *     essAirTemperature: 3.5 deg C
 *     essDewpointTemp: 7 deg C
 *     windSensorAvgSpeed: 4.5 m/sec
 *     essAtmosphericPressure: 7.5 mbar
 *     essRelativeHumidity: 15 %
 *     essSurfaceTemperature: 10 deg C
 *     essPavementTemperature: 10 deg C
 *     essSubSurfaceTemperature: 3 deg C
 *     essWetBulbTemp: 7 deg C
 * 
 *     The IQR test is only effective and thus only run if there are 5 or
 *     more Automated Surface Observing Systems (ASOS), Automated Weather
 *     Observing Systems (AWOS), and/or Environmental Sensor Station (ESS)
 *     neighbors that satisfy all of the following conditions:
 *     
 *     . Within a 69 mile radius of the target sensor reading
 *     . Within +/- 350 meters of elevation
 *     . Within 1 hour of the target sensor reading time
 *     
 *     Otherwise, the test will not run.  Up to 20 of the nearest sensors to
 *     the target sensor reading satisfying the above criteria are chosen to
 *     makeup the background field.
 */
class vdt_spatial_check_iqr
{
public:
  /**
   * @brief vdt_spatial_chec_iqr constructor
   * 
   * param[in] min_tolerance_bound minimum tolerance bound for met variable being evaluated
   * param[in] multiplier_value typically 3.0 except for relative humidity where it's set to 2.5
   */
  vdt_spatial_check_iqr(float min_tolerance_value, float multiplier_value): min_tolerance(min_tolerance_value), multiplier(multiplier_value)
  {
  }

  vdt_spatial_check_iqr()
  {
    min_tolerance = 0;
    multiplier = 0;
  }

  /** @brief air_temperature_tolerance_degC air temperature tolerance value for performing IQR test */
  static const float air_temperature_tolerance_degC;

  /** @brief air_pressure_tolerance_mb air pressure tolerance value for performing IQR test */
  static const float air_pressure_tolerance_mb;

  /** @brief  standard_multiplier multipler value for performing IQR test */
  static const float standard_multiplier;

  /** @brief  relative_humidity_multiplier multipler value for performing IQR test */
  static const float relative_humidity_multiplier;

  /** @brief min_iqr_size minimum number of values to use for performing IQR */
  static const size_t min_iqr_size;

  /** @brief unbiased_scale unbiased scale factor for performing IQR */
  static const float unbiased_scale;

  /**
   * @brief perform_iqr perform iqr check
   *
   * @param[in] values nearby station observation values
   * @param[in] target_reading target reading of sensor to compare against nearby station observation values
   * @return vdt_probe_message_qc::NOT_APPLICABLE - test cannot be performed, vdt_probe_message_qc::FAILED - test was performed and failed, vdt_probe_message_qc::PASSED - test was performed and passed
   */
  int perform_iqr(vector<float> &values, float target_reading) const;

  /**
   * @brief perform_iqr perform iqr check by supplying iqr25, iqr75, median, and num_values
   *
   * @param[in] num_values number of values used in calculating iqr25, iqr75, and median
   * @param[in] iqr25 25th percentile value
   * @param[in] iqr75 75th percentile value
   * @param[in] median median value
   * @param[in] target_reading target reading of sensor to compare against 
   * @return vdt_probe_message_qc::NOT_APPLICABLE - test cannot be performed, vdt_probe_message_qc::FAILED - test was performed and failed, vdt_probe_message_qc::PASSED - test was performed and passed
   */
  int perform_iqr(size_t num_values, float iqr25, float iqr75, float median, float target_reading) const;

  /**
   * @brief iqr_median find iqr values and median
   *
   * @param[in] values nearby station observation values
   * @param[out] iqr25 25th percentile value
   * @param[out] iqr75 75th percentile value
   * @param[out] median median percentile value
   * @return -1 if values size too small and test is invalid. Return 0 otherwise.
   */
  static int iqr_median(vector<float> &values, float *iqr25, float *iqr75, float *median);

  static int iqr_median(vector<float> &values, double *iqr25, double *iqr75, double *median)
  {
    float fiqr25;
    float fiqr75;
    float fmedian;
    int ret = iqr_median(values, &fiqr25, &fiqr75, &fmedian);
    *iqr25 = fiqr25;
    *iqr75 = fiqr75;
    *median = fmedian;
    return ret;
  }

  /** @brief min_tolerance */
  float min_tolerance;

  /** @brief multiplier */
  float multiplier;
};

/**
 * @class vdt_spatial_check_barnes
 *
 * Like the IQR test, the neighboring stations used for spatial
 * comparison are determined by a formula based on configurable tolerance
 * bounds. Unlike the IQR test, neighboring sensor readings are weighted
 * according to their distance from the original sensor, with the weight
 * decreasing exponentially with the distance from the station. In the
 * Clarus System, neighboring values (Zi) are based on ASOS, AWOS, and
 * ESS in situ data. The ASOS program is a joint effort of the National
 * Weather Service (NWS), the Federal Aviation Administration (FAA), and
 * the Department of Defense (DOD). The ASOS systems serve as the
 * nation's primary surface weather observing network. ASOS is designed
 * to support weather forecast activities and aviation operations and, at
 * the same time, support the needs of the meteorological, hydrological,
 * and climatological research communities. The AWOS provides basic
 * aviation weather observations directly to pilots approaching the
 * airport. The majority of the ~170 systems were installed at various
 * non-towered airports (OFCM, 2009).  The neighboring stations used for
 * spatial comparison must fall within a distance set by a configuration
 * parameter. Neighboring observations are weighted according to their
 * distance from the original sensor, with the weight decreasing
 * exponentially with the distance from the station.
 * 
 * Each time this test is invoked, it is given a single
 * observation. Through the observation instance, it obtains a sensor and
 * a station. If the sensor or station cannot be obtained, the test
 * returns immediately with an error condition indicating that it was
 * unable to run. Then a set of observations to use for the spatial
 * analysis is determined. If there are fewer than two observations of
 * the same type available or if the IQR Spatial Test was able to run,
 * the test returns immediately with an error condition indicating that
 * it was unable to run.  A target observation does not pass the Barnes
 * Spatial Test when the target observation value (Z0) falls outside of
 * the range defined by the number of configured standard deviations
 * about the weighted mean of the neighboring observations (Ze).  An
 * observation does not pass the Barnes Spatial Test when:
 *
 * |Ze - Zo| > SdMax * sigma
 * where
 * Zo = Target observation
 * Ze = Weighted mean of neighboring observations
 * Zi = The ith neighboring observation
 * SdMax = The configured allowable standard deviations
 * sigma = Estimated standard deviation
 * and where the weighted mean is computed as follows:
 * Ze = (SUM(W(ri) * Zi)/(SUM(W(ri))
 * 
 * where
 * W(ri) = exp( -|Zi - Z0|**2 / (2*(ri/sigma)**2))
 * 
 * The Barnes spatial test only runs when the IQR test does not run and
 * if there are 2 or more ASOS, AWOS and/or ESS neighbors that satisfy
 * all of the following conditions:
 * 
 * . Within the configured radius of the target observation, typically 69 miles
 * . Within 65 minutes of the target observation time, -60 minutes to
 *   +5 minutes to accommodate potential skewed time reporting
 *
 *   Otherwise, the test will not run.
 */
class vdt_spatial_check_barnes
{
public:
  /** @brief barnes constructor
   * @param[in] barnes_min_size_arg minimum number of values to use for performing BARNES spatial test
   * @param[in] barnes_sd_max_arg maximum allowable number of standard deviations from weighted mean to pass test
   */
  vdt_spatial_check_barnes(size_t barnes_min_size_arg, size_t barnes_sd_max_arg);

  /**
   * @brief calculate distances from target point to vector of points
   *
   * @param[in] target target point
   * @param[in] near nearby station observation location points
   * @param[out] distances  distances from target to nearby stations
   */
  static void calc_distances(const vdt_point &target, const vector<vdt_point> &near, vector<float> &distances);

  /**
   * @brief perform_barnes perform barnes spatial check
   *
   * @param[in] values nearby station observation values
   * @param[in] distances  distances to nearby stations
   * @param[in] target_reading target reading of sensor to compare against nearby station observation values
   * @return vdt_probe_message_qc::NOT_APPLICABLE - test cannot be performed, vdt_probe_message_qc::FAILED - test was performed and failed, 1 - vdt_probe_message_qc::PASSED
   */
  int perform_barnes(vector<float> &values, vector<float> &distances, float target_reading);

private:
  size_t barnes_min_size;
  size_t barnes_max_sd;
  vdt_spatial_check_barnes & operator=(const vdt_spatial_check_barnes &vdt_spatial_check_barnes);
  vdt_spatial_check_barnes(const vdt_spatial_check_barnes &vdt_spatial_check_barnes);
};

#endif /* VDT_SPATIAL_CHECK_HH */
