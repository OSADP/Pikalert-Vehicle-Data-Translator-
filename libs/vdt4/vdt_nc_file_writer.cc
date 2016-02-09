
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
