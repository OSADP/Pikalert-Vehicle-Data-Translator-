// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:26:11 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: test_gtime.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   8/30/99
 *
 * Description:
 *     Test the gtime module.
 */

// Include files 
#include <stdio.h>
#include "gtime.h"

// Constant, macro and type definitions 

// Global variables 

// Functions and objects


void test(char * date)
{
  struct tm tms;

  int ret =  GT_atotm(date, &tms);
  if (ret != 0)
    printf("Error: date %s is invalid\n", date);
  else
    printf("date: %s, year: %d, month: %d, day %d\n", date, tms.tm_year+1900, tms.tm_mon+1, tms.tm_mday);
}

int main(int argc, char **argv)
{


  test("20030227000000");
  test("20030228000000");
  test("20030229000000");

  test("20040100000000");
  test("20040101000000");
  test("20040131000000");
  test("20040132000000");
  
  test("20040200000000");
  test("20040201000000");
  test("20040227000000");
  test("20040228000000");
  test("20040229000000");

  test("20040300000000");
  test("20040301000000");
  test("20040330000000");
  test("20040331000000");
  test("20040332000000");

  test("20040400000000");
  test("20040401000000");
  test("20040430000000");
  test("20040431000000");
  test("20040432000000");

  test("20040500000000");
  test("20040501000000");
  test("20040530000000");
  test("20040531000000");
  test("20040532000000");
  
  test("20040600000000");
  test("20040601000000");
  test("20040630000000");
  test("20040631000000");
  test("20040632000000");
  
  test("20040700000000");
  test("20040701000000");
  test("20040730000000");
  test("20040731000000");
  test("20040732000000");
  
  test("20040800000000");
  test("20040801000000");
  test("20040830000000");
  test("20040831000000");
  test("20040832000000");
  
  test("20040900000000");
  test("20040901000000");
  test("20040930000000");
  test("20040931000000");
  test("20040932000000");
  
  test("20041000000000");
  test("20041001000000");
  test("20041030000000");
  test("20041031000000");
  test("20041032000000");
  
  test("20041100000000");
  test("20041101000000");
  test("20041130000000");
  test("20041131000000");
  test("20041132000000");
  
  test("20041200000000");
  test("20041201000000");
  test("20041230000000");
  test("20041231000000");
  test("20041232000000");
  
}
