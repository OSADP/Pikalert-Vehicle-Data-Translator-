#include "vdt_road_segment_data.hh"

using namespace std;


field_info::field_info(const vector<string> &names, const vector<string> &units) : field_names(names), field_units(units)
{
  size_t min = 0;

  if (names.size() < units.size())
    {
      min = names.size();
    }
  else
    {
      min = units.size();
    }

  for (size_t i=0; i<min; i++)
    {
      field_map[names[i]] = i;
    }
};


vdt_road_segment_data::vdt_road_segment_data(const field_info &field_info_arg, double obs_time_arg, int number_probe_messages_arg, const vdt_road_segment &road_segment_arg) : field_info_obj(field_info_arg), obs_time(obs_time_arg), number_probe_messages(number_probe_messages_arg), road_segment(road_segment_arg)
{
  field_values.assign(field_info_obj.field_map.size(), FI_FILL_FLOAT);
}

vdt_road_segment_data::~vdt_road_segment_data()
{
}


ostream& operator<<(ostream &ostr, const vdt_road_segment_data &road_segment_data)
{
  ostr << "begin: \n";
  ostr << "id: " << road_segment_data.road_segment.get_id() << endl;
  ostr << "begin_point: " << road_segment_data.road_segment.get_begin_point();
  ostr << "mid_point: " << road_segment_data.road_segment.get_mid_point();
  ostr << "end_point: " << road_segment_data.road_segment.get_end_point();
  ostr << "road_name: " << road_segment_data.road_segment.get_road_name() << endl;
  ostr << "road_type: " << road_segment_data.road_segment.get_road_type() << endl;
  ostr << "route_class: " << road_segment_data.road_segment.get_route_class() << endl;
  ostr << "highway_symbol: " << road_segment_data.road_segment.get_hwy_symbol() << endl;
  ostr << "highway_type: " << road_segment_data.road_segment.get_hwy_type() << endl;
  ostr << "speed_limit: " << road_segment_data.road_segment.get_speed_limit() << endl;

#ifdef NOTNOW  
  const vector<vdt_point> &seg_points = road_segment_data.road_segment.get_points();
  for (unsigned int i=0; i<seg_points.size(); i++)
    {
      ostr << "points: " << seg_points[i] << endl;
    }
#endif

  ostr << "names, units, values sizes: " << road_segment_data.field_info_obj.field_names.size() << road_segment_data.field_info_obj.field_units.size() << road_segment_data.field_values.size() << endl;

  for (size_t i=0; i<road_segment_data.field_values.size(); i++)
    {
      if (road_segment_data.field_values[i] != FI_FILL_FLOAT)
	{
	  ostr << "field, units, value: " << road_segment_data.field_info_obj.field_names[i] << ", " <<  road_segment_data.field_info_obj.field_units[i] << ", " << road_segment_data.field_values[i] << endl;
	}
    }
  
  return ostr;
}

int vdt_road_segment_data::print_header(FILE *fp)
{
  int ret;

  fprintf(fp, "case, utc_seconds, number_probe, ");
  size_t end = field_info_obj.field_names.size()-1;
  for (size_t i=0; i<end; i++)
    {
      ret = fprintf(fp, "%s (%s), ", field_info_obj.field_names[i].c_str(), field_info_obj.field_units[i].c_str());
      if (ret < 0)
	{
	  return -1;
	}
    }

  ret = fprintf(fp, "%s (%s)\n", field_info_obj.field_names[end].c_str(), field_info_obj.field_units[end].c_str());

  if (ret < 0)
    {
      return -1;
    }

  return 0;
}

int vdt_road_segment_data::print_values(FILE *fp, int case_number)
{
  int ret;

  ret = fprintf(fp, "%d, %ld, %d, ", case_number, static_cast<long int>(obs_time), number_probe_messages);

  size_t end = field_info_obj.field_names.size()-1;
  for (size_t i=0; i<end; i++)
    {
      if (field_values[i] == FI_FILL_FLOAT)
	{
	  ret = fprintf(fp, "-9999, ");
	  if (ret < 0)
	    {
	      return -1;
	    }
	}
      else
	{
	  ret = fprintf(fp, "%f, ", field_values[i]);
	  if (ret < 0)
	    {
	      return -1;
	    }
	}
    }

  if (field_values[end] == FI_FILL_FLOAT)
    {
      ret = fprintf(fp, "-9999\n");
      if (ret < 0)
	{
	  return -1;
	}
    }
  else
    {
      ret = fprintf(fp, "%f\n", field_values[end]);
      if (ret < 0)
	{
	  return -1;
	}
    }

  return 0;
}

int vdt_road_segment_data::print_aux_id_field(FILE *fp, int field_number)
{
  int ret;

  const int MAX_TIME_STRING = 32;
  vector<char> time_char_vec(MAX_TIME_STRING);

  time_t time_value = obs_time;
  struct tm *ptms = gmtime(&time_value);

  strftime(&time_char_vec[0], MAX_TIME_STRING, "%Y-%m-%d %H:%M:%S", ptms);

  string time_string(time_char_vec.begin(), time_char_vec.end());
  string aux_id = road_segment.get_aux_id();

  float field_value = get_field(field_number);

  ret = fprintf(fp, "%s,%s,%f\n", aux_id.c_str(), time_string.c_str(), field_value);
  if (ret != 3)
    {
      return -1;
    }

  return 0;
}

