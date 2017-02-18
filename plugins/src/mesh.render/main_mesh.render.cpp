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
#define VSX_FONT_NO_FT 1
#include "vsx_font.h"
#include "vsx_glsl.h"


#include "module_mesh_render.h"
#include "module_mesh_render_billboards.h"
#include "module_mesh_render_dots.h"
#include "module_mesh_render_face_id.h"
#include "module_mesh_render_line.h"
#include "module_mesh_render_zsort.h"

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
#define MOD_CM vsx_module_mesh_render_cm
#define MOD_DM vsx_module_mesh_render_dm
#define MOD_NM vsx_module_mesh_render_nm
#endif


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch(module)
  {
    case 0: return (vsx_module*)(new module_mesh_render);
    case 1: return (vsx_module*)(new module_mesh_render_zsort);
    case 2: return (vsx_module*)(new module_mesh_render_line);
    case 3: return (vsx_module*)(new module_mesh_render_dots);
    case 4: return (vsx_module*)(new module_mesh_render_face_id);
    case 5: return (vsx_module*)(new module_mesh_render_billboards);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module)
{
  switch(module) {
    case 0: delete (module_mesh_render*)m; break;
    case 1: delete (module_mesh_render_zsort*)m; break;
    case 2: delete (module_mesh_render_line*)m; break;
    case 3: delete (module_mesh_render_dots*)m; break;
    case 4: delete (module_mesh_render_face_id*)m; break;
    case 5: delete (module_mesh_render_billboards*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment)
{
  VSX_UNUSED(environment);
  return 6;
}
