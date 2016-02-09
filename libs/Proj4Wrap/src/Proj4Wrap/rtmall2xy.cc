
// Include files 

#include <stdio.h>
#include <string>
#include <vector>
#include <proj_api.h>
#include "Proj4Wrap/Proj4Wrap.hh"

//#include <proj_api.h>

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

/*
La1 = 16.281; latitude of lower left corner of grid
 Lo1 = -126.138; longitude of lower left corner of grid
 Lov = -95.0; median aligned with cartesian y-axis
 Dx = 13545.087;
 Dy = 13545.087;
 Latin1 = 25.0 ; latitude at lambert conformal projection is true
 Latin2 = 25.0 ;
 ProjFlag = 0;
 SpLat = 0.0;
 SpLon = 0.0;
 ScanS2N = 1;
NX 451
NY 337
 */

using namespace std;

int MAX_LINE = 132;


int proj_offsets(projPJ proj_ref, double lat, double lon, double *xoffset, double *yoffset)
{
  projUV proj_data;

  proj_data.u = lon * DEG_TO_RAD;
  proj_data.v = lat * DEG_TO_RAD;
  proj_data = pj_fwd(proj_data, proj_ref);
  if (proj_data.u != HUGE_VAL)
    {
      *xoffset = proj_data.u;
      *yoffset = proj_data.v;
      return 0;
    }

  return -1;
}


int proj_ll2xy(projPJ *proj_ref, double lat, double lon, double xoff, double yoff, double dx, double dy, double *xc, double *yc)
{
  projUV proj_data;

  proj_data.u = lon * DEG_TO_RAD;
  proj_data.v = lat * DEG_TO_RAD;
  proj_data = pj_fwd(proj_data, proj_ref);

  if (proj_data.u != HUGE_VAL)
    {
      *xc = (proj_data.u + xoff)/dx;
      *yc = (proj_data.v + yoff)/dy;
      return 0;
    }

  return -1;
}

int proj_xy2ll(projPJ *proj_ref, double xc, double yc, double xoff, double yoff, double dx, double dy, double *lat, double *lon)
{
  projUV proj_data;

  proj_data.u = xc * dx - xoff;
  proj_data.v = yc * dy - yoff;

  proj_data = pj_inv(proj_data, proj_ref);

  if (proj_data.u != HUGE_VAL)
    {
      *lon = proj_data.u * RAD_TO_DEG;
      *lat = proj_data.v * RAD_TO_DEG;
      return 0;
    }

  return 1;
}

int init_lambert(double lov, double latin1, double latin2, double la1, double lo1, projPJ &proj_ref, double *pxoff, double *pyoff)
{
  const int LEN = 256;
  char lon_0[LEN];
  char lat_1[LEN];
  char lat_2[LEN];
  char proj_string[LEN];
  char ellipsoid[LEN];
  char no_defs[LEN];

  snprintf(proj_string, LEN, "proj=lcc");
  snprintf(ellipsoid, LEN, "R=6371200");
  snprintf(no_defs, LEN, "no_defs");

  snprintf(lon_0, LEN, "\"lon_0=%f\"", lov); 
  snprintf(lat_1, LEN, "\"lat_1=%f\"", latin1); 
  snprintf(lat_2, LEN, "\"lat_2=%f\"", latin2); 


#ifdef NOTNOW
  char *parms[] =
    {
      proj_string,
      ellipsoid,
      lon_0,
      lat_1,
      lat_2,
      no_defs
    };
#endif

  printf("step 1\n");
  proj_ref = pj_init_plus("+proj=lcc +R=6371200 +lon_0=-95.0 +lat_1=25.0 +lat_2=25.0");
  projPJ pj_latlong = pj_init_plus("+proj=latlong +R=6371200");
  double xoff;
  double yoff;
  printf("step 2\n");
  proj_offsets(proj_ref, la1, lo1, &xoff, &yoff);
  printf("step 3\n");
  printf("xoff, yoff %g %g\n", xoff, yoff);
  xoff = -xoff;
  yoff = -yoff;
  *pxoff = xoff;
  *pyoff = yoff;

  return 0;
}

int main(int argc, char **argv)
{
  // Uses latitude where lambert conformal projection is true and
  // median aligned with cartesian y-axis
  projPJ proj_ref;
  double xoff;
  double yoff;
  double lov = -95;
  double la1 = 20.192;
  double lo1 = -121.554;
  double latin1 = 25.0;
  double latin2 = 25.0;
  double dx = 5079.406;
  double dy = 5079.406;

#ifdef NOTNOW
  printf("init_lambert:\n");
  init_lambert(lov, latin1, latin2, la1, lo1, proj_ref, &xoff, &yoff);
  printf("xoff, yoff %.8lf %.8lf\n", xoff, yoff);
  printf("after init\n");

  for (int i=0; i<337; i++)
    for (int j=0; j<451; j++)
      {
	double xc = j;
	double yc = i;
	double lat;
	double lon;
	proj_xy2ll(proj_ref, xc, yc, xoff, yoff, dx, dy, &lat, &lon);
	printf("[%d %d]: %g %g\n", i, j, lat, lon);
      }
#endif

  string rucParamString = string("+proj=lcc +R=6371200 +lon_0=-95.0 +lat_0=25.0 +lat_1=25.0 +lat_2=25.0");
  p4w::Proj4Wrap lambertProj(rucParamString, p4w::Proj4Wrap::LON_LAT_TYPE, lo1, la1, dx, dy);

  double lat = 20.192;
  double lon = -121.554;
  double xc;
  double yc;
  lambertProj.ll2xy(lon, lat, &xc, &yc);
  printf("lon, lat, xc, yc: %lg %lg %lg %lg\n", lon, lat, xc, yc);

  lat  = 20.33182;
  lon = -69.2084;
  lambertProj.ll2xy(lon, lat, &xc, &yc);
  printf("lon, lat, xc, yc: %lg %lg %lg %lg\n", lon, lat, xc, yc);

  lat  = 50.10547;
  lon = -60.8859; 
  lambertProj.ll2xy(lon, lat, &xc, &yc);
  printf("lon, lat, xc, yc: %lg %lg %lg %lg\n", lon, lat, xc, yc);

  lat = 49.93959;
  lon = -130.1034;
  lambertProj.ll2xy(lon, lat, &xc, &yc);
  printf("lon, lat, xc, yc: %lg %lg %lg %lg\n", lon, lat, xc, yc);

}
