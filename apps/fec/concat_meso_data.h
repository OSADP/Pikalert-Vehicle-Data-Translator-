/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2015 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2015/02/02 20:21:52 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
#define FILL_VAL 34028234663852885981170418348451692.00
#define MISSING -9999

// Max and Min air temperature values (in degrees Celsius)
#define MAX_T 50.00
#define MIN_T -50.00

// Max and Min road temperature values (in degrees Celsius)
#define MAX_ROAD_T 70.00
#define MIN_ROAD_T -60.00

static const int MAX_ERROR = 50;
static const int MIN_ERROR = -50;

static const int MAX_STRING_LEN = 256;

struct site_struct
{
  int dicast_id;
  char *icao_id;
  float lat;
  float lon;
  char *long_name;
};
typedef struct site_struct site;

struct data_struct
{
  double *time_nominal;
  double creation_time;
  double obs_time;
  int    num_obs;
  int    num_sites;
  int    *dicast_id;
  int    type;
  float  *T;
  float  *dewpt;
  float  *wind_speed;
  int    *Precip;
  float  *road_T;
  float  *bridge_T;
  int    *road_condition;
  float  *subsurface_T;
  float  *T_lbls0;
  float  *T_lbls1;
  int    is_valid;
};
typedef struct data_struct ds;
