// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include <vdt4/vdt_const.hh>
#include "rwx_road_segment_assessment.hh"

// Forecast precip-type
const int rwx_road_segment_assessment::FCST_NONE = 0;
const int rwx_road_segment_assessment::FCST_RAIN = 1;
const int rwx_road_segment_assessment::FCST_SNOW = 2;
const int rwx_road_segment_assessment::FCST_ICE = 5;

// Forecast precip-intensity thresholds
const int rwx_road_segment_assessment::FCST_LIGHT_PRECIP_WINTER = 0.254; // <= 0.254 mm. (0.01 in.)
const int rwx_road_segment_assessment::FCST_MODERATE_PRECIP_WINTER = 2.54; // <= 2.54 mm. (0.10 in.)
// FCST_HEAVY_PRECIP_WINTER is: > 2.54 mm (0.10 in.)
const int rwx_road_segment_assessment::FCST_LIGHT_PRECIP_SUMMER = 2.54; // <= 2.54 mm. (0.10 in.)
const int rwx_road_segment_assessment::FCST_MODERATE_PRECIP_SUMMER = 7.62; // <= 7.62 mm. (0.30 in.)
// FCST_HEAVY_PRECIP_SUMMER is: > 7.62 mm (0.30 in.)

// VDT radar reflectivity precip-intensity thresholds 
const int rwx_road_segment_assessment::DEF_RADAR_LIGHT_PRECIP = 15; // <= 15 reflectivity (dBZ)
const int rwx_road_segment_assessment::DEF_RADAR_MODERATE_PRECIP = 30; // <= 30 reflectivity (dBZ)
// DEF_RADAR_HEAVY_PRECIP is: > 30 reflectivity (dBZ)
const int rwx_road_segment_assessment::WINTER_RADAR_LIGHT_PRECIP = 10; // <= 10 reflectivity (dBZ)
const int rwx_road_segment_assessment::WINTER_RADAR_MODERATE_PRECIP = 20; // <= 20 reflectivity (dBZ)
// WINTER_RADAR_HEAVY_PRECIP is: > 20 reflectivity (dBZ)
const int rwx_road_segment_assessment::SUMMER_RADAR_LIGHT_PRECIP = 20; // <= 20 reflectivity (dBZ)
const int rwx_road_segment_assessment::SUMMER_RADAR_MODERATE_PRECIP = 40; // <= 40 reflectivity (dBZ)
// SUMMER_RADAR_HEAVY_PRECIP is: > 40 reflectivity (dBZ)

// Wind-speed threshold for blowing snow (m/s)
const int rwx_road_segment_assessment::BLOWING_SNOW_WIND_SPEED = 10; // 10 m/s

// Precip Type enumeration
const int rwx_road_segment_assessment::NO_PRECIP = 0;
const int rwx_road_segment_assessment::RAIN = 1;
const int rwx_road_segment_assessment::MIX = 2;
const int rwx_road_segment_assessment::SNOW = 3;

// Precip Intensity enumeration
//const int rwx_road_segment_assessment::NO_PRECIP = 0;  // defined above
const int rwx_road_segment_assessment::LIGHT_PRECIP = 1;
const int rwx_road_segment_assessment::MODERATE_PRECIP = 2;
const int rwx_road_segment_assessment::HEAVY_PRECIP = 3;
const int rwx_road_segment_assessment::ROAD_SPLASH = 4;

// Pavement Condition enumeration
const int rwx_road_segment_assessment::DRY_PAVEMENT = 0;
const int rwx_road_segment_assessment::WET_PAVEMENT = 1;
const int rwx_road_segment_assessment::SNOW_COVERED = 2;
const int rwx_road_segment_assessment::ICE_COVERED = 3;
const int rwx_road_segment_assessment::HYDROPLANE = 4;
const int rwx_road_segment_assessment::BLACK_ICE = 5;
const int rwx_road_segment_assessment::DRY_WET_PAVEMENT = 6;
const int rwx_road_segment_assessment::DRY_FROZEN_PAVEMENT = 7;

// Visibility enumeration 
const int rwx_road_segment_assessment::VIS_NORMAL = 0;
const int rwx_road_segment_assessment::VIS_LOW = 1;
const int rwx_road_segment_assessment::VIS_HEAVY_RAIN = 2;
const int rwx_road_segment_assessment::VIS_HEAVY_SNOW = 3;
const int rwx_road_segment_assessment::VIS_BLOWING_SNOW = 4;
const int rwx_road_segment_assessment::VIS_FOG = 5;
const int rwx_road_segment_assessment::VIS_HAZE = 6;
const int rwx_road_segment_assessment::VIS_DUST = 7;
const int rwx_road_segment_assessment::VIS_SMOKE = 8;

// Confidence field weights

const float rwx_road_segment_assessment::fcst_wgt = 1.0;
const float rwx_road_segment_assessment::fcst_input_conf = 1.0;

const float rwx_road_segment_assessment::air_temp_wgt = 0.50;

const float rwx_road_segment_assessment::radar_ref_wgt = 0.50;
const float rwx_road_segment_assessment::wipers_wgt = 0.30;
const float rwx_road_segment_assessment::speed_ratio_wgt = 0.10;
const float rwx_road_segment_assessment::lights_wgt = 0.10;

const float rwx_road_segment_assessment::road_temp_wgt = 0.50;
const float rwx_road_segment_assessment::precip_type_wgt = 0.20;
const float rwx_road_segment_assessment::precip_intensity_wgt = 0.30;

const float rwx_road_segment_assessment::fog_wgt = 0.50;
const float rwx_road_segment_assessment::gen_vis_wgt = 0.30;

rwx_road_segment_assessment::rwx_road_segment_assessment()
{
  precip_type = vdt_const::FILL_VALUE;
  pavement_condition = vdt_const::FILL_VALUE;
  pavement_slickness = vdt_const::FILL_VALUE;
  visibility = vdt_const::FILL_VALUE;
}


void rwx_road_segment_assessment::perform_assessment(vdt_probe_message_qc_statistics& seg_stat)
{
  road_segment_id = seg_stat.id;  
  precip_type = precip_type_assessment(seg_stat, precip_type_confidence);
  precip_intensity = precip_intensity_assessment(seg_stat, precip_type, precip_intensity_confidence);
  pavement_condition = pavement_condition_assessment(seg_stat, precip_type, precip_type_confidence, precip_intensity, precip_intensity_confidence, pavement_condition_confidence);
  visibility = visibility_assessment(seg_stat, precip_type, precip_type_confidence, precip_intensity, precip_intensity_confidence, visibility_confidence);
  pavement_slickness = pavement_slickness_assessment(seg_stat, precip_type, precip_intensity, pavement_condition);
  
  // We don't think we need this for now...taken care of in the algorithms
  // Modify pavement slickness based on pavement conditon
  //if(pavement_condition != SNOW_COVERED &&
  //  pavement_condition != ICE_COVERED &&
  //  pavement_condition != HYDROPLANE &&
  //  pavement_condition != BLACK_ICE)
  //  pavement_slickness = 0;

}


int rwx_road_segment_assessment::precip_type_assessment(vdt_probe_message_qc_statistics& seg_stat, float &confidence)
{
  float precip_type = vdt_const::FILL_VALUE;
  confidence = 0;
  
  // Get forecast fields
  float fcst_precip_type = seg_stat.fcst_precip_type;
  float fcst_air_temp = seg_stat.fcst_air_temp;
  
  // Forecast case
  if(fcst_precip_type != vdt_const::FILL_VALUE && fcst_air_temp != vdt_const::FILL_VALUE)
    {
      if(fcst_precip_type == FCST_NONE)
	precip_type =  NO_PRECIP;
      else if(fcst_precip_type == FCST_RAIN)
	{
	  if(fcst_air_temp < 1.5)
	    precip_type =  MIX;
	  else
	    precip_type =  RAIN;
	}
      else if(fcst_precip_type == FCST_SNOW)
	{
	  if(fcst_air_temp > 1.5)
	    precip_type =  MIX;
	  else
	    precip_type =  SNOW;
	}
      else if(fcst_precip_type == FCST_ICE)
	{
	  if(fcst_air_temp < -2)
	    precip_type =  SNOW;
	  else if(fcst_air_temp > 2)
	    precip_type =  RAIN;
	  else
	    precip_type =  MIX;
	}
      
      confidence = confidence + (fcst_input_conf * fcst_wgt);
      return precip_type;
    }
  
  // Determine what VDT air temp we should use
  // for vdt precip-type assessment
  //
  float mobile_air_temp = get_air_temp(seg_stat);
  float env_air_temp = get_env_air_temp(seg_stat);
  float air_temp = determine_temp(mobile_air_temp, env_air_temp);
  //printf("air_temp: %f\n", air_temp);
  
  // Hardwire the input field confidence for now
  // eventually this will be coming from the seg-stats file
  float air_temp_input_conf = 1.0;
  
  // Do we want to use radar_precip_type first?
  
  // VDT case
  if(air_temp != vdt_const::FILL_VALUE) // vdt case
    {
      if(air_temp > 2)
	precip_type =  RAIN;
      else if(air_temp < -2)
	precip_type =  SNOW;
      else
	precip_type =  MIX;
      
      confidence = confidence + (air_temp_input_conf * air_temp_wgt);
    }
  
  return precip_type;
}


int rwx_road_segment_assessment::precip_intensity_assessment(vdt_probe_message_qc_statistics& seg_stat, const int precip_type, float &confidence)
{
  float precip_intensity = vdt_const::FILL_VALUE;
  confidence = 0;
  
  // Get forecast fields
  float fcst_precip_rate = seg_stat.fcst_precip_rate;
  
  // Forecast case
  if(fcst_precip_rate != vdt_const::FILL_VALUE)
    {
      if(fcst_precip_rate <= 0)
	precip_intensity =  NO_PRECIP;
      else if(fcst_precip_rate > 0 && fcst_precip_rate <= FCST_LIGHT_PRECIP_WINTER)
	precip_intensity =  LIGHT_PRECIP;
      else if(fcst_precip_rate > FCST_LIGHT_PRECIP_WINTER && fcst_precip_rate <= FCST_MODERATE_PRECIP_WINTER)
	precip_intensity =  MODERATE_PRECIP;
      else
	precip_intensity =  HEAVY_PRECIP;
      
      confidence = confidence + (fcst_input_conf * fcst_wgt);
      return precip_intensity;
    }
  
  // Get VDT fields 
  float radar_ref = seg_stat.radar_ref;
  float speed_ratio = seg_stat.speed_ratio;
  
  int num_msg_valid_lights = seg_stat.num_msg_valid_lights;
  int num_lights_off = seg_stat.num_lights_off;
  float percent_lights_off = calc_percentage((float)num_lights_off, (float)num_msg_valid_lights);
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: num_msg_valid_lights: %d, num_lights_off: %d, percent_lights_off: %f\n",  num_msg_valid_lights, num_lights_off, percent_lights_off);
  
  // Determine if we have any wiper data
  int wipers_flag = 0;
  if(seg_stat.num_wipers_off != vdt_const::FILL_VALUE ||
     seg_stat.num_wipers_intermittent != vdt_const::FILL_VALUE ||
     seg_stat.num_wipers_low != vdt_const::FILL_VALUE ||
     seg_stat.num_wipers_high != vdt_const::FILL_VALUE)
    wipers_flag = 1;
  
  bool wipers_off_flag = wipers_off(seg_stat);
  bool wipers_on_flag = wipers_on(seg_stat);
  bool wipers_interm_flag = wipers_intermittent(seg_stat);
  bool wipers_low_flag = wipers_low(seg_stat);
  bool wipers_high_flag = wipers_high(seg_stat);
  
  // Hardwire the input field confidence for now
  // eventually this will be coming from the seg-stats file
  float radar_ref_input_conf = 1.0;
  float wipers_input_conf = 1.0;
  float speed_ratio_input_conf = 1.0;
  float lights_input_conf = 1.0;
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: wipers_flag: %d, wipers_off_flag: %d, wipers_on_flag: %d, wipers_interm_flag: %d, wipers_low_flag: %d, wipers_high_flag: %d\n", wipers_flag, wipers_off_flag, wipers_on_flag,  wipers_interm_flag, wipers_low_flag, wipers_high_flag);

  // Use precip-type to determine radar-composite-reflectivity (dBz) thresholds
  int RADAR_LIGHT_PRECIP;
  int RADAR_MODERATE_PRECIP;
  if(precip_type == MIX || precip_type == SNOW)
    {
      RADAR_LIGHT_PRECIP = WINTER_RADAR_LIGHT_PRECIP;
      RADAR_MODERATE_PRECIP = WINTER_RADAR_MODERATE_PRECIP;
    }
  else if(precip_type == RAIN)
    {
      RADAR_LIGHT_PRECIP = SUMMER_RADAR_LIGHT_PRECIP;
      RADAR_MODERATE_PRECIP = SUMMER_RADAR_MODERATE_PRECIP;
    }
  else
    {
      RADAR_LIGHT_PRECIP = DEF_RADAR_LIGHT_PRECIP;
      RADAR_MODERATE_PRECIP = DEF_RADAR_MODERATE_PRECIP;
    }
  
  // VDT case using just radar-composite-reflectivity
  if(radar_ref != vdt_const::FILL_VALUE)
    {
      if(radar_ref <= 0)
	precip_intensity =  NO_PRECIP;
      else if(radar_ref > 0 && radar_ref <= RADAR_LIGHT_PRECIP)
	precip_intensity =  LIGHT_PRECIP;
      else if(radar_ref > RADAR_LIGHT_PRECIP && radar_ref <= RADAR_MODERATE_PRECIP)
	precip_intensity =  MODERATE_PRECIP;
      else
	precip_intensity =  HEAVY_PRECIP;
      
      confidence = confidence +  (radar_ref_input_conf * radar_ref_wgt);
    }
  
  // VDT case: modify intensity based on wipers or determine intensity if we just have wipers and no radar data
  if(wipers_flag == 1)
    {
      if(precip_intensity == vdt_const::FILL_VALUE) // This for the case with no radar data
	{
	  if(wipers_off_flag)
	    precip_intensity =  NO_PRECIP;
	  else if(wipers_interm_flag)
	    precip_intensity = LIGHT_PRECIP;
	  else // wipers_low_flag || wipers_high_flag
	    precip_intensity = MODERATE_PRECIP;
	}
      else if(precip_intensity == NO_PRECIP)
	{
	  if(wipers_on_flag)
	    precip_intensity = ROAD_SPLASH;
	}
      else if(precip_intensity == LIGHT_PRECIP)
	{
	  if(wipers_low_flag || wipers_high_flag)
	    precip_intensity = MODERATE_PRECIP;
	}
      else if(precip_intensity == MODERATE_PRECIP)
	{
	  if(wipers_high_flag)
	    precip_intensity = HEAVY_PRECIP;
	  else if(wipers_off_flag)  
	    precip_intensity = LIGHT_PRECIP;
	}
      else // precip_intensity == HEAVY_PRECIP
	{
	  if(wipers_interm_flag)  
	    precip_intensity = MODERATE_PRECIP;
	  else if(wipers_off_flag)
	    precip_intensity = LIGHT_PRECIP;
	}      
      confidence = confidence +  (wipers_input_conf * wipers_wgt);
    }
  
  // VDT case: modify intensity based on speed-ratio
  if(speed_ratio != vdt_const::FILL_VALUE)
    {
      if(precip_intensity == HEAVY_PRECIP)
	{
	  if(speed_ratio >= 0.7)
	    precip_intensity = MODERATE_PRECIP;
	}
      confidence = confidence +  (speed_ratio_input_conf * speed_ratio_wgt);
    }
  
  // VDT case: modify intensity based on headlights
  if(percent_lights_off != vdt_const::FILL_VALUE)
    {
      if(precip_intensity == MODERATE_PRECIP)
	{
	  if(percent_lights_off > 0.50)
	    precip_intensity = LIGHT_PRECIP;
	}
      else if(precip_intensity == HEAVY_PRECIP)
	{
	  if(percent_lights_off > 0.50)
	    precip_intensity = MODERATE_PRECIP;
	}
      confidence = confidence +  (lights_input_conf * lights_wgt);
    }
  
  return precip_intensity;
}


int rwx_road_segment_assessment::pavement_condition_assessment(vdt_probe_message_qc_statistics& seg_stat, const int precip_type, const float precip_type_input_conf, const int precip_intensity, const float precip_intensity_input_conf, float &confidence)
{
  float pavement_condition = vdt_const::FILL_VALUE;
  confidence = 0;

  // Get forecast fields
  float fcst_road_temp = seg_stat.fcst_road_temp;
  float fcst_road_water_phase_type = seg_stat.fcst_road_water_phase_type; // Not sure if we can use this as it shows wet-pavement for NO_PRECIP
  //printf("fcst_road_temp: %f, fcst_road_water_phase_type: %f\n", fcst_road_temp, fcst_road_water_phase_type);
    
  // Forecast case
  if(fcst_road_temp != vdt_const::FILL_VALUE && precip_type != vdt_const::FILL_VALUE && precip_intensity != vdt_const::FILL_VALUE)
    {
      pavement_condition = get_pavement_condition_basic(precip_type, precip_intensity, fcst_road_temp);
      confidence = confidence + (fcst_input_conf * fcst_wgt);
      return pavement_condition;
    }
  
  // Get VDT fields
  float road_temp = seg_stat.surface_temp_mean;
  bool black_ice_possible  = 0;
  bool hydroplane_possible = 0;
  
  // Hardwire the input field confidence for now
  // eventually this will be coming from the seg-stats file
  float road_temp_input_conf = 1.0;
  
  // VDT case with road-temp, precip-type and precip-intensity
  if(road_temp != vdt_const::FILL_VALUE && precip_type != vdt_const::FILL_VALUE && precip_intensity != vdt_const::FILL_VALUE)
    {
      //printf("pavement condition assessment: with road_temp, precip_type and precip_intensity\n");
      //printf("road_temp_input_conf: %f, road_temp_wgt: %f, precip_type_input_conf: %f, precip_type_wgt: %f, precip_intensity_input_conf: %f, precip_intensity_wgt: %f\n", road_temp_input_conf, road_temp_wgt, precip_type_input_conf, precip_type_wgt, precip_intensity_input_conf, precip_intensity_wgt);
      pavement_condition = get_pavement_condition_basic(precip_type, precip_intensity, road_temp);
      if(pavement_condition == DRY_PAVEMENT && road_temp < 1)
	black_ice_possible = 1;
      else if(pavement_condition == WET_PAVEMENT)
	hydroplane_possible = 1;
      
      confidence = confidence + (road_temp_input_conf * road_temp_wgt) + (precip_type_input_conf * precip_type_wgt) + (precip_intensity_input_conf * precip_intensity_wgt);
    }
  else if(precip_type != vdt_const::FILL_VALUE && precip_intensity != vdt_const::FILL_VALUE) // VDT case with just precip-type and precip-intensity
    {
      //printf("pavement condition assessment: with precip_type and precip_intensity\n");
      //printf("precip_type_input_conf: %f, precip_type_wgt: %f, precip_intensity_input_conf: %f, precip_intensity_wgt: %f\n", precip_type_input_conf, precip_type_wgt, precip_intensity_input_conf, precip_intensity_wgt);
      if(precip_intensity == NO_PRECIP)
	pavement_condition = DRY_PAVEMENT;
      else if(precip_type == RAIN)
	{
	  pavement_condition = WET_PAVEMENT;
	  hydroplane_possible = 1;
	}
      else if(precip_type == MIX && precip_intensity != ROAD_SPLASH)
	pavement_condition = ICE_COVERED;
      else if(precip_type == MIX && precip_intensity == ROAD_SPLASH)
	{
	  pavement_condition = WET_PAVEMENT;
	  hydroplane_possible = 1;
	}
      else
	pavement_condition = SNOW_COVERED;
      
      confidence = confidence + (precip_type_input_conf * precip_type_wgt) + (precip_intensity_input_conf * precip_intensity_wgt);
    }
  else if(road_temp != vdt_const::FILL_VALUE) // VDT case with just road-temp
    {
      //printf("pavement condition assessment: with road_temp\n");
      //printf("road_temp_input_conf: %f, road_temp_wgt: %f\n", road_temp_input_conf, road_temp_wgt);
      if(road_temp <= 0)
	{
	  pavement_condition = DRY_FROZEN_PAVEMENT;
	  black_ice_possible = 1;
	}
      else
	{
	  pavement_condition = DRY_WET_PAVEMENT;
	  hydroplane_possible = 1;
	}
      confidence = confidence + (road_temp_input_conf * road_temp_wgt);
    }
  else if(precip_type != vdt_const::FILL_VALUE) // VDT case with just precip-type
    {
      //printf("pavement condition assessment: with precip-type\n");
      //printf("precip_type_input_conf: %f, precip_type_wgt: %f\n", precip_type_input_conf, precip_type_wgt);
      if(precip_type == RAIN)
	{
	  pavement_condition = DRY_WET_PAVEMENT;
	  hydroplane_possible = 1;
	}
      else if(precip_type == MIX || precip_type == SNOW)
	{
	  pavement_condition = DRY_FROZEN_PAVEMENT;
	  black_ice_possible = 1;
	}
      confidence = confidence + (precip_type_input_conf * precip_type_wgt);
    }
  
  // Determine if there is black-ice or hydroplane hazard
  bool pavement_slickness = pavement_slickness_assessment(seg_stat, precip_type, precip_intensity, pavement_condition);
  if(pavement_slickness)
    {
      if(black_ice_possible)
	pavement_condition = BLACK_ICE;
      else if(hydroplane_possible)
	pavement_condition = HYDROPLANE;
    }
  
  return pavement_condition;
}


int rwx_road_segment_assessment::visibility_assessment(vdt_probe_message_qc_statistics& seg_stat, const int precip_type, const float precip_type_input_conf, const int precip_intensity, const float precip_intensity_input_conf, float &confidence)
{
  int visibility =  vdt_const::FILL_VALUE;
  confidence = 0;
  
  // Get forecast fields
  float fcst_visibility = seg_stat.fcst_visibility;
  float fcst_wind_speed = seg_stat.fcst_wind_speed;
  float fcst_prob_fog = seg_stat.fcst_prob_fog;
  
  // Forecast case
  if(fcst_visibility != vdt_const::FILL_VALUE && precip_type != vdt_const::FILL_VALUE && precip_intensity != vdt_const::FILL_VALUE)
    {
      if(fcst_visibility < 10) // May need to adjust this threshold? Do we even want to use forecast visibility?
	{
	  if(precip_type == RAIN && precip_intensity == HEAVY_PRECIP)
	    visibility = VIS_HEAVY_RAIN;
	  else if(precip_type == SNOW && precip_intensity != NO_PRECIP)
	    {
	      if(fcst_wind_speed != vdt_const::FILL_VALUE && fcst_wind_speed > BLOWING_SNOW_WIND_SPEED)
		visibility = VIS_BLOWING_SNOW;
	      else if(precip_intensity == HEAVY_PRECIP)
		visibility = VIS_HEAVY_SNOW;
	    }
	  // If the visibility is still missing look for fog
	  // NOTE: We currently do not have prob_fog values from logicast rdwx files
	  //       so this is a placeholder for now
	  // We could use RH to calculate fog?
	  if(visibility == vdt_const::FILL_VALUE && fcst_prob_fog != vdt_const::FILL_VALUE && fcst_prob_fog > 0.5) 
	    visibility = VIS_FOG;
	}
      else
	{
	  visibility = VIS_NORMAL;
	}
      confidence = confidence + (fcst_input_conf * fcst_wgt);
      return visibility;
    }
  
  // Determine what VDT air temp and dewpt we should use
  // to calculate rh for fog calculation and generic visibility calculations
  //
  float mobile_air_temp = get_air_temp(seg_stat);
  float env_air_temp = get_env_air_temp(seg_stat);
  float air_temp = determine_temp(mobile_air_temp, env_air_temp);
  
  float mobile_dew_temp = seg_stat.dew_temp_mean;
  float env_dew_temp = seg_stat.nss_dew_temp_mean;
  float dew_temp = determine_temp(mobile_dew_temp, env_dew_temp);
  
  float rh = calc_rh(air_temp, dew_temp);  
  //printf("air_temp: %f, dew_temp: %f, rh: %f\n", air_temp, dew_temp, rh);
  
  // Get addtional VDT variables
  float wind_speed = seg_stat.nss_wind_speed_mean;
  
  float speed_ratio = seg_stat.speed_ratio;
  float env_vis = seg_stat.nss_prevail_vis_mean;

  int num_msg_valid_lights = seg_stat.num_msg_valid_lights;
  int num_lights_off = seg_stat.num_lights_off;
  int num_lights_fog = seg_stat.num_lights_fog;
  int num_lights_high_beam = seg_stat.num_lights_high_beam;
  float percent_lights_off = calc_percentage((float)num_lights_off, (float)num_msg_valid_lights);
  float percent_lights_fog = calc_percentage((float)num_lights_fog, (float)num_msg_valid_lights); 
  float percent_lights_high_beam = calc_percentage((float)num_lights_high_beam, (float)num_msg_valid_lights); 
  
  // Determine percent lights on from percent lights off
  float percent_lights_on;
  if(percent_lights_off != vdt_const::FILL_VALUE)
    percent_lights_on = 1 - percent_lights_off;
  else
    percent_lights_on = vdt_const::FILL_VALUE;
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: rh: %f, speed_ratio: %f, env_vis: %f, percent_lights_off: %f, percent_lights_on: %f, percent_lights_fog: %f, percent_lights_high_beam: %f\n", rh, speed_ratio, env_vis, percent_lights_off, percent_lights_on, percent_lights_fog, percent_lights_high_beam);
  
  // Calculate fog interest 
  float fog_interest = calc_fog_interest(rh, speed_ratio, env_vis, percent_lights_fog, percent_lights_high_beam);
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: fog_interest: %f\n", fog_interest);
  
  // Calculate generic visibility interest
  float gen_vis_interest = calc_generic_vis_interest(rh, speed_ratio, env_vis, percent_lights_on, percent_lights_fog, percent_lights_high_beam);
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: gen_vis_interest: %f\n", gen_vis_interest);
  
  // Hardwire the "input" confidence for now
  // Do we want to calculate this in the calc_fog and cal_generic_vis interest functions?
  float fog_input_conf = 1.0;
  float gen_vis_input_conf = 1.0;
  
  // VDT case precip-type and precip-intensity
  if(precip_type != vdt_const::FILL_VALUE && precip_intensity != vdt_const::FILL_VALUE)
    {
      if(precip_type == RAIN && precip_intensity == HEAVY_PRECIP)
	visibility = VIS_HEAVY_RAIN;
      else if(precip_type == SNOW && precip_intensity != NO_PRECIP)
	{
	  if(wind_speed != vdt_const::FILL_VALUE && wind_speed > BLOWING_SNOW_WIND_SPEED)
	    visibility = VIS_BLOWING_SNOW;
	  else if(precip_intensity == HEAVY_PRECIP)
	    visibility = VIS_HEAVY_SNOW;
	  else
	    visibility = VIS_NORMAL; // For LIGHT, MODERATE AND ROAD_SPLASH intensity for type SNOW
	}
      else if(precip_type == MIX && precip_intensity == HEAVY_PRECIP)
	visibility = VIS_LOW;
      else
	visibility = VIS_NORMAL;
      
      confidence = confidence + (precip_type_input_conf * precip_type_wgt) + (precip_intensity_input_conf * precip_intensity_wgt);
    }
  else if(precip_intensity != vdt_const::FILL_VALUE ) // VDT case with just precip-intensity 
    {
      if(precip_intensity == HEAVY_PRECIP)
	visibility = VIS_LOW;
      else
	visibility = VIS_NORMAL;
      
      confidence = confidence + (precip_intensity_input_conf * precip_intensity_wgt);
    }
  else // VDT case when precip-type and precip-intensity are missing
    {
      visibility = VIS_NORMAL;
      confidence = confidence + (gen_vis_input_conf * gen_vis_wgt);
    }
  
  // If visibility is still missing look for fog or generic low visibiltiy
  // We have higher confidence in the fog calculation?
  // Less confidence in generic vis function?
  if(visibility == vdt_const::FILL_VALUE || visibility == VIS_NORMAL)
    {
      if(fog_interest > 0.4001) // VDT case with fog_interest
	{
	  visibility = VIS_FOG;
	  confidence = confidence + (fog_input_conf * fog_wgt);
	}
      else if(gen_vis_interest > 0.5001) // VDT case with generic visibility interest
	{
	  visibility = VIS_LOW;
	  confidence = confidence + (gen_vis_input_conf * gen_vis_wgt);
	}
    }
  
  return visibility;
}


bool rwx_road_segment_assessment::pavement_slickness_assessment(vdt_probe_message_qc_statistics& seg_stat, const int precip_type, const int precip_intensity, const int pavement_condition)
{
  bool pavement_slickness = 0;
  
  // Determine precipitation interest
  float precip_intrst;
  if(precip_intensity != vdt_const::FILL_VALUE && precip_type != vdt_const::FILL_VALUE)
    {
      if(precip_intensity == NO_PRECIP)
	precip_intrst = -1;
      else if(precip_type == RAIN)
	{
	  if(precip_intensity == LIGHT_PRECIP || precip_intensity == MODERATE_PRECIP || precip_intensity == ROAD_SPLASH)
	    precip_intrst = -0.5;
	  else // precip_intensity == HEAVY_PRECIP
	    precip_intrst = 0;
	}
      else if(precip_type == MIX)
	{
	  precip_intrst = 0.5;
	}
      else // precip_type == SNOW
	{
	  if(precip_intensity == LIGHT_PRECIP || precip_intensity == MODERATE_PRECIP || precip_intensity == ROAD_SPLASH)
	    precip_intrst = 0.5;
	  else // precip_intensity == HEAVY_PRECIP
	    precip_intrst = 1;
	}
    }
  else
    precip_intrst = vdt_const::FILL_VALUE;
  
  // Determine pavement condition interest
  float pav_cond_intrst;
  if(pavement_condition != vdt_const::FILL_VALUE)
    {
      if(pavement_condition == DRY_PAVEMENT)
	pav_cond_intrst = -1;
      else if(pavement_condition == WET_PAVEMENT || pavement_condition == DRY_WET_PAVEMENT || pavement_condition == HYDROPLANE)
	pav_cond_intrst = 0;
      else if(pavement_condition == DRY_FROZEN_PAVEMENT)
	pav_cond_intrst = 0.5;
      else // pavement_condition == SNOW_COVERED || pavement_condition == ICE_COVERED || pavement_condition == BLACK_ICE
	pav_cond_intrst = 1;
    }
  else
    pav_cond_intrst = vdt_const::FILL_VALUE;

  // For stability-interest, yaw-iqr-interest, yaw-median-interest:
  // If input to each interest component is missing
  // set the interest to 0.
  // Since we are doing a weighted sum, interest values of 0
  // will contribute nothing to the sum.
  // Note that even if inputs are not missing, interest values
  // can still be 0.
  
  // Determine if we have any abs, trac or stab data
  int stab_fields_flag = 0;
  int num_abs_engaged = seg_stat.num_abs_engaged;
  int num_trac_engaged = seg_stat.num_trac_engaged;
  int num_stab_engaged = seg_stat.num_stab_engaged;

  if(num_abs_engaged != vdt_const::FILL_VALUE || num_trac_engaged != vdt_const::FILL_VALUE || 
     num_stab_engaged != vdt_const::FILL_VALUE)
    stab_fields_flag = 1;
  
  // Determine stability interest 
  float stab_intrst;
  if(stab_fields_flag)
    {
      bool abs_engaged_flag = abs_engaged(seg_stat);
      bool trac_engaged_flag = trac_engaged(seg_stat);
      bool stab_engaged_flag = stab_engaged(seg_stat);
      int num_engaged = 0;
      if(abs_engaged_flag)
	num_engaged++;
      if(trac_engaged_flag)
	num_engaged++;
      if(stab_engaged_flag)
	num_engaged++;
      
      // If we have at least two out three, set stab_intrst to 1
      if(num_engaged >= 2)
	stab_intrst = 1;
      else
	stab_intrst = 0;
    }
  else
    stab_intrst = 0;
  
  // Deterime yaw-iqr interest
  float yaw_iqr_intrst;
  float yaw_iqr25 = seg_stat.yaw_iqr25;
  float yaw_iqr75 = seg_stat.yaw_iqr75;
  if(yaw_iqr25 != vdt_const::FILL_VALUE && yaw_iqr75 != vdt_const::FILL_VALUE)
    {
      float yaw_iqr_diff = yaw_iqr75 - yaw_iqr25;
      if(yaw_iqr_diff <= 1)
	yaw_iqr_intrst = yaw_iqr_diff;
      else // > 1
	yaw_iqr_intrst = 1;
    }
  else
    yaw_iqr_intrst = 0;

  // Determine yaw-median interest
  float yaw_median_intrst;
  float yaw_min = seg_stat.yaw_min;
  float yaw_max = seg_stat.yaw_max;
  float yaw_median = seg_stat.yaw_median;
  
  if(yaw_min != vdt_const::FILL_VALUE && yaw_max != vdt_const::FILL_VALUE &&
     yaw_median != vdt_const::FILL_VALUE)
    {
      float yaw_min_diff = fabs(yaw_min - yaw_median);
      float yaw_max_diff = fabs(yaw_max - yaw_median);
      float yaw_diff;
      if(yaw_min_diff > yaw_max_diff)
	yaw_diff = yaw_min_diff;
      else // yaw_max_diff > yaw_min_diff
	yaw_diff = yaw_max_diff;
      
      if(yaw_diff <=1)
	yaw_median_intrst = yaw_diff;
      else // > 1
	yaw_median_intrst = 1;
    }
  else
    yaw_median_intrst = 0;

  // Set weights of fields for slickness interest;
  float precip_wgt = 0.3;
  float pav_cond_wgt = 0.3;
  float stab_wgt = 0.2;
  float yaw_iqr_wgt = 0.1;
  float yaw_median_wgt = 0.1;
  
  float slickness_intrst;
  if(precip_intrst != vdt_const::FILL_VALUE && pav_cond_intrst != vdt_const::FILL_VALUE)
    {
      slickness_intrst = (precip_wgt * precip_intrst) + (pav_cond_wgt * pav_cond_intrst) + (stab_wgt * stab_intrst) + 
	(yaw_iqr_wgt * yaw_iqr_intrst) + (yaw_median_wgt * yaw_median_intrst);
    }
  else
    slickness_intrst = vdt_const::FILL_VALUE;
  
  if(slickness_intrst != vdt_const::FILL_VALUE && slickness_intrst >= 0.44)
    pavement_slickness = 1;
  
  return pavement_slickness;
}


float rwx_road_segment_assessment::get_pavement_condition_basic(float precip_type, float precip_intensity, float road_temp)
{
  float pavement_condition = vdt_const::FILL_VALUE;
  
  if(road_temp != vdt_const::FILL_VALUE && precip_type != vdt_const::FILL_VALUE && precip_intensity != vdt_const::FILL_VALUE)
    {
      if(precip_intensity == NO_PRECIP)
	pavement_condition = DRY_PAVEMENT;
      else if(precip_type == RAIN)
	{
	  if(road_temp <= 0 && precip_intensity != ROAD_SPLASH)
	    pavement_condition = ICE_COVERED;
	  else
	    pavement_condition = WET_PAVEMENT;
	}
      else if(precip_type == MIX)
	{
	  if(road_temp <= 0 && precip_intensity != ROAD_SPLASH)
	    pavement_condition = ICE_COVERED;
	  else
	    pavement_condition = WET_PAVEMENT; 
	}
      else if(precip_type == SNOW)
	{
	  if(road_temp > -1 && road_temp < 1 && precip_intensity != ROAD_SPLASH)
	    pavement_condition = ICE_COVERED;
	  else if(road_temp >= 1 && precip_intensity != HEAVY_PRECIP)
	    pavement_condition = WET_PAVEMENT;
	  else if(road_temp > 2)
	    pavement_condition = WET_PAVEMENT;
	  else
	    pavement_condition = SNOW_COVERED;
	}
    }
  
  return pavement_condition;
}


int rwx_road_segment_assessment::get_wipers_on(vdt_probe_message_qc_statistics& seg_stat)
{
  int num_wipers_on = 0;

  if (seg_stat.num_wipers_intermittent != vdt_const::FILL_VALUE)
    {
    num_wipers_on += seg_stat.num_wipers_intermittent;
    }

  if (seg_stat.num_wipers_low != vdt_const::FILL_VALUE)
    {
    num_wipers_on += seg_stat.num_wipers_low;
    }
 
  if (seg_stat.num_wipers_high != vdt_const::FILL_VALUE)
    {
    num_wipers_on += seg_stat.num_wipers_high;
    }

  if (seg_stat.num_wipers_automatic != vdt_const::FILL_VALUE)
    {
    num_wipers_on += seg_stat.num_wipers_automatic;
    }

  return num_wipers_on;
}


float rwx_road_segment_assessment::get_air_temp(vdt_probe_message_qc_statistics& seg_stat)
{
  if (seg_stat.air_temp_mean == vdt_const::FILL_VALUE &&
      seg_stat.air_temp2_mean == vdt_const::FILL_VALUE)
    {
      return vdt_const::FILL_VALUE;
    }
  
  // Use air_temp_mean if it is there, if not fall back on air_temp2_mean
  float air_temp;
  if(seg_stat.air_temp_mean != vdt_const::FILL_VALUE)
    air_temp = seg_stat.air_temp_mean;
  else
    air_temp = seg_stat.air_temp2_mean;
  
  return air_temp;
}


float rwx_road_segment_assessment::get_env_air_temp(vdt_probe_message_qc_statistics& seg_stat)
{
  if (seg_stat.nss_air_temp_mean == vdt_const::FILL_VALUE &&
      seg_stat.model_air_temp == vdt_const::FILL_VALUE)
    {
      return vdt_const::FILL_VALUE;
    }
  
  // Use nss_air_temp_mean if it is there, if not fall back on model_air_temp
  float env_air_temp;
  if(seg_stat.nss_air_temp_mean != vdt_const::FILL_VALUE)
    env_air_temp = seg_stat.nss_air_temp_mean;
  else
    env_air_temp = seg_stat.model_air_temp;
  
  return env_air_temp;
}


bool rwx_road_segment_assessment::wipers_on(vdt_probe_message_qc_statistics& seg_stat)
{
  int num_wipers_on = get_wipers_on(seg_stat);
  return (num_wipers_on > 0);
}


bool rwx_road_segment_assessment::wipers_off(vdt_probe_message_qc_statistics& seg_stat)
{
  //return (seg_stat.num_wipers_off > seg_stat.num_wipers_intermittent &&
  //        seg_stat.num_wipers_off > seg_stat.num_wipers_high &&
  //        seg_stat.num_wipers_off > seg_stat.num_wipers_low);
  
  // This is a hack for the demo
  return (seg_stat.num_wipers_intermittent == 0 &&
          seg_stat.num_wipers_high == 0 &&
          seg_stat.num_wipers_low == 0);
}


bool rwx_road_segment_assessment::wipers_high(vdt_probe_message_qc_statistics& seg_stat)
{
  //return (seg_stat.num_wipers_high >= seg_stat.num_wipers_intermittent &&
  //        seg_stat.num_wipers_high >= seg_stat.num_wipers_off &&
  //        seg_stat.num_wipers_high >= seg_stat.num_wipers_low);
  
  // This is a hack for the demo
  return (seg_stat.num_wipers_high >= seg_stat.num_wipers_intermittent &&
          seg_stat.num_wipers_high >= seg_stat.num_wipers_low);
}


bool rwx_road_segment_assessment::wipers_intermittent(vdt_probe_message_qc_statistics& seg_stat)
{
  //return (seg_stat.num_wipers_intermittent >= seg_stat.num_wipers_low &&
  //        seg_stat.num_wipers_intermittent > seg_stat.num_wipers_high &&
  //        seg_stat.num_wipers_intermittent >= seg_stat.num_wipers_off);
  
  // This is a hack for the demo
  return (seg_stat.num_wipers_intermittent >= seg_stat.num_wipers_low &&
          seg_stat.num_wipers_intermittent > seg_stat.num_wipers_high);
}


bool rwx_road_segment_assessment::wipers_low(vdt_probe_message_qc_statistics& seg_stat)
{
  //return (seg_stat.num_wipers_low > seg_stat.num_wipers_intermittent &&
  //        seg_stat.num_wipers_low > seg_stat.num_wipers_high &&
  //        seg_stat.num_wipers_low >= seg_stat.num_wipers_off);

  // This is a hack for the demo
  return (seg_stat.num_wipers_low > seg_stat.num_wipers_intermittent &&
          seg_stat.num_wipers_low > seg_stat.num_wipers_high);
}


bool rwx_road_segment_assessment::abs_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_abs > 0 &&
          seg_stat.num_abs_engaged > (seg_stat.num_abs_on + seg_stat.num_abs_off));
}


bool rwx_road_segment_assessment::abs_not_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_abs > 0 &&
          (seg_stat.num_abs_on + seg_stat.num_abs_off) > seg_stat.num_abs_engaged);
}


bool rwx_road_segment_assessment::trac_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_trac > 0 &&
          seg_stat.num_trac_engaged > (seg_stat.num_trac_on + seg_stat.num_trac_off));
}


bool rwx_road_segment_assessment::trac_not_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_trac > 0 &&
          (seg_stat.num_trac_on + seg_stat.num_trac_off) > seg_stat.num_trac_engaged);
}


bool rwx_road_segment_assessment::stab_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_stab > 0 &&
          seg_stat.num_stab_engaged > (seg_stat.num_stab_on + seg_stat.num_stab_off));
}


bool rwx_road_segment_assessment::stab_not_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_stab > 0 &&
          (seg_stat.num_stab_on + seg_stat.num_stab_off) > seg_stat.num_stab_engaged);
}


bool rwx_road_segment_assessment::wipers_missing(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_off == vdt_const::FILL_VALUE &&
          seg_stat.num_wipers_intermittent == vdt_const::FILL_VALUE &&
          seg_stat.num_wipers_low == vdt_const::FILL_VALUE &&
          seg_stat.num_wipers_high == vdt_const::FILL_VALUE);
}


bool rwx_road_segment_assessment::is_max(double val, double t1, double t2, double t3, double t4)
{
  if (val > t1 &&
      val > t2 &&
      val > t3 &&
      val > t4)
    {
      return true;
    }
  return false;
}


double rwx_road_segment_assessment::get_dry_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;
  if (seg_stat.speed_ratio != vdt_const::FILL_VALUE)
    {
      interest += seg_stat.speed_ratio;
    }

  /*
  float air_temp = get_air_temp(seg_stat);
  if (air_temp != vdt_const::FILL_VALUE)
    {
      //not sure??
    }
  interest += get_interest(seg_stat.radar_ref,0,15,-0.033,0.995);
  interest += get_interest(seg_stat.radar_ref,15,22,-0.071,1.565);
  if (wipers_low(seg_stat))
    {
      interest += 1;
    }
  else if (wipers_high(seg_stat))
    {
      interest += 0.5;
    }
  if (abs_not_engaged(seg_stat))
    {
      interest += 1;
    }
  if (trac_not_engaged(seg_stat))
    {
      interest += 1;
    }
  if (stab_not_engaged(seg_stat))
    {
      interest += 1;
    }
  */


  interest += get_interest(seg_stat.lat_accel_mean,0,0.2,1);
  interest += get_interest(seg_stat.lat_accel_mean,0.2,0.24,-25,6);
  interest += get_interest(seg_stat.lon_accel_mean,0,0.5,1);
  interest += get_interest(seg_stat.lon_accel_mean,0.5,1,-2,2);
  interest += get_interest(seg_stat.yaw_mean,0,1,1);
  interest += get_interest(seg_stat.yaw_mean,1,1.7,-1.428,2.428);
  interest += get_interest(seg_stat.steering_angle_mean,0,4,-0.25,1);

  return interest;
}


double rwx_road_segment_assessment::get_wet_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;

  interest += get_interest(seg_stat.speed_ratio, 0, 0.8, 1.25, 0);
  interest += get_interest(seg_stat.speed_ratio, 0.8, 1.0, -1.5, 2.2);

  /*
  float air_temp = get_air_temp(seg_stat);
  interest += get_interest(air_temp, -10, 0, 0.1, 1);
  interest += get_interest(air_temp, 0, 5, 1);

  interest += get_interest(seg_stat.radar_ref, 0, 10, 0.5);
  interest += get_interest(seg_stat.radar_ref, 10, 20, 0.05, 0);

  if (wipers_low(seg_stat) ||
      wipers_intermittent(seg_stat) ||
      wipers_high(seg_stat))
    {
      interest += 1;
    }
  */

  interest += get_abs_stab_trac_interest(seg_stat, 0, 1);
  interest += get_interest(seg_stat.lat_accel_mean, 0, 0.2, 1);
  interest += get_interest(seg_stat.lat_accel_mean, 0.2, 0.3, -10, 3);
  interest += get_interest(seg_stat.lon_accel_mean, 0, 0.5, 1);
  interest += get_interest(seg_stat.lon_accel_mean, 0.5, 1, -2, 2);
  interest += get_interest(seg_stat.yaw_mean, 0, 1, 1);
  interest += get_interest(seg_stat.yaw_mean, 1, 1.7, -1.428, 2.428);
  interest += get_interest(seg_stat.steering_angle_mean, 0, 4, -0.25, 1);

  return interest;
}


double rwx_road_segment_assessment::get_snow_covered_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;
  interest += get_interest(seg_stat.speed_ratio, 0, 0.6, 0.833, 0.502);
  interest += get_interest(seg_stat.speed_ratio, 0.6, 1, -2, 2.2);

  /*
  interest += get_interest(seg_stat.air_temp_mean, -10, 0, 1);
  interest += get_interest(seg_stat.air_temp_mean, 0, 1, -1, 1);

  interest += get_interest(seg_stat.radar_ref,0, 20, 0.025, 0.5);
  interest += get_interest(seg_stat.radar_ref,20, 25, 1);
  if (wipers_on(seg_stat))
    {
      interest += 1;
    }
  */
  
  interest += get_abs_stab_trac_interest(seg_stat, 1, 1);
  interest += get_interest(seg_stat.lat_accel_mean,0,0.2,1);
  interest += get_interest(seg_stat.lat_accel_mean,0.2,0.4,-5,2);
  interest += get_interest(seg_stat.lon_accel_mean,0,0.5,1);
  interest += get_interest(seg_stat.lon_accel_mean,0.5,1,-2,2);
  interest += get_interest(seg_stat.yaw_mean,0,1,1);
  interest += get_interest(seg_stat.yaw_mean,1,2,-1,2);
  interest += get_interest(seg_stat.steering_angle_mean,0,4,-0.25,1);

  return interest;
}


double rwx_road_segment_assessment::get_slick_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;
  interest += get_interest(seg_stat.speed_ratio, 0, 0.8, -1.25, 1);

  /*
  interest += get_interest(seg_stat.air_temp_mean, -10, -6, 1);
  interest += get_interest(seg_stat.air_temp_mean, -6, 1, -0.1428, 0.1432);

  interest += get_interest(seg_stat.radar_ref, 0, 20, 0.025, 0.5);
  interest += get_interest(seg_stat.radar_ref, 20, 25, 1);
  if (wipers_low(seg_stat) || wipers_high(seg_stat) || wipers_intermittent(seg_stat))
    {
      interest += 1;
    }
  */
  
  interest += get_abs_stab_trac_interest(seg_stat, 2, 1);
  interest += get_interest(seg_stat.lat_accel_mean, 0, 0.2, 5, 0);
  interest += get_interest(seg_stat.lat_accel_mean, 0.2, 0.4, 1);
  interest += get_interest(seg_stat.lon_accel_mean, 0.3, 0.5, 5, -1.5);
  interest += get_interest(seg_stat.lon_accel_mean, 0.5, 1, 1);
  interest += get_interest(seg_stat.yaw_mean, 0, 1, 1, 0);
  interest += get_interest(seg_stat.yaw_mean, 1, 2, 1);
  interest += get_interest(seg_stat.steering_angle_mean, 2, 3, 1, -2);
  interest += get_interest(seg_stat.steering_angle_mean, 3, 4, 1);

  return interest;
}


double rwx_road_segment_assessment::get_road_splash_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;

  interest += get_interest(seg_stat.speed_ratio,0,0.8,1.25,0);
  interest += get_interest(seg_stat.speed_ratio,0.8,1,-1.5,2.2);

  /*
  interest += get_interest(seg_stat.air_temp_mean,-10,0,0.1,1);
  interest += get_interest(seg_stat.air_temp_mean,0,5,1);

  interest += get_interest(seg_stat.radar_ref, 0, 25, 1);

  if (wipers_low(seg_stat) || wipers_high(seg_stat))
    {
      interest += 1;
    }
  */

  interest += get_abs_stab_trac_interest(seg_stat, 0.5, 1.0);
  interest += get_interest(seg_stat.lat_accel_mean, 0.2, 0.4, -5, 2);
  interest += get_interest(seg_stat.lon_accel_mean, 0.5, 1, -2, 2);
  interest += get_interest(seg_stat.yaw_mean, 1, 1.7, -1.428, 2.428);
  interest += get_interest(seg_stat.steering_angle_mean, 0, 4, 0.25, 1);

  return interest;
}


double rwx_road_segment_assessment::get_abs_stab_trac_interest(vdt_probe_message_qc_statistics& seg_stat, double engaged, double not_engaged)
{
  double interest = 0.0;
  if (abs_not_engaged(seg_stat))
    {
      interest += not_engaged;
    }
  if (trac_not_engaged(seg_stat))
    {
      interest += not_engaged;
    }
  if (stab_not_engaged(seg_stat))
    {
      interest += not_engaged;
    }
  if (abs_engaged(seg_stat))
    {
      interest += engaged;
    }
  if (trac_engaged(seg_stat))
    {
      interest += engaged;
    }
  if (stab_engaged(seg_stat))
    {
      interest += engaged;
    }
  return interest;
}


double rwx_road_segment_assessment::get_interest(double val, double slope_min, double slope_max, double m, double b)
{
  if (val == vdt_const::FILL_VALUE)
    {
      return 0;
    }
  if (val >= slope_min && val < slope_max)
    {
      return m * val + b;
    }
  return 0;
}


double rwx_road_segment_assessment::get_interest(double val, double min, double max, double flat_val)
{
  if (val == vdt_const::FILL_VALUE)
    {
      return 0;
    }
  if (val >= min && val < max)
    {
      return flat_val;
    }
  return 0;
}


float rwx_road_segment_assessment::calc_fog_interest(float rh, float speed_ratio, float station_vis, float percent_fog_lights, float percent_high_beams)
{
  // Determine interest components for fog
  // If input to each interest component is missing
  // set the interest to 0.
  // Since we are doing a weighted sum, interest values of 0
  // will contribute nothing to the sum.
  // Note that even if inputs are not missing, interest values
  // can still be 0.
  
  // Determine rh interest
  float rh_intrst;
  if(rh != vdt_const::FILL_VALUE)
    {
      // Convert rh to a percentage
      float rh_pct = rh * 100;
      
      if(rh_pct < 40)
	rh_intrst = -1;
      else if(rh_pct >= 40 && rh_pct <= 60)
	rh_intrst = (rh_pct / 20) - 3;
      else if(rh_pct > 60 && rh_pct <= 80)
	  rh_intrst = 0;
      else if(rh_pct > 80 && rh_pct <= 100)
	  rh_intrst = (rh_pct / 20) - 4;
      else
	rh_intrst = 1;
    }
  else
    rh_intrst = 0;
  
  // Determine speed ratio interest 
  float speed_ratio_intrst;
  if(speed_ratio != vdt_const::FILL_VALUE)
    {
      if(speed_ratio < 0.2)
	speed_ratio_intrst = 5 * speed_ratio;
      else if(speed_ratio >= 0.2 && speed_ratio <= 0.5)
	speed_ratio_intrst = (-2 * speed_ratio / 3) + (17.0/15.0);
      else if(speed_ratio > 0.5 && speed_ratio <= 1)
	speed_ratio_intrst = (-8 * speed_ratio / 5) + (8.0/5.0);
      else
	speed_ratio_intrst = 0;
    }
  else
    speed_ratio_intrst = 0;
  
  // Determine station visibility interest
  float station_vis_intrst;
  if(station_vis != vdt_const::FILL_VALUE)
    {
      // Convert visibility from km to miles
      float station_vis_mi = station_vis * 0.621371;
      
      if(station_vis_mi >= 0 && station_vis_mi <= 10)
	station_vis_intrst = (-2 * station_vis_mi / 10) + 1;
      else
	station_vis_intrst = 0;
    }
  else
    station_vis_intrst = 0;

  // Determine fog lights interest
  float fog_lights_intrst;
  if(percent_fog_lights != vdt_const::FILL_VALUE)
    {
      if(percent_fog_lights > 0 && percent_fog_lights <= 1)
	fog_lights_intrst = percent_fog_lights;
      else
	fog_lights_intrst = 0;
    }
  else
    fog_lights_intrst = 0;
    
  // Determine high beams interest
  float high_beams_intrst;
  if(percent_high_beams != vdt_const::FILL_VALUE)
    {
      if(percent_high_beams > 0 && percent_high_beams < 1)
	high_beams_intrst = percent_high_beams;
      else
	high_beams_intrst = 0;
    }
  else
    high_beams_intrst = 0;
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: rh_intrst: %f, speed_ratio_intrst: %f, station_vis_intrst: %f, fog_lights_intrst: %f, high_beams_intrst: %f\n", rh_intrst, speed_ratio_intrst, station_vis_intrst, fog_lights_intrst, high_beams_intrst);  
  
  // Determine station fog interest
  // Currently don't have station fog at all
  
  // Set weights of fields for fog interest  
  float rh_wgt = 0.4;
  float fog_lights_wgt = 0.2;
  float high_beams_wgt = -0.2;
  float speed_ratio_wgt = 0.2;
  float station_vis_wgt = 0.2; // bumbed this up since we don't have station fog
  float station_fog_wgt = 0.1; // we currently don't have this
  
  // Determine fog interest
  float fog_intrst = (rh_wgt * rh_intrst) + (fog_lights_wgt * fog_lights_intrst) + (high_beams_wgt * high_beams_intrst) + (speed_ratio_wgt * speed_ratio_intrst) + (station_vis_wgt * station_vis_intrst);
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: fog_intrst: %f\n", fog_intrst);
  
  return fog_intrst;
}


float rwx_road_segment_assessment::calc_generic_vis_interest(float rh, float speed_ratio, float station_vis, float percent_lights_on, float percent_fog_lights, float percent_high_beams)
{
  // Determine interest components for generic visibility
  // If input to each interest component is missing
  // set the interest to 0.
  // Since we are doing a weighted sum, interest values of 0
  // will contribute nothing to the sum.
  // Note that even if inputs are not missing, interest values
  // can still be 0.
  
  // Determine rh interest
  float rh_intrst;
  if(rh != vdt_const::FILL_VALUE)
    {
      // Convert rh to a percentage
      float rh_pct = rh * 100;
      //printf("rh_pct: %f\n", rh_pct);
      
      if(rh_pct >= 0 && rh_pct < 40)
	rh_intrst = (rh_pct / 40) - 1;
      else if(rh_pct >= 40 && rh_pct <= 60)
	rh_intrst = 0;
      else if(rh_pct > 60 && rh_pct <= 100)
	rh_intrst = (rh_pct / 40) - (3.0/2.0);
      else
	rh_intrst = 1;
    }
  else
    rh_intrst = 0;
  
  // Determine speed ratio interest 
  float speed_ratio_intrst;
  if(speed_ratio != vdt_const::FILL_VALUE)
    {
      if(speed_ratio < 0.2)
	speed_ratio_intrst = 5 * speed_ratio;
      else if(speed_ratio >= 0.2 && speed_ratio <= 0.5)
	speed_ratio_intrst = (-2 * speed_ratio / 3) + (17.0/15.0);
      else if(speed_ratio > 0.5 && speed_ratio <= 1)
	speed_ratio_intrst = (-8 * speed_ratio / 5) + (8.0/5.0);
      else
	speed_ratio_intrst = 0;
    }
  else
    speed_ratio_intrst = 0;

  // Determine station visibility interest
  float station_vis_intrst;
  if(station_vis != vdt_const::FILL_VALUE)
    {
      // Convert visibility from km to miles
      float station_vis_mi = station_vis * 0.621371;
      //printf("station_vis(km): %f, station_vis_mi: %f\n", station_vis, station_vis_mi);
      
      if(station_vis_mi >= 0 && station_vis_mi <= 10)
	station_vis_intrst = (-2 * station_vis_mi / 10) + 1;
      else
	station_vis_intrst = 0;
    }
  else
    station_vis_intrst = 0;
  
  // Determine lights on interest
  float lights_on_intrst;
  if(percent_lights_on != vdt_const::FILL_VALUE)
    {
      if(percent_lights_on > 0 && percent_lights_on <= 1)
	lights_on_intrst = percent_lights_on;
      else
	lights_on_intrst = 0;
    }
  else
    lights_on_intrst = 0;
    
  // Determine fog lights interest
  float fog_lights_intrst;
  if(percent_fog_lights != vdt_const::FILL_VALUE)
    {
      if(percent_fog_lights > 0 && percent_fog_lights <= 1)
	fog_lights_intrst = percent_fog_lights;
      else
	fog_lights_intrst = 0;
    }
  else
    fog_lights_intrst = 0;
    
  // Determine high beams interest
  float high_beams_intrst;
  if(percent_high_beams != vdt_const::FILL_VALUE)
    {
      if(percent_high_beams > 0 && percent_high_beams < 1)
	high_beams_intrst = percent_high_beams;
      else
	high_beams_intrst = 0;
    }
  else
    high_beams_intrst = 0;
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: rh_intrst: %f, speed_ratio_intrst: %f, station_vis_intrst: %f, lights_on_intrst: %f, fog_lights_intrst: %f, high_beams_intrst: %f\n", rh_intrst, speed_ratio_intrst, station_vis_intrst, lights_on_intrst, fog_lights_intrst, high_beams_intrst);  
  
  // Set weights of fields for light interest 
  float lights_on_wgt = 0.375;
  float fog_lights_wgt = 0.625;
  float high_beams_wgt = -0.125;
  
  // Determine light interest
  float lights_intrst = (lights_on_wgt * lights_on_intrst) + (fog_lights_wgt * fog_lights_intrst) + (high_beams_wgt * high_beams_intrst);
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: lights_intrst: %f\n", lights_intrst);
  
  // Set weights of fields for generic visibility interest
  float rh_wgt = 0.3;
  float speed_ratio_wgt = 0.25;
  float lights_wgt = 0.25;
  float station_vis_wgt = 0.2;
  
  // Determine generic visibility interest
  float vis_intrst = (rh_wgt * rh_intrst) + (speed_ratio_wgt * speed_ratio_intrst) + (lights_wgt * lights_intrst) + (station_vis_wgt * station_vis_intrst);
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: generic vis_intrst: %f\n", vis_intrst);
  
  return vis_intrst;
}


float rwx_road_segment_assessment::determine_temp(float mobile_temp, float env_temp)
{
  float temp;
  
  if(mobile_temp != vdt_const::FILL_VALUE)
    temp = mobile_temp;
  else if(env_temp != vdt_const::FILL_VALUE)
    temp = env_temp;
  else
    temp = vdt_const::FILL_VALUE;
  
  /*
  if(mobile_temp != vdt_const::FILL_VALUE && env_temp != vdt_const::FILL_VALUE)
    {
      float temp_abs_diff = fabs(mobile_temp - env_temp);
      if(temp_abs_diff < 1)
	temp = env_temp;
      else
	temp = mobile_temp;
    }
  else if(env_temp != vdt_const::FILL_VALUE)
    temp = env_temp;
  else if(mobile_temp != vdt_const::FILL_VALUE)
    temp = mobile_temp;
  else
    temp = vdt_const::FILL_VALUE;
  */
  
  return temp;
}


float rwx_road_segment_assessment::calc_rh(float temp, float dewpt)
{
  float rh;

  if(temp == vdt_const::FILL_VALUE || dewpt == vdt_const::FILL_VALUE)
    return vdt_const::FILL_VALUE;

  rh = calc_pr_vapr(dewpt)/calc_pr_vapr(temp);
  
  return rh;
}


float rwx_road_segment_assessment::calc_pr_vapr(float temp)
{
  float pr_vap = vdt_const::FILL_VALUE;
  
  if (temp != vdt_const::FILL_VALUE) 
    pr_vap = 6.112 * exp( (17.67*temp) / (temp+243.5));
  
  return (pr_vap);
}


float rwx_road_segment_assessment::calc_percentage(float num_values, float total_values)
{
  float percentage;
  
  if(num_values == vdt_const::FILL_VALUE || total_values == vdt_const::FILL_VALUE)
    return vdt_const::FILL_VALUE;
  
  percentage = num_values / total_values;
  return percentage;
}


string rwx_road_segment_assessment::get_precip_type_str(int precip_type)
{
  string precip_type_str = "";
  
  if(precip_type == NO_PRECIP)
    precip_type_str = "NO_PRECIP";
  else if(precip_type == RAIN)
    precip_type_str = "RAIN";
  else if(precip_type == MIX)
    precip_type_str = "MIX";
  else if(precip_type == SNOW)
    precip_type_str = "SNOW";
  else
    precip_type_str = "MISSING";
  
  return precip_type_str;
}


string rwx_road_segment_assessment::get_precip_intensity_str(int precip_intensity)
{
   string precip_intensity_str = "";
   
   if(precip_intensity == NO_PRECIP)
     precip_intensity_str = "NO_PRECIP";
   else if(precip_intensity == LIGHT_PRECIP)
     precip_intensity_str = "LIGHT_PRECIP";
   else if(precip_intensity == MODERATE_PRECIP)
     precip_intensity_str = "MODERATE_PRECIP";
   else if(precip_intensity == HEAVY_PRECIP)
     precip_intensity_str = "HEAVY_PRECIP";
   else if(precip_intensity == ROAD_SPLASH)
    precip_intensity_str = "ROAD_SPLASH";
  else
     precip_intensity_str = "MISSING";
   
   return precip_intensity_str;
}


string rwx_road_segment_assessment::get_pavement_condition_str(int pavement_condition)
{
   string pavement_condtion_str = "";
   
   if(pavement_condition == DRY_PAVEMENT)
     pavement_condtion_str = "DRY_PAVEMENT";
   else if(pavement_condition == WET_PAVEMENT)
      pavement_condtion_str = "WET_PAVEMENT";
   else if(pavement_condition == SNOW_COVERED)
      pavement_condtion_str = "SNOW_COVERED";
   else if(pavement_condition == ICE_COVERED)
      pavement_condtion_str = "ICE_COVERED";
   else if(pavement_condition == HYDROPLANE)
      pavement_condtion_str = "HYDROPLANE";
   else if(pavement_condition == BLACK_ICE)
      pavement_condtion_str = "BLACK_ICE";
   else if(pavement_condition == DRY_WET_PAVEMENT)
      pavement_condtion_str = "DRY_WET_PAVEMENT";
   else if(pavement_condition == DRY_FROZEN_PAVEMENT)
      pavement_condtion_str = "DRY_FROZEN_PAVEMENT";
   else
     pavement_condtion_str = "MISSING";
   
   return pavement_condtion_str;
}


string rwx_road_segment_assessment::get_visibility_str(int visibility)
{
  string visibility_str = "";
  
  if(visibility == VIS_NORMAL)
    visibility_str = "VIS_NORMAL";
  else if(visibility == VIS_LOW)
    visibility_str = "VIS_LOW";
  else if(visibility == VIS_HEAVY_RAIN)
    visibility_str = "VIS_HEAVY_RAIN";
  else if(visibility == VIS_HEAVY_SNOW)
    visibility_str = "VIS_HEAVY_SNOW";
  else if(visibility == VIS_BLOWING_SNOW)
    visibility_str = "VIS_BLOWING_SNOW";
  else if(visibility == VIS_FOG)
    visibility_str = "VIS_FOG";
  else if(visibility == VIS_HAZE)
    visibility_str = "VIS_HAZE";
  else if(visibility == VIS_DUST)
    visibility_str = "VIS_DUST";
  else if(visibility == VIS_SMOKE)
    visibility_str = "VIS_SMOKE";
  else
    visibility_str = "MISSING";
  
  return visibility_str;
}


rwx_road_segment_assessment & rwx_road_segment_assessment::operator=(const rwx_road_segment_assessment &assessment)
{
  if (this == &assessment)
    {
      return *this;
    }

  rwx_road_segment_assessment::operator= (assessment);

  precip_type = assessment.precip_type;
  precip_intensity = assessment.precip_intensity;
  pavement_condition = assessment.pavement_condition;
  pavement_slickness = assessment.pavement_slickness;
  visibility = assessment.visibility;
  
  road_segment_id = assessment.road_segment_id;
  
  return *this;  
}
