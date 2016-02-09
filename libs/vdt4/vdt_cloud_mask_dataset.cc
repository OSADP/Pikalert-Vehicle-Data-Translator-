
#include "vdt_cloud_mask_dataset.hh"
#include "vdt_const.hh"
#include <gdal.h>
#include <iostream>

using namespace std;

const char* vdt_cloud_mask_dataset::X_DIM = "x";
const char* vdt_cloud_mask_dataset::Y_DIM = "y";

const char* vdt_cloud_mask_dataset::DX = "Dx";
const char* vdt_cloud_mask_dataset::DY = "Dy";
const char* vdt_cloud_mask_dataset::LOWER_LEFT_LAT = "La1";
const char* vdt_cloud_mask_dataset::LOWER_LEFT_LON = "Lo1";
const char* vdt_cloud_mask_dataset::LAT_1_INTERSECT = "Latin1";
const char* vdt_cloud_mask_dataset::LAT_2_INTERSECT = "Latin2";
const char* vdt_cloud_mask_dataset::ORIENTATION = "Lov";
const char* vdt_cloud_mask_dataset::LAT = "lat";
const char* vdt_cloud_mask_dataset::LON = "lon";
const char* vdt_cloud_mask_dataset::IS_CLOUD = "is_cloud";
const char* vdt_cloud_mask_dataset::PROJ4_STRING = "+proj=lcc +lon_0=95W +lat_0=25N +lat_1=25N";
const size_t vdt_cloud_mask_dataset::GEO_TRANSFORM_SIZE = 6;

vdt_cloud_mask_dataset::vdt_cloud_mask_dataset(const char* fname):vdt_nc_file_reader(fname)
{

  NcVar* int1_var = nc->get_var(LAT_1_INTERSECT);
  NcVar* int2_var = nc->get_var(LAT_2_INTERSECT);
  NcVar* orient_var = nc->get_var(ORIENTATION);
  int1_var->get(&la1);
  int2_var->get(&la2);
  orient_var->get(&lov);

  NcVar* ll_lat_var = nc->get_var(LOWER_LEFT_LAT);
  NcVar* ll_lon_var = nc->get_var(LOWER_LEFT_LON);
  ll_lat_var->get(&ll_lat);
  ll_lon_var->get(&ll_lon);

  NcVar* dx_var = nc->get_var(DX);
  NcVar* dy_var = nc->get_var(DY);
  x = nc->get_dim(X_DIM)->size();
  y = nc->get_dim(Y_DIM)->size();
  dx_var->get(&dx);
  dy_var->get(&dy);

  NcVar* lat_var = nc->get_var(LAT);
  NcVar* lon_var = nc->get_var(LON);
  lat_var->set_cur(y-1,0);
  lon_var->set_cur(y-1,0);
  float ul_lat, ul_lon;
  lat_var->get(&ul_lat,1,1);
  lon_var->get(&ul_lon,1,1);

  model_ref = new OGRSpatialReference();
  model_ref->importFromProj4(PROJ4_STRING);
  def_ref = new OGRSpatialReference();

  def_ref->importFromEPSG(4326);
  trans = OGRCreateCoordinateTransformation(def_ref,model_ref);
  geo = new double[GEO_TRANSFORM_SIZE];
  inv_geo = new double[GEO_TRANSFORM_SIZE];
  double ul_x = ul_lon;
  double ul_y = ul_lat;
  trans->Transform(1,&ul_x,&ul_y);
  geo[0] = ul_x;
  geo[1] = dx;
  geo[2] = 0;
  geo[3] = ul_y;
  geo[4] = 0;
  geo[5] = -dy;
  GDALInvGeoTransform(geo,inv_geo);
}

vdt_cloud_mask_dataset::~vdt_cloud_mask_dataset()
{
  delete [] geo;
  delete [] inv_geo;
  delete model_ref;
  delete def_ref;
  delete trans;
}

int vdt_cloud_mask_dataset::get_cloud(double lat, double lon, short &cloud)
{
  if (nc == NULL)
    {
      cloud = vdt_const::SHORT_FILL_VALUE;
      return 1;
    }
  double px,py;
  double dlat = lat;
  double dlon = lon;
  if (!trans->Transform(1, &lon, &lat))
    {
      cloud = vdt_const::SHORT_FILL_VALUE;
      return vdt_const::SHORT_FILL_VALUE;
    }
  GDALApplyGeoTransform(inv_geo,lon,lat,&px,&py);

  NcVar* lat_var = nc->get_var("lat");
  NcVar* lon_var = nc->get_var("lon");
  int ppy = y - py; //nc dataset is lower left origin, gdal provides pixels ins upper left origin
  int ppx = (int)px;
  if (ppy < 0 || ppy >= y ||
      ppx < 0 || ppx >= x)
    {
      printf("%f, %f is outside of cloud mask bounds\n", dlat, dlon);
      cloud = vdt_const::SHORT_FILL_VALUE;
      return vdt_const::SHORT_FILL_VALUE;
    }
  lat_var->set_cur(ppy,ppx);
  lon_var->set_cur(ppy,ppx);
  float llat,llon;
  lat_var->get(&llat,1,1);
  lon_var->get(&llon,1,1);

  NcVar* cloud_var = nc->get_var(IS_CLOUD);
  cloud_var->set_cur(ppy,ppx);
  ncbyte is_cloud;
  cloud_var->get(&is_cloud,1,1);
  if (is_cloud)
    {
      cloud = 0;
    }
  cloud = 1;
  return 0;
}
