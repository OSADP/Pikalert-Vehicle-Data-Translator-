// Include files
#include <string>
#include <vector>
#include <stdio.h>
#include <log/log.hh>

#include "site_vars_fcst_data.hh"

site_vars_fcst_data::site_vars_fcst_data(Log *L)
{
  log = L;
  errorString = "";
}

site_vars_fcst_data::~site_vars_fcst_data()
{
}

// This functions organizes the forecast data by site (segment)
// and stores all the site-vars-fcst data in this class's site_vars_data vector
//
int site_vars_fcst_data::get_fcst_data(time_t begin_time, const vector<site *> &sites, const vector< vector<string> > &field_names, const vector<ncfc_io *> &data, int max_days, int max_lead_time)
{
  //unsigned int i, s,v;
  size_t i,s,v;
  int d,t;
  int var_cnt;

  double rnd_begin_time = (double)( ((int)(begin_time/3600)) * 3600 ); 
  
  // Create ncfc_utils object so we can use this class's functions
  ncfc_utils = new NcfcUtils(log);
  
  for(s = 0; s < sites.size(); s++)
    {
      site_vars_fcst *this_site = new site_vars_fcst;
      this_site->icao_id = sites[s]->icao_id;      
      var_cnt = 0;
      
      for(i = 0; i < data.size(); i++)
	{  
	  int ft_var_ind = ncfc_utils->get_var_index(data[i], "forc_time");
	  double forc_time = ((double *)data[i]->var_ptrs[ft_var_ind])[0];  
	  double time_diff = rnd_begin_time - forc_time;
	  int first_hour = time_diff / 3600;
	  //printf("time_diff: %d, first_hour: %d\n", (int)time_diff, first_hour);
	  int day_start = 0;
	  int day_end = max_days;
	  if(first_hour > 47)
	    {
	     log->write_time("Error: first_hour: %d is greater than 47, skipping data.\n", first_hour);
	     continue;
	    }
	  else if(first_hour > 23)
	    {
	      //log->write_time("Warning: first_hour: %d is greater than 23, adjusting day_start.\n", first_hour);
	      first_hour = first_hour - 24;
	      day_start = 1;
	      day_end++;
	    }
	  //printf("day_start: %d, day_end: %d, first_hour: %d, max_lead_time: %d\n", day_start, day_end, first_hour, max_lead_time);
	  
	  int site_ind = data[i]->site_indicies[s];
	  //printf("sites[%d]->icao_id: %s, site_ind: %d\n", s, sites[s]->icao_id, site_ind);
	  
	  for(v = 0; v < field_names[i].size(); v++)
	    {
	      this_site->field_names.push_back(field_names[i][v]);
	      this_site->field_ind_map[field_names[i][v]] = var_cnt;
	      
	      int var_ind = ncfc_utils->get_var_index(data[i], field_names[i][v].c_str());
	      int num_days = data[i]->num_days[var_ind];
	      int num_times_per_day = data[i]->num_times_per_day[var_ind];
	      double time_step = data[i]->time_step[var_ind];
	      int num_params = data[i]->num_params[var_ind];
	      
	      //printf("var_cnt: %d, field_names[%d][%d]: %s, num_days: %d, num_times_per_day: %d, time_step: %d, num_params: %d\n", 
	      //	     var_cnt, i, v, field_names[i][v].c_str(), num_days, num_times_per_day, (int)time_step, num_params);
	      
	      if(num_days == MISSING)
		{
		  log->write_time("Warning: num_days is MISSING for field_names[%d][%d]: %s, skipping data.\n", i, v, field_names[i][v].c_str());
		  continue;
		}
	      
	      int hour_step = 24 / num_times_per_day;
	      int first_t = first_hour / hour_step;
	      
	      unordered_map<double, float> var_data_map;
	      int lead_time = 0;
	      
	      for(d = day_start; d < day_end; d++)
		{
		  for(t = 0; t < num_times_per_day; t++)
		    {
		      double valid_time = forc_time + (d * 86400) + (t * time_step);
		      
		      int data_ind = (site_ind * num_days * num_times_per_day * num_params) + (d * num_times_per_day * num_params) + (t * num_params);
		      float raw_value = ((float *)data[i]->var_ptrs[var_ind])[data_ind];
		      
		      float value;
		      if(raw_value != NC_FILL_FLOAT)  
			value = raw_value;
		      else
			value = MISSING;
		      
		      // Only save values that are between the first non-hindcast forecast hour (first_t) and max-lead-time.
		      //
		      if(d == day_start && t >= first_t && lead_time <= max_lead_time)
			{
			  var_data_map[valid_time] = value;
			  //printf("lead_time: %d\n", lead_time);
			  lead_time++;
			}
		      else if(d > day_start && lead_time <= max_lead_time)
			{
			  var_data_map[valid_time] = value;
			  //printf("lead_time: %d\n", lead_time);
			  lead_time++;
			}
		    } // end loop over times_per_day

		} // end loop over days
	  
	      // Store the data maps for each variable
	      this_site->data_maps.push_back(var_data_map);
	      var_cnt++;

	    } // end loop over variables
      
	} // end loop over all fcst data
      
      //Save all the data for this site
      site_vars_data.push_back(this_site);
      
    } // end loop over sites
  
  return 0;
}


float site_vars_fcst_data::get_value(double valid_time, string field_name, site_vars_fcst *site_vars_data)
{
  int var_ind;
  float value;
  
  if(site_vars_data->field_ind_map.find(field_name) != site_vars_data->field_ind_map.end())
    {
      var_ind = site_vars_data->field_ind_map[field_name];
      //printf("var_ind: %d\n", var_ind);
    }
  else
    {
      log->write_time(2, "Warning: can't find field_ind_map value for field_name: %s\n", field_name.c_str());
      return MISSING;
    }

  if(site_vars_data->data_maps[var_ind].find(valid_time) != site_vars_data->data_maps[var_ind].end())
    {
      value = site_vars_data->data_maps[var_ind][valid_time];
      //printf("value: %f\n", value);
      return value;
    }
  else
    {
      log->write_time(2, "Warning: can't find data_maps value for field_name: %s, valid_time: %d\n", field_name.c_str(), (int)valid_time);
      return MISSING;
    }
}
