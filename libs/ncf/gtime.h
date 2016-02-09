/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2015 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2015/02/02 20:32:06 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/*
 *   Module: gtime.h
 *
 *   Author: Gerry Wiener
 *
 *   Date:   7/29/97
 *
 *   Description:
 */
#ifndef GTIME_H
#define GTIME_H

#include <time.h>

#ifdef  __cplusplus
extern "C" {
#endif

extern struct tm *localtime_r(const time_t *, struct tm *);
time_t NCF_mkgtime(const struct tm *timeptr);

#ifdef  __cplusplus
}
#endif


#endif /* GTIME_H */
