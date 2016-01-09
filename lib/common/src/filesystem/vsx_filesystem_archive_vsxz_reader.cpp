#include <filesystem/archive/vsx_filesystem_archive_vsxz_reader.h>


namespace vsx
{

bool filesystem_archive_vsxz_reader::load(const char* archive_filename, bool load_data_multithreaded)
{
  mmap = filesystem_mmap::create(archive_filename);
  req_v(mmap, false);
  req_v(mmap->size > sizeof(vsxz_header) + sizeof(vsxz_header_file_info), false);
  header = (vsxz_header*)mmap->data;

  req_v(header->identifier[0] == 'V', false);
  req_v(header->identifier[1] == 'S', false);
  req_v(header->identifier[2] == 'X', false);
  req_v(header->identifier[3] == 'Z', false);


  return 1;
}

void filesystem_archive_vsxz_reader::file_open(const char* filename, file* &handle)
{

}

void filesystem_archive_vsxz_reader::close()
{

}


bool filesystem_archive_vsxz_reader::is_archive()
{
  return 0;
}


bool filesystem_archive_vsxz_reader::is_archive_populated()
{
  return 0;
}


bool filesystem_archive_vsxz_reader::is_file(vsx_string<> filename)
{
  return 0;
}


vsx_nw_vector<filesystem_archive_file_read>* filesystem_archive_vsxz_reader::files_get()
{
  return 0;
}



}

