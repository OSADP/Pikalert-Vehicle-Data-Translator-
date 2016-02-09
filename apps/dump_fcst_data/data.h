/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2015 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2015/02/02 20:21:07 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
#define FILL_VAL 34028234663852885981170418348451692.00
#define MISSING -9999

// Max and Min air temperature values (in degrees Celsius)
#define MAX_AIR_T 45.00
#define MIN_AIR_T -45.00

// Max and Min road temperature values (in degrees Celsius)
#define MAX_ROAD_T 70.00
#define MIN_ROAD_T -20.00
#define MAX_SUBSURFACE_T 40.00
#define MIN_SUBSURFACE_T -10.00

static const int MAX_ERROR = 50;
static const int MIN_ERROR = -50;

static const int MAX_DAYS = 16;
static const int MAX_FCST_TIMES_PER_DAY = 24;
static const int DAILY_TIME = 1;
static const int MAX_NUM_OBS_PER_DAY = 24;
static const int NUM_MESO_FILES_PER_DAY = 24;
static const int MAX_STRING_LEN = 200;


struct ls_struct
{
  int site_id;
  std::vector<char *> variables;
};
typedef struct ls_struct ls;


struct site_struct
{
  int dicast_id;
  char *icao_id;
  float lat;
  float lon;
};
typedef struct site_struct site;


struct fcst_struct
{
  int is_valid;
  int var_is_valid;
  int diff;
  int days;
  int fc_times_per_day;
  int daily_time;
  int max_node;
  double creation_time;
  double forc_time;
  int num_sites;
  int *site_list;
  char **units;
  int num_dim;
  long *dim;
  float *var_ptrs;
};
typedef struct fcst_struct fs;


struct obs_struct
{
  int is_valid;
  int var_is_valid;
  int times_per_day;
  int daily_time;
  int num_sites;
  int *site_list;
  long num_obs;
  int stnid_len;
  char *station_ids;
  double *obs_times;
  char **units;
  int num_dim;
  long *dim;
  float *var_ptrs;
};
typedef struct obs_struct os; 
