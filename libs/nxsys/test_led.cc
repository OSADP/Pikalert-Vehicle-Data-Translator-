// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:33:48 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: test_check_led.cc
 *
 * Author: Jim Cowie
 *
 * Date:   7/24/02
 *
 * Description: Tests the nxsys "led" library routines
 *     
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "led.h"


int main(int argc, char **argv)
{
  if (argc == 2 && strncmp(argv[1], "-h", 2) == 0) {
    printf("Usage: %s [-h]\n",argv[0]);
    printf("\nOptions:\n");
    printf("-h     this message\n");
    printf("\nThis test program calls check_led() to test the expiration function.\n");
    printf("An expiration time is built into the binary from the file led.h. \n");
    printf("\nNote that if the license has expired, this binary will be removed.\n");
    exit(0);
  }

  int ret;

  ret = get_led();
  if (ret==0)
    printf("Program not expired.\n");
  else if (ret==-1)
    printf("Program expired\n");
  else
    printf("Program will expire in %d days\n", ret);


#ifdef TEST_CHECK_LED
  ret = check_led(argv[0], led);
  if (ret)
    printf("Program expired.\n");
  else
    printf("Program not expired.\n");
#endif
}
