// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:20:23 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/************************************************************************

Module:	gen_cubic_spline.cc

Author:	Dave Albo

Date:	Fri Jun 13 14:02:32 1997
        11/18/04 Borrowed by Jim Cowie (11/04) for use in DICAST. Changed
	function names, removed tetwws lib calls and added a bounds check
	for the x value.

Description: 
             Cubic Spline.
 	     See Numerical Recipes in C, Section 3.3.

************************************************************************/


/* System include files / Local include files */
#include <stdio.h>
#include <math.h>


/************************************************************************

Function Name: 	init_spline

Description:	Given a set of (x,y) data points, and the first derivative
                of the interpolating function at the endpoints, compute the
		2nd derivative of the interpolating function at each x 
		data point. X values must be increasing in the array.

Returns:	none

Globals:	none

Notes:	see Numerical Recipes in C, Section 3.3

************************************************************************/

void init_spline( double *x, /* I - x data values */
		  double *y, /* I - y data values */
		  int n,     /* I - number of elements in array	*/
		  double yp1,/* I - first derivate of interp. function
			      *     at point 0 */
		  double ypn,/* I - first derivate of interp. function
			      *     at point n-1 */
		  double *y2 /* O - 2nd deriv. of interp. function at
			      *     points 0 to n-1 */
		  )
{
    int i, k;
    double p, qn, sig, un, *u;

    if ( n <= 1 )
	return;

    u = new double[n];

    /*
     * The lower boundary condition is set to either 'natural' or
     * to have a specified first derivative
     */
    if (yp1 > 0.99e30)
	y2[0] = u[0] = 0.0;
    else
    {
	y2[0] = -0.5;
	u[0] = (3.0/(x[1]-x[0]))*((y[1] -y[0])/(x[1]-x[0])-yp1);
    }
    /*
     * This is the decomposition loop of the tridiagonal algorithm.
     * y2 and u are used for temporary storage of the decomposed factors 
     */
    for (i=1; i<n-1; ++i)
    {
	sig = (x[i] - x[i-1])/(x[i+1] - x[i-1]);
	p = sig*y2[i-1] + 2.0;
	y2[i] = (sig-1.0)/p;
	u[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
	u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
    }

    /*
     * Upper bound set either to be "natural" or else have a specified
     * first derivative
     */
    if (ypn > 0.99e30)
	qn = un = 0.0;
    else
    {
	qn = 0.5;
	un = (3.0/(x[n-1]-x[n-2]))*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
    }
    /*
     * Back substitution loop of tridiagonal algorithm.
     */
    y2[n-1] = (un-qn*u[n-2])/(qn*y2[n-2]+1.0);
    for (k=n-2; k>=0; k--)
	y2[k] = y2[k]*y2[k+1]+u[k];

    delete [] u;
}



/************************************************************************

Function Name: 	calc_splint

Description:	Given a set of (x,y) data points, and the 2nd derivative
		of the interpolating function at each x (as gotten from a
		call to NRspline), return the data value at the input 
		location x. If the x value is outside the defined x
		array bounds, a missing value is returned. This means
		the function does not extrapolate.

Returns:	the data value.

Globals:	none

Notes:	see Numerical Recipes in C, Section 3.3

************************************************************************/

double calc_splint( double *xa, /* I - x data values */
		    double *ya, /* I - y data values */
		    double *y2a,/* I - 2nd deriv. of interp. function at
				 *     points 0 to n-1 */
		    int n,      /* I - number of elements in array */
		    double x,    /* I - x data point */
		    double FILL_VALUE /* value to return on failure */
		    )
{
    int klo, khi, k;
    double h, b, a, y;

    // Check that x value falls within previously declared x array

    if (n <= 1 || x < xa[0] || x > xa[n-1])
    {
        return FILL_VALUE;
    }

    klo = 0;
    khi = n-1;
    while (khi-klo > 1)
    {
	k = ((khi+klo)/2);
	if (xa[k] > x)
	    khi = k;
	else
	    klo = k;
    }
    h = xa[khi] - xa[klo];
    if (h == 0.0)
    {
	return FILL_VALUE;
    }
    a = (xa[khi]-x)/h;
    b = (x-xa[klo])/h;
    y = a*ya[klo] +
	b*ya[khi] +
	((a*a*a-a)*y2a[klo] + (b*b*b-b)*y2a[khi])*(h*h)/6.0;

    return y;
}

