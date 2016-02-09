/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2015 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2015/02/02 20:34:09 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/*
 * Header: led.h
 *
 * Author: Jim Cowie
 *
 * Date:   7/24/02
 *
 * Description: General header file for led
 *     
 */

#ifndef LED_H
#define LED_H

//
// check_led():
//
//   progname = name of the program
//   led      = license expiration date (unix time)
//
//   Returns: 0 = not expired
//            1 = binary expired
//
//   Note: if the binary has expired it is removed
//
int check_led(char *progname, double led);


//
// get_led():
//
//   Returns: 0 = no expiration or before grace period
//           -1 = binary expired
//            N = binary in grace period with N days left
//
//   Note: this routine does not remove binary.
//
int get_led();

//
// Variables for checking the expiration date of a binary.
// 'led' is the number of days past 00Z Jan 1, 1970, 'gp' is
// the grace period, in days the binary can still be used.
//
const double led = 0;
const int gp = 90;


#endif /* LED_H */
