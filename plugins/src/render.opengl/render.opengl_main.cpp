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
#include <vsx_param.h>
#include <module/vsx_module.h>
#include <math/quaternion/vsx_quaternion.h>


const unsigned int matrix_target_get_vsx[] =
  {
    VSX_GL_MODELVIEW_MATRIX,
    VSX_GL_PROJECTION_MATRIX,
    VSX_GL_TEXTURE_MATRIX
  }
;


#include "module_render_gl_backface_culling.h"
#include "module_render_gl_blend_mode.h"
#include "module_render_gl_buffer_clear.h"
#include "module_render_gl_color.h"
#include "module_render_gl_depth_buffer_clear.h"
#include "module_render_gl_depth_buffer.h"
#include "module_render_gl_depth_func.h"
#include "module_render_gl_fog.h"
#include "module_render_gl_freelook_camera.h"
#include "module_render_gl_frustum.h"
#include "module_render_gl_light_directional.h"
#include "module_render_gl_line_width.h"
#include "module_render_gl_material_param.h"
#include "module_render_gl_matrix_get.h"
#include "module_render_gl_matrix_multiply.h"
#include "module_render_gl_orbit_camera.h"
#include "module_render_gl_ortho_2d.h"
#include "module_render_gl_rendering_mode.h"
#include "module_render_gl_rotate.h"
#include "module_render_gl_rotate_quat.h"
#include "module_render_gl_scale.h"
#include "module_render_gl_target_camera.h"
#include "module_render_gl_translate.h"
#include "module_render_gl_texture_bind.h"
#include "module_render_gl_viewport_size.h"
#include "module_render_gl_texture_coord_gen.h"
#include "module_render_gl_normalize.h"
#include "module_render_gl_get_camera_orientation.h"
#include "module_render_gl_lighting_disable.h"




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
#define MOD_CM vsx_module_render_opengl_cm
#define MOD_DM vsx_module_render_opengl_dm
#define MOD_NM vsx_module_render_opengl_nm
#endif



vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch(module)
  {
    case 0 : return (vsx_module*)(new module_render_gl_blend_mode);
    case 1 : return (vsx_module*)(new module_render_gl_orbit_camera);
    case 2 : return (vsx_module*)(new module_render_gl_target_camera);
    case 3 : return (vsx_module*)(new module_render_gl_freelook_camera);
    case 4 : return (vsx_module*)(new module_render_gl_translate);
    case 5 : return (vsx_module*)(new module_render_gl_depth_buffer);
    case 6 : return (vsx_module*)(new module_render_gl_rotate);
    case 7 : return (vsx_module*)(new module_render_gl_light_directional);
    case 8 : return (vsx_module*)(new module_render_gl_rendering_mode);
    case 9 : return (vsx_module*)(new module_render_gl_material_param);
    case 10: return (vsx_module*)(new module_render_gl_scale);
    case 11: return (vsx_module*)(new module_render_gl_matrix_multiply);
    case 12: return (vsx_module*)(new module_render_gl_color);
    case 13: return (vsx_module*)(new module_render_gl_ortho_2d);
    case 14: return (vsx_module*)(new module_render_gl_fog);
    case 15: return (vsx_module*)(new module_render_gl_backface_culling);
    case 16: return (vsx_module*)(new module_render_gl_rotate_quat);
    case 17: return (vsx_module*)(new module_render_gl_normalize);
    case 18: return (vsx_module*)(new module_render_gl_matrix_get);
    case 19: return (vsx_module*)(new module_render_gl_line_width);
    case 20: return (vsx_module*)(new module_render_gl_depth_buffer_clear);
    case 21: return (vsx_module*)(new module_render_gl_depth_func);
    case 22: return (vsx_module*)(new module_render_gl_texture_bind);
    case 23: return (vsx_module*)(new module_render_gl_viewport_size);
    case 24: return (vsx_module*)(new module_render_gl_buffer_clear);
    case 25: return (vsx_module*)(new module_render_gl_texture_coord_gen);
    case 26: return (vsx_module*)(new module_render_gl_get_camera_orientation);
    case 27: return (vsx_module*)(new module_render_gl_frustum);
    case 28: return (vsx_module*)(new module_render_gl_lighting_disable);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module)
{
  switch(module)
  {
    case 0: delete (module_render_gl_blend_mode*)m; break;
    case 1: delete (module_render_gl_orbit_camera*)m; break;
    case 2: delete (module_render_gl_target_camera*)m; break;
    case 3: delete (module_render_gl_freelook_camera*)m; break;
    case 4: delete (module_render_gl_translate*)m; break;
    case 5: delete (module_render_gl_depth_buffer*)m; break;
    case 6: delete (module_render_gl_rotate*)m; break;
    case 7: delete (module_render_gl_light_directional*)m; break;
    case 8: delete (module_render_gl_rendering_mode*)m; break;
    case 9: delete (module_render_gl_material_param*)m; break;
    case 10: delete (module_render_gl_scale*)m; break;
    case 11: delete (module_render_gl_matrix_multiply*)m; break;
    case 12: delete (module_render_gl_color*)m; break;
    case 13: delete (module_render_gl_ortho_2d*)m; break;
    case 14: delete (module_render_gl_fog*)m; break;
    case 15: delete (module_render_gl_backface_culling*)m; break;
    case 16: delete (module_render_gl_rotate_quat*)m; break;
    case 17: delete (module_render_gl_normalize*)m; break;
    case 18: delete (module_render_gl_matrix_get*)m; break;
    case 19: delete (module_render_gl_line_width*)m; break;
    case 20: delete (module_render_gl_depth_buffer_clear*)m; break;
    case 21: delete (module_render_gl_depth_func*)m; break;
    case 22: delete (module_render_gl_texture_bind*)m; break;
    case 23: delete (module_render_gl_viewport_size*)m; break;
    case 24: delete (module_render_gl_buffer_clear*)m; break;
    case 25: delete (module_render_gl_texture_coord_gen*)m; break;
    case 26: delete (module_render_gl_get_camera_orientation*)m; break;
    case 27: delete (module_render_gl_frustum*)m; break;
    case 28: delete (module_render_gl_lighting_disable*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment)
{
  VSX_UNUSED(environment);

  // on windows glewInit has to be run per DLL
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    #ifndef VSXU_STATIC
      glewInit();
    #endif
  #endif

  return 29;
}

