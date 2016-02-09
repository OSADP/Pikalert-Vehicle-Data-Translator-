//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: site_info.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2013/12/16 21:45:49 $
//
//==============================================================================

/**
 * @file site_info.hh
 *
 * @class site_info
 *
 *  <file/class description>
 *  
 * @date 11/13/13
 */

#ifndef SITE_INFO_HH
#define SITE_INFO_HH

#include <string>
#include <map>
#include <vector>

class site_fields
{
public:
  int id;
  int wmo_id;
  std::string aux_id_string;
  double lat;
  double lon;
  float altitude;
  int wmo_region;
  std::string long_name;
  std::string state;
  std::string country;
};

/**
 * @class site_info
 */
class site_info
{
public:
  /** Constructor */
  site_info(const std::string &file_name);

  /** Destructor */
  virtual ~site_info();

  std::map<std::string, site_fields> fcst_site_map;

private:

   site_info & operator=(const site_info &site_info);
   site_info(const site_info &site_info);
};

#endif /* SITE_INFO_HH */
