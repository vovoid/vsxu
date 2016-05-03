/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
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


#include "_configuration.h"
#include "vsx_param.h"
#include <module/vsx_module.h>
#include "gravity_lines/gravity_lines.h"
#include "gravity_lines/gravity_strip.h"



#include "vsx_module_render_gravity_lines.h"
#include "vsx_module_render_gravity_ribbon.h"
#include "vsx_module_render_gravity_ribbon_particles.h"
#include "vsx_module_render_gravity_ribbon_mesh.h"

//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules(vsx_module_engine_environment* environment);
}

#ifndef MOD_CM
#define MOD_CM vsx_module_render_gravity_lines_cm
#define MOD_DM vsx_module_render_gravity_lines_dm
#define MOD_NM vsx_module_render_gravity_lines_nm
#endif

vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch (module) {
    case 0: return (vsx_module*)new vsx_module_render_gravity_lines;
    case 1: return (vsx_module*)new vsx_module_render_gravity_ribbon;
    case 2: return (vsx_module*)new vsx_module_render_gravity_ribbon_particles;
    case 3: return (vsx_module*)new vsx_module_render_gravity_ribbon_mesh;
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch (module) {
    case 0: delete (vsx_module_render_gravity_lines*)m; break;
    case 1: delete (vsx_module_render_gravity_ribbon*)m; break;
    case 2: delete (vsx_module_render_gravity_ribbon_particles*)m; break;
    case 3: delete (vsx_module_render_gravity_ribbon_mesh*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment)
{
  VSX_UNUSED(environment);
  return 4;
}
