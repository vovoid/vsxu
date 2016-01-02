#include <filesystem/archive/vsx_filesystem_archive_vsxz_writer.h>


namespace vsx
{
void filesystem_archive_vsxz_writer::create(const char* filename)
{
  archive_name = filename;
  file = fopen(filename,"wb");
}


void filesystem_archive_vsxz_writer::file_add_all_worker(vsx_nw_vector<filesystem_archive_file_write*>* work_list)
{
}

void filesystem_archive_vsxz_writer::file_add_all()
{

}

int filesystem_archive_vsxz_writer::add_file
(
  vsx_string<> filename,
  vsx_string<> disk_filename,
  bool deferred_multithreaded
)
{
  return 0;
}


int filesystem_archive_vsxz_writer::add_string(vsx_string<> filename, vsx_string<> payload, bool deferred_multithreaded)
{
  filesystem_archive_file_write file_info;
  file_info.filename = filename;
  file_info.uncompressed_data.allocate( payload.size() );
  file_info.operation = filesystem_archive_file_write::operation_add;

  foreach (payload, i)
    file_info.uncompressed_data[i] = (unsigned char)payload[i];

  archive_files.push_back(file_info);
  return 0;
}


void filesystem_archive_vsxz_writer::close()
{
}

}

