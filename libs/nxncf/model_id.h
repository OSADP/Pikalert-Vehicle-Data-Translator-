#ifndef MODEL_ID_H
#define MODEL_ID_H


using namespace std;

namespace
{
  // Model ID's. Some of these are used in their respective forecast module
  // (climo, MAV and MEX mos) while others are defined here but not used in
  // any code. Instead, they are also defined and used from a config file.
  
  // Historically, the last two digits of the model id refered to the run
  // time (ie, 00 = 00Z run, 12 = 12Z run, etc). This was relevant when each
  // model run was considered independent and all runs were integrated into
  // a forecast. That is no longer done however, as we use only the most recent
  // run of a model, hence only the values ending in 00 are in use today.
  // There is also no longer a distinction between ST and LT model IDs.
  
  // **************************************************************************
  // If you add a model to this list, kindly add a case to the get_model_name()
  // function so we have a name for it. (BTW, I tried replacing all this with
  // a static map (model_id -> string name) but it can't be done. JRC)
  // **************************************************************************

  const int MRF_DMOS_MODEL_ID = 100;
  const int AVN_00Z_DMOS_MODEL_ID = 200;
  const int AVN_12Z_LT_DMOS_MODEL_ID = 212;
  const int AVN_00Z_ST_DMOS_MODEL_ID = 300;
  const int AVN_12Z_ST_DMOS_MODEL_ID = 312;
  const int GFS_00Z_DMOS_MODEL_ID = 300;
  const int GFS_12Z_DMOS_MODEL_ID = 312;
  const int GFS_DMOS_MODEL_ID = GFS_00Z_DMOS_MODEL_ID;
  const int GFS_GEAVG_DMOS_MODEL_ID = 360;
  const int ETA_00Z_DMOS_MODEL_ID = 1000;
  const int ETA_12Z_DMOS_MODEL_ID = 1012;
  const int ETA_DMOS_MODEL_ID = ETA_00Z_DMOS_MODEL_ID;
  const int NAM_DMOS_MODEL_ID = ETA_DMOS_MODEL_ID;
  const int SREF_MEAN_DMOS_MODEL_ID = 1100;
  const int NGMMOS_00Z_MODEL_ID = 8100;
  const int NGMMOS_12Z_MODEL_ID = 8112;
  const int AVNMOS_12Z_MODEL_ID = 8212;
  const int MRFMOS_MODEL_ID = 8300;
  const int MEXMOS_00Z_MODEL_ID = 8400;
  const int MEXMOS_MODEL_ID = MEXMOS_00Z_MODEL_ID;
  const int MAVMOS_00Z_MODEL_ID = 8500;
  const int MAVMOS_12Z_MODEL_ID = 8512;
  const int MAVMOS_MODEL_ID = MAVMOS_00Z_MODEL_ID;
  const int METMOS_MODEL_ID = 8600;
  const int LAMPMOS_MODEL_ID = 8700;
  const int ST_PERSISTENCE_MODEL_ID = 9198;
  const int PERSISTENCE_MODEL_ID = 9199;
  const int CLIMATOLOGY_MODEL_ID = 9099;
  const int CLIMOTOLOGY_MODEL_ID = CLIMATOLOGY_MODEL_ID;
  const int INTEGRATOR_MODEL_ID = 1;

  // FSL models
  
  const int MM5AVN_MODEL_ID = 2000;
  const int RAMSAVN_MODEL_ID = 2500;
  const int RAMSETA_MODEL_ID = 2600;
  const int WRFAVN_MODEL_ID = 3000;
  
  // Generally, the "00" number means "most recent" version of a model.
  // Other values (eg; 01, 02...) mean a model that many hours ago.
  
  const int MM5ETA_MODEL_ID = 2100;
  const int MM5ETA1_MODEL_ID = 2101;
  const int MM5ETA2_MODEL_ID = 2102;
  const int MM5ETA3_MODEL_ID = 2103;
  const int MM5ETA4_MODEL_ID = 2104;
  
  const int WRFETA_MODEL_ID = 3100;
  const int WRFETA1_MODEL_ID = 3101;
  const int WRFETA2_MODEL_ID = 3102;
  const int WRFETA3_MODEL_ID = 3103;
  const int WRFETA4_MODEL_ID = 3104;
  
  const int RUC_DMOS_MODEL_ID = 5000;

  // NCEP WRF Rapid Refresh
  const int RAP_DMOS_MODEL_ID = 6000;   // NCEP's name for Rapid Refresh
  
  // NCEP WRF High Resolution Rapid Refresh
  const int HRRR_DMOS_MODEL_ID = 6100;
  
  // RAL WRF model
  const int RALWRF_DMOS_MODEL_ID = 7000;
  
  // Models from other countries
  
  const int UKMET_DMOS_MODEL_ID = 500;
  const int GEM_DMOS_MODEL_ID = 600;
  const int CMC_GEAVG_DMOS_MODEL_ID = 660;  
  const int ECMWF_DMOS_MODEL_ID = 700;
  const int ECMWF_EPS_DMOS_MODEL_ID = 760;
  const int ECMWF_GEC_DMOS_MODEL_ID = 760; // Another name for Global Ensemble Control run
  const int ECMWF_EPSAVG_DMOS_MODEL_ID = 799;
  const int HIRLAM_DMOS_MODEL_ID = 800;
  const int ACCESS_DMOS_MODEL_ID = 900;

};


const char *get_model_name(int model_id);

//inline int make_model_reg_id(int model_id, int reg_id) {return 10000*model_id + reg_id;};
inline int make_model_reg_id(int model_id, int reg_id) {return reg_id;};


#endif
