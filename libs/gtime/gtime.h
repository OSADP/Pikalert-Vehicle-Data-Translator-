/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2015 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2015/02/02 20:26:11 */
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
#ifndef G_TIME_H
#define G_TIME_H

#include <sys/types.h>
#include <time.h>
    
#ifdef SUNOS5

struct tm *localtime_r(const time_t *clock, struct tm *res);

#endif

// Convert UTC time given in tm struct to UNIX time in UTC
// seconds. NOTE: the fields in *timeptr are not reset as is done in
// mktime and the const interface reflects that fact. Note that
// timeptr->tm_isdst is ignored.
time_t GT_mkgtime(const struct tm *timeptr);

// Reset tms to 0
void GT_cleartm(struct tm *tms);

// Convert ascii string to time structure. Do bounds checking. 
int32_t GT_atotm
(
 const char *string,
 struct tm *tms
);

#endif /* G_TIME_H */
