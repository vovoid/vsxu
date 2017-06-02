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

#include <vsx_version.h>
#include <container/vsx_nw_vector.h>
#include <vsx_argvector.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <filesystem/vsx_filesystem.h>
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

#include "info.h"
#include "extract.h"
#include "create_vsx.h"
#include "create_vsxz.h"

int main(int argc, char* argv[])
{
  vsx_printf(
        L"\nVSXz Archiver\n"
        "Part of %hs %hs\n", VSXU_VERSION, VSXU_VERSION_COPYRIGHT);
  vsx_argvector::get_instance()->init_from_argc_argv(argc, argv);

  info();
  extract();
  create_vsxz();
  create_vsx();

  vsx_printf
  (
    L"\n"
    "Extract archive: \n"
    "  vsxz -x [archive filename] \n"
    "       -dry  dry run\n"
    "       -m    measure time\n"
    "\n"
    "Create archive: \n"
    "  vsxz -c [archive filename]                    (mandatory)\n"
    "       -f file1:file2:file3                     (mandatory)\n"
    "       -fl [file with file list like -f]        (optional)\n"
    "       -fn [file w/file list one file per line] (optional)\n"
    "       -z pack archive as vsxz                  (optional)\n"
    "       -nc when creating vsxz, don't compress   (optional)\n"
    "\n"
    "Information:\n"
    "  vsxz -info [archive filename]                 show meta data about archive\n"
    "\n"
    "  Under Linux, you can reset the disk cache like this:\n"
    "       sync && echo 3 > /proc/sys/vm/drop_caches\n"
    "\n"
  );


  return 0;
}
