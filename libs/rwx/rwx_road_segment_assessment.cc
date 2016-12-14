
#include <vdt4/vdt_const.hh>
#include "rwx_road_segment_assessment.hh"

// Input Forecast precip-type definitions
const int rwx_road_segment_assessment::FCST_NONE = 0;
const int rwx_road_segment_assessment::FCST_RAIN = 1;
const int rwx_road_segment_assessment::FCST_SNOW = 2;
const int rwx_road_segment_assessment::FCST_ICE = 5;

// Input Dual-Pol Radar Hydrometeor-Class definitions
const int rwx_road_segment_assessment::HID_BIO_SCATTER = 10;
const int rwx_road_segment_assessment::HID_GROUND_CLUTTER = 20;
const int rwx_road_segment_assessment::HID_ICE_CRYSTALS = 30;
const int rwx_road_segment_assessment::HID_DRY_SNOW = 40;
const int rwx_road_segment_assessment::HID_WET_SNOW = 50;
const int rwx_road_segment_assessment::HID_LGT_MDT_RAIN = 60;
const int rwx_road_segment_assessment::HID_HVY_RAIN = 70;
const int rwx_road_segment_assessment::HID_BIG_DROPS = 80;
const int rwx_road_segment_assessment::HID_GRAUPEL = 90;
const int rwx_road_segment_assessment::HID_HAIL = 100;
const int rwx_road_segment_assessment::HID_UNKNOWN = 140;

// Input RWIS Road-State definitions
const int rwx_road_segment_assessment::RS_NO_REPORT = 0;
const int rwx_road_segment_assessment::RS_DRY = 1;
const int rwx_road_segment_assessment::RS_MOIST = 2;
const int rwx_road_segment_assessment::RS_MOIST_CHEM_TMT = 3;
const int rwx_road_segment_assessment::RS_WET = 4;
const int rwx_road_segment_assessment::RS_WET_CHEM_TMT = 5;
const int rwx_road_segment_assessment::RS_ICE = 6;
const int rwx_road_segment_assessment::RS_FROST = 7;
const int rwx_road_segment_assessment::RS_SNOW = 8;
const int rwx_road_segment_assessment::RS_SNOW_ICE_WACH = 9;
const int rwx_road_segment_assessment::RS_SNOW_ICE_WARN = 10;
const int rwx_road_segment_assessment::RS_WET_ABOVE_FRZ = 11;
const int rwx_road_segment_assessment::RS_WET_BELOW_FRZ = 12;
const int rwx_road_segment_assessment::RS_ABSORPTION = 13;
const int rwx_road_segment_assessment::RS_ABSORPTION_DEWPT = 14;
const int rwx_road_segment_assessment::RS_DEW = 15;
const int rwx_road_segment_assessment::RS_BLACK_ICE = 16;
const int rwx_road_segment_assessment::RS_OTHER = 17;
const int rwx_road_segment_assessment::RS_SLUSH = 18;

// Input Wind-speed threshold for blowing snow (m/s)
const int rwx_road_segment_assessment::BLOWING_SNOW_WIND_SPEED = 10; // 10 m/s

// Precip Type enumeration
const int rwx_road_segment_assessment::NO_PRECIP = 0;
const int rwx_road_segment_assessment::RAIN = 1;
const int rwx_road_segment_assessment::MIX = 2;
const int rwx_road_segment_assessment::SNOW = 3;
const int rwx_road_segment_assessment::HAIL = 4;

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
const float rwx_road_segment_assessment::dual_pol_wgt = 0.50;

const float rwx_road_segment_assessment::radar_ref_wgt = 0.50;
const float rwx_road_segment_assessment::wipers_wgt = 0.30;
const float rwx_road_segment_assessment::speed_ratio_wgt = 0.10;
const float rwx_road_segment_assessment::lights_wgt = 0.10;

const float rwx_road_segment_assessment::road_temp_wgt = 0.50;
const float rwx_road_segment_assessment::precip_type_wgt = 0.20;
const float rwx_road_segment_assessment::precip_intensity_wgt = 0.30;

const float rwx_road_segment_assessment::fog_wgt = 0.50;
const float rwx_road_segment_assessment::haze_wgt = 0.50;
const float rwx_road_segment_assessment::normal_wgt = 0.50;


rwx_road_segment_assessment::rwx_road_segment_assessment()
{
  precip_type = vdt_const::FILL_VALUE;
  precip_intensity = vdt_const::FILL_VALUE;
  pavement_condition = vdt_const::FILL_VALUE;
  pavement_slickness = vdt_const::FILL_VALUE;
  visibility = vdt_const::FILL_VALUE;
}


void rwx_road_segment_assessment::perform_assessment(vdt_probe_message_qc_statistics& seg_stat)
{
  road_segment_id = seg_stat.id;

  set_precip_thresholds(seg_stat, fcst_light_precip, fcst_moderate_precip, fcst_heavy_precip, radar_light_precip, radar_moderate_precip, radar_heavy_precip);

  precip_type = precip_type_assessment(seg_stat, precip_type_confidence);

  precip_intensity = precip_intensity_assessment(seg_stat, precip_type, fcst_light_precip, fcst_moderate_precip, fcst_heavy_precip, radar_light_precip, radar_moderate_precip, radar_heavy_precip, precip_intensity_confidence);

  visibility = visibility_assessment(seg_stat, precip_type, precip_type_confidence, precip_intensity, precip_intensity_confidence, visibility_confidence);
  
  pavement_condition = pavement_condition_assessment(seg_stat, precip_type, precip_type_confidence, precip_intensity, precip_intensity_confidence, pavement_condition_confidence);

  // If the precip-intensity is missing that indicates we did not have enough mobile-data (or forecast data)
  // First, check if we have road_state value, if it's non-missing use it to set pavement-condtion
  //
  // Second, if both precip-intensity and road-state are missing we set the precip-type, pavement-condtion to missing (precip-intensity is already missing)
  // and this is the indication to the display that we don't have enough data to make an assessment.
  // Note, visibility could still have a non-missing hazard (for fog or haze) and slickness could have non-missing hazard (do to some other mobile fields) so we leave those as is
  //
  // Third, if precip-intensity is non-missing (indicates valid pavement-condition), cross-check pavement-condition with road-state, adjust pavement-condition if necessary
  //
  
  // Get the road-state, QC if necessary
  int road_state = get_rwis_road_state(seg_stat);
  
  if(precip_intensity == vdt_const::FILL_VALUE)
    {
      if(road_state != vdt_const::FILL_VALUE)
	{
	  printf("  RWH LIB: found MISSING precip-intensity, trying to use road_state: %d for pavement-condition\n", road_state);
	  if(road_state == RS_DRY)
	    pavement_condition = DRY_PAVEMENT;
	  else if(road_state == RS_MOIST || road_state == RS_MOIST_CHEM_TMT || road_state == RS_WET || road_state == RS_WET_CHEM_TMT || road_state == RS_WET_ABOVE_FRZ || road_state == RS_DEW)
	    pavement_condition = WET_PAVEMENT;
	  else if(road_state == RS_ICE || road_state == RS_FROST || road_state == RS_WET_BELOW_FRZ)
	    pavement_condition = ICE_COVERED;
	  else if(road_state == RS_BLACK_ICE)
	    pavement_condition = BLACK_ICE;
	  else if(road_state == RS_SNOW || road_state == RS_SNOW_ICE_WACH || road_state == RS_SNOW_ICE_WARN || road_state == RS_SLUSH)
	    pavement_condition = SNOW_COVERED;
	  else
	    {
	      printf("  RWH LIB: unknown road_state: %d for setting pavement-condition, precip-intensity is MISSING so setting precip-type and pavement-condition to MISSING.\n", road_state);
	      precip_type = vdt_const::FILL_VALUE;
	      pavement_condition = vdt_const::FILL_VALUE;
	    }
	}
      else
	{
	  printf("  RWH LIB: found MISSING precip-intensity and road_state is MISSING, setting precip-type and pavement-condition to MISSING.\n");
	  precip_type = vdt_const::FILL_VALUE;
	  pavement_condition = vdt_const::FILL_VALUE;
	}
    }
  else // precip-intensity non-missing (valid pavement-condition), cross-check road-state
    {
      if(road_state != vdt_const::FILL_VALUE)
	{
	  //printf("  RWH LIB: precip-intensity is non-missing, using road_state: %d to cross-check pavement-condition\n", road_state);
	  //if(pavement_condition == WET_PAVEMENT) // We commented this out for now because of strange behaviour (likely due to using an RWIS that is not on the segment it's self)
	  //  {
	  //    if(road_state == 1)
	  //	pavement_condition = DRY_PAVEMENT;
	  //  }
	  if(pavement_condition == SNOW_COVERED || pavement_condition == ICE_COVERED || pavement_condition == BLACK_ICE)
	    {
	      if(road_state == 1 || road_state == 2 || road_state == 3 || road_state == 4 || road_state == 5 || road_state == 11 || road_state == 15)
		pavement_condition = WET_PAVEMENT;
	    }
	} // end if road_state is non-missing
    } // end else
  
  // Change the current pavement-condition if there are no current pavement-condition hazards, road_state is missing
  // and we have a non-missing prev-pavement-condition indicated
  prev_pavement_condition = determine_prev_pavement_condition(seg_stat);
  if(road_state == vdt_const::FILL_VALUE && pavement_condition != WET_PAVEMENT && pavement_condition != SNOW_COVERED && pavement_condition != ICE_COVERED &&
     pavement_condition != HYDROPLANE && pavement_condition != BLACK_ICE)
    {
      if(prev_pavement_condition != vdt_const::FILL_VALUE)
	{
	  //printf("  RWH LIB, REPLACING pavement_condition: %d with prev_pavement_condition: %d\n", pavement_condition, prev_pavement_condition);
	  pavement_condition = prev_pavement_condition;
	}    
    }
    
  pavement_slickness = pavement_slickness_assessment(seg_stat, precip_type, precip_intensity, pavement_condition);
  
  // Check for dry pavement slickness if weather related pavement slickness is 0
  // Dry pavement slickness is based on abs, trac and stab
  if(pavement_slickness == 0)
    pavement_slickness = dry_pavement_slickness_assessment(seg_stat);

}


int rwx_road_segment_assessment::determine_prev_pavement_condition(vdt_probe_message_qc_statistics& seg_stat)
{
  int prev_pavement_condition = vdt_const::FILL_VALUE;

  // Get the previous rwh data fields
  int prev_precip_type = seg_stat.prev_precip_type;
  int prev_precip_intensity = seg_stat.prev_precip_intensity;
  double prev_precip_time_gap = seg_stat.prev_precip_time_gap;
  
  if(prev_precip_time_gap == -9999 || prev_precip_time_gap == vdt_const::FILL_VALUE)
    {
      //printf("  RWH LIB: prev_precip_time_gap is missing, can't set previous pavement condition for this segment\n");
      return prev_pavement_condition;
    }
  
  // Get forecast fields 
  float fcst_road_temp = seg_stat.fcst_road_temp;
  //printf("  RWH LIB: fcst_road_temp: %f\n", fcst_road_temp);
  
  // Get VDT fields
  float mobile_road_temp = seg_stat.surface_temp_mean;
  float env_road_temp = seg_stat.nss_surface_temp_mean;
  float vdt_road_temp = determine_temp(mobile_road_temp, env_road_temp);
  //printf("  RWH LIB: vdt_road_temp: %f\n", vdt_road_temp);
  
  float mobile_air_temp = get_air_temp(seg_stat);
  float env_air_temp = get_env_air_temp(seg_stat);
  float vdt_air_temp = determine_temp(mobile_air_temp, env_air_temp);
  //printf("  RWH LIB: vdt_air_temp: %f\n", vdt_air_temp);
  
  // Determine if we are dealing with forecast data or vdt data
  // If we have no vdt road-T data, use air-T data as proxy
  float road_temp = vdt_const::FILL_VALUE;
  if(fcst_road_temp != vdt_const::FILL_VALUE)
    {
      //printf("  RWH LIB: FORECAST CASE\n");
      road_temp = fcst_road_temp;
    }
  else if(vdt_road_temp != vdt_const::FILL_VALUE)
    {
      //printf("  RWH LIB: VDT CASE, using vdt_road_temp\n");
      road_temp = vdt_road_temp;
    }
  else if(vdt_air_temp != vdt_const::FILL_VALUE)
    {
      //printf("  RWH LIB: VDT CASE, using vdt_air_temp\n");
      road_temp = vdt_air_temp;
    }
  else
    {
      //printf("  RWH LIB: VDT road-T / air-T missing, can't set previous pavement conditions for this segment\n");
      return prev_pavement_condition;
    }
  
  //printf("  RWH LIB: prev_precip_type: %d, prev_precip_intensity: %d, prev_precip_time_gap: %d, road_temp: %.2f\n", prev_precip_type, prev_precip_intensity, (int)prev_precip_time_gap, road_temp);
  
  // Set pavement-condtion hazard that's on the road based on the previous rwh data
  // Note that the prev_precip_time_gap is in seconds (7200 = 2 hours, 3600 = 1 hour, 1800 = 30 minutes)
  //
  if(prev_precip_intensity == LIGHT_PRECIP || prev_precip_intensity == MODERATE_PRECIP || prev_precip_intensity == HEAVY_PRECIP)
    {
      if(prev_precip_type == SNOW)
	{
	  if(prev_precip_intensity == HEAVY_PRECIP && prev_precip_time_gap <= 7200)
	    {
	      if(road_temp > -1 && road_temp < 2)
		prev_pavement_condition = ICE_COVERED;
	      else if(road_temp >= 2)
		prev_pavement_condition = WET_PAVEMENT;
	      else
		prev_pavement_condition = SNOW_COVERED;
	    }
	  else if(prev_precip_intensity == MODERATE_PRECIP && prev_precip_time_gap <= 3600)
	    {
	      if(road_temp > -1 && road_temp < 1.5)
		prev_pavement_condition = ICE_COVERED;
	      else if(road_temp >= 1.5)
		prev_pavement_condition = WET_PAVEMENT;
	      else
		prev_pavement_condition = SNOW_COVERED;
	    }
	  else if(prev_precip_intensity == LIGHT_PRECIP && prev_precip_time_gap <= 1800)
	    {
	      if(road_temp > -1 && road_temp < 1.5)
		prev_pavement_condition = ICE_COVERED;
	      else if(road_temp >= 1.5)
		prev_pavement_condition = WET_PAVEMENT;
	      else
		prev_pavement_condition = SNOW_COVERED;
	    }
	  else
	    prev_pavement_condition = vdt_const::FILL_VALUE;
	}
      else if(prev_precip_type == MIX)
	{
	  if(prev_precip_intensity == HEAVY_PRECIP && prev_precip_time_gap <= 7200)
	    {
	      if(road_temp < 1.5)
		prev_pavement_condition = ICE_COVERED;
	      else
		prev_pavement_condition = WET_PAVEMENT;
	    }
	  else if(prev_precip_intensity == MODERATE_PRECIP && prev_precip_time_gap <= 3600)
	    {
	      if(road_temp < 1.5)
		prev_pavement_condition = ICE_COVERED;
	      else
		prev_pavement_condition = WET_PAVEMENT;
	    }
	  else if(prev_precip_intensity == LIGHT_PRECIP && prev_precip_time_gap <= 1800)
	    {
	      if(road_temp < 1)
		prev_pavement_condition = ICE_COVERED;
	      else
		prev_pavement_condition = WET_PAVEMENT;
	    }
	  else
	    prev_pavement_condition = vdt_const::FILL_VALUE;
	}
      else if(prev_precip_type == RAIN)
	{
	  if(prev_precip_intensity == HEAVY_PRECIP && prev_precip_time_gap <= 3600)
	    {
	      if(road_temp < 1.5)
		prev_pavement_condition = ICE_COVERED;
	      else if(road_temp >= 1.5 && road_temp < 20)
		prev_pavement_condition = WET_PAVEMENT;
	      else
		prev_pavement_condition = vdt_const::FILL_VALUE;
	    }
	  else if(prev_precip_intensity == MODERATE_PRECIP && prev_precip_time_gap <= 1800)
	    {
	      if(road_temp < 1)
		prev_pavement_condition = ICE_COVERED;
	      else if(road_temp >= 1 && road_temp < 20)
		prev_pavement_condition = WET_PAVEMENT;
	      else
		prev_pavement_condition = vdt_const::FILL_VALUE;
	    }
	  else
	    prev_pavement_condition = vdt_const::FILL_VALUE;
	}
    } // end if precip-type is light, moderate or heavy
  
  return prev_pavement_condition;
}


void rwx_road_segment_assessment::set_precip_thresholds(vdt_probe_message_qc_statistics& seg_stat, float &FCST_LIGHT_PRECIP, float &FCST_MODERATE_PRECIP, float &FCST_HEAVY_PRECIP, float &RADAR_LIGHT_PRECIP, float &RADAR_MODERATE_PRECIP, float &RADAR_HEAVY_PRECIP)
{
  // Initialize fcst thresholds with values for the 15 degF to the 50 degF range
  // just in case fcst_air_temp is missing
  FCST_LIGHT_PRECIP = 0.127; // 0.127 mm. (0.005 in)
  FCST_MODERATE_PRECIP = 1.27; // 1.27 mm. (0.05 in.)
  FCST_HEAVY_PRECIP = 3.81; // 3.81 mm (0.15 in.)
  
  // Get forecast air-temp
  float fcst_air_temp = seg_stat.fcst_air_temp;
  
  // Set forecast precip thresholds based on forecast air-temp
  if(fcst_air_temp != vdt_const::FILL_VALUE)
    {
      if(fcst_air_temp <= -10) // (below 15 degF)
	{
	  FCST_LIGHT_PRECIP = 0.0635; // 0.0635 mm. (0.0025 in.)
	  FCST_MODERATE_PRECIP = 0.254; // 0.254 mm. (0.01 in.)
	  FCST_HEAVY_PRECIP = 2.54; // 2.54 mm. (0.10 in.)
	}
      else if(fcst_air_temp > -10 && fcst_air_temp <= 10) // (between 15 degF and 50 degF)
	{
	  FCST_LIGHT_PRECIP = 0.127; // 0.127 mm. (0.005 in)
	  FCST_MODERATE_PRECIP = 1.27; // 1.27 mm. (0.05 in.)
	  FCST_HEAVY_PRECIP = 3.81; // 3.81 mm (0.15 in.)
	}
      else // (above 50 degF)
	{
	  FCST_LIGHT_PRECIP =  0.254; // 0.254 mm. (0.01 in.)
	  FCST_MODERATE_PRECIP = 2.54; // 2.54 mm. (0.10 in.)
	  FCST_HEAVY_PRECIP = 6.35; // 6.35 mm. (0.25 in.)
	}
    }

  //printf("fcst_air_temp: %.3f, FCST_LIGHT_PRECIP: %.3f, FCST_MODERATE_PRECIP: %.3f, FCST_HEAVY_PRECIP: %.3f\n", fcst_air_temp, FCST_LIGHT_PRECIP, FCST_MODERATE_PRECIP, FCST_HEAVY_PRECIP);

  // Initialize radar thresholds with values for the 15 degF to the 50 degF range
  // just in case vdt air_temp is missing
  RADAR_LIGHT_PRECIP = 15; //    15 dBZ 
  RADAR_MODERATE_PRECIP = 30; // 30 dBZ
  RADAR_HEAVY_PRECIP = 40; //    40 dBZ
  
  // Determine what VDT air temp we should use 
  float mobile_air_temp = get_air_temp(seg_stat);
  float env_air_temp = get_env_air_temp(seg_stat);
  float air_temp = determine_temp(mobile_air_temp, env_air_temp);
  
  // Set radar precip thresholds based on vdt air-temp
  if(air_temp != vdt_const::FILL_VALUE)
    {
      if(air_temp <= -10) // (below 15 degF)
	{
	  RADAR_LIGHT_PRECIP = 12; //    12 dBZ
	  RADAR_MODERATE_PRECIP = 20; // 20 dBZ 
	  RADAR_HEAVY_PRECIP = 30; //    30 dBZ
	}
      else if(air_temp > -10 && air_temp <= 10) // (between 15 degF and 50 degF)
	{
	  RADAR_LIGHT_PRECIP = 15; //    15 dBZ 
	  RADAR_MODERATE_PRECIP = 30; // 30 dBZ
	  RADAR_HEAVY_PRECIP = 40; //    40 dBZ
	}
      else // (above 50 degF)
	{
	  RADAR_LIGHT_PRECIP = 18; //    18 dBZ
	  RADAR_MODERATE_PRECIP = 40; // 40 dBZ
	  RADAR_HEAVY_PRECIP = 55; //    50 dBZ
	}
    }
  
  //printf("vdt air_temp: %.3f, RADAR_LIGHT_PRECIP: %.3f, RADAR_MODERATE_PRECIP: %.3f, RADAR_HEAVY_PRECIP: %.3f\n", air_temp, RADAR_LIGHT_PRECIP, RADAR_MODERATE_PRECIP, RADAR_HEAVY_PRECIP);
  
}


int rwx_road_segment_assessment::get_rwis_road_state(vdt_probe_message_qc_statistics& seg_stat)
{
  int road_state = seg_stat.road_state;
  
  float mobile_air_temp = get_air_temp(seg_stat);
  float env_air_temp = get_env_air_temp(seg_stat);
  float air_temp = determine_temp(mobile_air_temp, env_air_temp);

  //printf("  RWH LIB: initial road_state: %d, air_temp: %f\n", road_state, air_temp);
  
  if(road_state == -32767)
    road_state = vdt_const::FILL_VALUE;

  // Only set road-state to missing if it does not make sense relative to the air-temp
  // If a frozen category is given only look for air-temps above 1 degC
  // If a warm/wet category is given, only look for air-temp at or below 0 degC
  // We don't change anyting if the air-temp is between 0-1 (including a value of 1)
  // If air-temp is mssing we leave the road-state as is
  //
  if(air_temp != vdt_const::FILL_VALUE && road_state != vdt_const::FILL_VALUE)
    {
      if(road_state == RS_ICE || road_state == RS_FROST || road_state == RS_WET_BELOW_FRZ || road_state == RS_BLACK_ICE ||
	 road_state == RS_SNOW || road_state == RS_SNOW_ICE_WACH || road_state == RS_SNOW_ICE_WARN || road_state == RS_SLUSH)
	{
	  if(air_temp > 1)
	    {
	      //printf("  RWH LIB: found cold road_state: %d with warm air_temp: %.2f, setting road_state to missing\n", road_state, air_temp);
	      road_state = vdt_const::FILL_VALUE;
	    }
	}
      else if(road_state == RS_MOIST || road_state == RS_MOIST_CHEM_TMT || road_state == RS_WET || road_state == RS_WET_CHEM_TMT ||
	      road_state == RS_WET_ABOVE_FRZ || road_state == RS_DEW)
	{
	  if(air_temp <= 0)
	    {
	      //printf("  RWH LIB: found warm road_state: %d with cold air_temp: %.2f, setting road_state to missing\n", road_state, air_temp);
	      road_state = vdt_const::FILL_VALUE;
	    }
	}	
    } // end if air-temp and road-state non-missing 
  
  return road_state;
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
  float dual_pol_input_conf = 1.0;
  
  // VDT case

  // First, try to use dual-pol-hydro-class (HID) to get precip-type
  // otherwise use air-T
  //
  int dual_pol_hid = (int)seg_stat.radar_dual_pol_hc;
  //printf("  RWH LIB: dual_pol_hid: %d\n", dual_pol_hid);
  
  if(dual_pol_hid == -999)
    dual_pol_hid = vdt_const::FILL_VALUE;

  // If the dual-pol HID value is "unknown"
  // then set it to missing and thus will not use
  if(dual_pol_hid == HID_UNKNOWN)
    dual_pol_hid = vdt_const::FILL_VALUE;
  
  if(dual_pol_hid != vdt_const::FILL_VALUE)
    {
      if(dual_pol_hid == HID_BIO_SCATTER || dual_pol_hid == HID_GROUND_CLUTTER)
	precip_type =  NO_PRECIP;
      else if(dual_pol_hid == HID_ICE_CRYSTALS || dual_pol_hid == HID_DRY_SNOW || dual_pol_hid == HID_WET_SNOW)
	{
	  if(air_temp != vdt_const::FILL_VALUE)
	    {
	      if(air_temp > 1)
		precip_type =  RAIN;
	      else
		precip_type =  SNOW;
	    }
	  else
	    precip_type =  SNOW;
	}
      else if(dual_pol_hid == HID_LGT_MDT_RAIN || dual_pol_hid == HID_HVY_RAIN || dual_pol_hid == HID_BIG_DROPS || dual_pol_hid == HID_GRAUPEL)
	{
	  if(air_temp != vdt_const::FILL_VALUE)
	    {
	      if(air_temp > 0)
		precip_type =  RAIN;
	      else
		precip_type =  MIX; 
	    }
	  else
	    precip_type =  RAIN;
	}
      else if(dual_pol_hid == HID_HAIL)
	{
	  precip_type =  HAIL;
	}

      //printf("  RWH LIB: Using dual_pol_hid: %d to set precip_type to %.0f\n", dual_pol_hid, precip_type);
      confidence = confidence + (dual_pol_input_conf * dual_pol_wgt);
      
    }
  else // Case with just air-temp dual-pol-hid MISSING
    {
      if(air_temp != vdt_const::FILL_VALUE) // vdt case
	{
	  if(air_temp > 2)
	    precip_type =  RAIN;
	  else if(air_temp < -2)
	    precip_type =  SNOW;
	  else
	    precip_type =  MIX;

	  //printf("  RWH LIB: Using air_temp: %.2f only to set precip_type to %.0f\n", air_temp, precip_type);
	  confidence = confidence + (air_temp_input_conf * air_temp_wgt);
	}
    }
  
  // Cross-check the precip-type with METAR present-wx report (precip-type and intensity)
  // Use air-temp to make sure we stick with correct type
  //
  float wx_precip_type = seg_stat.wx_precip_type;
  
  if(air_temp != vdt_const::FILL_VALUE && wx_precip_type != vdt_const::FILL_VALUE)
    {
      if(wx_precip_type == RAIN && air_temp > 0)
	{
	  //printf("Changing precip_type from %.0f to RAIN: wx_precip_type: %.0f, air_temp: %.2f\n", precip_type, wx_precip_type, air_temp);
	  precip_type = RAIN;
	}
      else if(wx_precip_type == SNOW && air_temp < 2)
	{
	  //printf("Changing precip_type from %.0f to SNOW: wx_precip_type: %.0f, air_temp: %.2f\n", precip_type, wx_precip_type, air_temp);
	  precip_type = SNOW;  
	}
      else if(wx_precip_type == MIX && (air_temp > -5 && air_temp < 5))
	{
	  //printf("Changing precip_type from %.0f to MIX: wx_precip_type: %.0f, air_temp: %.2f\n", precip_type, wx_precip_type, air_temp);
	  precip_type = MIX;  
	}
    }

  // If precip_type is still missing (e.g. don't have air-temp, ect) just use the wx_precip_type
  //
  if(precip_type == vdt_const::FILL_VALUE && wx_precip_type != vdt_const::FILL_VALUE)
    {
      //printf("Precip_type is still missing, setting precip_type using wx_precip_type: %.0f\n", wx_precip_type);
      precip_type = wx_precip_type;
    }
  
  return precip_type;
}


int rwx_road_segment_assessment::precip_intensity_assessment(vdt_probe_message_qc_statistics& seg_stat, const int precip_type, const float FCST_LIGHT_PRECIP, const float FCST_MODERATE_PRECIP, const float FCST_HEAVY_PRECIP, const float RADAR_LIGHT_PRECIP, const float RADAR_MODERATE_PRECIP, const float RADAR_HEAVY_PRECIP, float &confidence)
{
  float precip_intensity = vdt_const::FILL_VALUE;
  confidence = 0;
  
  // Get forecast fields
  float fcst_precip_rate = seg_stat.fcst_precip_rate;
  float fcst_air_temp = seg_stat.fcst_air_temp;
  float fcst_dew_temp = seg_stat.fcst_dew_temp;

  // Calculate dewpoint-depression
  float fcst_dew_depress = vdt_const::FILL_VALUE;
  if(fcst_air_temp != vdt_const::FILL_VALUE && fcst_dew_temp != vdt_const::FILL_VALUE)
    {
      fcst_dew_depress = fcst_air_temp - fcst_dew_temp;
      if(fcst_dew_depress < 0)
	fcst_dew_depress = 0;
    }
  
  // Forecast case
  // * Note, this now uses the dynamic fcst-precip-thresholds passed into this function.
  //   The precip-threshold values are set in the set_precip_thresholds() function above.
  if(fcst_precip_rate != vdt_const::FILL_VALUE)
    {
      if(fcst_precip_rate < FCST_LIGHT_PRECIP)
	precip_intensity =  NO_PRECIP;
      else if(fcst_precip_rate >= FCST_LIGHT_PRECIP && fcst_precip_rate < FCST_MODERATE_PRECIP)
	precip_intensity =  LIGHT_PRECIP;
      else if(fcst_precip_rate >= FCST_MODERATE_PRECIP && fcst_precip_rate < FCST_HEAVY_PRECIP)
	precip_intensity =  MODERATE_PRECIP;
      else
	precip_intensity =  HEAVY_PRECIP;

      /*
      // This was causing a problem during the summer, especially over Colorado
      // Since T, dewpoint, and precip fields (pop and rate) evolve independently
      // you can have cases during summer (thunderstorms forecasted) where meaningful
      // precip-rate and pop is forecast but the dewpoint-depression is > 20 degC even sometime 30 degC
      // so in order to keep the RWH hazards consitent with the forecast data, I'm commenting this out for now
      //
      // Lower precip-intensity if dewpoint-depression is too high
      // Forcast data usually does not need to be corrected, but for cases when there is precip indicated and the dewpoint depression > 5
      // knock the forecast category down one (based on testing > 5 depression can only occcur for very light forecast precip)
      if(fcst_dew_depress != vdt_const::FILL_VALUE && fcst_dew_depress > 5)
	{
	  if(precip_intensity > NO_PRECIP)
	    printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: Changing original fcst precip-intensity: %.0f based on >5 dewpoint-depression: %f.\n", precip_intensity, fcst_dew_depress);
	  
	  if(precip_intensity == HEAVY_PRECIP)
	    precip_intensity =  MODERATE_PRECIP;
	  else if(precip_intensity == MODERATE_PRECIP)
	    precip_intensity =  LIGHT_PRECIP;
	  else // previously LIGHT OR NO
	    precip_intensity =  NO_PRECIP;
	}
      */
      
      confidence = confidence + (fcst_input_conf * fcst_wgt);
      return precip_intensity;
    }
  
  // Get VDT fields
  //

  // Get Radar Reflectivity, look for the original (CONUS mosaic) reflectivity first
  // if it's missing look for the dual-pol reflectivity (non mosaic)
  //
  float radar_ref =  vdt_const::FILL_VALUE;
  float orig_radar_ref = seg_stat.radar_ref;
  float dual_pol_radar_ref = seg_stat.radar_dual_pol_hr;
  
  // Set radar_ref values of -999 to missing
  // Amanda indicated that a value of -999 means the location is beyond the radar coverage
  
  if(orig_radar_ref == -999)
    orig_radar_ref = vdt_const::FILL_VALUE;

  if(dual_pol_radar_ref == -999)
    dual_pol_radar_ref = vdt_const::FILL_VALUE;
  
  if(orig_radar_ref != vdt_const::FILL_VALUE) 
    radar_ref = orig_radar_ref;
  else if(dual_pol_radar_ref != vdt_const::FILL_VALUE)
    radar_ref = dual_pol_radar_ref;
  else
    radar_ref = vdt_const::FILL_VALUE;

  // Get Radar (Dual-Pol) Hydro-class (HID)
  int dual_pol_hid = (int)seg_stat.radar_dual_pol_hc;
  
  if(dual_pol_hid == -999)
    dual_pol_hid = vdt_const::FILL_VALUE;

  // If the dual-pol HID value is "unknown"
  // then set it to missing and thus will not use
  if(dual_pol_hid == HID_UNKNOWN)
    dual_pol_hid = vdt_const::FILL_VALUE;

  printf("  RWH LIB: orig_radar_ref: %.2f, dual_pol_radar_ref: %.2f, radar_ref: %.2f, dual_pol_hid: %d\n", orig_radar_ref, dual_pol_radar_ref, radar_ref, dual_pol_hid);
  
  float speed_ratio = seg_stat.speed_ratio;

  float mobile_air_temp = get_air_temp(seg_stat);
  float env_air_temp = get_env_air_temp(seg_stat);
  float air_temp = determine_temp(mobile_air_temp, env_air_temp);
  
  float mobile_dew_temp = seg_stat.dew_temp_mean;
  float env_dew_temp = get_env_dew_temp(seg_stat);
  float dew_temp = determine_temp(mobile_dew_temp, env_dew_temp);

  //printf("  RWH LIB: air_temp: %.2f, dew_temp: %.2f\n", air_temp, dew_temp);
  
  // Calculate dewpoint-depression
  float dew_depress = vdt_const::FILL_VALUE;
  if(air_temp != vdt_const::FILL_VALUE && dew_temp != vdt_const::FILL_VALUE)
    {
      dew_depress = air_temp - dew_temp;
      if(dew_depress < 0)
	dew_depress = 0;
    }

  // Determine if we have any head-light on/off data
  int num_msg_valid_lights = seg_stat.num_msg_valid_lights;
  int num_lights_off = seg_stat.num_lights_off;
  float percent_lights_off = calc_percentage((float)num_lights_off, (float)num_msg_valid_lights);
  
  // Determine if we have any wiper data
  int wipers_flag = 0;
  if(seg_stat.num_wipers_off != vdt_const::FILL_VALUE ||
     seg_stat.num_wipers_intermittent != vdt_const::FILL_VALUE ||
     seg_stat.num_wipers_low != vdt_const::FILL_VALUE ||
     seg_stat.num_wipers_high != vdt_const::FILL_VALUE)
    wipers_flag = 1;

  //printf("  RWH LIB: seg_stats: num_wipers_off: %d, num_wipers_intermittent: %d, num_wipers_low: %d, num_wipers_high: %d\n", seg_stat.num_wipers_off, seg_stat.num_wipers_intermittent, seg_stat.num_wipers_low, seg_stat.num_wipers_high);
  
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
  
  //printf("  RWH LIB: wipers_flag: %d, wipers_off_flag: %d, wipers_on_flag: %d, wipers_interm_flag: %d, wipers_low_flag: %d, wipers_high_flag: %d\n", wipers_flag, wipers_off_flag, wipers_on_flag,  wipers_interm_flag, wipers_low_flag, wipers_high_flag);

  // VDT case using just radar-reflectivity
  // * Note, this now uses the dynamic radar-precip-thresholds passed into this function.
  //   The precip-threshold values are set in the set_precip_thresholds() function above.
  //
  // A note about the radar-ref value. -99 indicates non-missing but no-return (no precip falling)
  // FILL_VALUE = -9999 (Note above, we set values of -999 to FILL_VALUE, -999 indicates outside of any radar coverage)
  //
  if(radar_ref != vdt_const::FILL_VALUE)
    {
      if(radar_ref < RADAR_LIGHT_PRECIP)
	precip_intensity =  NO_PRECIP;
      else if(radar_ref >= RADAR_LIGHT_PRECIP && radar_ref < RADAR_MODERATE_PRECIP) 
	precip_intensity =  LIGHT_PRECIP;
      else if(radar_ref >= RADAR_MODERATE_PRECIP && radar_ref < RADAR_HEAVY_PRECIP)
	precip_intensity =  MODERATE_PRECIP;
      else
	precip_intensity =  HEAVY_PRECIP;
      
      confidence = confidence +  (radar_ref_input_conf * radar_ref_wgt);
    }


  //VDT case: adjust precip-intensity using radar-dual-pol-hid (hydro-class)
  //
  if(precip_intensity != vdt_const::FILL_VALUE && dual_pol_hid != vdt_const::FILL_VALUE)
    {
      printf("  RWH LIB: Adjusting vdt precip-intensity up or down based on dual_pol_hid: %d\n", dual_pol_hid);
      
      if(dual_pol_hid == HID_BIO_SCATTER || dual_pol_hid == HID_GROUND_CLUTTER)
	precip_intensity = NO_PRECIP;
      else if(dual_pol_hid == HID_LGT_MDT_RAIN || dual_pol_hid == HID_BIG_DROPS)
	{
	  if(precip_intensity == HEAVY_PRECIP)
	    precip_intensity =  MODERATE_PRECIP;
	}
      else if(dual_pol_hid == HID_HVY_RAIN || dual_pol_hid == HID_GRAUPEL)
	{
	  if(precip_intensity == LIGHT_PRECIP)
	    precip_intensity =  MODERATE_PRECIP;
	}
    }
  
  // VDT case: lower precip-intensity if radar-ref is lower than 35 dBZ and dewpoint-depression is too high (> 8 degC)
  //
  int dew_adjust_flag = 0;
  if(precip_intensity != vdt_const::FILL_VALUE && dew_depress != vdt_const::FILL_VALUE)
    {
      if(radar_ref < 35 && dew_depress > 8)
	{
	  if(precip_intensity > NO_PRECIP)
	    {
	      printf("  RWH LIB: Adjusting vdt precip-intensity down one category based radar_ref: %.2f and dewpoint-depression: %.2f\n", radar_ref, dew_depress);
	      dew_adjust_flag = 1;
	    }
	  
	  if(precip_intensity == HEAVY_PRECIP)
	    precip_intensity =  MODERATE_PRECIP;
	  else if(precip_intensity == MODERATE_PRECIP)
	    precip_intensity =  LIGHT_PRECIP;
	  else // previously LIGHT
	    precip_intensity = NO_PRECIP;
	}
    }
  
  // VDT case: cross-check precip-intensity with METAR present-wx report (precip-type and intensity)
  //
  float wx_precip_intensity = seg_stat.wx_precip_intensity;
  
  if(precip_intensity != vdt_const::FILL_VALUE && wx_precip_intensity != vdt_const::FILL_VALUE)
    {
      printf("  RWH LIB: Adjusting precip-intensity using METAR present-wx report\n");
      
      if(wx_precip_intensity == LIGHT_PRECIP)
	{
	  if(precip_intensity == NO_PRECIP)
	    {
	      //printf("  Changing precip_intensity from %.0f to LIGHT_PRECIP: wx_precip_intensity: %.0f\n", precip_intensity, wx_precip_intensity);
	      precip_intensity = LIGHT_PRECIP;
	    }
	  else if(precip_intensity == HEAVY_PRECIP)
	    {
	      //printf("  Changing precip_intensity from %.0f to MODERATE_PRECIP: wx_precip_intensity: %.0f\n", precip_intensity, wx_precip_intensity);
	      precip_intensity = MODERATE_PRECIP;
	    }
	}
      else if(wx_precip_intensity == MODERATE_PRECIP)
	{
	  if(precip_intensity == NO_PRECIP)
	    {
	      //printf("  Changing precip_intensity from %.0f to LIGHT_PRECIP: wx_precip_intensity: %.0f\n", precip_intensity, wx_precip_intensity);
	      precip_intensity = LIGHT_PRECIP;
	    }
	}
      else if(wx_precip_intensity == HEAVY_PRECIP)
	{
	  if(precip_intensity == NO_PRECIP || precip_intensity == LIGHT_PRECIP)
	    {
	      //printf("  Changing precip_intensity from %.0f to MODERATE_PRECIP: wx_precip_intensity: %.0f\n", precip_intensity, wx_precip_intensity);
	      precip_intensity = MODERATE_PRECIP; 
	    }
	}
    }

  // VDT case: If precip_intensity is still missing (e.g. no radar data, ect) just use wx_precip_intensity
  if(precip_intensity == vdt_const::FILL_VALUE && wx_precip_intensity != vdt_const::FILL_VALUE)
    {
      printf("  RWH LIBs: Precip_intensity is still missing, setting precip_intensity using wx_precip_intensity: %.0f\n", wx_precip_intensity);
      precip_intensity = wx_precip_intensity;
    }
  
  // VDT case: modify intensity based on wipers or determine intensity if we just have wipers and no radar data
  if(wipers_flag == 1)
    {
      printf("  RWH LIB: Adjusting vdt precip-intensity with wiper data\n");
      
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
	    {
	      if(dew_adjust_flag) // If we adjusted intensity down due to high dew-depression, bump back up to light if wipers are on (precip is likely hitting ground then)
		precip_intensity = LIGHT_PRECIP;
	      else
		precip_intensity = ROAD_SPLASH;
	    }
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
      //printf("  RWH LIB: Adjusting vdt precip-intensity with speed-ratio data\n");
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
      //printf("  RWH LIB: Adjusting vdt precip-intensity with headlights data\n");
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
  //
  // Get road-T
  float mobile_road_temp = seg_stat.surface_temp_mean;
  float env_road_temp = seg_stat.nss_surface_temp_mean;
  float road_temp = determine_temp(mobile_road_temp, env_road_temp);
  //printf("road_temp: %f\n", road_temp);

  // Get air-T
  float mobile_air_temp = get_air_temp(seg_stat);
  float env_air_temp = get_env_air_temp(seg_stat);
  float air_temp = determine_temp(mobile_air_temp, env_air_temp);

  // Look at difference between air-T and road-T
  // If absolue difference is > 25 degC set road-T to missing

  if(road_temp != vdt_const::FILL_VALUE && air_temp != vdt_const::FILL_VALUE)
    {
      float T_diff = fabs(road_temp - air_temp);
      if(T_diff > 25)
	{
	  //printf("The road_temp: %.2f, differs by more than 25 degC from the air_temp: %.2f, setting road_temp to missing\n", road_temp, air_temp);
	  road_temp = vdt_const::FILL_VALUE;
	}
    }
  
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
      else if(precip_type == HAIL)
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
      else if(precip_type == HAIL)
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
  // Re-worked to no longer use forecast-visibility as these values are bad
  // Check fcst_wind_speed to make sure we are dealing with forecast data
  if(fcst_wind_speed != vdt_const::FILL_VALUE && precip_type != vdt_const::FILL_VALUE && precip_intensity != vdt_const::FILL_VALUE)
    {
      if(precip_type == RAIN && precip_intensity == MODERATE_PRECIP)
	visibility = VIS_LOW;
      else if(precip_type == RAIN && precip_intensity == HEAVY_PRECIP)
	visibility = VIS_HEAVY_RAIN;
      else if(precip_type == SNOW && precip_intensity != NO_PRECIP)
	{
	  if(fcst_wind_speed != vdt_const::FILL_VALUE && fcst_wind_speed > BLOWING_SNOW_WIND_SPEED)
	    visibility = VIS_BLOWING_SNOW;
	  else if(precip_intensity == MODERATE_PRECIP)
	    visibility = VIS_LOW;
	  else if(precip_intensity == HEAVY_PRECIP)
	    visibility = VIS_HEAVY_SNOW;
	  else // for light snow
	    visibility = VIS_NORMAL;
	}
      else if(precip_type == MIX && (precip_intensity == HEAVY_PRECIP || precip_intensity == MODERATE_PRECIP))
	visibility = VIS_LOW;
      else // light rain, light mix or no precip at all
	visibility = VIS_NORMAL;
      
      confidence = confidence + (fcst_input_conf * fcst_wgt);
      return visibility;
    }

  // If the visibility is still missing look for fog
  // NOTE: We currently do not have prob_fog values from logicast rdwx files
  //       so this is a placeholder for now
  // We could use RH to calculate fog?
  //if(visibility == vdt_const::FILL_VALUE && fcst_prob_fog != vdt_const::FILL_VALUE && fcst_prob_fog > 0.5) 
  //visibility = VIS_FOG;
    
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
  printf("  RWH LIB, visibility-assessment: air_temp: %f, dew_temp: %f, rh: %f\n", air_temp, dew_temp, rh);
  
  // Get addtional VDT variables
  float wind_speed = seg_stat.nss_wind_speed_mean;
  
  float speed_ratio = seg_stat.speed_ratio;
  float env_vis = seg_stat.nss_prevail_vis_mean;

  float wx_visibility = seg_stat.wx_visibility;
  
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

  //printf("  RWH LIB: rh: %f, speed_ratio: %f, env_vis: %f, wx_visibility: %.0f, percent_lights_off: %f, percent_lights_on: %f, percent_lights_fog: %f, percent_lights_high_beam: %f\n", rh, speed_ratio, env_vis, percent_lights_off, percent_lights_on, percent_lights_fog, percent_lights_high_beam);
  
  // Calculate fog interest 
  float fog_interest = calc_fog_interest(rh, speed_ratio, env_vis, wx_visibility, percent_lights_fog, percent_lights_high_beam);
  printf("  RWH LIB, visibility-assessment: fog_interest: %f\n", fog_interest);
  
  // Calculate haze interest 
  float haze_interest = calc_haze_interest(rh, speed_ratio, env_vis, wx_visibility, percent_lights_fog, percent_lights_high_beam);
  printf("  RWH LIB, visibility-assessment: haze_interest: %f\n", haze_interest);

  // Calculate normal vis interest 
  float normal_interest = calc_normal_interest(rh, speed_ratio, env_vis, wx_visibility, percent_lights_fog, percent_lights_high_beam);
  printf("  RWH LIB, visibility-assessment: normal_interest: %f\n", normal_interest);
  
  // Hardwire the "input" confidence for now
  // Do we want to calculate this in the calc_fog, calc_haze and calc_normal interest functions?
  float fog_input_conf = 1.0;
  float haze_input_conf = 1.0;
  float normal_input_conf = 1.0;
  
  // VDT case precip-type and precip-intensity
  if(precip_type != vdt_const::FILL_VALUE && precip_intensity != vdt_const::FILL_VALUE)
    {
      if(precip_type == RAIN && precip_intensity == MODERATE_PRECIP)
	visibility = VIS_LOW;
      else if(precip_type == RAIN && precip_intensity == HEAVY_PRECIP)
	visibility = VIS_HEAVY_RAIN;
      else if(precip_type == SNOW && precip_intensity != NO_PRECIP)
	{
	  if(wind_speed != vdt_const::FILL_VALUE && wind_speed > BLOWING_SNOW_WIND_SPEED)
	    visibility = VIS_BLOWING_SNOW;
	  else if(precip_intensity == MODERATE_PRECIP)
	    visibility = VIS_LOW;
	  else if(precip_intensity == HEAVY_PRECIP)
	    visibility = VIS_HEAVY_SNOW;
	  else
	    visibility = VIS_NORMAL; // For LIGHT AND ROAD_SPLASH intensity for type SNOW
	}
      else if(precip_type == MIX && (precip_intensity == HEAVY_PRECIP || precip_intensity == MODERATE_PRECIP))
	visibility = VIS_LOW;
      else if(precip_type == HAIL && (precip_intensity == HEAVY_PRECIP || precip_intensity == MODERATE_PRECIP))
	visibility = VIS_LOW;
      else
	visibility = VIS_NORMAL;
      
      confidence = confidence + (precip_type_input_conf * precip_type_wgt) + (precip_intensity_input_conf * precip_intensity_wgt);
    }
  else if(precip_intensity != vdt_const::FILL_VALUE ) // VDT case with just precip-intensity 
    {
      if(precip_intensity == HEAVY_PRECIP || precip_intensity == MODERATE_PRECIP)
	visibility = VIS_LOW;
      else
	visibility = VIS_NORMAL;
      
      confidence = confidence + (precip_intensity_input_conf * precip_intensity_wgt);
    }
  else // VDT case when precip-type and precip-intensity are missing
    {
      visibility = VIS_NORMAL;
      confidence = confidence + (normal_input_conf * normal_wgt);
    }

  int precip_flag = 0;
  if(precip_intensity != vdt_const::FILL_VALUE && (precip_intensity == LIGHT_PRECIP || precip_intensity == MODERATE_PRECIP || precip_intensity == HEAVY_PRECIP))
    precip_flag = 1;
  
  // If visibility is still missing or the precip flag is zero (no precip) look for fog, haze or normal visibility hazards
  //
  if(visibility == vdt_const::FILL_VALUE || precip_flag == 0)
    {
      //printf("initial visibility: %d, normal_interest: %.2f, haze_interest: %.2f, fog_interest: %.2f\n", visibility, normal_interest, haze_interest, fog_interest);
      if(normal_interest >= haze_interest && normal_interest >= fog_interest)
	{
	  visibility = VIS_NORMAL;
	  confidence = confidence + (normal_input_conf * normal_wgt);
	}
      else if(haze_interest > normal_interest && haze_interest >= fog_interest)
	{
	  visibility = VIS_HAZE;
	  confidence = confidence + (haze_input_conf * haze_wgt);
	}
      else // fog_interest > normal_interest && fog_interest > haze_interest
	{
	  visibility = VIS_FOG;
	  confidence = confidence + (fog_input_conf * fog_wgt);
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
      else if(precip_type == HAIL)
	{
	  precip_intrst = 0.5;
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


bool rwx_road_segment_assessment::dry_pavement_slickness_assessment(vdt_probe_message_qc_statistics& seg_stat)
{
  bool pavement_slickness = 0;
  
  // Get abs, trac and stab fields
  int num_abs_on = seg_stat.num_abs_on;
  int num_abs_engaged = seg_stat.num_abs_engaged;
  int num_abs_off = seg_stat.num_abs_off;
  
  //printf("num_abs_on: %d, num_abs_engaged: %d, num_abs_off: %d\n", num_abs_on, num_abs_engaged, num_abs_off);
  
  int num_trac_on = seg_stat.num_trac_on;
  int num_trac_engaged = seg_stat.num_trac_engaged;
  int num_trac_off = seg_stat.num_trac_off;
  
  int num_stab_on = seg_stat.num_stab_on;
  int num_stab_engaged = seg_stat.num_stab_engaged;
  int num_stab_off = seg_stat.num_stab_off;
  
  float abs_percent;
  if(num_abs_on != vdt_const::FILL_VALUE && num_abs_engaged != vdt_const::FILL_VALUE && num_abs_off != vdt_const::FILL_VALUE)
    abs_percent = (float)(num_abs_on + num_abs_engaged) / (float)(num_abs_on + num_abs_engaged + num_abs_off);
  else
    abs_percent = 0;
  
  float trac_percent;
  if(num_trac_on != vdt_const::FILL_VALUE && num_trac_engaged != vdt_const::FILL_VALUE && num_trac_off != vdt_const::FILL_VALUE)
    trac_percent = (float)(num_trac_on + num_trac_engaged) / (float)(num_trac_on + num_trac_engaged + num_trac_off);
  else
    trac_percent = 0;
  
  float stab_percent;
  if(num_stab_on != vdt_const::FILL_VALUE && num_stab_engaged != vdt_const::FILL_VALUE && num_stab_off != vdt_const::FILL_VALUE)
    stab_percent = (float)(num_stab_on + num_stab_engaged) / (float)(num_stab_on + num_stab_engaged + num_stab_off);
  else
    stab_percent = 0;
  
  if(abs_percent >= 0.25 || trac_percent >= 0.25 || stab_percent >= 0.25)
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
      else if(precip_type == HAIL)
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


float rwx_road_segment_assessment::get_env_dew_temp(vdt_probe_message_qc_statistics& seg_stat)
{
  if (seg_stat.nss_dew_temp_mean == vdt_const::FILL_VALUE &&
      seg_stat.model_dewpoint == vdt_const::FILL_VALUE)
    {
      return vdt_const::FILL_VALUE;
    }
  
  // Use nss_dew_temp_mean if it is there, if not fall back on model_dewpoint
  float env_dew_temp;
  if(seg_stat.nss_dew_temp_mean != vdt_const::FILL_VALUE)
    env_dew_temp = seg_stat.nss_dew_temp_mean;
  else
    env_dew_temp = seg_stat.model_dewpoint;
  
  return env_dew_temp;
}


bool rwx_road_segment_assessment::wipers_on(vdt_probe_message_qc_statistics& seg_stat)
{
  int num_wipers_on = get_wipers_on(seg_stat);
  return (num_wipers_on > 0);
}


bool rwx_road_segment_assessment::wipers_off(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_off > seg_stat.num_wipers_intermittent &&
          seg_stat.num_wipers_off > seg_stat.num_wipers_high &&
          seg_stat.num_wipers_off > seg_stat.num_wipers_low);
  
}


bool rwx_road_segment_assessment::wipers_high(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_high >= seg_stat.num_wipers_intermittent &&
          seg_stat.num_wipers_high >= seg_stat.num_wipers_off &&
          seg_stat.num_wipers_high >= seg_stat.num_wipers_low);
  
}


bool rwx_road_segment_assessment::wipers_intermittent(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_intermittent >= seg_stat.num_wipers_low &&
          seg_stat.num_wipers_intermittent > seg_stat.num_wipers_high &&
          seg_stat.num_wipers_intermittent >= seg_stat.num_wipers_off);
  
}


bool rwx_road_segment_assessment::wipers_low(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_low > seg_stat.num_wipers_intermittent &&
          seg_stat.num_wipers_low > seg_stat.num_wipers_high &&
          seg_stat.num_wipers_low >= seg_stat.num_wipers_off);

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


float rwx_road_segment_assessment::calc_fog_interest(float rh, float speed_ratio, float station_vis, float wx_visibility, float percent_fog_lights, float percent_high_beams)
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
      if(rh < 0.60)
	rh_intrst = -1;
      else if(rh >= 0.60 && rh <= 0.75)
	rh_intrst = (rh/0.15)-(15/3);
      else if(rh > 0.75 && rh < 0.85)
	rh_intrst = 0;
      else if(rh >= 0.85 && rh <= 0.98)
	rh_intrst = (rh/0.13)-(0.85/0.13);
      else // rh > 0.98
	rh_intrst = 1;
    }
  else
    rh_intrst = 0;

  // Determine station visibility interest
  float vis_intrst;
  if(station_vis != vdt_const::FILL_VALUE)
    {
      if(station_vis < 2)
	vis_intrst = 1;
      else if(station_vis >= 2 && station_vis <= 5)
	vis_intrst = ((-1/3)*station_vis)+(5/3);
      else if(station_vis > 5 && station_vis < 8)
	vis_intrst = 0;
      else if(station_vis >= 8 && station_vis <= 10)
	vis_intrst = ((-1/2)*station_vis)+4;
      else // station_vis > 10
	vis_intrst = -1;
    }
  else
    vis_intrst = 0;
  
  // Determine observed wx interest from METAR present-wx visibility report
  float obs_wx_intrst;
  if(wx_visibility != vdt_const::FILL_VALUE)
    {
      if(wx_visibility == VIS_FOG)
	{
	  //printf("Wx_visibility indicates VIS_FOG, changing obs_wx_intrst to 1\n");
	  obs_wx_intrst = 1;
	}
      else
	{
	  //printf("Wx_visibility indicates NOT FOG, changing obs_wx_intrst to -1\n");
	  obs_wx_intrst = -1;
	}
    }
  else
    obs_wx_intrst = 0;
  
  // Determine fog lights interest
  float fog_lights_intrst;
  if(percent_fog_lights != vdt_const::FILL_VALUE)
    {
      if(percent_fog_lights <= 0)
	fog_lights_intrst = -1;
      else if(percent_fog_lights > 0 && percent_fog_lights < 1)
	fog_lights_intrst = (2*percent_fog_lights)-1;
      else // percent_fog_lights >= 1
	fog_lights_intrst = 1;
    }
  else
    fog_lights_intrst = 0;
      
  // Determine high beams interest
  float high_beams_intrst;
  if(percent_high_beams != vdt_const::FILL_VALUE)
    {
      if(percent_high_beams <= 0)
	high_beams_intrst = 1;
      else if(percent_high_beams >0 && percent_high_beams < 1)
	high_beams_intrst = (-2*percent_high_beams)+1;
      else // percent_high_beams >= 1
	high_beams_intrst = -1;
    }
  else
    high_beams_intrst = 0;
  
  // Determine speed ratio interest 
  float speed_ratio_intrst;
  if(speed_ratio != vdt_const::FILL_VALUE)
    {
      if(speed_ratio <= 0)
	speed_ratio_intrst = 1;
      else if(speed_ratio > 0 && speed_ratio < 1)
	speed_ratio_intrst = (-2*speed_ratio)+1;
      else
	speed_ratio_intrst = -1;
    }
  else
    speed_ratio_intrst = 0;
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC calc_fog_interest(): rh_intrst: %f, vis_intrst: %f, obs_wx_intrst: %f, fog_lights_intrst: %f, high_beams_intrst: %f, speed_ratio_intrst: %f\n", rh_intrst, vis_intrst, obs_wx_intrst, fog_lights_intrst, high_beams_intrst, speed_ratio_intrst);  
  
  // Set weights of fields for fog interest  
  float rh_wgt = 0.35;
  float vis_wgt = 0.2; 
  float obs_wx_wgt = 0.15;
  float fog_lights_wgt = 0.1;
  float high_beams_wgt = 0.1;
  float speed_ratio_wgt = 0.1;
  
  // Determine fog interest
  float fog_intrst = (rh_wgt*rh_intrst) + (vis_wgt*vis_intrst) + (obs_wx_wgt*obs_wx_intrst) + (fog_lights_wgt*fog_lights_intrst) + (high_beams_wgt*high_beams_intrst) + (speed_ratio_wgt*speed_ratio_intrst);
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: calc_fog_interest(): fog_intrst: %f\n", fog_intrst);
  
  return fog_intrst;
}


float rwx_road_segment_assessment::calc_haze_interest(float rh, float speed_ratio, float station_vis, float wx_visibility, float percent_fog_lights, float percent_high_beams)
{
  // Determine interest components for haze
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
      if(rh < 0.50)
	rh_intrst = -1;
      else if(rh >= 0.50 && rh <= 0.75)
	rh_intrst = ((2*rh)/0.25)-5;
      else if(rh > 0.75 && rh < 0.95)
	rh_intrst = 1;
      else if(rh >= 0.95 && rh <= 1)
	rh_intrst = ((-2/0.05)*rh)+39;
      else // rh > 1.00
	rh_intrst = -1;
    }
  else
    rh_intrst = 0;

  // Determine station visibility interest
  float vis_intrst;
  if(station_vis != vdt_const::FILL_VALUE)
    {
      if(station_vis < 2)
	vis_intrst = -1;
      else if(station_vis >= 2 && station_vis <= 4)
	vis_intrst = station_vis-3;
      else if(station_vis > 4 && station_vis < 8)
	vis_intrst = 1;
      else if(station_vis >= 8 && station_vis <= 11)
	vis_intrst = ((-2/3)*station_vis)+(19/3);
      else // station_vis > 11
	vis_intrst = -1;
    }
  else
    vis_intrst = 0;
  
  // Determine observed wx interest from METAR present-wx visibility report
  float obs_wx_intrst;
  if(wx_visibility != vdt_const::FILL_VALUE)
    {
      if(wx_visibility == VIS_HAZE)
	{
	  //printf("Wx_visibility indicates VIS_HAZE, changing obs_wx_intrst to 1\n");
	  obs_wx_intrst = 1;
	}
      else
	{
	  //printf("Wx_visibility indicates NOT HAZE, changing obs_wx_intrst to -1\n");
	  obs_wx_intrst = -1;
	}
    }
  else
    obs_wx_intrst = 0;
  
  // Determine fog lights interest
  float fog_lights_intrst;
  if(percent_fog_lights != vdt_const::FILL_VALUE)
    {
      if(percent_fog_lights < 0.5)
	fog_lights_intrst = 1;
      else if(percent_fog_lights >= 0.5 && percent_fog_lights < 1)
	fog_lights_intrst = (-4*percent_fog_lights)+3;
      else // percent_fog_lights >= 1
	fog_lights_intrst = -1;
    }
  else
    fog_lights_intrst = 0;
  
  // Determine high beams interest
  float high_beams_intrst;
  high_beams_intrst = 1;
  
  // Determine speed ratio interest 
  float speed_ratio_intrst;
  speed_ratio_intrst = 1;
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC calc_haze_interest(): rh_intrst: %f, vis_intrst: %f, obs_wx_intrst: %f, fog_lights_intrst: %f, high_beams_intrst: %f, speed_ratio_intrst: %f\n", rh_intrst, vis_intrst, obs_wx_intrst, fog_lights_intrst, high_beams_intrst, speed_ratio_intrst);  
  
  // Set weights of fields for haze interest  
  float rh_wgt = 0.35;
  float vis_wgt = 0.2; 
  float obs_wx_wgt = 0.15;
  float fog_lights_wgt = 0.1;
  float high_beams_wgt = 0.1;
  float speed_ratio_wgt = 0.1;
  
  // Determine fog interest
  float haze_intrst = (rh_wgt*rh_intrst) + (vis_wgt*vis_intrst) + (obs_wx_wgt*obs_wx_intrst) + (fog_lights_wgt*fog_lights_intrst) + (high_beams_wgt*high_beams_intrst) + (speed_ratio_wgt*speed_ratio_intrst);
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: calc_fog_interest(): haze_intrst: %f\n", haze_intrst);
  
  return haze_intrst;
}


float rwx_road_segment_assessment::calc_normal_interest(float rh, float speed_ratio, float station_vis, float wx_visibility, float percent_fog_lights, float percent_high_beams)
{
  // Determine interest components for normal visibility (no haze or fog)
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
      if(rh < 0.70)
	rh_intrst = 1;
      else if(rh >= 0.70 && rh <= 1.00)
	rh_intrst = ((-1/0.15)*rh)+(17/3);
      else // rh > 1.00
	rh_intrst = -1;
    }
  else
    rh_intrst = 0;

  // Determine station visibility interest
  float vis_intrst;
  if(station_vis != vdt_const::FILL_VALUE)
    {
      if(station_vis < 5)
	vis_intrst = -1;
      else if(station_vis >= 5 && station_vis <= 11)
	vis_intrst = (station_vis/3)-(8/3);
      else // station_vis > 11
	vis_intrst = 1;
    }
  else
    vis_intrst = 0;
  
  // Determine observed wx interest from METAR present-wx visibility report
  float obs_wx_intrst;
  if(wx_visibility != vdt_const::FILL_VALUE)
    {
      if(wx_visibility == VIS_HAZE || wx_visibility == VIS_FOG)
	{
	  //printf("In calc_normal_interest(): Wx_visibility indicates VIS_HAZE or VIS_FOG, changing obs_wx_intrst to -1\n");
	  obs_wx_intrst = -1;
	}
      else if(wx_visibility == VIS_NORMAL)
	{
	  //printf("In calc_normal_interest(): Wx_visibility is VIS_NORMAL, changing obs_wx_intrst to 1\n");
	  obs_wx_intrst = 1;
	}
      else
	{
	  //printf("In calc_normal_interest(): Wx_visibility indicates NOT HAZE and NOT FOG changing obs_wx_intrst to 1\n");
	  obs_wx_intrst = 1;
	}
    }
  else
    obs_wx_intrst = 0;
  
  // Determine fog lights interest
  float fog_lights_intrst;
  if(percent_fog_lights != vdt_const::FILL_VALUE)
    {
      if(percent_fog_lights <= 0)
	fog_lights_intrst = 1;
      else if(percent_fog_lights > 0 && percent_fog_lights < 1)
	fog_lights_intrst = (-2*percent_fog_lights)+1;
      else // percent_fog_lights >= 1
	fog_lights_intrst = -1;
    }
  else
    fog_lights_intrst = 0;
  
  // Determine high beams interest
  float high_beams_intrst;
  high_beams_intrst = 1;
  
  // Determine speed ratio interest 
  float speed_ratio_intrst;
  speed_ratio_intrst = 1;
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC calc_normal_interest(): rh_intrst: %f, vis_intrst: %f, obs_wx_intrst: %f, fog_lights_intrst: %f, high_beams_intrst: %f, speed_ratio_intrst: %f\n", rh_intrst, vis_intrst, obs_wx_intrst, fog_lights_intrst, high_beams_intrst, speed_ratio_intrst);  
  
  // Set weights of fields for normal interest  
  float rh_wgt = 0.35;
  float vis_wgt = 0.2; 
  float obs_wx_wgt = 0.15;
  float fog_lights_wgt = 0.1;
  float high_beams_wgt = 0.1;
  float speed_ratio_wgt = 0.1;
  
  // Determine fog interest
  float normal_intrst = (rh_wgt*rh_intrst) + (vis_wgt*vis_intrst) + (obs_wx_wgt*obs_wx_intrst) + (fog_lights_wgt*fog_lights_intrst) + (high_beams_wgt*high_beams_intrst) + (speed_ratio_wgt*speed_ratio_intrst);
  
  //printf("RWX_ROAD_SEGMENT_ASSESSMENT.CC: calc_normal_interest(): normal_intrst: %f\n", normal_intrst);
  
  return normal_intrst;
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
