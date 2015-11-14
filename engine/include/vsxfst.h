/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSXFST_H
#define VSXFST_H

#include <vsx_platform.h>


#include <stdlib.h>
#include <inttypes.h>
#include "vsx_engine_dllimport.h"

#define VSXF_TYPE_FILESYSTEM 0
#define VSXF_TYPE_ARCHIVE 1

#define VSXF_MODE_READ 1
#define VSXF_MODE_WRITE 2

#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <map>
#include <list>
#include <vector>
#include <pthread.h>

// FILESYSTEM OPERATIONS

class vsxf_handle {
public:
  vsx_string<> filename;
  size_t position;   // position in the data stream
  size_t size;       // size of the data stream in bytes
  int mode;        // 0 = undefined,  1 = read, 2 = write
  void* file_data; // in the case of VSXF_TYPE_ARCHIVE this is the actual decompressed file in RAM
                   // don't mess with this! the file class will handle it.. 
  bool file_data_volatile;
  FILE* file_handle;

  vsxf_handle()
    :
      filename(),
      position(0),
      size(0),
      mode(0),
      file_data(0),
      file_data_volatile(false),
      file_handle(0)
  {}

  ~vsxf_handle()
  {
    if (!file_data)
      return;

    if (mode == VSXF_MODE_WRITE)
    {
      delete (vsx_nw_vector<char>*)file_data;
      return;
    }


    if (file_data_volatile)
      return;

    free(file_data);
  }
};



class vsxf_archive_info {
public:

  vsx_string<> filename;
  size_t archive_position;

  size_t compressed_size;
  void* compressed_data;

  size_t uncompressed_size;
  void* uncompressed_data;

  void* get_compressed_data()
  {
    return compressed_data;
  }

  void set_compressed_data(void* n)
  {
    compressed_data = n;
  }

  void clear_compressed_data()
  {
    if (compressed_data)
      free(compressed_data);
    compressed_data = 0;
  }

  void clear_uncompressed_data()
  {
    if (uncompressed_data)
      free(uncompressed_data);
    uncompressed_data = 0;
  }


  vsxf_archive_info()
    :
      filename(),
      archive_position(0),
      compressed_size(0),
      compressed_data(0x0),
      uncompressed_size(0),
      uncompressed_data(0x0)
  {}

  ~vsxf_archive_info()
  {
  }
};


#define VSXF_NUM_ADD_THREADS 8
#define VSXF_WORK_CHUNK_MAX_SIZE 1024*1024*5

class ENGINE_DLLIMPORT vsxf {
  // filesystem functions
  vsx_nw_vector<vsxf_archive_info> archive_files;
  vsx_nw_vector<vsxf_archive_info*> archive_files_p;
  int type; // 0 = regular filesystem, 1 = archive
  FILE* archive_handle;
  vsx_string<> archive_name;
  // base path for opening file system files
  vsx_string<> base_path;

  pthread_mutex_t mutex1;

  void aquire_lock() {
    pthread_mutex_lock( &mutex1 );
  }
  void release_lock() {
    pthread_mutex_unlock( &mutex1 );
  }

  vsx_nw_vector<vsxf_archive_info*> add_work_pool[VSXF_NUM_ADD_THREADS];
  size_t add_work_pool_iterator;
  size_t work_chunk_current_size;
  void archive_close_handle_workers();


public:
  vsxf();
  vsxf(const vsxf& f);
  void set_base_path(vsx_string<> new_base_path);
  vsx_string<> get_base_path();
  vsx_nw_vector<vsxf_archive_info>* get_archive_files();
  int archive_load(const char* filename, bool preload_compressed_data = true);
  void archive_load_all_mt(const char* filename);
  void archive_create(const char* filename);
  void archive_close();
  int archive_add_file(vsx_string<> filename, char* data = 0, uint32_t data_size = 0, vsx_string<> disk_filename = "");
  int archive_add_file_mt(vsx_string<> filename);
  bool is_archive();
  bool is_archive_populated();


  static void* archive_add_file_worker(void* p);
  static void* archive_load_worker(void* p);

  bool          is_file(const char* filename);
  bool          is_file(const vsx_string<> filename);
  vsxf_handle*  f_open(const char* filename, const char* mode);
  void          f_close(vsxf_handle* handle);  
  int           f_puts(const char* buf, vsxf_handle* handle);
  char*         f_gets(char* buf, unsigned long max_buf_size, vsxf_handle* handle);
  wchar_t*      f_getws(wchar_t* buf, unsigned long max_buf_size, vsxf_handle* handle);
  char*         f_gets_entire(vsxf_handle* handle);
  int           f_read(void* buf, unsigned long num_bytes, vsxf_handle* handle);
  unsigned long f_get_size(vsxf_handle* handle);

  static vsxf* get_instance()
  {
    static vsxf f;
    return &f;
  }
};

ENGINE_DLLIMPORT bool verify_filesuffix(vsx_string<>& input, const char* type);
ENGINE_DLLIMPORT void create_directory(const char* path);
ENGINE_DLLIMPORT void get_files_recursive(
    vsx_string<> startpos,
    std::list< vsx_string<> >* filenames,
    vsx_string<> include_filter = "",
    vsx_string<> exclude_filter = "",
    vsx_string<> dir_ignore_token = ".vsx_hidden"
    );

ENGINE_DLLIMPORT vsx_string<> get_path_from_filename(vsx_string<> filename, char override_directory_separator = 0);


// STRING OPERATIONS
#define STR_PAD_LEFT 0
#define STR_PAD_RIGHT 1
#define STR_PAD_OVERFLOW_LEFT 0
#define STR_PAD_OVERFLOW_RIGHT 1
ENGINE_DLLIMPORT vsx_string<> str_pad(const vsx_string<>& str, const vsx_string<>& chr, size_t t_len, int pad_type = STR_PAD_LEFT,int overflow_adjust = STR_PAD_OVERFLOW_RIGHT);

// this function replaces only one item per token. 
ENGINE_DLLIMPORT vsx_string<> str_replace(vsx_string<> search, vsx_string<> replace, vsx_string<> subject, int max_replacements = 0, int required_pos = -1);
ENGINE_DLLIMPORT const vsx_string<>str_replace_char_pad(vsx_string<>search, vsx_string<>replace, vsx_string<>subject, vsx_string<>subject_r, int max_replacements = 0, int required_pos = -1);

ENGINE_DLLIMPORT int explode(vsx_string<>& input, vsx_string<>& delimiter, vsx_nw_vector< vsx_string<> >& results, int max_parts = 0);
ENGINE_DLLIMPORT vsx_string<>implode(vsx_nw_vector< vsx_string<> >& in, vsx_string<>& delimiter);

ENGINE_DLLIMPORT int explode(vsx_string<>& input, vsx_string<>& delimiter, std::vector <vsx_string<> >& results, int max_parts = 0);
ENGINE_DLLIMPORT vsx_string<>implode(std::vector <vsx_string<> > in,vsx_string<>delimiter);

ENGINE_DLLIMPORT void str_remove_equal_prefix(vsx_string<>* str1, vsx_string<>* str2, vsx_string<>delimiter);
ENGINE_DLLIMPORT int split_string(vsx_string<>& input, vsx_string<>& delimiter, std::vector <vsx_string<> >& results, int max_parts = 0);


ENGINE_DLLIMPORT int split_string(vsx_string<>& input, vsx_string<>& delimiter, std::list< vsx_string<> >& results, int max_parts = 0);
ENGINE_DLLIMPORT int explode(vsx_string<>& input, vsx_string<>& delimiter, std::list< vsx_string<> >& results, int max_parts = 0);

ENGINE_DLLIMPORT vsx_string<>implode(std::list< vsx_string<> > in,vsx_string<>delimiter);
ENGINE_DLLIMPORT vsx_string<>vsx_get_directory_separator();

ENGINE_DLLIMPORT size_t file_get_size(vsx_string<> filename);

#endif
