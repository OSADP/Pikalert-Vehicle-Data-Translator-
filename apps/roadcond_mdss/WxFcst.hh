// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:23:05 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: WxFcst.hh
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Modified by: Julie Prestopnik
 *
 * Date: 01/29/07
 *
 * Description: This file contains definitions of constants and 
 *              structures used to describe the weather forecast
 *              parameters relevant to this application. The weather
 *              at one forecast lead time is described by the
 *              structure "WxForecast". A time series of forecasts
 *              is described by "WxFcstTS".
 *
 */


// Precipitation types
// Determined by using % probabilities of each type, for now only 
// three types -- rain, snow, or freezing rain -- unclear if a 
// "mixed" precip mode would be useful for this application 
// (perhaps useful for chemical conc or snow depth calculations or
// for a more complicated treatment module that uses chemical dilution).
//
#define C_PTYPE_NONE 0
#define C_PTYPE_RAIN 1
#define C_PTYPE_SNOW 2
#define C_PTYPE_MIXED 3 // snow/rain
#define C_PTYPE_SLEET 4 // snow/rain
#define C_PTYPE_FRZRAIN 5
#define C_N_PTYPES 6

const float MM_TO_CM = 0.1;
const float CM_TO_MM = 10;
const float MPS_TO_KMPH = 3.6;       // m/s to km/h
const float OCTAL = 8;
const float MM_PER_M = 1000.;
const float MM_PER_INCH = 25.4;      // approximately
const float SNOW_TO_SWE_RATIO = 10.; // "Idealized" dry snow -- may want to
//                                       use a different value for warm storms
//                                       Note: SWE means "Snow-Water Equivalent"

const int FCST_MISSING_VALUE = -9999;

// This is now set dynamically in read_wx.cc
//const int MAX_FORECAST = 192;

struct wxforecast {
  float T;               // air Temp (degC)
  float dewpt;           // dew point (degC)
  float rh;              // relative humidity (%)
  float wind_speed;      // wind speed (m/s)
  float wind_dir;         // wind direction clockwise from north (degrees)
  float P_sfc;            // pressure at 2m above sfc (mb)
  float cloud_cov;        // cloud cover (percent need octal)
  float snow_accum;       // snowfall accumulation in last hour
  float qpf01;           // quantative precip forecast (precip amount expected)
  float LiquidPrecipRate; // SWE in inches/hr
  float FrozenPrecipRate; // Use 10-1 ratio
  int Ptype;              // See precip types described above
  float dlwrf_sfc;        // Downward long wave radiation flux
  float dswrf_sfc;        // Downward short wave radiation flux
  int blowing_snow_potential;   // Blowing snow potential (0-3 index)
  float blowing_snow_pot_vals;  // Blowing snow potential (0.0 - 1.0)
};
typedef struct wxforecast WxForecast;

struct wx_fcst_time_series {
  //WxForecast fcst[MAX_FORECAST];
  WxForecast *fcst;  // The size is allocated dynamically in read_wx.cc
};
typedef struct wx_fcst_time_series WxFcstTS;

