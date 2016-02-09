//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_cloud_mask_dataset.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_cloud_mask_dataset.hh
 *
 * @class vdt_cloud_mask_dataset
 *
 *  vdt cloud mask dataset class
 *
 */


#ifndef VDT_CLOUD_MASK_DATASET_HH
#define VDT_CLOUD_MASK_DATASET_HH

#include "vdt_nc_file_reader.hh"
//#include <ogr_spatialref.h>

/**
 * @class vdt_cloud_mask_dataset
 */
class vdt_cloud_mask_dataset : public vdt_nc_file_reader
{

public:

  /** @brief x dimension netcdf name */
  static const char* X_DIM;

  /** @brief y dimension netcdf name */
  static const char* Y_DIM;

  /** @brief x-direction grid length netcdf variable name */
  static const char* DX;

  /** @brief y-direction grid length netcdf variable name */
  static const char* DY;

  /** @brief lower left latitude netcdf variable name */
  static const char* LOWER_LEFT_LAT;

  /** @brief lower left longitude netcdf variable name */
  static const char* LOWER_LEFT_LON;

  /** @brief first intersecting latitude netcdf variable name */
  static const char* LAT_1_INTERSECT;

  /** @brief second intersecting latitude netcdf variable name */
  static const char* LAT_2_INTERSECT;

  /** @brief grid orientation netcdf variable name */
  static const char* ORIENTATION;
 
  /** @brief latitude netcdf variable name */
  static const char* LAT;
 
  /** @brief longitude netcdf variable name */
  static const char* LON;

  /** @brief cloud mask variable name */
  static const char* IS_CLOUD;

  /** @brief proj4 string name */
  static const char* PROJ4_STRING;

  /** @brief geo transform size */
  static const size_t GEO_TRANSFORM_SIZE;

  /**
   * @brief constructor
   * @param[in] fname  netcdf file name
   * @param[in] logg   log file
   */
  vdt_cloud_mask_dataset(const char* fname);

  ~vdt_cloud_mask_dataset();

  /**
   * @brief  get cloud value
   * @param[in] lat  latitude
   * @param[in] lon  longitude
   * @param[out] cloud  cloud value
   * @return 0 on success, -1 on failure
   */
  int get_cloud(double lat, double lon, short &cloud);

private:

  /** @brief x dimension */
  int x;
  
  /** @brief y dimension */
  int y;

  /** @brief lower left latitude */
  float ll_lat;

  /** @brief lower left longitude */
  float ll_lon;

  /** @brief first intersecting latitude */
  float la1;

  /** @brief second intersecting latitude */
  float la2;

  /** @brief orientation of the grid */
  float lov;

  /** @brief x-direction grid length */
  float dx;

  /** @brief y-direction grid length */
  float dy;

  /** @brief pointer to an OGRSpatialReference object for the target spatial reference system*/
  //OGRSpatialReference* model_ref;

  /** @brief pointer to an OGRSpatialReference object for the source spatial reference system */
  //OGRSpatialReference* def_ref;

  /** @brief pointer to an OGRCoordinateTransformation object */
  //OGRCoordinateTransformation* trans;

  /** @brief input geotransform */
  double* geo;

  /** @brief output geotransform */
  double* inv_geo;
};

#endif /* VDT_CLOUD_MASK_DATASET_HH */
