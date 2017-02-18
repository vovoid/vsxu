#pragma once

#include <container/vsx_ma_vector.h>
#include <filesystem/mmap/vsx_filesystem_mmap.h>
#include <filesystem/vsx_file.h>
#include <filesystem/archive/vsx_filesystem_archive_reader_base.h>
#include "vsx_filesystem_archive_vsxz_header.h"
#include <filesystem/tree/vsx_filesystem_tree_reader.h>

namespace vsx
{

class filesystem_archive_vsxz_reader
    : public filesystem_archive_reader_base
{
  struct uncompress_info
  {
    unsigned char* compressed_data;
    unsigned char* uncompressed_data;
    vsxz_header_file_info* file_info;
  };

  vsxz_header* header = 0x0;
  file_mmap* mmap = 0x0;
  vsx_ma_vector<unsigned char> uncompressed_data;
  vsx_filesystem_tree_reader tree;
  unsigned char* uncompressed_data_start_pointers[9] = {0,0,0,0,0,0,0,0,0};
  vsxz_header_file_info* file_info_table = 0x0;

public:

  bool load(const char* archive_filename, bool load_data_multithreaded);

  void files_get(vsx_nw_vector<filesystem_archive_file_read>& files);
  void file_open(const char* filename, file* &handle);

  void close();

  bool is_archive();
  bool is_archive_populated();
  bool is_file(vsx_string<> filename);

  ~filesystem_archive_vsxz_reader()
  {}

};

}
