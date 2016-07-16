#include <filesystem/archive/vsx_filesystem_archive_reader.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <debug/vsx_error.h>
#include <tools/vsx_req_error.h>
#include <tools/vsx_thread_pool.h>
#include <string/vsx_string_helper.h>

#include <filesystem/archive/vsx/vsx_filesystem_archive_vsx_reader.h>
#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_reader.h>

namespace vsx
{

bool filesystem_archive_reader::load(const char* archive_filename, bool load_data_multithreaded)
{
  if (archive)
    close();

  vsx_string<> filename(archive_filename);

  if (vsx_string_helper::verify_filesuffix(filename, "vsx"))
    archive_type =  archive_vsx;

  if (vsx_string_helper::verify_filesuffix(filename, "vsxz"))
    archive_type = archive_vsxz;

  if (archive_type == archive_vsx)
    archive = new filesystem_archive_vsx_reader();

  if (archive_type == archive_vsxz)
    archive = new filesystem_archive_vsxz_reader();

  req_error_v(archive, "unsupported file format", 1);

  return archive->load(archive_filename, load_data_multithreaded);
}

void filesystem_archive_reader::close()
{
  req(archive);
  archive->close();

  if (archive_type == archive_vsx)
    delete ((filesystem_archive_vsx_reader*)archive);

  if (archive_type == archive_vsxz)
    delete (filesystem_archive_vsxz_reader*)archive;

  archive_type = archive_none;
  archive = 0x0;
}

bool filesystem_archive_reader::is_archive()
{
  reqrv(archive, false);
  return archive->is_archive();
}

bool filesystem_archive_reader::is_archive_populated()
{
  reqrv(archive, false);
  return archive->is_archive_populated();
}

bool filesystem_archive_reader::is_file(vsx_string<> filename)
{
  reqrv(archive, false);
  return archive->is_file(filename);
}

void filesystem_archive_reader::files_get(vsx_nw_vector<filesystem_archive_file_read>& files)
{
  req(archive);
  archive->files_get(files);
}

void filesystem_archive_reader::file_open(const char* filename, file* &handle)
{
  req(archive);
  archive->file_open(filename, handle);
}

}
