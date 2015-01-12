//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_road_segment_assessment.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/04/10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_road_segment_assessment.hh
 *
 * @class vdt_road_segment_assessment
 *
 *  vdt road_segment assessment class
 *
 */


#ifndef VDT_ROAD_SEGMENT_ASSESSMENT_HH
#define VDT_ROAD_SEGMENT_ASSESSMENT_HH


#include "vdt_probe_message_qc_statistics.hh"

/**
 * @class vdt_road_segment_assessment
 */
class vdt_road_segment_assessment
{

public:

  /** @brief non precipitation value */
  static const int NO_PRECIP;

  /** @brief rain precipitation value */
  static const int PRECIP_RAIN;

  /** @brief heavy rain precipitation value */
  static const int PRECIP_HEAVY_RAIN;

  /** @brief frozen precipitation value */
  static const int PRECIP_FROZEN;

  /** @brief heavy frozen precipitation value */
  static const int PRECIP_HEAVY_FROZEN;

  /** @brief dry pavement condition value */
  static const int DRY_PAVEMENT;

  /** @brief wet  pavement condition value */
  static const int WET_PAVEMENT;

  /** @brief road splash pavement condition value */
  static const int ROAD_SPLASH;

  /** @brief snow covered pavement condition value */
  static const int SNOW_COVERED;

  /** @brief slick pavement condition value */
  static const int SLICK_PAVEMENT;

  /** @brief heavy rain visibility value */
  static const int VIS_HEAVY_RAIN;

  /** @brief blowing snow visibility value */
  static const int VIS_BLOWING_SNOW;

  /** @brief low visibility value */
  static const int VIS_LOW;

  /** @brief normal visibility value */
  static const int VIS_NORMAL;

  /** @brief no hazards value */
  static const int NO_HAZARDS;

  /** @brief heavy rain hazard value */
  static const int HEAVY_RAIN_HAZARD;

  /** @brief low visibility hazard value */
  static const int LOW_VIS_HAZARD;

  /** @brief heavy snow hazard value */
  static const int HEAVY_SNOW_HAZARD;

  /** @brief heavy freezing mix hazard value */
  static const int HEAVY_FREEZING_MIX_HAZARD;

  /** @brief slippery roads hazard value */
  static const int SLIPPERY_ROADS_HAZARD;

  /** @brief constructor */
  vdt_road_segment_assessment();

  /** @brief performs assessment of precipitation type, pavement condition, and visibility
   * @param[in] seg_stat  vdt road segment statistic value
   */
  void perform_assessment(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief makes an assessment about precipitation type
   * @param[in] seg_stat  vdt road segment statistic value
   * @return type of precip or fill value
   */
  int precip_assessment(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief makes an assessment about pavement condition
   * @param[in] seg_stat  vdt road segment statistic value
   * @return pavement condition  or fill value
   */
  int pavement_condition_assessment(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief makes an assessment about visibility given segment statistics and a precipitation assessment
   * @param[in] seg_stat  vdt road segment statistic value
   * @param[in] precip_assessment  precipitation assessment value
   * @return visibility value or fill value
   */
  int visibility_assessment(vdt_probe_message_qc_statistics& seg_stat, int precip_assessment);

  /** @brief makes an assessment about visibility given only segment statistics
   * @param[in] seg_stat  vdt road segment statistic value
   * @return visibility value or fill value
   */
  int visibility_assessment(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief makes an assessment about the overall hazard given precip type, pavement condition, and visibility
   * @param[in] precip_type         derived precipitation field
   * @param[in] pavement_condition  derived pavement condition field
   * @param[in] visibility          derived visibility field
   * @return all hazards value or fill value
   */
  int all_hazards_assessment(int precip_type, int pavement_condition, int visibility);

  /** @brief gets the derived preciptation type */
  int get_precip_type() const { return precip_type; };

  /** @brief gets the derived pavement condition */
  int get_pavement_condition() const { return pavement_condition; };

  /** @brief gets the derived visibility */
  int get_visibility() const { return visibility; };

  /** @brief gets the road segment id */
  int get_road_segment_id() const { return road_segment_id; };

  /** @brief gets the derived all hazards value */
  int get_all_hazards() const { return all_hazards; };
  
  /** @brief assignment operator */
  vdt_road_segment_assessment & operator=(const vdt_road_segment_assessment &assessment);
  

private:

  /** @brief precipitation type */
  int precip_type;

  /** @brief pavement condition */
  int pavement_condition;

  /** @brief visibility */
  int visibility;

  /** @brief road_segment_id */
  int road_segment_id;

  /** @brief all_hazards */
  int all_hazards;

  /** @brief determines if the precipitation type is rain or heavy rain or none
   * @param[in] seg_stat  vdt road segment statistic value
   * @return precipitation type
   */
  int precip_type_rain_branch(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if the precipitation type is mixed
   * @param[in] seg_stat  vdt road segment statistic value
   * @return precipitation type
   */
  int precip_type_mixed_branch(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if the precipitation type is frozen or heavy frozen or none
   * @param[in] seg_stat  vdt road segment statistic value
   * @return precipitation type
   */
  int precip_type_frozen_branch(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines is pavement condition is dry
   * @param[in] seg_stat  vdt road segment statistic value
   * @return interest value
   */
  double get_dry_pavement(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines is pavement condition is wet
   * @param[in] seg_stat  vdt road segment statistic value
   * @return interest value
   */
  double get_wet_pavement(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines is pavement condition is road splash
   * @param[in] seg_stat  vdt road segment statistic value
   * @return interest value
   */
  double get_road_splash_pavement(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines is pavement condition is snow covered
   * @param[in] seg_stat  vdt road segment statistic value
   * @return interest value
   */
  double get_snow_covered_pavement(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines is pavement condition is slick pavement
   * @param[in] seg_stat  vdt road segment statistic value
   * @return interest value
   */
  double get_slick_pavement(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if the value is the maximum value
   * @param[in] val
   * @param[in] t1
   * @param[in] t2
   * @param[in] t3
   * @param[in] t4
   * @return true if val is the maximum, otherwise false
   */
  bool is_max(double val, double t1, double t2, double t3, double t4);

  /** @brief determines if the wipers are on
   * @param[in] seg_stat  vdt road segment statistic value
   * @return 0 if wipers are off, otherwise num_wipers_on
   */
  int get_wipers_on(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if the value is greater than or equal to the slope_min and less than or equal to the slope max
   * @param[in] val value
   * @param[in] slope_min minumum slope
   * @param[in] slope_max maximum slope
   * @param[in] m  slope
   * @param[in] b  y-intercept
   * @return interest value or 0
   */
  double get_interest(double val, double slope_min, double slope_max, double m, double b);

  /** @brief determines if the value is greater than or equal to the min and less than or equal to the max 
   * @param[in] val value
   * @param[in] min minimum
   * @param[in] max maximum
   * @param[in] flat_val flat value
   * @return interest value or 0
   */
  double get_interest(double val, double min, double max, double flat_val);

  /** @brief determines the interest value based on the anti lock brake status, stability, and traction
   * @param[in] seg_stat     vdt road segment statistic value
   * @param[in] engaged      engaged
   * @param[in] not_engaged  not engaged
   * @return interest value or 0
   */
  double get_abs_stab_trac_interest(vdt_probe_message_qc_statistics& seg_stat, double engaged, double not_engaged);

  /** @brief determines if the wiper status is intermittent
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the wiper status is intermittent, false otherwise
   */
  bool wipers_intermittent(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief  determines if the wipers status is on
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the wiper status is on, false otherwise
   */
  bool wipers_on(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief   determines if the wipers status is off
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the wiper status is off, false otherwise
   */
  bool wipers_off(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief   determines if the wipers status is high
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the wiper status is high, false otherwise
   */
  bool wipers_high(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief   determines if the wipers status is low
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the wiper status is low, false otherwise
   */
  bool wipers_low(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief   determines if the wipers status is missing
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the wiper status is missing, false otherwise
   */
  bool wipers_missing(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if the anti lock brake status is engaged
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the anti lock brake status is engaged, false otherwise
   */
  bool abs_engaged(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if the anti lock brake status is not engaged
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the anti lock brake status is not engaged, false otherwise
   */
  bool abs_not_engaged(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if stability is engaged
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the stability is engaged, false otherwise
   */
  bool stab_engaged(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if stability is not engaged
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the stability is not engaged, false otherwise
   */
  bool stab_not_engaged(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if traction is engaged 
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the traction is not engaged, false otherwise
   */
  bool trac_engaged(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief determines if traction is not engaged 
   * @param[in] seg_stat  vdt road segment statistic value
   * @return true if the traction is not engaged, false otherwise
   */
  bool trac_not_engaged(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief get the air temperature
   * @param[in] seg_stat  vdt road segment statistic value
   * @return air temperature value
   */
  float get_air_temp(vdt_probe_message_qc_statistics& seg_stat);
};


#endif /* VDT_ROAD_SEGMENT_ASSESSMENT_HH */
