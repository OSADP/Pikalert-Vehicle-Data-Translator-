// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_nssl_dataset.hh"
#include "math.h"
#include <log/log.hh>
#include "vdt_const.hh"
#include "vdt_util.hh"

//#define SPOT_TESTS 1

//Values from MRMS upgrade doc March 2015: https://docs.google.com/document/d/1LeVcn_taIXZgzZb5JgWqaVr0xVs7GmA6RpHcb8ZGiwk/edit
//Grid spacing in degress LAT/LON
const float vdt_nssl_radar_dataset2::NSSL_RADAR_LATITUDE_GRID_SPACING = 0.01f; 
const float vdt_nssl_radar_dataset2::NSSL_RADAR_LONGITUDE_GRID_SPACING = 0.01f;
const float vdt_nssl_radar_dataset2::NSSL_RADAR_NW_LATITUDE = 55.0f;
const float vdt_nssl_radar_dataset2::NSSL_RADAR_NW_LONGITUDE = -130.0f;
const char* vdt_nssl_radar_dataset2::NSSL_RADAR_NC_DIM_NAME_LATITUDE = "latitude";
const char* vdt_nssl_radar_dataset2::NSSL_RADAR_NC_DIM_NAME_LONGITUDE = "longitude";
const char* vdt_nssl_radar_dataset2::NSSL_RADAR_NC_VAR_NAME_BASE_REFLECTIVITY = "MergedBaseReflectivityQC";
const char* vdt_nssl_radar_dataset2::NSSL_RADAR_NC_VAR_NAME_COMPOSITE_REFLECTIVITY = "MergedReflectivityQCComposite";
const char* vdt_nssl_radar_dataset2::NSSL_RADAR_NC_VAR_ATTR_NAME_FILL = "_FillValue";

//Values for old MRMS Binary format
const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_ATTR_NAME_LATITUDE = "Latitude";
const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_ATTR_NAME_LONGITUDE = "Longitude";
const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_ATTR_NAME_LATITUDE_GRID_SPACING = "LatGridSpacing";
const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_ATTR_NAME_LONGITUDE_GRID_SPACING = "LonGridSpacing";
const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_DIM_NAME_LATITUDE = "Lat";
const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_DIM_NAME_LONGITUDE = "Lon";
const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_VAR_NAME_BASE_REFLECTIVITY = "BASE_REFL";
const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_VAR_NAME_COMPOSITE_REFLECTIVITY = "CREF";
//const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_VAR_ATTR_NAME_SCALE = "Scale";
//const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_VAR_ATTR_NAME_FILL = "MissingData";
const char* vdt_nssl_radar_dataset::NSSL_RADAR_NC_VAR_ATTR_NAME_FILL = "_FillValue";


vdt_nssl_radar_dataset2::vdt_nssl_radar_dataset2(const char* filename, Log* logg)
{
  //use string error variable instead of logg
  this->logg = logg;
  if (!fileExists(filename))
    {
      if (logg != NULL)
        {
          logg->write_time_info("%s not found\n", filename);
        }
      nc = NULL;
      return;
    }
  else if (logg != NULL)
    {
      logg->write_time_info("vdt_nssl_radar_dataset2: opening %s\n", filename);
    }

  this->nc_error = new NcError(NcError::silent_nonfatal);

  this->nc = new NcFile(filename,NcFile::ReadOnly);
  if (!this->nc->is_valid())
    {
      this->nc = NULL;
      return;
    }

  this->filename = filename;
  this->ul_lat = NSSL_RADAR_NW_LATITUDE;
  this->ul_lon = NSSL_RADAR_NW_LONGITUDE;
  this->lat_space = NSSL_RADAR_LATITUDE_GRID_SPACING;
  this->lon_space = NSSL_RADAR_LONGITUDE_GRID_SPACING;
  this->width = this->nc->get_dim(NSSL_RADAR_NC_DIM_NAME_LONGITUDE)->size();
  this->height = this->nc->get_dim(NSSL_RADAR_NC_DIM_NAME_LATITUDE)->size();
  this->lr_lat = this->ul_lat - (this->height * this->lat_space);
  this->lr_lon = this->ul_lon + (this->width * this->lon_space);
  //printf("file name, ul_lat, ul_lon, lr_lat, lr_lon, height, width: %s %f %f %f %f %ld %ld\n", filename, this->ul_lat, this->ul_lon, this->lr_lat, this->lr_lon, height, width);
  this->refl_var = this->nc->get_var(NSSL_RADAR_NC_VAR_NAME_BASE_REFLECTIVITY);
  if (this->refl_var == NULL)
    {
      this->refl_var = this->nc->get_var(NSSL_RADAR_NC_VAR_NAME_COMPOSITE_REFLECTIVITY);
      if (!this->refl_var->is_valid())
	{
	  this->refl_var = NULL;
	  return;
	}
    }

  this->ref_fill = this->refl_var->get_att(NSSL_RADAR_NC_VAR_ATTR_NAME_FILL)->values()->as_float(0);
}

vdt_nssl_radar_dataset2::vdt_nssl_radar_dataset2(const vdt_nssl_radar_dataset2& o)
{
  vdt_nssl_radar_dataset2(o.filename);
}

vdt_nssl_radar_dataset2::~vdt_nssl_radar_dataset2()
{
  delete this->nc_error;
  if (this->nc != NULL)
    {
      this->nc->close();
      delete this->nc;
    }
}

void vdt_nssl_radar_dataset2::corner_tests()
{
  if(!this->nc)
  {
    return;
  }

  logg->write_time_info("nssl radar corner test results\n");

  int x, y;
  float test_val1;
  int test = get_x_y(this->lr_lat, this->ul_lon, x, y);
  if(test != 0)
  {
    logg->write_time_info("Error: unable to get radar x, y from lat, lon");
    return;
  }

  if (y >= height)
  {
    y = height-1;
  }
  if (x >= width)
  {
    x = width-1;
  }
  this->refl_var->set_cur(0,y,x);
  this->refl_var->get(&test_val1,1,1,1);
  logg->write_time_info("lower left nssl:  y, x %d %d for lr_lat, ul_lon %f %f, test_val %f\n", y, x, this->lr_lat, this->ul_lon, test_val1);

  test = get_x_y(this->lr_lat, this->lr_lon, x, y);
  if(test != 0)
  {
    logg->write_time_info("Error: unable to get radar x, y from lat, lon");
    return;
  }

  if (y >= height)
  {
    y = height-1;
  }
  if (x >= width)
  {
    x = width-1;
  }
  this->refl_var->set_cur(0,y,x);
  this->refl_var->get(&test_val1,1,1,1);
  logg->write_time_info("lower right nssl:  y, x %d %d for lr_lat, lr_lon %f %f, test_val %f\n", y, x, this->lr_lat, this->lr_lon, test_val1);

  test = get_x_y(this->ul_lat, this->ul_lon, x, y);
  if(test != 0)
  {
    logg->write_time_info("Error: unable to get radar x, y from lat, lon");
    return;
  }
  if (y >= height)
  {
    y = height-1;
  }
  if (x >= width)
  {
    x = width-1;
  }
  this->refl_var->set_cur(0,y,x);
  this->refl_var->get(&test_val1,1,1,1);
  logg->write_time_info("upper left nssl:  y, x %d %d for ul_lat, ul_lon %f %f, test_val %f\n", y, x, this->ul_lat, this->ul_lon, test_val1);

  test = get_x_y(this->ul_lat, this->lr_lon, x, y);
  if(test != 0)
  {
    logg->write_time_info("Error: unable to get radar x, y from lat, lon");
    return;
  }
  if (y >= height)
  {
    y = height-1;
  }
  if (x >= width)
  {
    x = width-1;
  }
  this->refl_var->set_cur(0,y,x);
  this->refl_var->get(&test_val1,1,1,1);
  logg->write_time_info("upper right nssl:  y, x %d %d for ul_lat, lr_lon %f %f, test_val %f\n", y, x, this->ul_lat, this->lr_lon, test_val1);
}

int vdt_nssl_radar_dataset2::get_reflectivity(float lat, float lon, float& v, int& x, int& y)
{
  if (nc == NULL)
    {
      return 1;
    }

  int r = get_x_y(lat,lon,x,y);
  if (r != 0)
    {
      return r;
    }


  if (y >= height)
    {
      y = height-1;
    }
  if (x >= width)
    {
      x = width-1;
    }
  this->refl_var->set_cur(0,y,x);
  this->refl_var->get(&v,1,1,1);

  return 0;
}

int vdt_nssl_radar_dataset2::get_x_y(float lat, float lon, int& x, int& y)
{
  if (lat > ul_lat || lat < lr_lat || lon > lr_lon || lon < ul_lon)
    {
      return 1;
    }

#ifdef UPPERLEFT
  // Use these if the grids are stored top downwards
  float diff_lat = fabs(ul_lat - lat);
  float diff_lon = fabs(ul_lon - lon);
#else
  // Use these if the grids are stored bottom upwards
  float diff_lat = fabs(lat-lr_lat);
  float diff_lon = fabs(lon-ul_lon);
#endif

  y = floor(diff_lat / this->lat_space);
  x = floor(diff_lon / this->lon_space);
  //printf("nssl: given lat %f, lon %f, ul_lat %f, ul_lon %f, diff_lat %f, diff_lon %f, y %d, x %d\n", lat, lon, ul_lat, ul_lon, diff_lat, diff_lon, y, x);
  return 0;
}

void vdt_nssl_radar_dataset2::get_attribute_value(const char* attr_name, float& value)
{
  NcValues* vals = this->nc->get_att(attr_name)->values();
  value = vals->as_float(0);
}

//-----------------------------------------------------------------------------
//Old MRMS Binary Implementation
//-----------------------------------------------------------------------------
vdt_nssl_radar_dataset::vdt_nssl_radar_dataset(const char* filename, Log* logg)
{
  //use string error variable instead of logg
  this->logg = logg;
  if (!fileExists(filename))
    {
      if (logg != NULL)
        {
          logg->write_time_info("%s not found\n", filename);
        }
      nc = NULL;
      return;
    }
  else if (logg != NULL)
    {
      logg->write_time_info("vdt_nssl_radar_dataset: opening %s\n", filename);
    }

  this->nc_error = new NcError(NcError::silent_nonfatal);

  this->nc = new NcFile(filename,NcFile::ReadOnly);
  if (!this->nc->is_valid())
    {
      this->nc = NULL;
      return;
    }

  this->filename = filename;
  this->get_attribute_value(NSSL_RADAR_NC_ATTR_NAME_LATITUDE, this->ul_lat);
  this->get_attribute_value(NSSL_RADAR_NC_ATTR_NAME_LONGITUDE, this->ul_lon);
  this->get_attribute_value(NSSL_RADAR_NC_ATTR_NAME_LATITUDE_GRID_SPACING, this->lat_space);
  this->get_attribute_value(NSSL_RADAR_NC_ATTR_NAME_LONGITUDE_GRID_SPACING, this->lon_space);
  this->width = this->nc->get_dim(NSSL_RADAR_NC_DIM_NAME_LONGITUDE)->size();
  this->height = this->nc->get_dim(NSSL_RADAR_NC_DIM_NAME_LATITUDE)->size();
  this->lr_lat = this->ul_lat - (this->height * this->lat_space);
  this->lr_lon = this->ul_lon + (this->width * this->lon_space);
  //printf("file name, ul_lat, ul_lon, lr_lat, lr_lon, height, width: %s %f %f %f %f %ld %ld\n", filename, this->ul_lat, this->ul_lon, this->lr_lat, this->lr_lon, height, width);
  this->refl_var = this->nc->get_var(NSSL_RADAR_NC_VAR_NAME_BASE_REFLECTIVITY);
  if (this->refl_var == NULL)
    {
      this->refl_var = this->nc->get_var(NSSL_RADAR_NC_VAR_NAME_COMPOSITE_REFLECTIVITY);
      if (!this->refl_var->is_valid())
	{
	  this->refl_var = NULL;
	  return;
	}
    }

  this->ref_fill = this->refl_var->get_att(NSSL_RADAR_NC_VAR_ATTR_NAME_FILL)->values()->as_float(0);
}

vdt_nssl_radar_dataset::vdt_nssl_radar_dataset(const vdt_nssl_radar_dataset& o)
{
  vdt_nssl_radar_dataset(o.filename);
}

vdt_nssl_radar_dataset::~vdt_nssl_radar_dataset()
{
  delete this->nc_error;
  if (this->nc != NULL)
    {
      this->nc->close();
      delete this->nc;
    }
}

int vdt_nssl_radar_dataset::get_reflectivity(float lat, float lon, float& v, int& x, int& y)
{
  if (nc == NULL)
    {
      return 1;
    }

  int r = get_x_y(lat,lon,x,y);
  if (r != 0)
    {
      return r;
    }


  if (y >= height)
    {
      y = height-1;
    }
  if (x >= width)
    {
      x = width-1;
    }
  this->refl_var->set_cur(y,x);
  this->refl_var->get(&v,1,1);

#ifdef SPOT_TESTS
  printf("nssl: v %f for y, x %d %d\n", v, y, x);

  float test_val1;
  int test = get_x_y(this->lr_lat, this->ul_lon, x, y);
  if (y >= height)
    {
      y = height-1;
    }
  if (x >= width)
    {
      x = width-1;
    }
  this->refl_var->set_cur(y,x);
  this->refl_var->get(&test_val1,1,1);
  printf("lower left nssl:  y, x %d %d for lr_lat, ul_lon %f %f, test_val1 %f\n", y, x, this->lr_lat, this->ul_lon, test_val1);


  test = get_x_y(this->ul_lat, this->lr_lon, x, y);
  if (y >= height)
    {
      y = height-1;
    }
  if (x >= width)
    {
      x = width-1;
    }
  printf("resetting cur to %d %d\n", y,x);
  this->refl_var->set_cur(y,x);
  float test_val2;
  this->refl_var->get(&test_val2,1,1);
  printf("upper right nssl:  y, x %d %d for ur_lat, lr_lon %f %f, test_val2 %f\n", y, x, this->ur_lat, this->lr_lon, test_val2);

  test = get_x_y(43.545, -90.145, x, y);
  if (y >= height)
    {
      y = height-1;
    }
  if (x >= width)
    {
      x = width-1;
    }
  printf("resetting cur to %d %d\n", y,x);
  this->refl_var->set_cur(y,x);
  float test_val3;
  this->refl_var->get(&test_val3,1,1);
  printf("lower right nssl:  y, x %d %d for 43.545, -90.145, test_val3 %f\n", y, x, test_val3);
#endif
  return 0;
}

int vdt_nssl_radar_dataset::get_x_y(float lat, float lon, int& x, int& y)
{
  if (lat > ul_lat || lat < lr_lat || lon > lr_lon || lon < ul_lon)
    {
      return 1;
    }

#ifdef UPPERLEFT
  // Use these if the grids are stored top downwards
  float diff_lat = fabs(ul_lat - lat);
  float diff_lon = fabs(ul_lon - lon);
#else
  // Use these if the grids are stored bottom upwards
  float diff_lat = fabs(lat-lr_lat);
  float diff_lon = fabs(lon-ul_lon);
#endif

  y = floor(diff_lat / this->lat_space);
  x = floor(diff_lon / this->lon_space);
  //printf("nssl: given lat %f, lon %f, ul_lat %f, ul_lon %f, diff_lat %f, diff_lon %f, y %d, x %d\n", lat, lon, ul_lat, ul_lon, diff_lat, diff_lon, y, x);
  return 0;
}

void vdt_nssl_radar_dataset::get_attribute_value(const char* attr_name, float& value)
{
  NcValues* vals = this->nc->get_att(attr_name)->values();
  value = vals->as_float(0);
}

//const int vdt_nssl_tiled_radar_dataset::NUM_NSSL_TILES = 8;
const int vdt_nssl_tiled_radar_dataset::NUM_NSSL_TILES = 4;
const int vdt_nssl_tiled_radar_dataset::MAX_FILE_NAME_LEN = 2048;

vdt_nssl_tiled_radar_dataset::vdt_nssl_tiled_radar_dataset(const char* radar_file_pattern, Log* logg)
{
  this->logg = logg;
  char* fpath = new char[MAX_FILE_NAME_LEN];
  tiles = new vector<vdt_nssl_radar_dataset*>();
  for (int i = 0; i < NUM_NSSL_TILES; i++)
    {
      snprintf(fpath, MAX_FILE_NAME_LEN, radar_file_pattern, i+1);
      vdt_nssl_radar_dataset* ds = new vdt_nssl_radar_dataset(fpath,logg);
      //printf("tile %d = %s\n", i, fpath);
      tiles->push_back(ds);
    }
  delete [] fpath;
}

vdt_nssl_tiled_radar_dataset::~vdt_nssl_tiled_radar_dataset()
{
  for (unsigned int i = 0; i < tiles->size(); i++)
    {
      vdt_nssl_radar_dataset* ds = tiles->at(i);
      delete ds;
    }
  delete tiles;
}

int vdt_nssl_tiled_radar_dataset::get_reflectivity(float lat, float lon, float& v, int& x, int &y)
{
  for (unsigned int i = 0; i < tiles->size(); i++)
    {
      vdt_nssl_radar_dataset* ds = tiles->at(i);
      int r = ds->get_reflectivity(lat,lon,v,x,y);
      if (r == 0)
        {
	  //printf("in nssl tile %d, cref value for lat, lon %f, %f: %f\n", i, lat, lon, v);
          return r;
        }
    }
  v = vdt_const::FILL_VALUE;
  return 1;
}
