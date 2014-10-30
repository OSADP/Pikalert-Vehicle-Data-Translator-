/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2014 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2014/10/20 13:00:00 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */

// struc double
struct doubleStruct
{
  double* pdArray;
  long nSize;
};

// struc float
struct floatStruct
{
  float* pfArray;
  long nSize;
};

// struc long
struct longStruct
{
  long* plArray;
  long nSize;
};

// struc short
struct shortStruct
{
  short* pnArray;
  long nSize;
};

// struc int
struct intStruct
{
  int* pnArray;
  long nSize;
};

// double
void PrintDoubleArray(double* pdArray, long nSize);
double* GiveMeADoublePointer();
struct doubleStruct GiveMeADoubleStruct();
// float
float* GiveMeAFloatPointer();
struct floatStruct GiveMeAFloatStruct();
// long
void PrintLongArray(long* plArray, long nSize);
long* GiveMeALongPointer();
struct longStruct GiveMeALongStruct();
// short
short* GiveMeAShortPointer();
struct shortStruct GiveMeAShortStruct();
// int
extern int* GiveMeAIntPointer();
extern struct intStruct GiveMeAIntStruct();
