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

#include <vsx_avector.h>
#include <vsx_argvector.h>
#include <vsx_string.h>
#include <vsxfst.h>
#include <vsx_error.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif
#ifdef _WIN32
#include <io.h>
#endif

vsx_string current_path = vsx_argvector::get_executable_directory();
vsxf filesystem;
vsx_argvector* arg = vsx_argvector::get_instance();

void extract()
{
  vsx_string filename = arg->get_param_value("x");

  // Sanitize file
  if (!filesystem.is_file(filename))
    ERROR_EXIT( (vsx_string("Invalid archive supplied,")+filename).c_str(), 1);

  filesystem.archive_load( filename.c_str() );

  // Sanitize archive
  if (!filesystem.is_archive_populated())
    ERROR_EXIT("Archive contains no files or failed to load", 2);

  vsx_avector<vsxf_archive_info>* archive_files = filesystem.get_archive_files();
  for (unsigned long i = 0; i < (*archive_files).size(); ++i)
  {
    vsx_string out_filename = (*archive_files)[i].filename;
    vsx_string out_dir = get_path_from_filename(out_filename);
    vsx_string full_out_path = current_path + "/" + out_filename;
    vsx_string out_directory = current_path + "/" + out_dir;
    vsx_printf("Creating directory: %s\n", out_directory.c_str());
    create_directory( (char*) (current_path + "/" + out_dir).c_str() );

    printf("Extracting:%s\n", (*archive_files)[i].filename.c_str() );

    vsxf_handle* fpi = filesystem.f_open((*archive_files)[i].filename.c_str(), "r");

      if (!fpi)
        ERROR_EXIT( (vsx_string("Internal Error: fpi invalid when reading ")+(*archive_files)[i].filename).c_str(), 3);

      FILE* fpo = fopen(full_out_path.c_str(), "wb");
        if (!fpo)
          ERROR_EXIT( (vsx_string("Internal Error: fpo invalid when opening file for writing: ")+full_out_path).c_str(), 3 );

        char* buf = filesystem.f_gets_entire(fpi);
          if (!buf)
          {
            fclose(fpo);
            ERROR_EXIT("Internal Error: buf invalid", 4);
          }
          fwrite(buf,sizeof(char),fpi->size-1,fpo);
        free(buf);
      fclose(fpo);

    filesystem.f_close(fpi);
  }
}


int main(int argc, char* argv[])
{
  arg->init_from_argc_argv(argc, argv);

  printf("-- Vovoid VSXu Packer --\n");

  if (argc == 1 || vsx_argvector::get_instance()->has_param("help"))
  {
    printf(
      "syntax: \n"
      "  vsxz -x [filename] (extract) \n"
    );
  }

  if ( arg->has_param_with_value("x") )
  {
    extract();
  }

  return 0;
}
