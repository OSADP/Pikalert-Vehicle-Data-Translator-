
#include <string.h>
#include "vdt_probe_message_qc_file_writer.hh"

vdt_probe_message_qc_file_writer::~vdt_probe_message_qc_file_writer()
{
}

int vdt_probe_message_qc_file_writer::write_probe_messages(const vector<vdt_probe_message_qc>& msgs)
{
 return write_pika_messages(msgs);
}
