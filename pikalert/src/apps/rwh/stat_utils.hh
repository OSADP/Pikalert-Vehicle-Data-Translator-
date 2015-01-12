// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#ifndef StatUtils_H
#define StatUtils_H

#define MISSING -9999
#define LARGE_FILL_VAL 999999999

#include <vector>
#include <site_list/site_list.hh>
#include "ncfc_utils.hh"

using namespace std;

static const int MAX_STR_LEN = 200;

struct site_struct
{
  int dicast_id;
  int wmo_id;
  char *icao_id;
  float lat;
  float lon;
  float elev;
  char *long_name;
};
typedef struct site_struct site;

class StatUtils
{

public:

  // constructor
  StatUtils(Log *plog);
  
  // destructor
  ~StatUtils();
  
  int get_site_info(char *fname, vector<site *>* sites);
  
  int get_site_list_ind(ncfc_io *data, int dicast_id); // for int_obs
  int get_stationId_ind(ncfc_io *data, char *stn_id); // for mesonet (madis) files
  
  int is_dir(char *dir_path);
  int get_dir_ent(char *dir_path, vector<char *>* entries);
  
  time_t date2utime(char *date);
  time_t dateTime2utime(char *datetime);
  time_t julianDate2utime(char *year_str, int jday);
  void utime2date(time_t utime, char** date);
  void utime2year(time_t utime, char** year);
  void utime2month(time_t utime, char** month);
  void utime2day(time_t utime, char** day);
  void utime2hour(time_t utime, char** hour);
  void utime2minute(time_t utime, char** minute);
  void utime2hhmm(time_t utime, char** hhmm);
  void utime2FileDate(time_t utime, char** date);
  int calc_timezone_offset(float lat, float lon);
  
private:

  // Copy of the log handle
  Log *log;
  
};

#endif // StatUtils_H
