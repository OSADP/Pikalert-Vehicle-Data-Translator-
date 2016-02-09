// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:18:48 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: post_proc - main.cc
 *
 * Author: Bill Myers/Jim Cowie
 *
 * Date:   5/25/99
 *         Modified 10/20/06 to get max_nbr from ncfile
 *         Modified 05/28/09 to add altitude difference to output
 *
 * Description: Creates a netcdf file containing the DICAST ID of the 
 *              closest neighbors in distance order. (Includes self.)
 *              The number of neighbors is fitted to the output file
 *              using the "max_nbr" dimension. The altitude difference
 *              is also added if the file has the proper dimensionality,
 *              ie, the last dimension is of size 3.
 */

/* System include files / Local include files */
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "nxncf/nxncf.hh"
#include "site_list/site_list.hh"
#include "netcdf.h"


/* Constant definitions / Macro definitions / Type definitions */

#define SEC_PER_HOUR (60*60)
#define SEC_PER_QUARTILE (SEC_PER_HOUR*6)
#define SEC_PER_DAY (SEC_PER_HOUR*24)
#define EARTH_RADIUS (6.37e+3)

extern float init_grid(float start_lon, float start_lat, float end_lon, float end_lat);


void usage(char *prog_string)
{
  fprintf(stderr, "usage: %s potential_nbr_list site_list cdl_name output_file\n",
  	prog_string);
  exit(2);
}

main(int argc, char **argv)
{
  char *cdl_name = NULL;
  char *core_site_file_name = NULL;
  char *big_site_file_name = NULL;
  char *output_name = NULL;
  int  num_core_sites = 0;
  long num_output_sites = 0;
  long max_sites = 0;
  int  *out_var_array_i;
  int  *output_site_array = 0;
  int  *core_site_array = 0;
  float *output_ll_array;
  float *core_ll_array;
  float *core_alt_array = 0;
  float *output_alt_array = 0;
  int  i, j, k;
  int  ret = 0;
  int debug_level;


  if (argc != 5)
    {
      usage(argv[0]);
    }

  core_site_file_name = argv[1];
  big_site_file_name = argv[2];
  cdl_name = argv[3];
  output_name = argv[4];


  /* generate netcdf file from cdl_file */
  ret = NCF_cdl2nc(cdl_name, output_name);
  if (ret != 0)
    {
      printf("Error: Unable to generate output file %s\n",output_name);
      printf("Ending: exit status = 1.\n");
      exit(1);
    }

  /* open netcdf file for writing */
  NcFile output_file(output_name, NcFile::Write);
  if (!output_file.is_valid())
    {
      printf("Error: Unable to open output file %s\n",output_name);
      printf("Ending: exit status = 1.\n");
      exit(1);
    }


  /* create Site_list obj */
  Site_list core_sites(core_site_file_name);
  if (core_sites.error())
    {
      printf("Error: Unable to make core_sites object\n");
      printf(core_sites.error());
      printf("Ending: exit status = 1.\n");
      return(1);
    }

  num_core_sites = core_sites.make_id_array(&core_site_array);
  if (num_core_sites < 0)
    {
      printf("Error: could not make_id_array\n");
      printf(core_sites.error());
      printf("Ending: exit status = 1.\n");
      return(1);
    }

  ret = core_sites.make_latlon_array(&core_ll_array);   
  ret = core_sites.make_alt_array(&core_alt_array);   

  printf("Number of neighbor sites: %d\n", ret);



  /* create Site_list obj */
  Site_list output_sites(big_site_file_name);
  if (output_sites.error())
    {
      printf("Error: Unable to make output_sites object\n");
      printf(output_sites.error());
      printf("Ending: exit status = 1.\n");
      return(1);
    }

  num_output_sites = output_sites.make_id_array(&output_site_array);
  if (num_output_sites < 0)
    {
      printf("Error: could not make_id_array\n");
      printf(output_sites.error());
      printf("Ending: exit status = 1.\n");
      return(1);
    }

  ret = output_sites.make_latlon_array(&output_ll_array);   
  ret = output_sites.make_alt_array(&output_alt_array);   
  printf("Number of output sites: %d\n", num_output_sites);


  /* Handle unlimited max_site dimension */
  NcDim *dptr = output_file.get_dim(MAX_SITE_NUM_NAME);
  if (dptr == 0)
    {
      printf("Error: Unable to get max_site_num dimension from output file.\n");
      printf("Ending: exit status = 1.\n");
      exit(1);
    }

  if (dptr->is_unlimited())
    max_sites = num_output_sites;
  else
    max_sites = dptr->size();

  printf("Max number of output sites: %d\n", max_sites);

  if (num_output_sites > max_sites)
    {
      printf("Error: num_sites (%d) > max_sites allowed (%d).\n", num_output_sites, max_sites);
      printf("Ending: exit status = 1.\n");
      exit(1);
    }


  /* Get max_nbr size  */
  dptr = output_file.get_dim("max_nbr");
  if (dptr == 0)
    {
      printf("Error: Unable to get max_nbr dimension from output file.\n");
      printf("Ending: exit status = 1.\n");
      exit(1);
    }

  int max_nbr = dptr->size();
  int nbr[max_nbr];
  float nbr_dist[max_nbr];
  float nbr_alt[max_nbr];
  printf("Info: Number of neighbors to find: %d\n", max_nbr);


  /* Get param_vals size */
  dptr = output_file.get_dim(PARAM_VALS_DIM_NAME);
  if (dptr == 0)
    {
      printf("Error: Unable to get %s dimension from output file.\n", PARAM_VALS_DIM_NAME);
      printf("Ending: exit status = 1.\n");
      exit(1);
    }

  int param_vals = dptr->size();

  double *out_data = new double[num_output_sites*max_nbr*param_vals];
  for (i=0; i<num_output_sites*max_nbr*param_vals; i++)
    out_data[i] = NC_FILL_DOUBLE;


  float max_d = -999999.;
  for (j=0; j<num_output_sites; j++)
    {
      for (i=0; i<max_nbr; i++)
	{
	  nbr[i] = -999;
	  nbr_dist[i] = 9999999999.;
	  nbr_alt[i] = -9999;
	}

      for (i=0; i<num_core_sites; i++)
	{
	  float dist, alt;
	  dist = init_grid(core_ll_array[2*i+1], core_ll_array[2*i], 
			 output_ll_array[2*j+1], output_ll_array[2*j]);

	  if (core_alt_array[i] != -9999 && output_alt_array[j] != -9999)
	    alt = core_alt_array[i] - output_alt_array[j];
	  else
	    alt = -9999;

	  k=0;
	  int found = 0;
	  int nbr_ind = max_nbr;
	  while (k<max_nbr && !found)
	    {
	      if (dist < nbr_dist[k])
		{
		  nbr_ind = k;
		  found = 1;
		}
	      else
		k++;
	    }

	  for (k=max_nbr-1; k>nbr_ind; k--)
	    {
	      nbr[k] = nbr[k-1];
	      nbr_dist[k] = nbr_dist[k-1];
	      nbr_alt[k] = nbr_alt[k-1];
	    }

	  if (nbr_ind < max_nbr)
	    {
	      nbr[nbr_ind] = core_site_array[i];
	      if (dist < 0.001) {
		nbr_dist[nbr_ind] = 0.;
		nbr_alt[nbr_ind] = 0.;
	      }
	      else {
		nbr_dist[nbr_ind] = dist;
		nbr_alt[nbr_ind] = alt;
	      }
	    }

	}

      printf("output site %d  %05d  %.2f %.2f %.0f\n", j,output_site_array[j], 
	     output_ll_array[2*j], output_ll_array[2*j+1], output_alt_array[j]);
      for (k=0; k<max_nbr; k++)
	{
	    printf("%d %08d %.2f %.0f\n", k, (int)nbr[k], nbr_dist[k],
		   nbr_alt[k]);

	  out_data[j*max_nbr*param_vals+k*param_vals] = (double)nbr[k];
	  out_data[j*max_nbr*param_vals+k*param_vals+1] = nbr_dist[k];
	  if (param_vals > 2)
	    out_data[j*max_nbr*param_vals+k*param_vals+2] = nbr_alt[k];

	  if ( out_data[j*max_nbr*param_vals+k*param_vals] != nbr[k])
	    {
	      printf("vals do not match\n");
	      exit(0);
	    }
	}
    }

  /* find core site_density */
  float site_density;
  float max_lat = -9999.;
  float min_lat = 9999.;
  for (i=0; i<num_core_sites; i++)
    {
      if (core_ll_array[2*i] > max_lat)
	max_lat = core_ll_array[2*i];
      if (core_ll_array[2*i] < min_lat)
	min_lat = core_ll_array[2*i];
    }
  printf("max %f  min %f\n", max_lat, min_lat);
  site_density = 2 * M_PI * EARTH_RADIUS * EARTH_RADIUS *
    (sin(max_lat*M_PI/180.) - sin(min_lat*M_PI/180.)) /
    (float)num_core_sites;
  printf("site_density %f\n", site_density);

  // Write out num_sites
  NcVar *out_var_ptr = output_file.get_var(NUM_SITES_NAME);
  if (!out_var_ptr)
    {
      printf("Error: Unable to get ptr for var %s\n",NUM_SITES_NAME);
      printf("Ending: exit status = 1.\n");
      return(1);
    }
  
  ret = out_var_ptr->put(&num_output_sites, 1);
  if (ret == 0)
    {
      printf("Error: Unable to put data for var %s\n",NUM_SITES_NAME);
      printf("Ending: exit status = 1.\n");
      return(1);
    }
  else
      printf("Info: Number of output sites is %d\n",num_output_sites);


  // Write out Site List
  out_var_ptr = output_file.get_var(SITE_LIST_NAME);
  if (!out_var_ptr)
    {
      printf("Error: Unable to get ptr for var %s\n",SITE_LIST_NAME);
      printf("Ending: exit status = 1.\n");
      return(1);
    }

  ret = out_var_ptr->put(output_site_array, num_output_sites);
  if (ret == 0)
    {
      printf("Error: Unable to put data for var %s\n",SITE_LIST_NAME);
      printf("Ending: exit status = 1.\n");
      return(1);
    }
  else
      printf("Info: Successfully wrote %s\n", SITE_LIST_NAME);

  // Write out Site Density
  out_var_ptr = output_file.get_var("site_density");
  if (!out_var_ptr)
    {
      printf("Error: Unable to get ptr for var %s\n","site_density");
      printf("Ending: exit status = 1.\n");
      return(1);
    }

  ret = out_var_ptr->put(&site_density, 1);
  if (ret == 0)
    {
      printf("Error: Unable to put data for var %s\n","site_density");
      printf("Ending: exit status = 1.\n");
      return(1);
    }
  else
      printf("Info: Successfully wrote %s\n", "site_density");

  // Write out nbr List
  out_var_ptr = output_file.get_var("nbr_list");
  if (!out_var_ptr)
    {
      printf("Error: Unable to get ptr for var %s\n","nbr_list");
      printf("Ending: exit status = 1.\n");
      return(1);
    }

  long *edges; 
  edges = out_var_ptr->edges();
  edges[0] = num_output_sites;

  double *p;
  p = out_data;
  ret = out_var_ptr->put(p, edges);
  if (ret == 0)
    {
      printf("Error: Unable to put data for var %s\n", "nbr_list");
      printf("Ending: exit status = 1.\n");
      return(1);
    }


  if (param_vals == 2)
    printf("\n*** NOTE: Set %s to 3 in the CDL to get elevation difference at neighbors. ***\n\n", PARAM_VALS_DIM_NAME);



  delete [] edges;
  delete [] output_site_array;
  delete [] output_ll_array;
  delete [] output_alt_array;
  delete [] core_site_array;
  delete [] core_ll_array;
  delete [] core_alt_array;
  delete [] out_data;

}
