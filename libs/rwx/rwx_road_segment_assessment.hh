//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: rwx_road_segment_assessment.hh,v $
//       Version: $Revision: 1.16 $  Dated: $Date: 2014/07/15 20:55:18 $
//
//==============================================================================

/**
 *
 * @file rwx_road_segment_assessment.hh
 *
 * @class rwx_road_segment_assessment
 *
 *  rwx road_segment assessment class
 *
 */


#ifndef RWX_ROAD_SEGMENT_ASSESSMENT_HH
#define RWX_ROAD_SEGMENT_ASSESSMENT_HH

#include "vdt_probe_message_qc_statistics.hh"

using namespace rwx;

/**
 * @class vdt_road_segment_assessment
 */
class rwx_road_segment_assessment
{

public:

  // Input Forecast precip-type definitions

  /** @brief forecast precipitation type - no precip */
  static const int FCST_NONE;
  
  /** @brief forecast precipitation type - rain */
  static const int FCST_RAIN;
  
  /** @brief forecast precipitation type - snow */
  static const int FCST_SNOW;
  
  /** @brief forecast precipitation type - ice */
  static const int FCST_ICE;

  
  // Input Dual-Pol Radar Hydrometeor-Class definitions
  
  /** @brief dual-pol hid biological scatter */
  static const int HID_BIO_SCATTER;

  /** @brief dual-pol hid ground clutter */
  static const int HID_GROUND_CLUTTER;

  /** @brief dual-pol hid ice crystals */
  static const int HID_ICE_CRYSTALS;

  /** @brief dual-pol hid dry snow */
  static const int HID_DRY_SNOW;

  /** @brief dual-pol hid wet snow */
  static const int HID_WET_SNOW;

  /** @brief dual-pol hid light/moderate rain */
  static const int HID_LGT_MDT_RAIN;

  /** @brief dual-pol hid heavy rain */
  static const int HID_HVY_RAIN;

  /** @brief dual-pol hid big rain drops */
  static const int HID_BIG_DROPS;
  
  /** @brief dual-pol hid graupel */
  static const int HID_GRAUPEL;

  /** @brief dual-pol hid hail */
  static const int HID_HAIL;

  /** @brief dual-pol hid unknown */
  static const int HID_UNKNOWN;

  
  // Input RWIS Road-State definitions
  
  /** @brief rwis road-state no report */
  static const int RS_NO_REPORT;

  /** @brief rwis road-state dry */
  static const int RS_DRY;

  /** @brief rwis road-state moist */
  static const int RS_MOIST;

  /** @brief rwis road-state moist chemically treated */
  static const int RS_MOIST_CHEM_TMT;

  /** @brief rwis road-state wet */
  static const int RS_WET;

  /** @brief rwis road-state wet chemically treated */
  static const int RS_WET_CHEM_TMT;

  /** @brief rwis road-state ice */
  static const int RS_ICE;

  /** @brief rwis road-state frost */
  static const int RS_FROST;

  /** @brief rwis road-state snow */
  static const int RS_SNOW;

  /** @brief rwis road-state snow ice watch */
  static const int RS_SNOW_ICE_WACH;

  /** @brief rwis road-state snow ice warning */
  static const int RS_SNOW_ICE_WARN;

  /** @brief rwis wet above freezing */
  static const int RS_WET_ABOVE_FRZ;

  /** @brief rwis road-state wet below freezing */
  static const int RS_WET_BELOW_FRZ;

  /** @brief rwis road-state absorption */
  static const int RS_ABSORPTION;

  /** @brief rwis road-state absorption at dewpoint */
  static const int RS_ABSORPTION_DEWPT;

  /** @brief rwis road-state dew */
  static const int RS_DEW;

  /** @brief rwis road-state black ice */
  static const int RS_BLACK_ICE;

  /** @brief rwis road-state other */
  static const int RS_OTHER;

  /** @brief rwis road-state slush */
  static const int RS_SLUSH;

  
  // Input Wind-speed threshold for blowing snow
  static const int BLOWING_SNOW_WIND_SPEED;


  // Precip Type enumeration

  /** @brief precip type: no-precip */
  static const int NO_PRECIP;
  
  /** @brief precip type: rain */
  static const int RAIN;
  
  /** @brief precip type: mix */
  static const int MIX;
  
  /** @brief precip type: snow */
  static const int SNOW;

  /** @brief precip type: hail */
  static const int HAIL;
  
  
  // Precip Intensity enumeration
  
  /** @brief precip intensity: no-precip */
  //static const int NO_PRECIP; // defined above
  
  /** @brief precip intensity: light-precip */
  static const int LIGHT_PRECIP;
  
  /** @brief precip intensity: moderate-precip */
  static const int MODERATE_PRECIP;
  
  /** @brief precip intensity: heavy-precip */
  static const int HEAVY_PRECIP;
 
  /** @brief precip intensity: road-splash */
  static const int ROAD_SPLASH;

  
  // Pavement Condition enumeration
  
  /** @brief pavement condition: dry-pavement */
  static const int DRY_PAVEMENT;

  /** @brief pavement condition: wet-pavement */
  static const int WET_PAVEMENT;

  /** @brief pavement condition: snow-covered */
  static const int SNOW_COVERED;

  /** @brief pavement condition: ice-covered */
  static const int ICE_COVERED;
  
  /** @brief pavement condition: hydroplane */
  static const int HYDROPLANE;
  
  /** @brief pavement condition: black-ice */
  static const int BLACK_ICE;
  
  /** @brief pavement condition: dry-wet-pavement */
  static const int DRY_WET_PAVEMENT;
  
  /** @brief pavement condition: dry-frozen-pavement */
  static const int DRY_FROZEN_PAVEMENT;

  
  // Visibility enumeration
  
  /** @brief visibility: normal */
  static const int VIS_NORMAL;

  /** @brief visibility: low */
  static const int VIS_LOW;
  
  /** @brief visibility: heavy-rain */
  static const int VIS_HEAVY_RAIN;

  /** @brief visibility: heavy-snow */
  static const int VIS_HEAVY_SNOW;

  /** @brief visibility: blowing-snow */
  static const int VIS_BLOWING_SNOW;

  /** @brief visibility: fog */
  static const int VIS_FOG;
  
  /** @brief visibility: haze */
  static const int VIS_HAZE;
  
  /** @brief visibility: dust */
  static const int VIS_DUST;
  
  /** @brief visibility: smoke */
  static const int VIS_SMOKE;


  // Confidence field weights

  /** @brief forecast field weight for assessment confidence calculation */
  static const float fcst_wgt;
  
  /** @brief forecast field input confidence for assessment confidence calculation */
  static const float fcst_input_conf;
  
  /** @brief vdt air-temp weight for assessment confidence calculation */
  static const float air_temp_wgt;
  
  /** @brief vdt dual-pol-hid weight for assessment confidence calculation */
  static const float dual_pol_wgt;
  
  /** @brief vdt radar_ref weight for assessment confidence calculation */
  static const float radar_ref_wgt;
  
  /** @brief vdt wipers weight for assessment confidence calculation */
  static const float wipers_wgt;

  /** @brief vdt speed-ratio weight for assessment confidence calculation */
  static const float speed_ratio_wgt;

  /** @brief vdt headlights weight for assessment confidence calculation */
  static const float lights_wgt;
  
  /** @brief vdt road-temp weight for assessment confidence calculation */
  static const float road_temp_wgt;
  
  /** @brief precip-type weight for assessment confidence calculation */
  static const float precip_type_wgt;

  /** @brief precip-intensity weight for assessment confidence calculation */
  static const float precip_intensity_wgt;
  
  /** @brief fog weight for assessment confidence calculation */
  static const float fog_wgt;

  /** @brief haze weight for assessment confidence calculation */
  static const float haze_wgt;

  /** @brief normal vis weight for assessment confidence calculation */
  static const float normal_wgt;

  
  /** @brief constructor */
  rwx_road_segment_assessment();

  /** @brief performs assessment of precipitation type, pavement condition, and visibility
   * @param[in] seg_stat  vdt road segment statistic value
   */
  void perform_assessment(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief, uses previous rwh data to determine if there is a pavement-condtion hazard on the road, even if precip is not currently falling
    * @param[in] seg_stat vdt road segment statistic value
    * @return pavement condition  or fill value 
   */
  int determine_prev_pavement_condition(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief, returns the rwis road-state integer value. It QC's the road-state value by cross-checking with air-T
    * @param[in] seg_stat vdt road segment statistic value
    * @return rwis road state or fill value 
   */
  int get_rwis_road_state(vdt_probe_message_qc_statistics& seg_stat);
  
  /** @brief sets precipitation thresholds for determning precip-intensity based on air temperature 
   * @param[in] seg_stat vdt road segment statistic value
   * [out] sets thresholds for fcst precip and vdt radar based precip thresholds
   */
  void set_precip_thresholds(vdt_probe_message_qc_statistics& seg_stat, float &FCST_LIGHT_PRECIP, float &FCST_MODERATE_PRECIP, float &FCST_HEAVY_PRECIP, float &RADAR_LIGHT_PRECIP, float &RADAR_MODERATE_PRECIP, float &RADAR_HEAVY_PRECIP);
  
  /** @brief makes an assessment about precipitation type
   * @param[in] seg_stat  vdt road segment statistic value
   * @return precip type enumeration (0,1,2,3)
   */
  int precip_type_assessment(vdt_probe_message_qc_statistics& seg_stat, float &confidence);
  
  /** @brief makes an assessment about precipitation intensity
   * @param[in] seg_stat  vdt road segment statistic value
   * @param[in] precip type value
   * @param[in] all fcst and radar based precip thresholds
   * @return precip intesity enumeration (0,1,2,3)
   */
  int precip_intensity_assessment(vdt_probe_message_qc_statistics& seg_stat, const int precip_type, const float FCST_LIGHT_PRECIP, const float FCST_MODERATE_PRECIP, const float FCST_HEAVY_PRECIP, const float RADAR_LIGHT_PRECIP, const float RADAR_MODERATE_PRECIP, const float RADAR_HEAVY_PRECIP, float &confidence);
  
  /** @brief makes an assessment about pavement condition
   * @param[in] seg_stat  vdt road segment statistic value
   * @param[in] precip type value and confidence
   * @param[in] precip intensity value and confidence
   * @return pavement condition  or fill value
   */
  int pavement_condition_assessment(vdt_probe_message_qc_statistics& seg_stat, const int precip_type, const float precip_type_input_conf, const int precip_intensity, const float precip_intensity_input_conf, float &confidence);
  
  /** @brief makes an assessment about visibility given segment statistics and a precipitation assessment
   * @param[in] seg_stat  vdt road segment statistic value
   * @param[in] precip_assessment  precipitation assessment value
   * @return visibility value or fill value
   */
  int visibility_assessment(vdt_probe_message_qc_statistics& seg_stat, const int precip_type, const float precip_type_input_conf, const int precip_intensity, const float precip_intensity_input_conf, float &confidence);
  
  /** @brief determines if the road is slick
   *  @param[in] seg_stat            vdt road segment statistic value
   *  @param[in] precip_type         derived precip-type field
   *  @param[in] precip_intensity    derived precip-intensity field
   *  @param[in] pavement_condition  derived pavement-condition field
   *  @return 1 for slick pavement 0 for not slick
   */ 
  bool pavement_slickness_assessment(vdt_probe_message_qc_statistics& seg_stat, const int precip_type, const int precip_intensity, const int pavement_condition);
  
  /** @brief determines if the road is slick under dry pavement conditions based on abs, trac and stab
   *  @param[in] seg_stat            vdt road segment statistic value
   *  @return 1 for slick pavement 0 for not slick
   */ 
  bool dry_pavement_slickness_assessment(vdt_probe_message_qc_statistics& seg_stat);
  
  /** @brief returns basic pavement condition assessment using the following fields
   * @param[in] precip_type         derived precip-type field
   * @param[in] precip_intensity    derived precip-intensity field
   * @param[in] road_temp           seg_stats road-temp field
   * @return pavement condition or fill-value
   */
  float get_pavement_condition_basic(float precip_type, float precip_intensity, float road_temp);
  
  /** @brief gets the derived preciptation intensity */
  int get_precip_intensity() const { return precip_intensity; };
  
  /** @brief gets the derived preciptation intensity confidence */
  float get_precip_intensity_confidence() const { return precip_intensity_confidence; };
  
  /** @brief gets the derived preciptation type */
  int get_precip_type() const { return precip_type; };

  /** @brief gets the derived preciptation type confidence */
  float get_precip_type_confidence() const { return precip_type_confidence; };
  
  /** @brief gets the derived pavement condition */
  int get_pavement_condition() const { return pavement_condition; };
  
  /** @brief gets the derived pavement condition confidence */
  float get_pavement_condition_confidence() const { return pavement_condition_confidence; };
  
  /** @brief gets the derived pavement slickness boolean */
  bool get_pavement_slickness() const { return pavement_slickness; };
  
  /** @brief gets the derived visibility */
  int get_visibility() const { return visibility; };

  /** @brief gets the derived visibility confidence */
  float get_visibility_confidence() const { return visibility_confidence; };
  
  /** @brief gets the road segment id */
  int get_road_segment_id() const { return road_segment_id; };

  /** @brief gets the string associated with the precip type */
  string get_precip_type_str(int precip_type);
  
  /** @brief gets the string associated with the precip intensity */
  string get_precip_intensity_str(int precip_intensity);

  /** @brief gets the string associated with the pavement condition */
  string get_pavement_condition_str(int pavement_condition);
  
  /** @brief gets the string associated with the visibility */
  string get_visibility_str(int visibility);
  
  /** @brief assignment operator */
  rwx_road_segment_assessment & operator=(const rwx_road_segment_assessment &assessment);
  
private:

  /** @brief threshold for indicating forecast light-precip */
  float fcst_light_precip;

  /** @brief threshold for indicating forecast moderate-precip */
  float fcst_moderate_precip;

  /** @brief threshold for indicating forecast heavy-precip */
  float fcst_heavy_precip;

  /** @brief threshold for indicating radar-based light-precip */
  float radar_light_precip;

  /** @brief threshold for indicating radar-based moderate-precip */
  float radar_moderate_precip;

  /** @brief threshold for indicating radar-based heavy-precip */
  float radar_heavy_precip;
  
  /** @brief precipitation type */
  int precip_type;

  /** @brief precipitation type confidence */
  float precip_type_confidence;
  
  /** @brief precipitation intensity */
  int precip_intensity;
  
  /** @brief precipitation intensity confidence */
  float precip_intensity_confidence;
  
  /** @brief previous pavement condition, from previous rwh files */
  int prev_pavement_condition;
  
  /** @brief pavement condition */
  int pavement_condition;
  
  /** @brief pavement condition confidence */
  float pavement_condition_confidence;

  /** @brief pavement slickness */
  bool pavement_slickness;

  /** @brief visibility */
  int visibility;
  
  /** @brief visibility confidence */
  float visibility_confidence;
  
  /** @brief road_segment_id */
  int road_segment_id;
  
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

  /** @brief get the environment (ancillary) air temperature
   * @param[in] seg_stat  vdt road segment statistic value
   * @return environmental air temperature value
   */
  float get_env_air_temp(vdt_probe_message_qc_statistics& seg_stat);

  /** @brief get the environment (ancillary) dewpoint temperature
   * @param[in] seg_stat  vdt road segment statistic value
   * @return environmental dew temperature value
   */
  float get_env_dew_temp(vdt_probe_message_qc_statistics& seg_stat);
  
  /** @brief determine what temperature value to use
   * @params[i] mobile-temperature (or dewpoint) and environmental temperature (or dewpoint)
   * @return air temperature (or dewpoint) value
   */
  float determine_temp(float mobile_temp, float env_temp);

  /** @brief calculates fog interest (fuzzy logic)
   * @params[in] relative-humidity, speed-ratio, station-visibility, metar-wx-visibility, percentage of fog-lights on, high-beams on
   * @return fog interest (0-1)
   */
  float calc_fog_interest(float rh, float speed_ratio, float station_vis, float wx_visibility, float percent_fog_lights, float percent_high_beams);

  /** @brief calculates haze interest (fuzzy logic)
   * @params[in] relative-humidity, speed-ratio, station-visibility, metar-wx-visibility, percentage of fog-lights on, high-beams on
   * @return fog interest (0-1)
   */
  float calc_haze_interest(float rh, float speed_ratio, float station_vis, float wx_visibility, float percent_fog_lights, float percent_high_beams);

  /** @brief calculates normal visibility interest (fuzzy logic)
   * @params[in] relative-humidity, speed-ratio, station-visibility, metar-wx-visibility, percentage of fog-lights on, high-beams on
   * @return fog interest (0-1)
   */
  float calc_normal_interest(float rh, float speed_ratio, float station_vis, float wx_visibility, float percent_fog_lights, float percent_high_beams);
  
  /** @brief calculates relative-humidity from air-temp and dewpoint using vapor pressure calculation
   * @params[in] temp and dewpt in Celsius
   * @return relative humidity (0-1)
   */
  float calc_rh(float temp, float dewpt);
  
  /** @brief calculats vapor pressure used in relative-humidity calculaton
   * @params[in] temperature (or dewpt) in Celsius
   * @return vapor pressure in millibars
   */
  float calc_pr_vapr(float temp);

  /** @brief generic function to calculate a percentage 
   * @params[in] num_values, total_num_values
   * @returns percentage (0-1)
   */
  float calc_percentage(float num_values, float total_values);
};


#endif /* RWX_ROAD_SEGMENT_ASSESSMENT_HH */
