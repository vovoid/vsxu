/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSXFST_H
#define VSXFST_H

#include <vsx_platform.h>
#include <stdlib.h>
#include <inttypes.h>

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


// FILESYSTEM OPERATIONS

class vsxf_handle {
public:
  vsx_string filename;
  unsigned long position;   // position in the data stream
  unsigned long size;       // size of the data stream in bytes
  int mode;        // 0 = undefined,  1 = read, 2 = write
  void* file_data; // in the case of type == 1 this is the actual decompressed file in RAM
                   // don't mess with this! the file class will handle it.. 
  FILE* file_handle;
  vsxf_handle() : position(0), size(0),mode(0), file_data(0), file_handle(0) {}
  ~vsxf_handle() {
    #ifdef VSXU_DEBUG
      printf("vsxf_handle destructor, %s\n", filename.c_str() );
    #endif
    if (file_data) {
      if (mode == VSXF_MODE_WRITE)
      {
        #ifdef VSXU_DEBUG
          printf(">>> deleting file data #1, %s\n", filename.c_str() );
        #endif
        delete (vsx_avector<char>*)file_data;
      }
      else
      {
        #ifdef VSXU_DEBUG
          printf(">>> deleting file data #2, %s\n", filename.c_str() );
        #endif
        free(file_data);
      }
    }
  }
};



typedef struct {
  vsx_string filename;
  long position;
  long size;
} vsxf_archive_info;


class VSXFSTDLLIMPORT vsxf {
  // filesystem functions
  vsx_avector<vsxf_archive_info> archive_files;
  int type; // 0 = regular filesystem, 1 = archive
  FILE* archive_handle;
  vsx_string archive_name;
  // base path for opening file system files
  vsx_string base_path;
public:
  vsxf();
  vsxf(const vsxf& f);
  void set_base_path(vsx_string new_base_path);
  vsx_avector<vsxf_archive_info>* get_archive_files();
  int archive_load(const char* filename);
  void archive_create(const char* filename);
  void archive_close();
  int archive_add_file(vsx_string filename, char* data = 0, uint32_t data_size = 0, vsx_string disk_filename = "");
  bool is_archive();
  bool is_archive_populated();

  vsxf_handle*  f_open(const char* filename, const char* mode);
  void          f_close(vsxf_handle* handle);  
  int           f_puts(const char* buf, vsxf_handle* handle);
  char*         f_gets(char* buf, unsigned long max_buf_size, vsxf_handle* handle);
  char*         f_gets_entire(vsxf_handle* handle);
  int           f_read(void* buf, unsigned long num_bytes, vsxf_handle* handle);
  unsigned long f_get_size(vsxf_handle* handle);
};

VSXFSTDLLIMPORT bool verify_filesuffix(vsx_string& input, const char* type);
VSXFSTDLLIMPORT void create_directory(char* path);
VSXFSTDLLIMPORT void get_files_recursive(vsx_string startpos, std::list<vsx_string>* filenames,vsx_string include_filter = "",vsx_string exclude_filter = "CVS");
VSXFSTDLLIMPORT vsx_string get_path_from_filename(vsx_string filename);


// STRING OPERATIONS

VSXFSTDLLIMPORT int s2i(const vsx_string& in);
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




#endif
