// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: layer_info.hh
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Modified by: Julie Prestopnik
 *
 * Date: 03/15/07
 * 
 * Description: This file contains the definition of a structure
 *              that describes the subsurface structure of a roadway.
 *              Each layer is made up of a different material and has
 *              a thickness. For example, 6" of asphalt. These layers 
 *              are the points where the temperature is estimates by
 *              METRo. 
 *
 */

#include <string>

const int MAX_LAYER = 10;

/*
 * METRo workaround:
 * Cement conductivity is higher than for asphalt and needs a higher 
 * space resolution. For a thin layer of cement, METRo does not have
 * enough resolution.  To have a numerically stable solution, we 
 * need to have a thickness of at least MIN_CEMENT_THICKNESS  for cement.
 */
const float MIN_CEMENT_THICKNESS = .2750;

struct layer_info {
  int num_layer;
  string tz_code;
  int layer_mat[MAX_LAYER]; // Mat type of each layer (deepest first)
  float layer_thickness[MAX_LAYER]; // Thickness of each layer
};
typedef struct layer_info LayerInfo;
