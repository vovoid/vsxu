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

#include <vsx_version.h>
#include <container/vsx_nw_vector.h>
#include <vsx_argvector.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <filesystem/vsx_filesystem.h>
#include <debug/vsx_error.h>
#include <bitmap/vsx_bitmap_loader.h>
#include <bitmap/vsx_bitmap_writer.h>
#include <bitmap/vsx_bitmap_transform.h>

#include <stdio.h>
#include <stdlib.h>

vsx_string<>current_path = "./";

int main(int argc, char* argv[])
{
  vsx_printf(
        L"\nVSX Bitmap Transform\n"
        "Part of %s %s\n", VSXU_VERSION, VSXU_VERSION_COPYRIGHT);
  vsx_argvector::get_instance()->init_from_argc_argv(argc, argv);

  if (!vsx_argvector::get_instance()->has_param("s") &&  !vsx_argvector::get_instance()->has_param("v") )
  {
    vsx_printf
    (
      L"\n"
      "Split image into cubemap: -s[source file.png|jpg|tga] -d target_file.tga\n"
      "Example:    vsxbt source.png dest.tga\n"
      "Will produce 6 files: dest_0.tga dest_1.tga etc.\n"
      "\n"
    );
    exit(0);
  }


  vsx_string<> source_file = vsx_argvector::get_instance()->get_param_value("s");
  vsx_string<> destination_file = vsx_argvector::get_instance()->get_param_value("d");

  vsx_bitmap bitmap;
  vsx_bitmap_loader::load(&bitmap, source_file, vsx::filesystem::get_instance(), false, 0);
  vsx_bitmap_transform::get_instance()->split_into_cubemap(&bitmap);
  vsx_bitmap_writer::write(&bitmap, destination_file);


  return 0;
}
