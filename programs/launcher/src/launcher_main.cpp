/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Public License (GPL)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <vsx_version.h>
#include <vsx_platform.h>
#include <vsx_data_path.h>

#include "vsx_application_launcher.h"

#include <vsx_application_manager.h>
#include <vsx_application_run.h>

int main(int argc, char* argv[])
{
  vsx_argvector::get_instance()->init_from_argc_argv(argc, argv);
  vsx_data_path::get_instance()->init();
  vsx_application_launcher application;
  vsx_application_manager::get_instance()->application_set(&application);
  vsx_application_run::run();
  return 0;
}

