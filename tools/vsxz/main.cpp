/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <container/vsx_nw_vector.h>
#include <vsx_argvector.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <vsxfst.h>
#include <debug/vsx_error.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif
#ifdef _WIN32
#include <io.h>
#endif

vsx_string<>current_path = "./";
vsxf filesystem;
vsx_argvector* arg = vsx_argvector::get_instance();

void extract()
{
  bool preload_compressed_data = arg->has_param("preload");
  bool dry_run = arg->has_param("dry");


  // Sanitize current_path
  if (!current_path.size())
    VSX_ERROR_EXIT("Fatal error: current_path is empty",1);

  if (access(current_path.c_str(),W_OK))
    VSX_ERROR_EXIT("current_path is not accessible for writing",1);

  vsx_string<>filename = arg->get_param_value("x");

  // Sanitize file
  if (!filesystem.is_file(filename))
    VSX_ERROR_EXIT( (vsx_string<>("Invalid archive supplied,")+filename).c_str(), 1);

  if (arg->has_param("mt"))
    filesystem.archive_load_all_mt( filename.c_str() );
  else
    filesystem.archive_load( filename.c_str(), preload_compressed_data );

  // Sanitize archive
  if (!filesystem.is_archive_populated())
    VSX_ERROR_EXIT("Archive contains no files or failed to load", 2);

  vsx_nw_vector<vsxf_archive_info>* archive_files = filesystem.get_archive_files();
  for (unsigned long i = 0; i < (*archive_files).size(); ++i)
  {
    vsx_string<>out_filename = (*archive_files)[i].filename;
    vsx_string<>out_dir = vsx_string_helper::path_from_filename(out_filename);
    vsx_string<>full_out_path = current_path + "/" + out_filename;
    vsx_string<>out_directory = current_path + "/" + out_dir;

    if (!dry_run)
      vsx_filesystem_helper::create_directory( out_directory.c_str() );

    vsxf_handle* fpi = filesystem.f_open((*archive_files)[i].filename.c_str(), "r");
      if (!fpi)
        VSX_ERROR_EXIT( (vsx_string<>("Internal Error: fpi invalid when reading ")+(*archive_files)[i].filename).c_str(), 3);

      FILE* fpo = 0;
      if (!dry_run)
      {
        fpo = fopen(full_out_path.c_str(), "wb");
        if (!fpo)
          VSX_ERROR_EXIT( (vsx_string<>("Internal Error: fpo invalid when opening file for writing: ")+full_out_path).c_str(), 3 );
      }

        char* buf = filesystem.f_gets_entire(fpi);
          if (!buf)
          {
            if (!dry_run)
            fclose(fpo);
            VSX_ERROR_EXIT("Internal Error: buf invalid", 4);
          }

          if (!dry_run)
            fwrite(buf,sizeof(char),fpi->size,fpo);

        free(buf);

      if (!dry_run)
        fclose(fpo);
    filesystem.f_close(fpi);
  }

  filesystem.archive_close();

  // success
  exit(0);
}

void create()
{
  // Sanitize the presence of -f / -fl
  if (!arg->has_param_with_value("f") && !arg->has_param_with_value("fl"))
    VSX_ERROR_EXIT("Error, you must supply files to be added to the archive", 100);

  vsx_string<> filenames;
  vsx_nw_vector< vsx_string<> > parts;
  vsxf fs;

  if (arg->has_param_with_value("f"))
    filenames = arg->get_param_value("f");

  if (arg->has_param_with_value("fl"))
  {
    vsx_string<> file_list_file = arg->get_param_value("fl");
    if (!fs.is_file(file_list_file))
      VSX_ERROR_EXIT("Error, can not read file list file...", 101);
    filenames = vsx_string_helper::read_from_file<1024*1024>( file_list_file );
  }

  vsx_printf(L"filenames: %s\n", filenames.c_str());

  vsx_string<>deli = ":";
  vsx_string_helper::explode(filenames, deli, parts);

  if (!parts.size())
    VSX_ERROR_EXIT("There seems to be no entries in the file list", 101);

  // Make sure we can read all the files
  for (size_t i = 0; i < parts.size(); i++)
  {
    if (access(parts[i].c_str(),R_OK))
      VSX_ERROR_EXIT( ( vsx_string<>("Pre-check failed, error accessing file: ") + parts[i]).c_str()  ,1);
  }

  vsx_string<>archive_filename = arg->get_param_value("c");

  filesystem.archive_create(archive_filename.c_str());

  for (size_t i = 0; i < parts.size(); i++)
  {
    vsx_printf(L"* adding: %s \n", parts[i].c_str() );
    int ret = filesystem.archive_add_file_mt( parts[i] );
    if ( ret )
      VSX_ERROR_EXIT( "archive_add_file failed", ret );
  }

  filesystem.archive_close();

  vsx_printf(L"-- successfully created the archive: %s\n", archive_filename.c_str());

  // success
  exit(0);
}

int main(int argc, char* argv[])
{
  vsx_printf(L"vsxz\n");
  arg->init_from_argc_argv(argc, argv);

  if ( arg->has_param_with_value("x") )
  {
    extract();
  }

  if ( arg->has_param_with_value("c") )
  {
    vsx_printf(L"creating archive\n");
    create();
  }

  vsx_printf
  (
    L"VSXz packer (operates on .vsx files)\n"
    "\n"
    "\n"
    "Extract archive: \n"
    "  vsxz -x [archive filename] \n"
    "\n"
    "OPTIONS:\n"
    "  -preload\n"
    "        Reads all the compressed data into RAM upon loading the archive\n"
    "\n"
    "Create archive: \n"
    "  vsxz -c [archive filename] -f file1:file2:file3 -fl [file with file list like -f]\n"
  );


  return 0;
}
