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
#include <vsx_platform.h>

#include "player_application.h"

#include <vsx_application_manager.h>
#include <vsx_application_run.h>
#include <math/vsx_rand_singleton.h>
#include <vsx_data_path.h>
#include <vsx_module_list_manager.h>

int main(int argc, char* argv[])
{
  vsx_argvector::get_instance()->init_from_argc_argv(argc, argv);
  vsx_rand_singleton::get()->rand.srand( (uint32_t)time(0x0) );
  vsx_data_path::get_instance()->init();
  player_application application;
  vsx_application_manager::get_instance()->application_set(&application);
  vsx_application_run::run();
  return 0;
}
