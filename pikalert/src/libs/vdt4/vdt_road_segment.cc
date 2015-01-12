// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include "vdt_road_segment.hh"
#include "vdt_spatial_check.hh"

using namespace std;

vdt_road_segment::vdt_road_segment()
{
};

vdt_road_segment::~vdt_road_segment()
{
}

void vdt_road_segment::set_points(const vector<vdt_point> &point_vector)
{
  points = point_vector;
  unsigned int num_points = point_vector.size();
  double total_distance = 0;
  vector<double> distances;
  if (num_points > 0)
    {
      // Note that we do not take the centroid to find the mid
      // point. If the road segment is curved like a semicircle, the
      // centroid would be the center of the circle. Instead we add
      // the great circle distances between the points
      // We then find the point just over half the total distance.  
      for (size_t i=0; i<num_points-1; i++)
	{
	  double delta = vdt_point::great_circle_distance(point_vector[i].get_latitude(), point_vector[i].get_longitude(), point_vector[i+1].get_latitude(), point_vector[i+1].get_longitude());
	  distances.push_back(delta);
	  total_distance += delta;
	}

      double half_total_distance = total_distance / 2.0;
      double partial_distance = 0;
      double previous_partial_distance = 0;
      size_t mid_index = 0;
      for (size_t i=0; i<num_points-1; i++)
	{
	  previous_partial_distance = partial_distance;
	  partial_distance += distances[i];
	  if (partial_distance >= half_total_distance)
	    {
	      if ((half_total_distance - previous_partial_distance) < (partial_distance - half_total_distance))
	      {
		mid_index = i;
	      }
	      else
	      {
		mid_index = i+1;
	      }
	      break;
	    }
	}

      mid_point = points[mid_index];
    }
}

ostream& operator<<(ostream &ostr, const vdt_road_segment &road_segment)
{
  ostr << "id: " << road_segment.id << endl;
  ostr << "begin_point: " << road_segment.begin_point;
  ostr << "mid_point: " << road_segment.mid_point;
  ostr << "end_point: " << road_segment.end_point;
  ostr << "road_name: " << road_segment.road_name << endl;
  ostr << "road_type: " << road_segment.road_type << endl;
  ostr << "route_class: " << road_segment.route_class << endl;
  ostr << "highway_symbol: " << road_segment.hwy_symbol << endl;
  ostr << "highway_type: " << road_segment.hwy_type << endl;
  ostr << "speed_limit: " << road_segment.speed_limit << endl;
  for (unsigned int i=0; i<road_segment.points.size(); i++)
    {
      ostr << "points: " << road_segment.points[i] << endl;
    }

  return ostr;
}
