// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "../include/site_list/site_list.hh"

Site_list::Site_list(void)
{
  init();
}

Site_list::Site_list(char *input_file)
{
  const int MAX_LINE = 256;
  int count = 0;
  char in_line[MAX_LINE];
  struct site_loc *loc;
  char *ret_ptr;
  int ret;
  

  init();
  FILE *site_file = fopen(input_file, "r");

  if (!site_file) {
    err_string ="Unable to open site_list_file.\n";
    return;
  }

  memset(in_line, 0, MAX_LINE); // valgrind likes this
  while( fgets( in_line, MAX_LINE, site_file ) != NULL ) {

    count++;

    //
    // Skip short or comment lines
    //
    if( strlen(in_line) <= 10 || in_line[0] == '#') {
      continue;
    }

    loc = new site_loc;
    if(!loc) {
      err_string = "Allocation error in creating site list.\n";
      fclose(site_file);
      return;
    }
    
    char temp[ICAO_LEN];
    memset(temp, 0, ICAO_LEN); // valgrind likes this
    ret = sscanf(in_line,"%d;%d;%10[^;];%f;%f;%f;%d;%[^;];%[^;];%[^\n]",
     		&(loc->id), &(loc->wmo_id), temp, &(loc->lat),
		&(loc->lon), &(loc->alt), &(loc->wmo_region), loc->long_name,
		loc->state, loc->country_name);
    
    temp[ICAO_LEN-1] = '\0';

    //printf("id %d  wmo_id %d icao %s ret %d\n", loc->id, loc->wmo_id, temp, ret);
    if (ret != 10) {  // Try old-style site list format
      ret = sscanf(in_line,"%d%f%f%f%*[\t]%[^\n]", &(loc->id), &(loc->lat), &(loc->lon), 
		 &(loc->alt), loc->long_name);
      if (ret != 5) {
        char *estring = new char[MAX_LINE];
        sprintf(estring,"Problem reading line %d from %s\n",count,input_file);
        err_string = estring;
	fclose(site_file);
        return;
      }
    }

    // Remove any whitspace from beginning of string
    int i = 0;
    while (i < ICAO_LEN && temp[i] == ' ')
      i++;

    if (i >= ICAO_LEN)
      {
        char *estring = new char[MAX_LINE];
	sprintf(estring, "Problem reading icao id from line %d. id: %s\n", count, temp);
	err_string = estring;
	fclose(site_file);
	return;
      }

    strncpy(loc->icao_id, &(temp[i]), ICAO_LEN-1);

    site_list.push_back(loc);

  }

  fclose(site_file);

  if (site_list.size() == 0) {
    err_string = "No sites in file\n";
    return;
  }

  num_sites = site_list.size();
}

Site_list::~Site_list()
{
  vector <site_loc*>::iterator it;

  for (it = site_list.begin(); it != site_list.end(); it++)
    delete *it;
}

void Site_list::init()
{
  num_sites = 0;
  err_string = 0;
}

int Site_list::make_id_array(int **id_array)
{
  long array_len;
  long i;
  int *code_array;

  array_len = site_list.size();
  code_array = new int[array_len];
  
  for (i=0; i<array_len; i++)
    code_array[i] = site_list[i]->id;
  
  *id_array = code_array;

  return (site_list.size());
}

int Site_list::make_latlon_array(float **ll_array)
{
  long array_len;
  long i;
  float *latlon_arr;

  array_len = site_list.size() * 2;
  latlon_arr = new float[array_len];
  for (i=0; i< (int) site_list.size(); i++) {
    latlon_arr[i*2] = site_list[i]->lat;
    latlon_arr[i*2+1] = site_list[i]->lon;
  }

  *ll_array = latlon_arr;
  return(site_list.size());
}

int Site_list::make_alt_array(float **alt_array)
{
  long array_len;
  long i;
  float *altitudes = 0;

  array_len = site_list.size();
  altitudes = new float[array_len];
  for (i=0; i<array_len; i++)
    altitudes[i] = site_list[i]->alt;
  *alt_array = altitudes;
  return (array_len);
}

int Site_list::make_icao_array(string **icao_array)
{
  long array_len = site_list.size();
  long i;
  string *ids = new string[array_len];

  for (i=0; i<array_len; i++)
    ids[i] = site_list[i]->icao_id;
  *icao_array = ids;
  return (array_len);
}

void Site_list::print() {
  vector <struct site_loc*>::iterator it;
  
  for (it = site_list.begin(); it != site_list.end(); it++) {
    cout << (*it)->id << ": " << (*it)->lat << "  " << (*it)->lon;
    cout << "  " << (*it)->alt << "  " << (*it)->long_name << "\n";
  }
}

int Site_list::site(int site_offset, site_loc *site_info)
{
  if ((site_offset >= 0) && (site_offset < (int) site_list.size())) {
    *site_info = *site_list[site_offset];
    return(0);
  }
  else
    return(-1);
}
