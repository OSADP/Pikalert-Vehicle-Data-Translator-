// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:20:23 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include "netcdf.h"
#include "precip_type.hh"
#include "Params.hh"


extern Params conf_params;


// These are now obtained through the TDRP params structure

//static const float PROB_THRESH = 0.25;
//static const float QPF_THRESH = 0.05;
//static const float ICE_THRESH = 0.25;
//static const float SNOW_THRESH = 0.40;


int find_precip_type(float T, float qpf, float prob_precip,
		     float cprob_rain, float cprob_snow, float cprob_ice)
{
  if (prob_precip == NC_FILL_FLOAT || qpf == NC_FILL_FLOAT)
    return(C_PTYPE_NONE);

  // No precip
  if (prob_precip < conf_params.pop_threshold || qpf < conf_params.qpf_threshold)
    return(C_PTYPE_NONE);

  if (cprob_rain == NC_FILL_FLOAT || cprob_snow == NC_FILL_FLOAT || 
      cprob_ice == NC_FILL_FLOAT)
    {
      if (T != NC_FILL_FLOAT && T > 0.)
	return(C_PTYPE_RAIN);
      else if (T != NC_FILL_FLOAT && T <= 0.)
	return(C_PTYPE_SNOW);
      else
	return(C_PTYPE_NONE); // for no good reason...
    }

  // otherwise determine precip type
  if (cprob_ice > conf_params.ice_threshold)
    return(C_PTYPE_FRZRAIN);
  else if (cprob_snow > conf_params.snow_threshold)
    return(C_PTYPE_SNOW);
  else
    return(C_PTYPE_RAIN);
  
}
