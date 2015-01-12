// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: finite.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   12/1/99
 *
 * Description:
 *     Reset non-finite array values to a given fill_value
 */

// Include files 
#if defined(SUNOS4) || defined(SUNOS5) || defined(SUNOS5_ETG) 
#include <ieeefp.h>
#endif

#include <math.h>
#include <ncf/ncf.hh>
using namespace std;

// Constant, macro and type definitions 

// Global variables 

// Functions and objects


// Reset all non-finite values in array to fill_value
int NCF_finite(float *array, int len, float fill_value)
{
  int i;
  int ret = 0;

  for (i=0; i<len; i++)
    {
      if (!finite(array[i]))
	{
	  array[i] = fill_value;
	  ret = 1;
	}
    }

  return(ret);
}
#ifdef MAIN  
main(int argc, char **argv)
{
  double d;
  float x;
  float array[1];
  int ret = 0;

  x = pow(2.0, 1000);

  if (finite(x))
    printf("%f is finite\n", x);
  else
    printf("%f is infinite\n", x);

  array[0] = x;
  ret = NCF_finite(array, 1, 10);
  printf("ret %d\n", ret);
  printf("array[0] is %f\n", array[0]);

  array[0] = 5;
  ret = NCF_finite(array, 1, 10);
  printf("ret %d\n", ret);
  printf("array[0] is %f\n", array[0]);
}
#endif
