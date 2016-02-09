// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:22:27 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <math.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <nxncf/nxncf.hh>
#include <log/log.hh>
#include "obs_history.h"

extern Log *logfile;

int create_obs_history(int num_out_times, char *output_file_name, char *cdl_file_name, ds *data)
{
  int i, ret;
  char var_name[80];
  long *dim = new long[2];


  // Create output netcdf file
  ret = NCF_cdl2nc(cdl_file_name, output_file_name);
  if (ret != 0)
  {
      logfile->write_time("Error: Unable to generate output file %s\n", output_file_name);
      return (-1);
  }

  /* open netcdf file for writing */
  NcFile output_file(output_file_name, NcFile::Write);
  if (!output_file.is_valid())
  {
      logfile->write_time("Error: Unable to open output file %s\n",output_file_name);
      return (-1);
  }

  // Write creation time
  ret = NXNCF_write_creation_time(output_file);
  if (ret < 0)
  {
      logfile->write_time("Error: Could not put creation time\n");
      return (-1);
  }

  // Write obs_time
  strcpy(var_name, "obs_time");
  if ((NCF_put_var(&output_file, var_name, &data->obs_time, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }

  // Write num_sites
  strcpy(var_name, "num_sites");
  if ((NCF_put_var(&output_file, var_name, &data->num_sites, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }

  // Write station id's
  dim[0] = data->num_sites;
  dim[1] = 0;
  strcpy(var_name, "site_list");
  if ((NCF_put_var(&output_file, var_name, data->dicast_id, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }

  /****************************************************/
  // Store the met data - hourly data first
  /****************************************************/
  
  // T
  strcpy(var_name, "T");
  dim[1] = num_out_times;
  if ((NCF_put_var(&output_file, var_name, data->T, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }
  
  // dewpt
  strcpy(var_name, "dewpt");
  if ((NCF_put_var(&output_file, var_name, data->dewpt, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }

  // wind_speed
  strcpy(var_name, "wind_speed");
  if ((NCF_put_var(&output_file, var_name, data->wind_speed, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }

  // Precip
  strcpy(var_name, "Precip");
  if ((NCF_put_var(&output_file, var_name, data->Precip, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }

  // road_T
  strcpy(var_name, "road_T");
  if ((NCF_put_var(&output_file, var_name, data->road_T, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }
  
  // bridge_T
  strcpy(var_name, "bridge_T");
  if ((NCF_put_var(&output_file, var_name, data->bridge_T, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }
  
  // road_condition
  strcpy(var_name, "road_condition");
  if ((NCF_put_var(&output_file, var_name, data->road_condition, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }
  
  // subsurface_T
  strcpy(var_name, "subsurface_T");
  if ((NCF_put_var(&output_file, var_name, data->subsurface_T, dim)) < 0)
  {
      logfile->write_time("Error: Unable to write data for variable %s\n", var_name);
      return (-1);
  }
  
  
  logfile->write_time(1, "Info: Wrote %d sites to %s\n", data->num_sites, output_file_name);

  return (0);
}
