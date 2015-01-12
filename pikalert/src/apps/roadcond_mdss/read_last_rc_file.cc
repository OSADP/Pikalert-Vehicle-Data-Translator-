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
 * Module: read_last_rc_file.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Description: 
 *              This file contains several routines that are used to initialize the 
 *              initial state of the road. If available, forecasts are available from 
 *              a previous road_cond run are read in. If no info is available, a soil 
 *              temperature profile is used.
 *              
 *     read_last_rc_file()......Reads road conditions from a previous run of this program.
 *     init_site_subsurface()...Initializes subsurface temperatures data structure from either 
 *                              a previous road_cond run or a soil temperature profile.
 *     init_road_cond().........Initializes the road condition structre with missing data.
 *
 */

/* System include files / Local include files */
#include <iostream>
#include <math.h>
#include <new>
#include <vector>

#include "nxncf/nxncf.hh"
#include "site_list/site_list.hh"
#include "log/log.hh"
#include "RulesOfPractice.hh"
#include "layer_info.hh"

extern float calc_layer_depth(int layer, int node, LayerInfo *site_layer);
extern float calc_layer_temp(float *soil_temp, float depth);

extern Log *proc_log;

inline float kelvin2celsius(float temp_kel) {return (temp_kel - 273.15);};

//  
//  Routine: read_last_rc_file()
//  
//  Description:
//      This module reads the output from an earlier run of this program. It 
//  extracts the forecast start of the earlier run and calculates an offset 
//  into that file based on the forecast start time of this run. The subsurface
//  temperatures valid at the current forecast start time are then extracted
//  and stored as initial conditions for this run.
//  
//  Inputs: 
//    last_rc_file.......The name of the output file from a previous run of this program.
//    fcst_time..........The unix time of the start of this run's forecast.
//    num_fcst...........The number of time steps in the forecast period.
//    output_site_array..An array containing the id's of the output forecast site list.
//    num_output_sites...The number of sites in the output forecast site list.
//    site_layer.........An array containing structures describing the subsurface structure 
//                       at each site.
//
//  
//  Outputs:
//    num_node...........The number of nodes stored in the previous file. This is the
//                       node dimension of the data stored, not the number of nodes at
//                       any particular site. The number of nodes at any site is assumed
//                       to be unchanged from run to run.
//    last_subsurf_T.....An array containing the nodal subsurface temperatures (valid at 
//                       each forecast time) extracted from the file.
//    last_pavement_T....An array containing the road surface temperatures (valid at each 
//                       forecast time) extracted from the file.
//    last_chem_conc.....An array containing the chemical concentrations (valid at 
//                       each forecast time) extracted from the file.
//    last_nom_chem_conc.An array containing the theoretical chemical concentrations (valid at 
//                       each forecast time) extracted from the file.
//    last_availH2O......An array containing the water available to dilute chemicals (valid at 
//                       each forecast time) extracted from the file.
//    last_avail_chem....An array containing the pure de-icing chemicals on the road (valid at 
//                       each forecast time) extracted from the file.
//    last_snow_depth....An array containing the snow depths on the road (valid at 
//                       each forecast time) extracted from the file.
//    last_chem_type.....An array containing the dominant type of chemical on the road (valid at 
//                       each forecast time) extracted from the file.
//  
//  Returns:
//    0..................If non-fatal processing occurred.
//    1..................If  a fatal processing condition was encountered.
//

int read_last_rc_file(char *last_rc_file, double fcst_time, int num_fcst,
		      int *output_site_array, long num_output_sites, LayerInfo *site_layer,
		      float **last_chem_conc, float **last_nom_chem_conc, 
		      float **last_availH2O, float **last_avail_chem,
		      float **last_snow_depth, float **last_chem_type)
{
  NcFile *nc_last_rc_file; 
  NcVar *ptr;
  NcDim *nc_dim;
  long *dim_array;
  long *last_rc_site_index_array = 0;
  double last_fcst_time;
  int num_days;
  int num_fc_times_per_day;
  int num_vals;
  int num_dims;
  int max_vals;
  int ret;
  int i, j, index;
  float *last_data;
  int hour_offset;
  int vals_per_site;

  nc_last_rc_file = new NcFile(last_rc_file, NcFile::ReadOnly);
  ptr = nc_last_rc_file->get_var("subsurface_T");
  if (ptr == NULL)
    {
      proc_log->write_time("Info: Unable to open last road cond file %s\n", last_rc_file);
      delete nc_last_rc_file;
      return(0);
    }

  /* create index array of "previous" road subsurface temp sites */
  ret = NXNCF_create_index_array(output_site_array, num_output_sites, nc_last_rc_file,
				 &last_rc_site_index_array);
  if (ret < 0) {
    proc_log->write_time("Error: failure in call to last_rc's create_index_array.\n");
    return(1);
  }

  /* Get number of forecast days */
  nc_dim = nc_last_rc_file->get_dim("days");
  num_days  = nc_dim->size();
  if (num_days < 0) {
    proc_log->write_time("Error: Unable to get dimension %s\n", "num_days");
    return(1);
  }

  /* Get number of forecast days */
  nc_dim = nc_last_rc_file->get_dim("fc_times_per_day");
  num_fc_times_per_day  = nc_dim->size();
  if (num_fc_times_per_day < 0) {
    proc_log->write_time("Error: Unable to get dimension %s\n", "fc_times_per_day");
    return(1);
  }

  ptr = nc_last_rc_file->get_var("forc_time");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s\n", 
			   "forc_time");
      return(1);
    }

  ret = ptr->get(&last_fcst_time, 1);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s\n", 
			   "forc_time");
      return(1);
    } 
  hour_offset = (int)(fcst_time- last_fcst_time) / SEC_PER_HOUR;

  // Now get chemical concentration at start time
  ptr = nc_last_rc_file->get_var("chemical_concentration");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s\n", 
			   "chemical_concentration");
      return(1);
    }

  num_dims = ptr->num_dims();
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  vals_per_site = num_vals / dim_array[0];

  last_data = new float[num_vals];
  max_vals = num_vals;

  ret = ptr->get(last_data, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s\n", 
			   "chemical_concentration");
      return(1);
    } 

  *last_chem_conc = new float[num_output_sites];
  for (i=0; i<num_output_sites; i++)
    {
      index = last_rc_site_index_array[i];
      if (index < 0)
	(*last_chem_conc)[i] = 0.;
      else
	(*last_chem_conc)[i] = last_data[index*vals_per_site+ hour_offset];
    }

  // Now get nominal chemical concentration at start time
  ptr = nc_last_rc_file->get_var("nominal_chem");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s\n", 
			   "nominal_chem");
      return(1);
    }

  delete [] dim_array;
  num_dims = ptr->num_dims();
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  vals_per_site = num_vals / dim_array[0];

  if (num_vals > max_vals)
    {
      delete [] last_data;
      last_data = new float[num_vals];
      max_vals = num_vals;
    }

  ret = ptr->get(last_data, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s\n", 
			   "nominal_chem");
      return(1);
    } 

  *last_nom_chem_conc = new float[num_output_sites];
  for (i=0; i<num_output_sites; i++)
    {
      index = last_rc_site_index_array[i];
      if (index < 0)
	(*last_nom_chem_conc)[i] = 0.;
      else
	(*last_nom_chem_conc)[i] = last_data[index*vals_per_site+ hour_offset];

    }

  // Now get available H2O at start time
  ptr = nc_last_rc_file->get_var("available_H2O");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s\n", 
			   "available_H2O");
      return(1);
    }

  delete [] dim_array;
  num_dims = ptr->num_dims();
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  vals_per_site = num_vals / dim_array[0];

  if (num_vals > max_vals)
    {
      delete [] last_data;
      last_data = new float[num_vals];
      max_vals = num_vals;
    }

  ret = ptr->get(last_data, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s\n", 
			   "available_H2O");
      return(1);
    } 

  *last_availH2O = new float[num_output_sites];
  for (i=0; i<num_output_sites; i++)
    {
      index = last_rc_site_index_array[i];
      if (index < 0)
	(*last_availH2O)[i] = 0.;
      else
	(*last_availH2O)[i] = last_data[index*vals_per_site+ hour_offset];
    }

  // Now get available chemical at start time
  ptr = nc_last_rc_file->get_var("available_chem");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s\n", 
			   "available_chem");
      return(1);
    }

  delete [] dim_array;
  num_dims = ptr->num_dims();
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  vals_per_site = num_vals / dim_array[0];

  if (num_vals > max_vals)
    {
      delete [] last_data;
      last_data = new float[num_vals];
      max_vals = num_vals;
    }

  ret = ptr->get(last_data, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s\n", 
			   "chemical_concentration");
      return(1);
    } 

  *last_avail_chem = new float[num_output_sites];
  for (i=0; i<num_output_sites; i++)
    {
      index = last_rc_site_index_array[i];
      if (index < 0)
	(*last_avail_chem)[i] = 0.;
      else
	(*last_avail_chem)[i] = last_data[index*vals_per_site+ hour_offset];
    }

  // Now get snow depth at start time
  ptr = nc_last_rc_file->get_var("snow_depth");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s\n", 
			   "snow_depth");
      return(1);
    }

  delete [] dim_array;
  num_dims = ptr->num_dims();
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  vals_per_site = num_vals / dim_array[0];

  if (num_vals > max_vals)
    {
      delete [] last_data;
      last_data = new float[num_vals];
      max_vals = num_vals;
    }

  ret = ptr->get(last_data, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s\n", 
			   "snow_depth");
      return(1);
    } 

  *last_snow_depth = new float[num_output_sites];
  for (i=0; i<num_output_sites; i++)
    {
      index = last_rc_site_index_array[i];
      if (index < 0)
	(*last_snow_depth)[i] = 0.;
      else
	(*last_snow_depth)[i] = last_data[index*vals_per_site+ hour_offset];
    }

  // Now get solution type at start time from solution_type or chem_type
  ptr = nc_last_rc_file->get_var("solution_type");
  if (ptr == NULL)
    {
      proc_log->write_time("Warning: Unable to get ptr to %s, using chem_type instead\n", 
			   "solution_type");
      ptr = nc_last_rc_file->get_var("chem_type");
    }

  delete [] dim_array;
  num_dims = ptr->num_dims();
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  vals_per_site = num_vals / dim_array[0];
  if (num_vals > max_vals)
    {
      delete [] last_data;
      last_data = new float[num_vals];
      max_vals = num_vals;
    }

  ret = ptr->get(last_data, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get data for %s\n", 
			   "ptr->name()");
      return(1);
    } 

  *last_chem_type = new float[num_output_sites];
  for (i=0; i<num_output_sites; i++)
    {
      index = last_rc_site_index_array[i];
      if (index < 0)
	(*last_chem_type)[i] = 1;
      else
	(*last_chem_type)[i] = last_data[index*vals_per_site+ hour_offset];

    }	     


  delete [] dim_array;
  delete [] last_data;
  delete [] last_rc_site_index_array;

  delete nc_last_rc_file;

  return(0);
}


//
//  Routine: init_road_cond()
//
//  Description:
//      This module initializes the values in the road condition structure.
//
//  Inputs:
//    num_fcst...........The number of times in the forecast period.
//
//
//  Outputs:
//    roadcond...........An array of initialized forecast road conditions.
//
//  Returns:
//    None.
//

void init_road_cond(int num_fcst, RoadConditions *roadcond)
{
  int i, j;

  for (i=0; i<num_fcst+1; i++)
    {
      roadcond[i].road_T = NC_FILL_FLOAT;
      roadcond[i].road_condition = 1;
      roadcond[i].subsurface_T = NC_FILL_FLOAT;
      roadcond[i].Tfreezepoint = NC_FILL_FLOAT;
      roadcond[i].PliquidOnRoad = NC_FILL_FLOAT;
      roadcond[i].PsnowOnRoad = 0;
      roadcond[i].AvailH2O = 0.;
      roadcond[i].NominalChemConc = 0.;
      roadcond[i].AvailableChem = 0.;
      roadcond[i].FinalChemConc = 0.;
      roadcond[i].Friction = NC_FILL_FLOAT;
      roadcond[i].NetMobility = NC_FILL_FLOAT;
			// 
			// RGH - Initialize these to baseline values in case 
			// NCAR: Should these simply be NC_FILL_INT values?? or 0?
			// 
      roadcond[i].ChemSolutionType = 0;
      roadcond[i].roadWphase       = C_RTYPE_DRY;

      roadcond[i].roadPtype        = C_PTYPE_NONE;
      roadcond[i].TrafficIntensity = -1;

    }

  return;
}
