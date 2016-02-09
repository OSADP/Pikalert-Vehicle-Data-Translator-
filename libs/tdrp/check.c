/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2015 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2015/02/02 20:35:09 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */

/**********************************************************
 * check.c
 *
 * TDRP check functions.
 *
 * Mike Dixon, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307, USA
 *
 * May 1997
 */

#include "tdrp.h"
#include <string.h>

/***********************************************************
 * tdrpCheckAllSet()
 *
 * Return TRUE if all set, FALSE if not.
 *
 * If out is non-NULL, prints out warning messages for those
 * parameters which are not set.
 */

int tdrpCheckAllSet(FILE *out, const TDRPtable *table, const void *params)
{
  int ret = TRUE;
  while (table->param_name != NULL) {
    if (table->ptype != COMMENT_TYPE &&
	!table->is_private &&
	!table->is_set) {

      /*
       * Exception - empty arrays, 2D and 1D - are OK - Niles Oien
       */

      if (
	  ((table->is_array2D) && (table->array_n1 == 0) && (table->array_n2 == 0)) ||
	  ((table->is_array)   && (table->array_n == 0))
	  ){
	table++;
	continue;
      }

      if (out) {
	fprintf(out,
		"TDRP_WARNING: parameter not set, using default - '%s'\n",
		table->param_name);
      }
      ret = FALSE;
    }
    table++;
  }
  return (ret);
}

/***********************************************************
 * tdrpCheckIsSet()
 *
 * Return TRUE if parameter is set, FALSE if not.
 *
 */

int tdrpCheckIsSet(const char *paramName, const TDRPtable *table, const void *params)
{
  int ret;

  while ( strcmp(table->param_name, paramName)) {
    table++;
  }

  if (table->ptype != COMMENT_TYPE && !table->is_private) {
    ret = table->is_set;
  }
  else {
    ret = FALSE;
  }
  return (ret);
}
