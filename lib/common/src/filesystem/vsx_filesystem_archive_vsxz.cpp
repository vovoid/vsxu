#include <filesystem/vsx_filesystem_archive_vsxz.h>


namespace vsx
{

void filesystem_archive_vsxz::create(const char* filename)
{

}


int filesystem_archive_vsxz::load(const char* archive_filename, bool load_data_multithreaded)
{
  return 0;
}


void filesystem_archive_vsxz::close()
{

}


int filesystem_archive_vsxz::file_add(vsx_string<> filename, vsx_string<> disk_filename, bool deferred_multithreaded)
{
  return 0;
}


bool filesystem_archive_vsxz::is_archive()
{
  return 0;
}


bool filesystem_archive_vsxz::is_archive_populated()
{
  return 0;
}


bool filesystem_archive_vsxz::is_file(vsx_string<> filename)
{
  return 0;
}


vsx_nw_vector<filesystem_archive_info>* filesystem_archive_vsxz::files_get()
{
  return 0;
}


void filesystem_archive_vsxz::file_open(const char* filename, const char* mode, file* &handle)
{

}

void filesystem_archive_vsxz::file_close(file* handle)
{

}

}

