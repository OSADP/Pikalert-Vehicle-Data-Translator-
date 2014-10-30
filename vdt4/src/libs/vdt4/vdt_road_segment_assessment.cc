// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_road_segment_assessment.hh"
#include "vdt_const.hh"

const int vdt_road_segment_assessment::NO_PRECIP = 0;
const int vdt_road_segment_assessment::PRECIP_RAIN = 1;
const int vdt_road_segment_assessment::PRECIP_HEAVY_RAIN = 2;
const int vdt_road_segment_assessment::PRECIP_FROZEN = 3;
const int vdt_road_segment_assessment::PRECIP_HEAVY_FROZEN = 4;

const int vdt_road_segment_assessment::DRY_PAVEMENT = 5;
const int vdt_road_segment_assessment::WET_PAVEMENT = 6;
const int vdt_road_segment_assessment::ROAD_SPLASH = 7;
const int vdt_road_segment_assessment::SNOW_COVERED = 8;
const int vdt_road_segment_assessment::SLICK_PAVEMENT = 9;

const int vdt_road_segment_assessment::VIS_HEAVY_RAIN = 10;
const int vdt_road_segment_assessment::VIS_BLOWING_SNOW = 11;
const int vdt_road_segment_assessment::VIS_LOW = 12;
const int vdt_road_segment_assessment::VIS_NORMAL = 13;

const int vdt_road_segment_assessment::NO_HAZARDS = 14;
const int vdt_road_segment_assessment::HEAVY_RAIN_HAZARD = 15;
const int vdt_road_segment_assessment::LOW_VIS_HAZARD = 16;
const int vdt_road_segment_assessment::HEAVY_SNOW_HAZARD = 17;
const int vdt_road_segment_assessment::HEAVY_FREEZING_MIX_HAZARD = 18;
const int vdt_road_segment_assessment::SLIPPERY_ROADS_HAZARD = 19;

vdt_road_segment_assessment::vdt_road_segment_assessment()
{
  precip_type = vdt_const::FILL_VALUE;
  pavement_condition = vdt_const::FILL_VALUE;
  visibility = vdt_const::FILL_VALUE;
}

void vdt_road_segment_assessment::perform_assessment(vdt_probe_message_qc_statistics& seg_stat)
{
  road_segment_id = seg_stat.id;
  precip_type = precip_assessment(seg_stat);
  pavement_condition = pavement_condition_assessment(seg_stat);
  visibility = visibility_assessment(seg_stat);
  all_hazards = all_hazards_assessment(precip_type, pavement_condition, visibility);
}

int vdt_road_segment_assessment::precip_assessment(vdt_probe_message_qc_statistics& seg_stat)
{
  float air_temp = get_air_temp(seg_stat);
  if (air_temp > 5)
    {
      return precip_type_rain_branch(seg_stat);
    }
  else if (air_temp > -5 && air_temp <= 5)
    {
      return precip_type_mixed_branch(seg_stat);
    }
  else if (air_temp <= -5)
    {
      return precip_type_frozen_branch(seg_stat);
    }
  return vdt_const::FILL_VALUE;
}

int vdt_road_segment_assessment::pavement_condition_assessment(vdt_probe_message_qc_statistics& seg_stat)
{
  double dry = get_dry_pavement(seg_stat);
  double wet = get_wet_pavement(seg_stat);
  double snow = get_snow_covered_pavement(seg_stat);
  double slick = get_slick_pavement(seg_stat);
  double splash = get_road_splash_pavement(seg_stat);
  if (is_max(dry, wet, snow, slick, splash))
    {
      return DRY_PAVEMENT;
    }
  if (is_max(wet, dry, snow, slick, splash))
    {
      return WET_PAVEMENT;
    }
  if (is_max(snow, dry, wet, slick, splash))
    {
      return SNOW_COVERED;
    }
  if (is_max(slick, dry, wet, snow, splash))
    {
      return SLICK_PAVEMENT;
    }
  if (is_max(splash, dry, wet, snow, slick))
    {
      return ROAD_SPLASH;
    }
  return vdt_const::FILL_VALUE;
}

int vdt_road_segment_assessment::visibility_assessment(vdt_probe_message_qc_statistics& seg_stat, int precip_assessment)
{
  if (precip_assessment == PRECIP_HEAVY_RAIN)
    {
      return VIS_HEAVY_RAIN;
    }
  if (precip_assessment == PRECIP_FROZEN ||
      precip_assessment == PRECIP_HEAVY_FROZEN)
    {
      if (seg_stat.nss_wind_speed_mean == vdt_const::FILL_VALUE)
        {
          return vdt_const::FILL_VALUE;
        }
      else if (seg_stat.nss_wind_speed_mean > 10)
        {
          return VIS_BLOWING_SNOW;
        }
    }
  return visibility_assessment(seg_stat);
}

int vdt_road_segment_assessment::visibility_assessment(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;
  if (wipers_on(seg_stat))
    {
      interest += 0.25;
    }
  if (seg_stat.nss_prevail_vis_mean != vdt_const::FILL_VALUE &&
      seg_stat.nss_prevail_vis_mean <= 5)
    {
      interest += 0.25;
    }
  if (interest >= 0.5)
    {
      return VIS_LOW;
    }
  return VIS_NORMAL;
}

int vdt_road_segment_assessment::all_hazards_assessment(int precip_type, int pavement_condition, int visibility)
{
  if (pavement_condition == SLICK_PAVEMENT)
    {
    return SLIPPERY_ROADS_HAZARD;
    }

  if (precip_type == PRECIP_HEAVY_FROZEN)
    {
    return HEAVY_FREEZING_MIX_HAZARD;
    }

  if (((precip_type == PRECIP_HEAVY_FROZEN) && (pavement_condition == SNOW_COVERED)) || ((pavement_condition == SNOW_COVERED) && (visibility == VIS_BLOWING_SNOW)))
    {
    return HEAVY_SNOW_HAZARD;
    }

  if (visibility == VIS_LOW)
    {
    return LOW_VIS_HAZARD;
    }

  if (precip_type == PRECIP_HEAVY_RAIN)
    {
    return HEAVY_RAIN_HAZARD;
    }

  if ((precip_type == NO_PRECIP) && (pavement_condition == DRY_PAVEMENT) && (visibility == VIS_NORMAL))
    {
    return NO_HAZARDS;
    }

  return vdt_const::FILL_VALUE;
}

int vdt_road_segment_assessment::get_wipers_on(vdt_probe_message_qc_statistics& seg_stat)
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

float vdt_road_segment_assessment::get_air_temp(vdt_probe_message_qc_statistics& seg_stat)
{
  if (seg_stat.air_temp_mean == vdt_const::FILL_VALUE &&
      seg_stat.air_temp2_mean == vdt_const::FILL_VALUE)
    {
      return vdt_const::FILL_VALUE;
    }
  //use air_temp_mean if it is there, if not fall back on air_temp2_mean
  float air_temp = seg_stat.air_temp_mean != vdt_const::FILL_VALUE ? seg_stat.air_temp_mean : seg_stat.air_temp2_mean;
  return air_temp;
}

bool vdt_road_segment_assessment::wipers_intermittent(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_intermittent > seg_stat.num_wipers_low &&
          seg_stat.num_wipers_intermittent > seg_stat.num_wipers_high &&
          seg_stat.num_wipers_intermittent > seg_stat.num_wipers_off);
}

bool vdt_road_segment_assessment::wipers_high(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_high > seg_stat.num_wipers_intermittent &&
          seg_stat.num_wipers_high > seg_stat.num_wipers_off &&
          seg_stat.num_wipers_high > seg_stat.num_wipers_low);
}

bool vdt_road_segment_assessment::wipers_low(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_low > seg_stat.num_wipers_intermittent &&
          seg_stat.num_wipers_low > seg_stat.num_wipers_high &&
          seg_stat.num_wipers_low > seg_stat.num_wipers_off);
}

bool vdt_road_segment_assessment::wipers_off(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_off > seg_stat.num_wipers_intermittent &&
          seg_stat.num_wipers_off > seg_stat.num_wipers_high &&
          seg_stat.num_wipers_off > seg_stat.num_wipers_low);
}

bool vdt_road_segment_assessment::wipers_on(vdt_probe_message_qc_statistics& seg_stat)
{
  int num_wipers_on = get_wipers_on(seg_stat);
  return (num_wipers_on > 0);
}

bool vdt_road_segment_assessment::abs_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_abs > 0 &&
          seg_stat.num_abs_engaged > (seg_stat.num_abs_on + seg_stat.num_abs_off));
}

bool vdt_road_segment_assessment::abs_not_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_abs > 0 &&
          (seg_stat.num_abs_on + seg_stat.num_abs_off) > seg_stat.num_abs_engaged);
}

bool vdt_road_segment_assessment::trac_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_trac > 0 &&
          seg_stat.num_trac_engaged > (seg_stat.num_trac_on + seg_stat.num_trac_off));
}

bool vdt_road_segment_assessment::trac_not_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_trac > 0 &&
          (seg_stat.num_trac_on + seg_stat.num_trac_off) > seg_stat.num_trac_engaged);
}

bool vdt_road_segment_assessment::stab_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_stab > 0 &&
          seg_stat.num_stab_engaged > (seg_stat.num_stab_on + seg_stat.num_stab_off));
}

bool vdt_road_segment_assessment::stab_not_engaged(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_msg_valid_stab > 0 &&
          (seg_stat.num_stab_on + seg_stat.num_stab_off) > seg_stat.num_stab_engaged);
}

bool vdt_road_segment_assessment::wipers_missing(vdt_probe_message_qc_statistics& seg_stat)
{
  return (seg_stat.num_wipers_off == vdt_const::FILL_VALUE &&
          seg_stat.num_wipers_intermittent == vdt_const::FILL_VALUE &&
          seg_stat.num_wipers_low == vdt_const::FILL_VALUE &&
          seg_stat.num_wipers_high == vdt_const::FILL_VALUE);
}

int vdt_road_segment_assessment::precip_type_frozen_branch(vdt_probe_message_qc_statistics& seg_stat)
{
  float radar_ref = seg_stat.radar_ref;
  if (radar_ref == vdt_const::FILL_VALUE)
    {
      return vdt_const::FILL_VALUE;
    }
  float speed_ratio = seg_stat.speed_ratio;
  if (radar_ref < -10)
    {
      return NO_PRECIP;
    }
  else
    {
      if (wipers_missing(seg_stat))
        {
          return vdt_const::FILL_VALUE;
        }
      if (wipers_off(seg_stat))
        {
          if (speed_ratio == vdt_const::FILL_VALUE)
            {
              return vdt_const::FILL_VALUE;
            }
          if (speed_ratio >= 0.2 &&
              speed_ratio < 0.7)
            {
              return PRECIP_FROZEN;
            }
          else
            {
              return NO_PRECIP;
            }
        }
      else if (wipers_intermittent(seg_stat) ||
               wipers_on(seg_stat))
        {
          if (speed_ratio == vdt_const::FILL_VALUE)
            {
              return vdt_const::FILL_VALUE;
            }
          if (speed_ratio >= 0.2 &&
              speed_ratio < 0.7)
            {
              return PRECIP_HEAVY_FROZEN;
            }
          else
            {
              return PRECIP_FROZEN;
            }
        }
      else if (wipers_high(seg_stat))
        {
          return PRECIP_HEAVY_FROZEN;
        }
    }
  return vdt_const::FILL_VALUE;
}

int vdt_road_segment_assessment::precip_type_mixed_branch(vdt_probe_message_qc_statistics& seg_stat)
{
  float radar_ref = seg_stat.radar_ref;
  if (radar_ref == vdt_const::FILL_VALUE)
    {
      return vdt_const::FILL_VALUE;
    }
  int cloud_mask = seg_stat.cloud_mask;
  float speed_ratio = seg_stat.speed_ratio;
  float air_temp = get_air_temp(seg_stat);
  float vis = seg_stat.nss_prevail_vis_mean;
  if (radar_ref <= -99)
    {
      if (wipers_missing(seg_stat))
        {
          return vdt_const::FILL_VALUE;
        }
      else if (wipers_off(seg_stat))
        {
          return NO_PRECIP;
        }
      else
        {
          return PRECIP_FROZEN;
        }
    }
  else if (radar_ref > -99 &&
           radar_ref <= 10)
    {
      if (wipers_missing(seg_stat))
        {
          return vdt_const::FILL_VALUE;
        }
      else if (wipers_off(seg_stat))
        {
          if (cloud_mask == vdt_const::FILL_VALUE)
            {
              return vdt_const::FILL_VALUE;
            }
          else if (cloud_mask == 0)
            {
              return NO_PRECIP;
            }
          else if (cloud_mask > 0)
            {
              if (speed_ratio == vdt_const::FILL_VALUE)
                {
                  return vdt_const::FILL_VALUE;
                }
              else if (speed_ratio >= 0.2 &&
                       speed_ratio <= 0.7)
                {
                  return PRECIP_FROZEN;
                }
              else
                {
                  return NO_PRECIP;
                }
            }
        }
      else if (wipers_intermittent(seg_stat) ||
               wipers_high(seg_stat))
        {
          return PRECIP_FROZEN;
        }
      else if (wipers_low(seg_stat))
        {
          if (speed_ratio >= 0.2 &&
              speed_ratio < 0.6)
            {
              return PRECIP_HEAVY_FROZEN;
            }
          else if (speed_ratio >= 0.6)
            {
              return PRECIP_FROZEN;
            }
        }
    }
  else if (radar_ref > 10 &&
           radar_ref <= 30)
    {
      if (wipers_off(seg_stat))
        {
          if (speed_ratio == vdt_const::FILL_VALUE)
            {
              return vdt_const::FILL_VALUE;
            }
          if (speed_ratio >= 0.2 &&
              speed_ratio < 0.6)
            {
              return PRECIP_FROZEN;
            }
          else
            {
              return NO_PRECIP;
            }
        }
      else if (wipers_intermittent(seg_stat))
        {
          if (speed_ratio == vdt_const::FILL_VALUE)
            {
              return vdt_const::FILL_VALUE;
            }
          if (speed_ratio >= 0.2 &&
              speed_ratio < 0.7)
            {
              if (radar_ref > 25)
                {
                  return PRECIP_HEAVY_FROZEN;
                }
              else
                {
                  return PRECIP_FROZEN;
                }
            }
          else
            {
              if (air_temp == vdt_const::FILL_VALUE)
                {
                  return vdt_const::FILL_VALUE;
                }
              if (air_temp > 0)
                {
                  return PRECIP_RAIN;
                }
              else
                {
                  return PRECIP_FROZEN;
                }
            }
        }
      else if (wipers_low(seg_stat))
        {
          if (speed_ratio == vdt_const::FILL_VALUE)
            {
              return vdt_const::FILL_VALUE;
            }
          if (speed_ratio >= 0.2 &&
              speed_ratio < 0.5)
            {
              return PRECIP_FROZEN;
            }
          else
            {
              return PRECIP_RAIN;
            }
        }
      else if (wipers_high(seg_stat))
        {
          if (speed_ratio == vdt_const::FILL_VALUE)
            {
              return vdt_const::FILL_VALUE;
            }
          if (speed_ratio >= 0.2 &&
              speed_ratio < 0.5)
            {
              return PRECIP_HEAVY_FROZEN;
            }
          else
            {
              return PRECIP_HEAVY_RAIN;
            }
        }
    }
  else if (radar_ref > 30)
    {
      if (vis == vdt_const::FILL_VALUE)
        {
          return vdt_const::FILL_VALUE;
        }
      if (vis > 5)
        {
          if (wipers_missing(seg_stat))
            {
              return vdt_const::FILL_VALUE;
            }
          if (wipers_off(seg_stat) ||
              wipers_intermittent(seg_stat))
            {
              return PRECIP_RAIN;
            }
          else
            {
              return PRECIP_HEAVY_RAIN;
            }
        }
      else
        {
          if (speed_ratio == vdt_const::FILL_VALUE)
            {
              return vdt_const::FILL_VALUE;
            }
          if (speed_ratio >= 0.2 &&
              speed_ratio < 0.7)
            {
              return PRECIP_HEAVY_FROZEN;
            }
          else
            {
              return PRECIP_FROZEN;
            }
        }
    }
  return vdt_const::FILL_VALUE;
}

int vdt_road_segment_assessment::precip_type_rain_branch(vdt_probe_message_qc_statistics& seg_stat)
{
  float radar_ref = seg_stat.radar_ref;
  if (radar_ref == vdt_const::FILL_VALUE)
    {
      return vdt_const::FILL_VALUE;
    }

  if (radar_ref < 0)
    {
      return NO_PRECIP;
    }
  int num_wipers_on = get_wipers_on(seg_stat);
  if (radar_ref >= 0 &&
      radar_ref <= 30)
    {
      if (num_wipers_on == 0)
        {
          return NO_PRECIP;
        }
      else if (seg_stat.num_wipers_intermittent > 0 ||
               seg_stat.num_wipers_low > 0)
        {
          return PRECIP_RAIN;
        }
      else if (seg_stat.num_wipers_high > 0)
        {
          return PRECIP_HEAVY_RAIN;
        }
    }
  if (radar_ref > 30)
    {
      if (seg_stat.num_wipers_intermittent > 0 ||
          seg_stat.num_wipers_off > 0)
        {
          return PRECIP_RAIN;
        }
      else if (seg_stat.num_wipers_low > 0 ||
               seg_stat.num_wipers_high > 0)
        {
          return PRECIP_HEAVY_RAIN;
        }
    }
  return vdt_const::FILL_VALUE;
}

bool vdt_road_segment_assessment::is_max(double val, double t1, double t2, double t3, double t4)
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

double vdt_road_segment_assessment::get_dry_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;
  if (seg_stat.speed_ratio != vdt_const::FILL_VALUE)
    {
      interest += seg_stat.speed_ratio;
    }
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
  interest += get_interest(seg_stat.lat_accel_mean,0,0.2,1);
  interest += get_interest(seg_stat.lat_accel_mean,0.2,0.24,-25,6);
  interest += get_interest(seg_stat.lon_accel_mean,0,0.5,1);
  interest += get_interest(seg_stat.lon_accel_mean,0.5,1,-2,2);
  interest += get_interest(seg_stat.yaw_mean,0,1,1);
  interest += get_interest(seg_stat.yaw_mean,1,1.7,-1.428,2.428);
  interest += get_interest(seg_stat.steering_angle_mean,0,4,-0.25,1);
  return interest;
}

double vdt_road_segment_assessment::get_wet_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;

  interest += get_interest(seg_stat.speed_ratio, 0, 0.8, 1.25, 0);
  interest += get_interest(seg_stat.speed_ratio, 0.8, 1.0, -1.5, 2.2);

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

double vdt_road_segment_assessment::get_snow_covered_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;
  interest += get_interest(seg_stat.speed_ratio, 0, 0.6, 0.833, 0.502);
  interest += get_interest(seg_stat.speed_ratio, 0.6, 1, -2, 2.2);

  interest += get_interest(seg_stat.air_temp_mean, -10, 0, 1);
  interest += get_interest(seg_stat.air_temp_mean, 0, 1, -1, 1);

  interest += get_interest(seg_stat.radar_ref,0, 20, 0.025, 0.5);
  interest += get_interest(seg_stat.radar_ref,20, 25, 1);
  if (wipers_on(seg_stat))
    {
      interest += 1;
    }
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

double vdt_road_segment_assessment::get_slick_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;
  interest += get_interest(seg_stat.speed_ratio, 0, 0.8, -1.25, 1);

  interest += get_interest(seg_stat.air_temp_mean, -10, -6, 1);
  interest += get_interest(seg_stat.air_temp_mean, -6, 1, -0.1428, 0.1432);

  interest += get_interest(seg_stat.radar_ref, 0, 20, 0.025, 0.5);
  interest += get_interest(seg_stat.radar_ref, 20, 25, 1);
  if (wipers_low(seg_stat) || wipers_high(seg_stat) || wipers_intermittent(seg_stat))
    {
      interest += 1;
    }
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

double vdt_road_segment_assessment::get_road_splash_pavement(vdt_probe_message_qc_statistics& seg_stat)
{
  double interest = 0.0;

  interest += get_interest(seg_stat.speed_ratio,0,0.8,1.25,0);
  interest += get_interest(seg_stat.speed_ratio,0.8,1,-1.5,2.2);

  interest += get_interest(seg_stat.air_temp_mean,-10,0,0.1,1);
  interest += get_interest(seg_stat.air_temp_mean,0,5,1);

  interest += get_interest(seg_stat.radar_ref, 0, 25, 1);

  if (wipers_low(seg_stat) || wipers_high(seg_stat))
    {
      interest += 1;
    }

  interest += get_abs_stab_trac_interest(seg_stat, 0.5, 1.0);

  interest += get_interest(seg_stat.lat_accel_mean, 0.2, 0.4, -5, 2);

  interest += get_interest(seg_stat.lon_accel_mean, 0.5, 1, -2, 2);

  interest += get_interest(seg_stat.yaw_mean, 1, 1.7, -1.428, 2.428);

  interest += get_interest(seg_stat.steering_angle_mean, 0, 4, 0.25, 1);

  return interest;
}

double vdt_road_segment_assessment::get_abs_stab_trac_interest(vdt_probe_message_qc_statistics& seg_stat, double engaged, double not_engaged)
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

double vdt_road_segment_assessment::get_interest(double val, double slope_min, double slope_max, double m, double b)
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

double vdt_road_segment_assessment::get_interest(double val, double min, double max, double flat_val)
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


vdt_road_segment_assessment & vdt_road_segment_assessment::operator=(const vdt_road_segment_assessment &assessment)
{
  if (this == &assessment)
    {
      return *this;
    }

  vdt_road_segment_assessment::operator= (assessment);

  precip_type = assessment.precip_type;
  pavement_condition = assessment.pavement_condition;
  visibility = assessment.visibility;
  road_segment_id = assessment.road_segment_id;
  all_hazards = assessment.all_hazards;

  return *this;  
}
