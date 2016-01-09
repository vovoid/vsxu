#pragma once

#include <container/vsx_ma_vector.h>
#include <filesystem/mmap/vsx_filesystem_mmap.h>
#include <filesystem/vsx_file.h>
#include <filesystem/archive/vsx_filesystem_archive_vsxz_header.h>
#include <filesystem/archive/vsx_filesystem_archive_file_read.h>
#include <filesystem/archive/vsx_filesystem_archive_reader_base.h>

namespace vsx
{

class filesystem_archive_vsxz_reader
    : public filesystem_archive_reader_base
{
  vsxz_header* header = 0x0;
  vsx_ma_vector<vsxz_header_file_info*> directory;
  vsx_ma_vector<char*> directory_file_names;
  file_mmap* mmap = 0x0;
  vsx_nw_vector<filesystem_archive_file_read> archive_files;

public:

  bool load(const char* archive_filename, bool load_data_multithreaded);

  vsx_nw_vector<filesystem_archive_file_read>* files_get();
  void file_open(const char* filename, file* &handle);

  void close();

  bool is_archive();
  bool is_archive_populated();
  bool is_file(vsx_string<> filename);

  ~filesystem_archive_vsxz_reader()
  {}

};

}
