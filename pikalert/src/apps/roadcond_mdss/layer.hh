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
 * Module: layer.hh
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Description: This file contains definitions of parameters used in
 *              the creation of SNTHERM configuration files. All these
 *              have the "LYR_" at the start of the name.
 *
 */

// Info on soil depth extracted from the models.
const int NUM_MODEL_DEPTH = 5;
const float soil_temp_depth[NUM_MODEL_DEPTH] = {0.05, 0.25, 0.70, 1.5, 3.};



// Constants describing parameters used in SNTHERM config files
const int LYR_NO = 0;
const int LYR_YES = 1;
const int LYR_MISSING = 999;

const int LYR_NUM_LAYER = 5;
const int LYR_PRINT_INTERVAL = 1;
const int LYR_AVG_PRES = 990;
const int LYR_IR_EXTINCTION = 400;
const int LYR_TIME_INTERVAL = 3600;
const float LYR_SNOW_ALBEDO = 1.78;
const float LYR_IRREDUCIBLE_WATER_SAT = 0.04;
const int LYR_SNOW_DENSITY_LIMIT = 100;
const int LYR_SET_MAX_RICH_NUM = 2;
const char LYR_VISCOSITY_COEFF[6] = "0.9d6";

const int LYR_SNOW_MAT_CODE = 1;
const int LYR_SOIL_MAT_CODE = 92;
const int LYR_GRAVEL_MAT_CODE = 91;
const int LYR_ASPHALT_MAT_CODE = 90;

const int LYR_MAX_NODES_PER_LAYER = 10;
const int LYR_MAX_NODES = LYR_MAX_NODES_PER_LAYER * LYR_NUM_LAYER;

const int LYR_NUM_GOOD_CALC = 2;
const float LYR_MIN_TIME_STEP = 5.0;
const float LYR_WATER_MIN_TIME_STEP = 1.0;
const float LYR_MAX_TIME_STEP = 900.0;
const float LYR_WATER_MAX_TIME_STEP = 900.0;
const float LYR_MAX_SATURATION_CHANGE = 0.01;
const float LYR_MAX_TEMP_CHANGE = 0.05;

const float LYR_ELEV_SLOPE = 0.0;
const float LYR_AZ = 0.0;

const float LYR_GRAIN_SIZE = 0.0;
