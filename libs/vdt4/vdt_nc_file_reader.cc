// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_nc_file_reader.hh"
#include "vdt_util.hh"

vdt_nc_file_reader::vdt_nc_file_reader(const char* fpath)
{
  this->fpath = fpath;
  if (!fileExists(fpath))
    {

      error = fpath + string(" not found");
      nc = NULL;
      return;
    }
  this->nc = new NcFile(fpath,NcFile::ReadOnly);
}

vdt_nc_file_reader::~vdt_nc_file_reader()
{
  if (nc != NULL)
    {
      this->nc->close();
      delete this->nc;
    }
}
