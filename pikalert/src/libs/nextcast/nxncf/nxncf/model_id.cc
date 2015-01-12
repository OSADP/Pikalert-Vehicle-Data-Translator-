// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: model_id.cc
 *
 * Author: Jim Cowie
 *
 * Date:   05/08/09
 *
 * Description:  Gets model name from ID.
 *     
 */

// Include files 
#include <nxncf/model_id.h>



// get_model_name(): returns model name from model_id, or 'UNKNOWN' if
// model_id is not defined.


const char *get_model_name(int model_id)
{
  switch (model_id)
    {
    case ACCESS_DMOS_MODEL_ID:
      return((const char *)"ACCESS");
    case CLIMATOLOGY_MODEL_ID:
      return((const char *)"CLIMO");
    case CMC_GEAVG_DMOS_MODEL_ID:
      return((const char *)"CMC_GEAVG");
    case ECMWF_DMOS_MODEL_ID:
      return((const char *)"ECMWF");
    case ECMWF_EPS_DMOS_MODEL_ID:
      return((const char *)"ECMWF_EPS");
    case ECMWF_EPSAVG_DMOS_MODEL_ID:
      return((const char *)"ECMWF_EPSAVG");
    case GEM_DMOS_MODEL_ID:
      return((const char *)"GEM");
    case GFS_DMOS_MODEL_ID:
      return((const char *)"GFS");
    case GFS_GEAVG_DMOS_MODEL_ID:
      return((const char *)"GFS_GEAVG");
    case HRRR_DMOS_MODEL_ID:
      return((const char *)"HRRR");
    case HIRLAM_DMOS_MODEL_ID:
      return((const char *)"HIRLAM");
    case LAMPMOS_MODEL_ID:
      return((const char *)"LAMP");
    case MAVMOS_MODEL_ID:
      return((const char *)"MAV");
    case METMOS_MODEL_ID:
      return((const char *)"MET");
    case MEXMOS_MODEL_ID:
      return((const char *)"MEX");
    case NAM_DMOS_MODEL_ID:
      return((const char *)"NAM");
    case RALWRF_DMOS_MODEL_ID:
      return((const char *)"RALWRF");
    case RUC_DMOS_MODEL_ID:
      return((const char *)"RUC");
    case RAP_DMOS_MODEL_ID:
      return((const char *)"RAP");
    case SREF_MEAN_DMOS_MODEL_ID:
      return((const char *)"SREF_MEAN");
    case UKMET_DMOS_MODEL_ID:
      return((const char *)"UKMET");
    default:
      return((const char *)"UNKNOWN");
    }
}
