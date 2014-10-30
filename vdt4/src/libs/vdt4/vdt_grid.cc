//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: vdt_grid.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/04/10 17:43:27 $
//
//==============================================================================

/**
 * @file vdt_grid.cc
 *
 */

// Include files 
#include <math.h>
#include "vdt_grid.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions


int vdt_grid::round_to_integer_coords(double ycoord, double xcoord, int *iycoord, int *ixcoord) const
{
  // Ensure ycoord, xcoord is on grid
  if (ycoord < 0)
    *iycoord = 0;
  else if (ycoord > (latitude_size - 1))
    {
      if (ycoord < latitude_size)
	{
	  *iycoord = latitude_size - 1;
	}
      else
	{
	  return -1;
	}
    }
  else
    *iycoord = static_cast<int>(rint(ycoord));

  if (xcoord < 0)
    *ixcoord = 0;
  else if (xcoord > (longitude_size - 1))
    {
      if (xcoord < longitude_size)
	{
	  *ixcoord = longitude_size - 1;
	}
      else
	{
	  return -1;
	}
    }
  else
    *ixcoord = static_cast<int>(rint(xcoord));

  return 0;
}

void vdt_grid::latlon2xy(double lat, double lon, double *xoffset, double *yoffset) const
{
  double xoff = (longitude_size - 1) * (lon - lower_left_longitude) / (upper_right_longitude - lower_left_longitude);
  double yoff = (latitude_size - 1) * (lat - lower_left_latitude) / (upper_right_latitude - lower_left_latitude);
  *xoffset = xoff;
  *yoffset = yoff;
}

void vdt_grid::xy2latlon(double xoffset, double yoffset, double *lat, double *lon) const
{
  *lon = lower_left_longitude + xoffset * (upper_right_longitude - lower_left_longitude) / (longitude_size - 1);
  *lat = lower_left_latitude + yoffset * (upper_right_latitude - lower_left_latitude) / (latitude_size - 1);
}

int vdt_grid::get_nearest_indices(const vector<double> &latitude, const vector<double> &longitude, vector<int> &grid_cell_offsets, vector<double> &distances) const
{
  if (latitude.size() != longitude.size())
    {
      return -1;
    }

  int iycoord;
  int ixcoord;

  double xcoord;
  double ycoord;
  double lat;
  double lon;
  for (size_t i=0; i<latitude.size(); i++)
    {
      latlon2xy(latitude[i], longitude[i], &xcoord, &ycoord);
      int ret = round_to_integer_coords(ycoord, xcoord, &iycoord, &ixcoord);
      if (ret == 0)
	{
	  int offset = get_grid_offset(iycoord, ixcoord);
	  grid_cell_offsets.push_back(offset);
	  xy2latlon(ixcoord, iycoord, &lat, &lon);
	  double delta_lat = latitude[i] - lat;
	  double delta_lon = longitude[i] - lon;
	  double distance = delta_lat * delta_lat + delta_lon * delta_lon;
	  distances.push_back(distance);
	}
      else
	{
	  printf("bad return got coords xcoord ycoord %lf %lf\n", xcoord, ycoord);

	  grid_cell_offsets.push_back(-1);
	}
    }

  return 0;
}
