#include <filesystem/vsx_filesystem_archive.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <debug/vsx_error.h>
#include <tools/vsx_req_error.h>
#include <tools/vsx_thread_pool.h>
#include <string/vsx_string_helper.h>

#include <filesystem/vsx_filesystem_archive_vsx.h>
#include <filesystem/vsx_filesystem_archive_vsxz.h>


namespace vsx
{

void filesystem_archive::create(const char* filename, archive_type_t type)
{
  if (archive)
    close();

  if (type == archive_vsx)
    archive = new filesystem_archive_vsx();

  if (type == archive_vsxz)
    archive = new filesystem_archive_vsxz();

  archive->create(filename);
}

int filesystem_archive::load(const char* archive_filename, bool load_data_multithreaded)
{
  if (archive)
    close();

  vsx_string<> filename(archive_filename);
  if (vsx_string_helper::verify_filesuffix(filename, "vsx"))
    archive = new filesystem_archive_vsx();

  if (vsx_string_helper::verify_filesuffix(filename, "vsxz"))
    archive = new filesystem_archive_vsxz();

  req_error_v(archive, "unsupported file format", 1);

  return archive->load(archive_filename, load_data_multithreaded);
}

void filesystem_archive::close()
{
  req(archive);
  archive->close();
}

bool filesystem_archive::is_archive()
{
  req_v(archive, false);
  return archive->is_archive();
}

bool filesystem_archive::is_archive_populated()
{
  req_v(archive, false);
  return archive->is_archive_populated();
}

bool filesystem_archive::is_file(vsx_string<> filename)
{
  req_v(archive, false);
  return archive->is_file(filename);
}

vsx_nw_vector<filesystem_archive_info>* filesystem_archive::files_get()
{
  req_v(archive, 0x0);
  return archive->files_get();
}


void filesystem_archive::file_open(const char* filename, const char* mode, file* &handle)
{
  req(archive);
  archive->file_open(filename, mode, handle);
}


void filesystem_archive::file_close(file* handle)
{
  req(archive);
  archive->file_close(handle);
}


int filesystem_archive::file_add
(
  vsx_string<>filename,
  vsx_string<>disk_filename,
  bool deferred_multithreaded
)
{
  req_v(archive, 1);
  return archive->file_add(filename,  disk_filename, deferred_multithreaded);
}


}
