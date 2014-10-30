// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_nc_file_writer.hh"

const int vdt_nc_file_writer::MAX_CMD_SIZE = 2048;

vdt_nc_file_writer::vdt_nc_file_writer(const char* fpath, const char* cdl)
{
  this->fpath = fpath;
  this->cdl = cdl;

  char* cmd = new char[MAX_CMD_SIZE];
  snprintf(cmd,MAX_CMD_SIZE,"ncgen -o %s %s", fpath, cdl);
  int ret = system(cmd);
  if (ret != 0)
    {
      printf("%s returned non-zero: %d, exiting\n", cmd, ret);
      exit(1);
    }
  delete [] cmd;

  this->nc = new NcFile(fpath,NcFile::Write);
}

vdt_nc_file_writer::~vdt_nc_file_writer()
{
  this->nc->close();
  delete this->nc;
}
