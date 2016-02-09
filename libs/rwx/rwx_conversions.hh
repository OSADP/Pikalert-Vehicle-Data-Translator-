//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: rwx_conversions.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/25 17:24:39 $
//
//==============================================================================

/**
 *
 * @file rwx_conversions.hh
 *
 * @class rwx_conversions
 *
 *  rwx conversions class
 *
 */

#ifndef RWX_CONVERSIONS_HH
#define RWX_CONVERSIONS_HH

/**
 * @class rwx_conversions
 */
class rwx_conversions
{
public:

  /** @brief miles per hour to meters per second conversion value */
  const static float MPH_TO_MS;

  /** @brief knots to meters per second conversion value */
  const static float KNOT_TO_MS;

  /** @brief miles to kilometers conversion value */
  const static float MI_TO_KM;

  /** @brief inches to millimeters conversion value */
  const static float IN_TO_MM;

  /** @brief kelvin to celsius conversion value */
  const static float K_TO_C;

  /** @brief pascal to millbars conversion value */
  const static float PA_TO_MB;

  /** @brief fill value */
  const static float FILL_VALUE;

  /** @brief maximum millibars */
  const static float MAX_MB;

  /** @brief minimum millibars */
  const static float MIN_MB;

  /**
   * @brief converts miles per hour to meters per second
   * @param[in] value value in miles per hour
   * @return value in meters per second
   */
  inline static float mph_to_ms(float value)
  {
    return value * MPH_TO_MS;
  }

  /**
   * @brief converts knots to meters per second
   * @param[in] v value in knots
   * @return value in meters per second
   */
  inline static float knot_to_ms(float value)
  {
    return value * KNOT_TO_MS;
  }
    

  /**
   * @brief converts miles to kilometers
   * @param[in] value value in miles
   * @return value in kilometers
   */
  inline static float mi_to_km(float value)
  {
    return value * MI_TO_KM;
  }

  /**
   * @brief converts inches to millimeters
   * @param[in] v value in inches
   * @return value in millimeters
   */
  static float in_to_mm(float value)
  {
    return value * IN_TO_MM;
  }

  /**
   * @brief converts kelvin to celsius
   * @param[in] v value in kelvin
   * @return value in celsius
   */
  inline static float k_to_c(float value)
  {
    return value - K_TO_C;
  }

  /**
   * @brief converts celsius to kelvin
   * @param[in] v value celsius
   * @return value in kelvin
   */
  inline static float c_to_k(float value)
  {
    return value + K_TO_C;
  }

  /**
   * @brief converts pascal to millbars
   * @param[in] v value in pascal
   * @return value in millibars
   */
  inline static float pa_to_mb(float v)
  {
    return v * PA_TO_MB;
  }
};

#endif
