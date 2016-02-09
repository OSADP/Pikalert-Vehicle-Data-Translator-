#ifndef StatUtils_H
#define StatUtils_H

#define MISSING -9999

#include <vector>
#include <site_list/site_list.hh>
#include "NcfcUtils.hh"

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
  int get_stn_name_ind(ncfc_io *data, char *stn_name); // for mtr_proc
  int get_StationName_ind(ncfc_io *data, char *stn_name); // for wind-interp files
  int get_stn_id_ind(ncfc_io *data, char *stn_id);     // for meso_proc
  int get_site_id_ind(ncfc_io *data, char *site_id);
  int get_wmoId_ind(ncfc_io *data, int wmo_id); // for syn_proc
  
  int is_dir(char *dir_path);
  int get_dir_ent(char *dir_path, vector<char *>* entries);
  
  time_t date2utime(char *date);
  void utime2date(time_t utime, char** date);
  void utime2year(time_t utime, char** year);
  void utime2month(time_t utime, char** month);
  void utime2day(time_t utime, char** day);
  void utime2hour(time_t utime, char** hour);
  void utime2minute(time_t utime, char** minute);
  void utime2hhmm(time_t utime, char** hhmm);
  void utime2FileDate(time_t utime, char** date);
  
private:

  // Copy of the log handle
  Log *log;
  
};

#endif // StatUtils_H
