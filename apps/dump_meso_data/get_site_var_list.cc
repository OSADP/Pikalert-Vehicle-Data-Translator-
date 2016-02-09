// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:20 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: get_site_var_list.cc
 *
 * Author: Seth Linden
 *
 * Date:   10/13/03
 *
 * Description: fills a vector with structs that contain specific variables per site.
 *              The site and variable information is read in from the site_var_file.
 */

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <vector>
#include "data.h"
using namespace std;

#define MAXLEN 256

int get_site_var_list(vector<ls *> &site_var_list, char *fname)
{
  int i, j, k;

  int start, end;
  int num_chars;

  FILE *fp;
  char line[MAXLEN];
  char *str;
  
  char var_name[MAXLEN];
  int site;
  
  fp = fopen(fname, "r");
 
  if (fp == NULL)
     return(1);
  
  while (fgets(&line[0], MAXLEN, fp) != NULL)
    {
      
      if (line[0] == '#' || strlen(line) < 2)
        continue;
      
      ls *site_var_data = new ls;
      
      // get all variables per line (per site)
      
      j = strlen(line) - 1;
      end = j;
      
      while(j >= 0){
	
	if(line[j] == ';'){	  
	  start = j + 1;
	  num_chars = end - start;
	  
	  strncpy(var_name, &line[start], num_chars);
	  var_name[num_chars] = '\0';
	  
	  str = strdup(var_name);
	  site_var_data->variables.push_back(str);
	  
	  end = j;
	}
	
	j--;
      }
      
      // get the site
      site = atoi(&line[0]);
      site_var_data->site_id = site;
      
      site_var_list.push_back(site_var_data);
      
    }
  
  fclose(fp);
  return (0);
}

