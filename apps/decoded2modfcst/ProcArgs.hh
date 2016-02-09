/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Copyright UCAR (c) 1992 - 2007
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization.
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

//----------------------------------------------------------------------
//   Module: ProcArgs.hh
//
//   Author: Seth Linden
//
//   Date:   3/27/09
//
//   Description: 
//----------------------------------------------------------------------

#ifndef PROCARGS_HH
#define PROCARGS_HH

#include <log/log.hh>

#include "Params.hh"

using namespace std;

class ProcArgs
{
  
public:
  
  // constructor
  ProcArgs(int argc, char **argv, int min_required_args, int &num_args);
  
  // destructor
  ~ProcArgs();
  
  Log *L;
  Params *P;
  
protected:
  
private:
  
  void usage(char *prog_name);
  
};

#endif /* PROCARGS_HH */
