//==============================================================================
//
//   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file wiper_status.hh
 *
 * @class Wiper_status
 *
 *  <file/class description>
 *  
 * @date 2/27/15
 */

#ifndef WIPER_STATUS_HH
#define WIPER_STATUS_HH


class Wiper_status
{
public:
  enum Wiper_status_values {WIPER_STATUS_MISSING =-9999, WIPER_STATUS_LOW_THRESHOLD=-999, WIPER_NOT_EQUIPPED=0, WIPER_STATUS_OFF=1, WIPER_STATUS_INTERMITTENT=2, WIPER_STATUS_LOW=3, WIPER_STATUS_HIGH=4, WIPER_STATUS_WASHER=5};

  Wiper_status(int off_arg, int intermittent_arg, int low_arg, int high_arg) : off(off_arg), intermittent(intermittent_arg), low(low_arg), high(high_arg)
  {
  }
  
  Wiper_status_values find_wiper_status(float reflectivity) const
  {
    if (reflectivity <= WIPER_STATUS_LOW_THRESHOLD || reflectivity == WIPER_STATUS_MISSING)
      {
	return WIPER_STATUS_MISSING;
      }
    else if (reflectivity < off)
      {
	return WIPER_STATUS_OFF;
      }
    else if (off <= reflectivity && reflectivity < intermittent)
      {
	return WIPER_STATUS_INTERMITTENT;
      }
    else if (intermittent <= reflectivity && reflectivity < low)
      {
	return WIPER_STATUS_LOW;
      }
    else
      {
	return WIPER_STATUS_HIGH;
      }
  }

private:
  int off;
  int intermittent;
  int low;
  int high;
};


#endif /* WIPER_STATUS_HH */
