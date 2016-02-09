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
 * override.c
 *
 * TDRP override functions.
 *
 * Mike Dixon, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307, USA
 *
 * May 1997
 */

#include "tdrp.h"
#include <string.h>

/********************
 * tdrpInitOverride()
 *
 * Initialize the override list
 */

void tdrpInitOverride(tdrp_override_t *override)
{
  override->n = 0;
  (override->list) =
    (char **) tdrpMalloc ((override->n + 1) * sizeof(char *));
  (override->list)[override->n] = (char *) NULL;
}

/*******************
 * tdprAddOverride()
 *
 * Add a string to the override list
 */

void tdrpAddOverride(tdrp_override_t *override, const char *override_str)
{
  
  override->list[override->n] =
    (char *) tdrpMalloc(strlen(override_str) + 1); 
  strcpy(override->list[override->n], override_str);
    
  override->n++;
  override->list =
    (char **)tdrpRealloc((char *)override->list,
			 (override->n + 1) * sizeof(char *));
  override->list[override->n] = (char *) NULL;
}

/********************
 * tdrpFreeOverride()
 *
 * Free up the override list.
 */

void tdrpFreeOverride(tdrp_override_t *override)
{
  
  int i;

  for (i = 0; i < override->n; i++) {
    tdrpFree ((void *) override->list[i]);
  }
  tdrpFree ((void *) override->list);
  override->n = 0;
}

