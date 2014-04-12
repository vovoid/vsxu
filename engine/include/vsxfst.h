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


// Directory Separators
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #define DIRECTORY_SEPARATOR                             "\\"
  #define DIRECTORY_SEPARATOR_CHAR                             '\\'
#endif

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #define DIRECTORY_SEPARATOR                             "/"
  #define DIRECTORY_SEPARATOR_CHAR                        '/'
#endif



#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSXFSTDLLIMPORT
#else
  #if defined(VSX_ENG_DLL)
    #define VSXFSTDLLIMPORT __declspec (dllexport)
  #else 
    #define VSXFSTDLLIMPORT __declspec (dllimport)
  #endif
#endif

#define VSXF_TYPE_FILESYSTEM 0
#define VSXF_TYPE_ARCHIVE 1

#define VSXF_MODE_READ 1
#define VSXF_MODE_WRITE 2

#include "vsx_avector.h"
#include "vsx_string.h"
#include <map>
#include <list>
#include <vector>
#include <pthread.h>

// FILESYSTEM OPERATIONS

class vsxf_handle {
public:
  vsx_string filename;
  size_t position;   // position in the data stream
  size_t size;       // size of the data stream in bytes
  int mode;        // 0 = undefined,  1 = read, 2 = write
  void* file_data; // in the case of VSXF_TYPE_ARCHIVE this is the actual decompressed file in RAM
                   // don't mess with this! the file class will handle it.. 
  bool file_data_volatile;
  FILE* file_handle;

  vsxf_handle()
    :
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
      delete (vsx_avector<char>*)file_data;
      return;
    }


    if (file_data_volatile)
      return;

    free(file_data);
  }
};



class vsxf_archive_info {

  void* compressed_data;


public:

  vsx_string filename;
  size_t archive_position;
  size_t compressed_size;
  void* uncompressed_data;
  size_t uncompressed_size;

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
      compressed_data(0x0),
      archive_position(0),
      compressed_size(0),
      uncompressed_data(0x0),
      uncompressed_size(0)
  {}

  ~vsxf_archive_info()
  {
  }
};


class VSXFSTDLLIMPORT vsxf {
  // filesystem functions
  vsx_avector<vsxf_archive_info> archive_files;
  int type; // 0 = regular filesystem, 1 = archive
  FILE* archive_handle;
  vsx_string archive_name;
  // base path for opening file system files
  vsx_string base_path;

  pthread_mutex_t mutex1;

  void aquire_lock() {
    pthread_mutex_lock( &mutex1 );
  }
  void release_lock() {
    pthread_mutex_unlock( &mutex1 );
  }

public:
  vsxf();
  vsxf(const vsxf& f);
  void set_base_path(vsx_string new_base_path);
  vsx_string get_base_path();
  vsx_avector<vsxf_archive_info>* get_archive_files();
  int archive_load(const char* filename, bool preload_compressed_data = true);
  void archive_load_all_mt(const char* filename);
  void archive_create(const char* filename);
  void archive_close();
  int archive_add_file(vsx_string filename, char* data = 0, uint32_t data_size = 0, vsx_string disk_filename = "");
  bool is_archive();
  bool is_archive_populated();

  static void* worker(void* p);

  bool          is_file(const char* filename);
  bool          is_file(const vsx_string filename);
  vsxf_handle*  f_open(const char* filename, const char* mode);
  void          f_close(vsxf_handle* handle);  
  int           f_puts(const char* buf, vsxf_handle* handle);
  char*         f_gets(char* buf, unsigned long max_buf_size, vsxf_handle* handle);
  char*         f_gets_entire(vsxf_handle* handle);
  int           f_read(void* buf, unsigned long num_bytes, vsxf_handle* handle);
  unsigned long f_get_size(vsxf_handle* handle);
};

VSXFSTDLLIMPORT bool verify_filesuffix(vsx_string& input, const char* type);
VSXFSTDLLIMPORT void create_directory(const char* path);
VSXFSTDLLIMPORT void get_files_recursive(
    vsx_string startpos,
    std::list<vsx_string>* filenames,
    vsx_string include_filter = "",
    vsx_string exclude_filter = "",
    vsx_string dir_ignore_token = ".vsx_hidden"
    );
VSXFSTDLLIMPORT vsx_string get_path_from_filename(vsx_string filename);


// STRING OPERATIONS

VSXFSTDLLIMPORT vsx_string i2s(int in);
VSXFSTDLLIMPORT float s2f(const vsx_string& in);
VSXFSTDLLIMPORT vsx_string f2s(float in);
VSXFSTDLLIMPORT vsx_string f2s(float in, int decimals);
VSXFSTDLLIMPORT bool crlf(char *buffer,int len);

#define STR_PAD_LEFT 0
#define STR_PAD_RIGHT 1
#define STR_PAD_OVERFLOW_LEFT 0
#define STR_PAD_OVERFLOW_RIGHT 1
VSXFSTDLLIMPORT vsx_string str_pad(const vsx_string& str, const vsx_string& chr, size_t t_len, int pad_type = STR_PAD_LEFT,int overflow_adjust = STR_PAD_OVERFLOW_RIGHT);

// this function replaces only one item per token. 
VSXFSTDLLIMPORT vsx_string str_replace(vsx_string search, vsx_string replace, vsx_string subject, int max_replacements = 0, int required_pos = -1);
VSXFSTDLLIMPORT const vsx_string str_replace_char_pad(vsx_string search, vsx_string replace, vsx_string subject, vsx_string subject_r, int max_replacements = 0, int required_pos = -1);

VSXFSTDLLIMPORT int explode(vsx_string& input, vsx_string& delimiter, vsx_avector<vsx_string>& results, int max_parts = 0);
VSXFSTDLLIMPORT vsx_string implode(vsx_avector<vsx_string>& in, vsx_string& delimiter);

VSXFSTDLLIMPORT void str_remove_equal_prefix(vsx_string* str1, vsx_string* str2, vsx_string delimiter);
VSXFSTDLLIMPORT int split_string(vsx_string& input, vsx_string& delimiter, std::vector<vsx_string>& results, int max_parts = 0);
VSXFSTDLLIMPORT int explode(vsx_string& input, vsx_string& delimiter, std::vector<vsx_string>& results, int max_parts = 0);
VSXFSTDLLIMPORT int split_string(vsx_string& input, vsx_string& delimiter, std::list<vsx_string>& results, int max_parts = 0);
VSXFSTDLLIMPORT int explode(vsx_string& input, vsx_string& delimiter, std::list<vsx_string>& results, int max_parts = 0);

VSXFSTDLLIMPORT vsx_string implode(std::vector<vsx_string> in,vsx_string delimiter);
VSXFSTDLLIMPORT vsx_string implode(std::list<vsx_string> in,vsx_string delimiter);

VSXFSTDLLIMPORT vsx_string base64_encode(vsx_string data);
VSXFSTDLLIMPORT vsx_string base64_decode(vsx_string data);

VSXFSTDLLIMPORT vsx_string vsx_get_data_path();
VSXFSTDLLIMPORT vsx_string vsx_get_directory_separator();

#endif
