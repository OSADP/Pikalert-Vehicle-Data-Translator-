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
 * Module: store_road_cond.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Description: 
 *              This file contains modules used in the storing of individual sites'
 *              data and the writing of the completely filled arrays to the output
 *              file. Some output is derived with certain assumptions as to available
 *              chemical stocks.
 *
 *      init_output_data()...Creates and initializes array space to hold the output of 
 *                           each site's forecasts.
 *      store_rc_fcst()......Stores the road condition forecast and treatment plan for
 *                           an individual site.
 *      write_output().......Writes the outpu arrays to the output file.
 *     
 */

/* System include files / Local include files */
#include <string.h>
#include <iostream>
#include <math.h>
#include <new>
#include <vector>

#include <nxncf/nxncf.hh>
#include <site_list/site_list.hh>
#include <log/log.hh>
#include "RulesOfPractice.hh"
extern Log *proc_log;

inline float celsius2fahrenheit(float temp_cel) {return (1.8*temp_cel + 32.);};
const float TONS_PER_LB = 1./2000.;
const int MAX_TMT_STR = 80;

static float *road_T;
static float *mobility;
static float *subsurface_T;
static float *subsurface_TempF;
static int *road_condition;
static float *road_TempF;
static float *sn_depth;
static float *sn_depth_inches;
static float *plow;
static float *apply_chem;
static float *treatment_time;
static float *chem_conc;
static float *availH2O;
static float *nom_chem_conc;
static float *avail_chem;
static float *application_rate;
static float *chem_type;
static float *chem_form;
static float *precip_type;
static float *phase_type;
static float *solution_type;
static float *tmt_explanation_index;
static char  *tmt_explanation;

//  
//  Routine: init_output_data()
//  
//  Description:
//      Create output data space and initialize the data values.
//  
//  Inputs: 
//    max_site_num..........The maximum number of sites as dimensioned in the output file.
//    num_output_fc_days....The number of forecast days dimensioned in the output file.
//    num_output_fc_per_day.The number of forecasts per day dimensioned in the output file.
//  
//  Outputs:
//    None.
//  
//  Returns:
//    Hard-wired to be zero for some reason(!?)
//  


int init_output_data(int max_site_num, int num_output_fc_days, int num_output_fc_per_day)
{
  int i;

  // create output data space (above structure copied into this space)
  road_T = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  subsurface_T = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  road_condition = new int[max_site_num*num_output_fc_days*num_output_fc_per_day];
  sn_depth = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  mobility = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  chem_conc = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  availH2O = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  nom_chem_conc = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  avail_chem = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  plow = new float [max_site_num*num_output_fc_days*num_output_fc_per_day];
  apply_chem = new float [max_site_num*num_output_fc_days*num_output_fc_per_day];
  treatment_time = new float [max_site_num*num_output_fc_days*num_output_fc_per_day];
  chem_type = new float [max_site_num*num_output_fc_days*num_output_fc_per_day];
  chem_form = new float [max_site_num*num_output_fc_days*num_output_fc_per_day];
  application_rate = new float [max_site_num*num_output_fc_days*num_output_fc_per_day];
  road_TempF = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  subsurface_TempF = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  sn_depth_inches = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  precip_type = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  phase_type = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  solution_type = new float [max_site_num*num_output_fc_days*num_output_fc_per_day];
  tmt_explanation_index = new float[max_site_num*num_output_fc_days*num_output_fc_per_day];
  tmt_explanation = new char[max_site_num*num_output_fc_days*num_output_fc_per_day*MAX_TMT_STR];

  // Initialize output data space
  for (i=0; i<max_site_num*num_output_fc_days*num_output_fc_per_day; i++)
    {
      road_T[i] = NC_FILL_FLOAT;
      road_TempF[i] = NC_FILL_FLOAT;
      subsurface_T[i] =  NC_FILL_FLOAT;
      subsurface_TempF[i] =  NC_FILL_FLOAT;
      sn_depth[i]  = NC_FILL_FLOAT;
      mobility[i] = NC_FILL_FLOAT;
      chem_conc[i] =  NC_FILL_FLOAT;
      availH2O[i] =  NC_FILL_FLOAT;
      nom_chem_conc[i] =  NC_FILL_FLOAT;
      avail_chem[i] =  NC_FILL_FLOAT;
      plow[i] = 0;
      apply_chem[i] = 0;
      treatment_time[i] = -1;
      chem_type[i] = 0;
      chem_form[i] = NC_FILL_FLOAT;
      application_rate[i] = NC_FILL_FLOAT;
      sn_depth_inches[i]  = NC_FILL_FLOAT;
      precip_type[i] = NC_FILL_FLOAT;
      phase_type[i] = NC_FILL_FLOAT;
      solution_type[i] = 0;
      tmt_explanation_index[i] = NC_FILL_FLOAT;
    }

  for (i=0; i<max_site_num*num_output_fc_days*num_output_fc_per_day*MAX_TMT_STR; i++)
    tmt_explanation[i] = NC_FILL_CHAR;

  return(0);
}

//  
//  Routine: free_output_data()
//  
//  Description:
//     Frees all the memory allocated for output
//  
void free_output_data()
{
  delete [] road_T;
  delete [] mobility;
  delete [] subsurface_T;
  delete [] subsurface_TempF;
  delete [] road_condition;
  delete [] road_TempF;
  delete [] sn_depth;
  delete [] sn_depth_inches;
  delete [] plow;
  delete [] apply_chem;
  delete [] treatment_time;
  delete [] chem_conc;
  delete [] availH2O;
  delete [] nom_chem_conc;
  delete [] avail_chem;
  delete [] application_rate;
  delete [] chem_type;
  delete [] chem_form;
  delete [] precip_type;
  delete [] phase_type;
  delete [] solution_type;
  delete [] tmt_explanation_index;
  delete [] tmt_explanation;
}

//  
//  Routine: store_rc_fcst()
//  
//  Description:
//      Takes the road condition forecast data and treatment plan for one site and copies
//      into the proper place in each of the output data arrays. Note that the output file
//      contains more days of data than the 48 hour forecast. The output file data starts
//      at 00Z of the forecast start day. So the offset where the first road condition
//      forecast output is written needs to be calculated.
//  
//  Inputs: 
//    ns......................The site index in the output file.
//    num_output_fc_days......The number of forecast days dimensioned in the output file.
//    num_output_fc_per_day...The number of forecasts per day dimensioned in the output file.
//    nt_start................The offset from 00Z of the start of several of the time series.
//    num_fcst................Number of hourly road condition forecasts.
//    bridge..................Indicates if this is a bridge site.
//    roadcond................An array containing this site's forecast road conditions.
//    last_chem_type..........The type of the last chemical applied to the segment.
//    TreatmentPlan...........A structure containing this site's treatment plan.
//    treatment_num...........The number of treatments in the treatment plan.
//    app_rate_units..........Units for the application rates
//    route_length............The length of the route segment associated with this site.
//    route_lanes.............The number of lanes on this route segment.
//  
//  Outputs:
//    None.
//  
//  Returns:
//    Hard-wired to be zero for some reason(!?)
//  

int store_rc_fcst(int ns, int num_output_fc_days, int num_output_fc_per_day, int nt_start,
		  int num_fcst, int bridge, RoadConditions *roadcond, 
		  float last_chem_type,
		  Treatment *TreatmentPlan, int treatment_num,
		  int app_rate_units,
		  float route_length, int route_lanes)
{
  int i, j;
  int fcst_per_site;
  int offset;

  fcst_per_site = num_output_fc_days * num_output_fc_per_day;
  offset = ns * fcst_per_site;

  // Get other road surface condition data
  for (i=0; i<num_fcst; i++)
    {
      road_T[offset + nt_start+i] = roadcond[i].road_T;
      if (road_T[offset + nt_start+i] != NC_FILL_FLOAT)
	road_TempF[offset + nt_start+i] = celsius2fahrenheit(road_T[offset + nt_start+i]);
      // If the site is a bridge site, there is no subsurface_T; want values to remain missing
      if (!bridge)
	{
	  subsurface_T[offset + nt_start+i] = roadcond[i].subsurface_T;
	  if (subsurface_T[offset + nt_start+i] != NC_FILL_FLOAT)
	    subsurface_TempF[offset + nt_start+i] = celsius2fahrenheit(subsurface_T[offset + nt_start+i]);
	}
      road_condition[offset + nt_start+i] = roadcond[i].road_condition;
      mobility[offset + nt_start+i] = roadcond[i].NetMobility;
      chem_conc[offset + nt_start+i] =  roadcond[i].FinalChemConc;
      availH2O[offset + nt_start+i] = roadcond[i].AvailH2O;
      nom_chem_conc[offset + nt_start+i] = roadcond[i].NominalChemConc;
      avail_chem[offset + nt_start+i] = roadcond[i].AvailableChem;
      sn_depth[offset + nt_start+i] = roadcond[i].PsnowOnRoad;
      if (sn_depth[offset + nt_start+i] != NC_FILL_FLOAT)
	sn_depth_inches[offset + nt_start+i] = sn_depth[offset + nt_start+i] / MM_PER_INCH;
      else
	sn_depth_inches[offset + nt_start+i] = NC_FILL_FLOAT;
      precip_type[offset + nt_start+i] = roadcond[i].roadPtype;
      phase_type[offset + nt_start+i] = roadcond[i].roadWphase;
      solution_type[offset + nt_start+i] = roadcond[i].ChemSolutionType;
    }

  // Now store components of treatment plan
  offset = ns * fcst_per_site;

  for (i=0; i<treatment_num; i++)
    {
      int hr = TreatmentPlan[i].StartTime;

      plow[offset + nt_start+hr] = TreatmentPlan[i].DoPlowing;
      apply_chem[offset + nt_start+hr] = TreatmentPlan[i].ApplyChemicals;
      //
      // Save treatment time and explanation only if applying chems or plowing
      //
      if (TreatmentPlan[i].ApplyChemicals || TreatmentPlan[i].DoPlowing)
	{
	  treatment_time[offset + nt_start+hr] = (float) TreatmentPlan[i].StartTime;
	  tmt_explanation_index[offset + nt_start+hr] = TreatmentPlan[i].ExplanationIndx;
	  strncpy(&tmt_explanation[(offset+nt_start+hr)*MAX_TMT_STR], TreatmentPlan[i].Explanation.c_str(), MAX_TMT_STR);
	}
      //
      // Transfer treatments or set to missing if none
      //
      if (TreatmentPlan[i].ApplyChemicals)
	{
	  //
	  // Convert from lb/lane-mile to output units
	  //
	  if (app_rate_units != C_CHEMUNITS_LBS_LANEMILE)
	    {
	      //
	      // If chem form is liquid, assume liters/lane-km will be unit, since
	      // this is currently the only other liquid (volume) supported.
	      //
	      if (TreatmentPlan[i].Chemicals.UserChemForm == C_CHEMFORM_LIQUID)
		application_rate[offset + nt_start+hr] =
		  ConvertChemUnits(TreatmentPlan[i].ApplicationRate,
				   TreatmentPlan[i].Chemicals.UserChemType,
				   C_CHEMUNITS_LBS_LANEMILE,
				   C_CHEMUNITS_LITERS_LANEKM);
	      else
		application_rate[offset + nt_start+hr] = 
		  ConvertChemUnits(TreatmentPlan[i].ApplicationRate,
				   TreatmentPlan[i].Chemicals.UserChemType,
				   C_CHEMUNITS_LBS_LANEMILE,
				   app_rate_units);
	    }
	  else
	    {
	      if (TreatmentPlan[i].Chemicals.UserChemForm == C_CHEMFORM_LIQUID ) 
		application_rate[offset + nt_start+hr] = 
		  ConvertChemUnits(TreatmentPlan[i].ApplicationRate,
				   TreatmentPlan[i].Chemicals.UserChemType,
				   C_CHEMUNITS_LBS_LANEMILE,
				   C_CHEMUNITS_GALS_LANEMILE);
	      else
		application_rate[offset + nt_start+hr] = TreatmentPlan[i].ApplicationRate;
	    }
      
	  chem_type[offset + nt_start+hr] = TreatmentPlan[i].Chemicals.UserChemType;
	  chem_form[offset + nt_start+hr] = TreatmentPlan[i].Chemicals.UserChemForm;

	}
      else
	{
	  application_rate[offset + nt_start+hr] = NC_FILL_FLOAT;
	  chem_type[offset + nt_start+hr] = 0;
	  chem_form[offset + nt_start+hr] = NC_FILL_FLOAT;
	}
    }

  return(0);
}



//  
//  Routine: write_output()
//  
//  Description:
//      Writes out the output data arrays into the output file.
//  
//  Inputs: 
//    nc_mod_fcst............The netcdf file object associated with the output netcdf file.
//  
//  Outputs:
//    None.
//  
//  Returns:
//    Hard-wired to be zero for some reason(!?)
//  

int write_output(NXNCF_fcst_output *nc_mod_fcst)
{
  int ret;

  // Write road condition and treatments to output file


  ret = nc_mod_fcst->set_var("application_rate");

  // num_vals only valid after variable is set
  int len = nc_mod_fcst->get_num_vals();

  NCF_finite(application_rate, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->write_var(application_rate);

  NCF_finite(apply_chem, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("apply_chem");
  ret = nc_mod_fcst->write_var(apply_chem);

  NCF_finite(availH2O, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("available_H2O");
  ret = nc_mod_fcst->write_var(availH2O);

  NCF_finite(avail_chem, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("available_chem");
  ret = nc_mod_fcst->write_var(avail_chem);

  NCF_finite(chem_form, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("chem_form");
  ret = nc_mod_fcst->write_var(chem_form);

  NCF_finite(chem_type, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("chem_type");
  ret = nc_mod_fcst->write_var(chem_type);

  NCF_finite(chem_conc, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("chemical_concentration");
  ret = nc_mod_fcst->write_var(chem_conc);

  NCF_finite(plow, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("do_plowing");
  ret = nc_mod_fcst->write_var(plow);

  NCF_finite(mobility, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("mobility");
  ret = nc_mod_fcst->write_var(mobility);

  NCF_finite(nom_chem_conc, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("nominal_chem");
  ret = nc_mod_fcst->write_var(nom_chem_conc);

  NCF_finite(phase_type, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("phase_type");
  ret = nc_mod_fcst->write_var(phase_type);

  NCF_finite(precip_type, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("precip_type");
  ret = nc_mod_fcst->write_var(precip_type);

  NCF_finite(road_T, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("road_T");
  ret = nc_mod_fcst->write_var(road_T);

  NCF_finite(road_TempF, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("road_TempF");
  ret = nc_mod_fcst->write_var(road_TempF);

  NCF_finite(subsurface_T, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("subsurface_T");
  ret = nc_mod_fcst->write_var(subsurface_T);

  NCF_finite(subsurface_TempF, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("subsurface_TempF");
  ret = nc_mod_fcst->write_var(subsurface_TempF);

  NCF_finite(sn_depth, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("snow_depth");
  ret = nc_mod_fcst->write_var(sn_depth);

  NCF_finite(sn_depth_inches, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("snow_depth_inches");
  ret = nc_mod_fcst->write_var(sn_depth_inches);

  NCF_finite(solution_type, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("solution_type");
  ret = nc_mod_fcst->write_var(solution_type);

  NCF_finite(treatment_time, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("treatment_time");
  ret = nc_mod_fcst->write_var(treatment_time);

  NCF_finite(tmt_explanation_index, len, NC_FILL_FLOAT);
  ret = nc_mod_fcst->set_var("treatment_explanation_index");
  if (ret != 0)
    proc_log->write_time("Warning: Could not set ptr to output variable 'treatment_explanation_index'.\n");
  else
    ret = nc_mod_fcst->write_var(tmt_explanation_index);

  ret = nc_mod_fcst->set_var("treatment_explanation");
  ret = nc_mod_fcst->write_var(tmt_explanation);

  return(0);
}



