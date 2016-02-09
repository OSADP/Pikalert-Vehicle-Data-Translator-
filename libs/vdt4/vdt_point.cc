
#include <math.h>
#include "vdt_point.hh"

using namespace std;

vdt_point::vdt_point()
{
};

const double vdt_point::operator[](const int n)
{
  if (n == 0)
    {
      return get_latitude();
    }
  return get_longitude();
}

bool vdt_point::operator==(const vdt_point& o)
{
  return (o.get_latitude() == get_latitude() &&
          o.get_longitude() == get_longitude());
}

bool vdt_point::operator!=(const vdt_point& o)
{
  return (o.get_latitude() != get_latitude() ||
          o.get_longitude() != get_longitude());
}

ostream& operator<<(ostream &ostr, const vdt_point &point)
{
  ostr << "latitude: " << point.get_latitude() << " longitude: " << point.get_longitude() << " elevation: " << point.get_elevation() << endl;
  return ostr;
}

const double vdt_point::GRC_EARTH_RADIUS = 6371.229;

double vdt_point::radians(double degrees)
{
  return (M_PI / 180.0) * degrees;
}

double vdt_point::great_circle_distance(double lat1, double lon1, double lat2, double lon2)
{
  // Return the great circle distance between two lat, lons

  double rlat1 = radians(lat1);
  double rlat2 = radians(lat2);
  double rlon1 = radians(lon1);
  double rlon2 = radians(lon2);
  double delta_lat = rlat2 - rlat1;
  double delta_lon = rlon2 - rlon1;

  double temp = pow(sin(delta_lat / 2.0), 2) + cos(rlat1) * cos(rlat2) * pow(sin(delta_lon / 2.0), 2);
  double distance = GRC_EARTH_RADIUS * 2 * atan2(sqrt(temp), sqrt(1 - temp));

  return distance;
}

