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
  struct uncompress_info
  {
    unsigned char* compressed_data;
    unsigned char* uncompressed_data;
    vsxz_header_file_info* file_info;
  };

  vsxz_header* header = 0x0;
  file_mmap* mmap = 0x0;
  vsx_ma_vector<unsigned char> uncompressed_data;

  void load_worker( vsx_ma_vector<uncompress_info>* uncompress_work_list, size_t start_index, size_t end_index );
  vsxz_header_file_info* get_file_info(const char* filename, uint32_t &compressed_offset, uint32_t &uncompressed_offset);

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
