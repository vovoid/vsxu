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

#ifndef VSX_MODULE_LIST_FACTORY_H
#define VSX_MODULE_LIST_FACTORY_H

#include "vsx_engine_dllimport.h"
#include "vsx_module_list_factory_module_info.h"

#ifdef VSXU_STATIC
  vsx_module_list_abs* vsx_module_list_factory_create(void* extra_modules = 0x0);
  void vsx_module_list_factory_destroy( vsx_module_list_abs* object );
#else
extern "C" {
  ENGINE_DLLIMPORT vsx_module_list_abs* vsx_module_list_factory_create(void* extra_modules = 0x0);
  ENGINE_DLLIMPORT void vsx_module_list_factory_destroy( vsx_module_list_abs* object );
}
#endif

#endif
