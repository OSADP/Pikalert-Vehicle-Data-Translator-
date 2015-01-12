// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#ifndef SITE_LIST_HH
#define SITE_LIST_HH

#include <string>
#include <vector>
using namespace std;

namespace
{
  const int LONG_NAME_LEN = 150;
  const int ICAO_LEN = 12; // This will handle actual icao-id's with up to 10 characters
};

struct site_loc {
  int id;
  int wmo_id;
  char icao_id[ICAO_LEN];
  float lat;
  float lon;
  float alt;
  int wmo_region;
  char long_name[LONG_NAME_LEN];
  char state[3];
  char country_name[LONG_NAME_LEN];
};

class Site_list {
 public:

  /* constructors, initialization, destructors */

  Site_list(void);
  Site_list(char *input_file);                // uses ascii site_list file in constructor
  ~Site_list();
  void init();

  /* routines */

  long get_num_sites() {return num_sites;};

  /* Memory for array is allocated in this function using 'new[]' and should
     be deleted by the calling function. The number of sites is returned. */
  int make_id_array(int **id_array);

  /* Memory for array is allocated in this function. The number of sites is
     returned. The length of the resulting array is 2*number_of_sites. For
     each site, the first coordinate is the lat and the second coordinate is
     the lon. */
  int make_latlon_array(float **ll_array);

  int make_icao_array(string **icao_array);

  /* Memory for array is allocated in this function using 'new[]'. The number 
     of sites is returned. */
  int make_alt_array(float **alt_array);
						  
  void print();
  const char *error() {return err_string;};

  /* Gets information from vector element at site_offset. Places information
     into site_info. Returns 0 if successful, -1 if not. */
  int site(int site_offset, site_loc *site_info);  

 private:

  vector <site_loc*> site_list;                // vector of pointers to struct site_loc
  long num_sites;                              // size of vector
  const char *err_string;                            
};

#endif
