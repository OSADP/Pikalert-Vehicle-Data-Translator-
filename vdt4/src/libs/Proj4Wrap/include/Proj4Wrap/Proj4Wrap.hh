//==============================================================================
//
//   (c) Copyright, 2010 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: Proj4Wrap.hh,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2013/10/25 05:12:36 $
//
//==============================================================================

/**
 * @file Proj4Wrap.hh
 *
 * @class Proj4wrap
 *
 *  <file/class description>
 *  
 * @date 3/24/10
 */

#ifndef PROJ4WRAP_HH
#define PROJ4WRAP_HH

#include <string>
#include <proj_api.h>

using std::string;

namespace p4w
{
  /**
   * @class Proj4wrap
   */
  class Proj4Wrap
  {
  public:
    enum OriginType {LON_LAT_TYPE, EASTING_NORTHING_TYPE};

    /** Constructor
     *  @param[in] mapParamStringArg proj4 map parameter string like:
     *  "+proj=lcc +R=6371200 +lon_0=-95.0 +lat_0=25.0 +lat_1=25.0 +lat_2=25.0"
     *  NOTE: Do not supply falseEasting/falseNorthing parameters x_0, y_0 in this string. 
     *  @param[in] originType LON_LAT_TYPE if originXArg, originYArg are longitude, latitude, EASTING_NORTHING_TYPE if originXArg, originYArg are falseEasting, falseNorthing
     *  @param[in] originXArg origin x argument (either origin longitude or false easting)
     *  @param[in] originYArg origin y argument (either origin latitude or false northing)
     *  @param[in] dxArg grid dx in meters. For example, for the RUC 13 km grid, dx = 13545.087.
     *  @param[in] dyArg grid dy in meters. For example, for the RUC 13 km grid, dy = 13545.087.
     */
    Proj4Wrap(string &mapParamStringArg, int originTypeArg, double originXArg, double originYArg, double dxArg, double dyArg);

    /** Destructor */
    virtual ~Proj4Wrap();

    /**
     * @brief Convert x, y in grid coordinates to lat-longs
     *
     * @param[in] xc x coord
     * @param[in] yc y coord
     * @param[out] lon output longitude
     * @param[out] lat output latitude
     * @return 0 on success and -1 on failure
     */
    int xy2ll(double xc, double yc, double *lon, double *lat) const;

    /**
     * @brief Convert lat-longs to grid x y coordinates
     *
     * @param[in] lon y output longitude
     * @param[in] lat output latitude
     * @param[out] xc x coord
     * @param[out] yc y coord
     * @return 0 on success and -1 on failure
     */
    int ll2xy(double lon, double lat, double *xc, double *yc) const;

    const string & getError() const { return error; }
    double getDx() const { return dx; }
    double getDy() const { return dy; }
    const string & getMapParamString() const { return mapParamString; }
    int getOriginType() const { return originType; }
    double getOriginLat() const { return originLat; }
    double getOriginLon() const { return originLon; }
    double getFalseEasting() const { return falseEasting; }
    double getFalseNorthing() const { return falseNorthing; }


  private:

    // error string
    string error;

    // projection reference used in conjunction with the proj4 library
    projPJ projRef;
  
    // mapParamString map parameter string 
    string mapParamString;
      
    // originLat grid origin latitude 
    double originLat;

    // originLon grid origin longitude 
    double originLon;

    // originType grid origin longitude 
    int originType;

    // falseEasting -x offset of grid origin in meters from center of projection 
    double falseEasting;

    // falseNorthing -y offset of grid origin in meters from center of projection 
    double falseNorthing;

    // dx grid dx distance 
    double dx;

    // dy grid dy distance 
    double dy;

    Proj4Wrap & operator=(const Proj4Wrap &proj4Wrap);
    Proj4Wrap(const Proj4Wrap &proj4Wrap);
  };
}

#endif /* PROJ4WRAP_HH */
