// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_dual_pol_dataset.hh"
#include "math.h"
#include <log/log.hh>
#include "vdt_const.hh"
#include "vdt_util.hh"
#include "vdt_point.hh"

//#define SPOT_TESTS 1
//#define DEBUG 1

const float vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_LATITUDE_GRID_SPACING = 0.01f; 
const float vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_LONGITUDE_GRID_SPACING = 0.01f;
const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_VAR_NAME_LATITUDE = "latitude";
const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_VAR_NAME_LONGITUDE = "longitude";
const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_DIM_NAME_LATITUDE = "latitude";
const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_DIM_NAME_LONGITUDE = "longitude";
const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_DIM_NAME_RADAR_NAME_LEN = "radar_name_len";
const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_DIM_NAME_NUM_RADARS = "num_radars";
const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_VAR_NAME_HYDROMETEOR_CLASSIFICATION = "HHC";
const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_VAR_NAME_DIGITAL_REFLECTIVITY = "DHR";
const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_VAR_NAME_RADAR_NAME = "Radar_name";

const char* vdt_dual_pol_tiled_radar_dataset::DUAL_POL_RADAR_NC_VAR_ATTR_NAME_FILL = "_FillValue";

map<int, int> vdt_dual_pol_tiled_radar_dataset::DHR_priority_table;


vdt_dual_pol_tiled_radar_dataset::vdt_dual_pol_tiled_radar_dataset(const char* radar_file_name, int isClassif, float radar_rad, Log* logg)
{

  // Initialize the priority table map
  DHR_priority_table[100] = 11;
  DHR_priority_table[70] = 10;
  DHR_priority_table[60] = 9;
  DHR_priority_table[80] = 8;
  DHR_priority_table[90] = 7;
  DHR_priority_table[50] = 6;
  DHR_priority_table[40] = 5;
  DHR_priority_table[30] = 4;
  DHR_priority_table[10] = 3;
  DHR_priority_table[20] = 2;
  DHR_priority_table[140] = 1;
  
  this->logg = logg;

  logg->write_time_info("Reading: %s\n", radar_file_name);  

  
  if (!fileExists(radar_file_name))
    {
      if (logg != NULL)
        {
          logg->write_time_info("%s not found\n", radar_file_name);
        }
      nc = NULL;
      return;
    }
  else if (logg != NULL)
    {
      logg->write_time_info("vdt_dual_pol_radar_dataset: opening %s\n", radar_file_name);
    }

  this->nc_error = new NcError(NcError::silent_nonfatal);

  this->nc = new NcFile(radar_file_name, NcFile::ReadOnly);
  
  if (!this->nc->is_valid())
    {
      this->nc = NULL;
      return;
    }
  
  this->filename = radar_file_name;
  this->lat_space = DUAL_POL_RADAR_LATITUDE_GRID_SPACING;
  this->lon_space = DUAL_POL_RADAR_LONGITUDE_GRID_SPACING;
  this->width = this->nc->get_dim(DUAL_POL_RADAR_NC_DIM_NAME_LONGITUDE)->size();
  this->height = this->nc->get_dim(DUAL_POL_RADAR_NC_DIM_NAME_LATITUDE)->size();
  this->num_radars = this->nc->get_dim(DUAL_POL_RADAR_NC_DIM_NAME_NUM_RADARS)->size();
  this->radar_name_len = this->nc->get_dim(DUAL_POL_RADAR_NC_DIM_NAME_RADAR_NAME_LEN)->size();
  this->radar_radius = radar_rad;
  this->isClassif = isClassif;
  
  //printf("file name, ul_lat, ul_lon, lr_lat, lr_lon, height, width: %s %f %f %f %f %ld %ld\n", filename, this->ul_lat, this->ul_lon, this->lr_lat, this->lr_lon, height, width);

  this->lat_var = this->nc->get_var(DUAL_POL_RADAR_NC_VAR_NAME_LATITUDE);
  this->lon_var = this->nc->get_var(DUAL_POL_RADAR_NC_VAR_NAME_LONGITUDE);
    
  this->refl_var = this->nc->get_var(DUAL_POL_RADAR_NC_VAR_NAME_HYDROMETEOR_CLASSIFICATION);
  if (this->refl_var == NULL)
    {
      this->refl_var = this->nc->get_var(DUAL_POL_RADAR_NC_VAR_NAME_DIGITAL_REFLECTIVITY);
      if (!this->refl_var->is_valid())
	{
	  this->refl_var = NULL;
	  return;
	}
    }

  this->radar_name_var = this->nc->get_var(DUAL_POL_RADAR_NC_VAR_NAME_RADAR_NAME);
  if (this->radar_name_var == NULL)
    {
      logg->write_time_info("Error: Couldn't read radar name variable\n");
    }

  /*
  logg->write_time_info("Var.size(): %d\n", this->refl_var->num_vals());
  for(int xx = 34920; xx < 34930; xx++){
    logg->write_time_info("Var.value(%d): %f\n", xx, this->refl_var->values()->as_float(xx));
  }
  */				      
  this->ref_fill = this->refl_var->get_att(DUAL_POL_RADAR_NC_VAR_ATTR_NAME_FILL)->values()->as_float(0);
  
  
}

vdt_dual_pol_tiled_radar_dataset::~vdt_dual_pol_tiled_radar_dataset()
{
  delete this->nc_error;
  if (this->nc != NULL)
    {
      this->nc->close();
      delete this->nc;
    }  
}

int vdt_dual_pol_tiled_radar_dataset::get_reflectivity(float lat, float lon, float& v, int& x, int &y)
{

  if (nc == NULL)
    {
      return 1;
    }

  int radar_ind = -1;
  vector<float> v_list;
  float tmp_v;
  // Get list of nearby points
  vector< pair<int, int> > radar_point_list = get_x_y_list(lat,lon, radar_ind);
  if (radar_ind == -1)
    {
      return 1;
    }
  
  // Get value at each radar point
  for (uint t = 0; t < radar_point_list.size(); t++)
    {
      this->refl_var->set_cur(radar_ind,radar_point_list[t].second,radar_point_list[t].first);
      this->refl_var->get(&tmp_v,1,1,1);
      // Only push back non-missing vals
      if (tmp_v != 9.96921e+36f){
	v_list.push_back(tmp_v);
      }
    }  

  // No non-missing vals, set value to missing and return
  if (v_list.size() == 0){
    v = vdt_const::FILL_VALUE;
    return 0;
  }

  // If a classification variable, calculate the mode, else calculate the mean
  if (this->isClassif == 1){
    v = calc_mode(v_list);
  }else{
    v = calc_mean(v_list);
  }

  
  //logg->write_time_info("Getting radar at index: %d,%d,%d  radius: %f   val: %f\n", radar_ind, y, x, this->radar_radius, v);
#ifdef DEBUG  
  logg->write_time_info("Getting radar at index: %d, radius: %f   val: %f\n", radar_ind, this->radar_radius, v);
#endif  
  // If the value is missing, set to the FILL_VALUE for this application
  if (v == 9.96921e+36f){
    v = vdt_const::FILL_VALUE;
  }

  return 0;
}

void vdt_dual_pol_tiled_radar_dataset::corner_tests()
{
  if(!this->nc)
  {
    return;
  }

  logg->write_time_info("dual_pol radar corner test results\n");
  
  float l_lat, u_lat;
  float l_lon, r_lon;

  // ASSUMING RADAR LAT/LONS START AT LOWER LEFT AND GO THROUGH UPPER RIGHT
  for (int n = 0; n < this->num_radars; n++)
    {
      this->lat_var->set_cur(n, 0);
      this->lat_var->get(&l_lat,1,1);

      this->lat_var->set_cur(n, this->height-1);
      this->lat_var->get(&u_lat,1,1);

      this->lon_var->set_cur(n, 0);
      this->lon_var->get(&l_lon,1,1);

      this->lon_var->set_cur(n, this->width-1);
      this->lon_var->get(&r_lon,1,1);
      
      logg->write_time_info("Getting corners for radar: %s  lower_left_corner: %f,%f  upper_right_corner: %f,%f\n", this->radar_name_var->values()->as_string(n*this->radar_name_len), l_lat, l_lon, u_lat, r_lon);      

    }
}

vector< pair< int, int> > vdt_dual_pol_tiled_radar_dataset::get_x_y_list(float lat, float lon, int& rad_ind)
{
  // Lower and upper lats/lons
  float l_lat, u_lat;
  float l_lon, r_lon;

  int x, y;
  vector< pair <int, int> > x_y_inds;
  pair<int, int> ind;
  
  for (int n = 0; n < this->num_radars; n++)
    {
      this->lat_var->set_cur(n, 0);
      this->lat_var->get(&l_lat,1,1);

      this->lat_var->set_cur(n, this->height-1);
      this->lat_var->get(&u_lat,1,1);

      this->lon_var->set_cur(n, 0);
      this->lon_var->get(&l_lon,1,1);

      this->lon_var->set_cur(n, this->width-1);
      this->lon_var->get(&r_lon,1,1);

      
      // Check to see if the lat lon falls inside this radar
#ifdef DEBUG
      logg->write_time_info("Looking for %f,%f inside grid %s.  %f,%f,%f,%f\n", lat, lon, this->radar_name_var->values()->as_string(n*this->radar_name_len), u_lat, l_lat, l_lon, r_lon);
#endif    

      // Check to see if the lat lon falls inside this radar
      if (lat > u_lat || lat < l_lat || lon > r_lon || lon < l_lon)
	{
#ifdef DEBUG	  
	  logg->write_time_info("Not found: %f,%f\n", lat, lon);
#endif	  	  
	  continue;
	}
      else
	{
	  float diff_lat = fabs(lat - l_lat);
	  float diff_lon = fabs(lon - l_lon);

	  float t_lat1, t_lon1, t_lat2, t_lon2;
	  
	  y = floor(diff_lat / this->lat_space);
	  x = floor(diff_lon / this->lon_space);

#ifdef DEBUG	  
          logg->write_time_info("Found %f,%f inside grid %s.  x: %d y: %d   diff_lat, diff_lon: %f,%f\n", lat, lon, this->radar_name_var->values()->as_string(n*this->radar_name_len), x, y, diff_lat, diff_lon);
#endif
	  // Get the latitude at the lower left hand corner, and the next latitude parallel above that
	  this->lat_var->set_cur(n, y);
	  this->lat_var->get(&t_lat1,1,1);
	  this->lat_var->set_cur(n, y+1);
	  this->lat_var->get(&t_lat2,1,1);
	  // Calculate the distance between the two points	  
	  double y_dist = vdt_point::great_circle_distance(t_lat1, l_lon, t_lat2, l_lon);
	  // Calculate how many latitude parallel's we need to go down to test the x dist
	  int ny = floor(this->radar_radius / y_dist)+1;
	  // Get the latitude value at this lower level (May need to change for southern hemisphere?)
	  this->lat_var->set_cur(n,y-ny);
	  this->lat_var->get(&t_lat1,1,1);	    

	  // Get the longitude at the lower left corner, and the next longitude bin
	  this->lon_var->set_cur(n, x);
	  this->lon_var->get(&t_lon1,1,1);
	  this->lon_var->set_cur(n, x+1);
	  this->lon_var->get(&t_lon2,1,1);
	  // Calculate the distance between these using the lowest acceptable lat bin
	  double x_dist = vdt_point::great_circle_distance(t_lat1, t_lon1, t_lat1, t_lon2);
	  //logg->write_time_info("t_lat1,t_lat2,t_lon1,tlon_2,l_lon: %f,%f,%f,%f,%f\n", t_lat1, t_lat2,t_lon1,t_lon2,l_lon);
	  //logg->write_time_info("x_dist(km): %f  y_dist(km): %f\n", x_dist, y_dist);
	  

	  // Get the amount of indices in x and y direction to look
	  int nx = floor(this->radar_radius / x_dist)+1;
	  //logg->write_time_info("Looking up and down %d y points\n", ny);
	  //logg->write_time_info("Looking left and right %d x points\n", nx);	  	    
	  
	  
	  // Look at values withen this recangle and test to see if their distance from the obs_point is less than the radius
	  for (int cy = y-ny; cy <= y+ny+1; cy++){
	    this->lat_var->set_cur(n, cy);
	    this->lat_var->get(&t_lat1,1,1);	    
	    for (int cx = x-nx; cx <= x+nx+1; cx++){
	      this->lon_var->set_cur(n, cx);
	      this->lon_var->get(&t_lon1,1,1);
	      
	      double dist = vdt_point::great_circle_distance(lat, lon, t_lat1, t_lon1);
	      if (dist <= this->radar_radius){
		//logg->write("%f,%f\n", t_lat1, t_lon1);
		ind = make_pair(cx, cy);
		x_y_inds.push_back(ind);
	      }
	      //logg->write_time_info("\tgridPoint(%d,%d) - %f,%f     DIST: %f\n", cy, cx, t_lat1, t_lon1, dist);
	    }
	  }

	  // Return the index of the specific radar
	  rad_ind = n;
	  return x_y_inds;
	}

    }

  return x_y_inds;
}

float vdt_dual_pol_tiled_radar_dataset::calc_mode(const vector<float> &v)
{
  vector< pair<int, int> > count_map;
  uint j;
  int rnd_val;
  for (uint z = 0; z < v.size(); z++)
    {
      rnd_val = (int)roundf(v[z]);
#ifdef DEBUG
      logg->write("Value: %d\n", rnd_val);
#endif      
      for (j = 0; j < count_map.size(); j++)
	{
	  
	  // Add the counts if you find a match
	  if (rnd_val == count_map[j].first){
	    count_map[j].second++;
	    break;
	  }
	}
      // Not found, add it
      if (j == count_map.size())
	{
	  count_map.push_back(make_pair(rnd_val, 1));
	}
    }
  int max_count = 0;
  float mode = vdt_const::FILL_VALUE;
  for (j = 0; j < count_map.size(); j++){
    if (count_map[j].second > max_count)
      {
	mode = count_map[j].first;	
	max_count = count_map[j].second;
      }
    // In case of a tie, check what has higher priority
    else if(count_map[j].second == max_count)
      {
	auto new_val_itr = DHR_priority_table.find(count_map[j].first);
	auto cur_val_itr = DHR_priority_table.find(mode);
	// If new val has a higher priority, use that
#ifdef DEBUG	
	logg->write("tie between %d(%d)(%d) and %f(%d)(%d)\n", count_map[j].first, count_map[j].second, new_val_itr->second, mode, max_count, cur_val_itr->second);
#endif	
	if (new_val_itr->second > cur_val_itr->second){
	  mode = count_map[j].first;
	}
      }
  }
  return mode;
}

float vdt_dual_pol_tiled_radar_dataset::calc_mean(const vector<float> &v)
{
  float sum = 0;
  for (uint j = 0; j < v.size(); j++)
    {
      sum = sum + v[j];
    }
  return (sum / v.size());
}
