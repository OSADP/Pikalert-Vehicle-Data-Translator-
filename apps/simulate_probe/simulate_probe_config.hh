//==============================================================================
//
//   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file simulate_probe_config.hh
 *
 * @class Simulate_Probe_Config
 *
 *  <file/class description>
 *  
 * @date 12/16/14
 */

#ifndef SIMULATE_PROBE_CONFIG_HH
#define SIMULATE_PROBE_CONFIG_HH


/**
 * @class simulate_probe_config
 */
class simulate_probe_config
{
public:
  /** Constructor */
  simulate_probe_config();

  /** Destructor */
  virtual ~simulate_probe_config();

  /** @brief pressure a variable containing the pressure */
  float pressure;

private:

   simulate_probe_config & operator=(const simulate_probe_config &simulate_probe_config);
   simulate_probe_config(const simulate_probe_config &simulate_probe_config);
};


#endif /* SIMULATE_PROBE_CONFIG_HH */
