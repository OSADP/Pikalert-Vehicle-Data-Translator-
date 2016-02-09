// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:18:48 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415916
#define EARTH_RADIUS (6371.)
#define MAX_Y_RANGE (0.08)

/* rotation matrices */
float R1[3][3], R2[3][3], R3[3][3], R[3][3];
float R1inv[3][3], R2inv[3][3], R3inv[3][3], Rinv[3][3];

/* maximum lat and lon in rotated coordinate system */
float max_lon, max_lat;
float delta_lon;

/* NOTE : nx and ny are interchanged in parameter passing as this
   routine rotates the route to an equatorial route going W to E.
   Meanwhile the output grid goes from y=0 up.
   */

void mat_mult(float A[3][3], float x[3], float b[3])
{
  int i, j;

  for (j=0; j<3; j++)
    {
      b[j] = 0.;
      for (i=0; i<3; i++)
	b[j] += A[j][i] * x[i];
    }

  return;
}

float init_grid(float start_lon, float start_lat, float end_lon, float end_lat)
{
  int i, j;
  float theta0, phi0;
  float theta1, phi1;
  float x0[3], x1[3], x2[3], x3[3];
  float y0[3], y1[3], y2[3], y3[3];
  float a0[3], a1[3], a2[3], a3[3];
  float alpha;
  float lat_angle, lon_angle;
  float dist;

  double ilat, ilon;
  double idx_x, idx_y;
  float  xidx, yidx;

  theta0 = start_lon * PI / 180.;
  phi0 = start_lat * PI / 180.;

  theta1 = end_lon * PI / 180.;
  phi1 = end_lat * PI / 180.;

  x0[0] = cos(phi0) * cos(theta0);
  x0[1] = cos(phi0) * sin(theta0);
  x0[2] = sin(phi0);

  y0[0] = cos(phi1) * cos(theta1);
  y0[1] = cos(phi1) * sin(theta1);
  y0[2] = sin(phi1);

  
  R1[0][0] = cos(-theta0);   R1[0][1] = -sin(-theta0);   R1[0][2] = 0.;
  R1[1][0] = sin(-theta0);   R1[1][1] = cos(-theta0);    R1[1][2] = 0.;
  R1[2][0] = 0.;             R1[2][1] = 0.;              R1[2][2] = 1.;

  R1inv[0][0] = cos(theta0); R1inv[0][1] = -sin(theta0); R1inv[0][2] = 0.;
  R1inv[1][0] = sin(theta0); R1inv[1][1] = cos(theta0);  R1inv[1][2] = 0.;
  R1inv[2][0] = 0.;          R1inv[2][1] = 0.;           R1inv[2][2] = 1.;

  mat_mult(R1, x0, x1);
  mat_mult(R1, y0, y1);

  /*
    printf("theta0 %f %f\n", start_lon, theta0);
    printf("phi0 %f %f\n", start_lat, phi0);
    printf("theta0 %f %f\n", end_lon, theta1);
    printf("phi0 %f %f\n", end_lat, phi1);

    printf("in  %f %f %f\n", x0[0], x0[1], x0[2]);
    printf("out %f %f %f\n", x1[0], x1[1], x1[2]);
    printf("out %f %f %f\n", y1[0], y1[1], y1[2]);
  */

  R2[0][0] = cos(-phi0);  R2[0][1] = 0.;   R2[0][2] = -sin(-phi0);
  R2[1][0] = 0.;          R2[1][1] = 1.;   R2[1][2] = 0.;
  R2[2][0] = sin(-phi0);  R2[2][1] = 0.;   R2[2][2] = cos(-phi0);

  R2inv[0][0] = cos(phi0);  R2inv[0][1] = 0.;   R2inv[0][2] = -sin(phi0);
  R2inv[1][0] = 0.;         R2inv[1][1] = 1.;   R2inv[1][2] = 0.;
  R2inv[2][0] = sin(phi0);  R2inv[2][1] = 0.;   R2inv[2][2] = cos(phi0);

  mat_mult(R2, x1, x2);
  mat_mult(R2, y1, y2);
  /*
    printf("out %f %f %f\n", x2[0], x2[1], x2[2]);
    printf("out %f %f %f\n", y2[0], y2[1], y2[2]);
  */

  alpha = (float)atan2(-y2[2], y2[1]);
  /* printf("alpha %f\n", alpha);*/

  R3[0][0] = 1.;  R3[0][1] = 0.;          R3[0][2] = 0.;
  R3[1][0] = 0.;  R3[1][1] = cos(alpha);  R3[1][2] = -sin(alpha);
  R3[2][0] = 0.;  R3[2][1] = sin(alpha);  R3[2][2] = cos(alpha);

  R3inv[0][0] = 1.;  R3inv[0][1] = 0.;           R3inv[0][2] = 0.;
  R3inv[1][0] = 0.;  R3inv[1][1] = cos(-alpha);  R3inv[1][2] = -sin(-alpha);
  R3inv[2][0] = 0.;  R3inv[2][1] = sin(-alpha);  R3inv[2][2] = cos(-alpha);

  mat_mult(R3, x2, x3);
  mat_mult(R3, y2, y3);
  /*
    printf("out %f %f %f\n", x3[0], x3[1], x3[2]);
    printf("out %f %f %f\n", y3[0], y3[1], y3[2]);
  */

  delta_lon = (float)atan2(y3[1], y3[0]);
  if (delta_lon < 0.)
    {
      printf("got negative lon %f, don't know if we can handle it\n");
      exit(0);
    }
  max_lon = delta_lon;

  dist = EARTH_RADIUS * delta_lon;
  /*
    printf("delta_lon %f = %f deg    dist %f\n", delta_lon, delta_lon * 180./ PI,
    dist);
  */

  return(dist);
}




