
#include "vdt_probe_message_qc_statistics_file_writer.hh"

vdt_probe_message_qc_statistics_file_writer::vdt_probe_message_qc_statistics_file_writer(const char* fpath, const char* cdl) : vdt_nc_file_dynamic_writer(fpath,cdl)
{
}

vdt_probe_message_qc_statistics_file_writer::~vdt_probe_message_qc_statistics_file_writer()
{
}

int vdt_probe_message_qc_statistics_file_writer::write_probe_message_qc_statistics(vector<vdt_probe_message_qc_statistics>& stats)
{
  return write_pika_messages(stats);
}
