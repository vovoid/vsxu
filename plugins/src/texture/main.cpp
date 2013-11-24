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
#include "pthread.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include <vector>
#include <map>
#include "vsx_glsl.h"
#include <vsx_texture.h>
#include <vsxg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct stat t_stat;


#include "module_texture_effect_blur.h"
#include "module_texture_parameter.h"
#include "module_texture_render_buffer.h"
#include "module_texture_render_surface_color_buffer.h"
#include "module_texture_render_surface_color_depth_buffer.h"
#include "module_texture_render_surface_single.h"
#include "module_texture_rotate.h"
#include "module_texture_scale.h"
#include "module_texture_translate.h"
#include "module_texture_visual_fader.h"
#include "module_texture_load_bitmap2texture.h"
#include "module_texture_load_jpeg_alpha.h"
#include "module_texture_load_jpeg.h"
#include "module_texture_load_png_cubemap.h"
#include "module_texture_load_png.h"
#include "module_texture_load_openexr.h"


//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  glewInit();
  switch(module) {
    case 0:
    case 1:
    {
      module_texture_load_png* m = new module_texture_load_png;
      m->m_type = module % 2;
      return (vsx_module*)m;
    }
    case 2:
    case 3:
    {
      module_texture_load_jpeg* m2 = new module_texture_load_jpeg;
      m2->m_type = module % 2;
      return (vsx_module*)m2;
    }
    case 4:
    case 5:
    {
      module_texture_load_jpeg_alpha* m2 = new module_texture_load_jpeg_alpha;
      m2->m_type = module % 2;
      return (vsx_module*)m2;
    }
    case 6:
    return (vsx_module*)(new module_texture_load_bitmap2texture);
    case 7:
    return (vsx_module*)(new module_texture_load_png_cubemap);
    case 8: return (vsx_module*)(new vsx_module_rendered_texture_single);
    case 9: return (vsx_module*)(new module_texture_translate);
    case 10: return (vsx_module*)(new module_texture_scale);
    case 11: return (vsx_module*)(new module_texture_rotate);
    case 12: return (vsx_module*)(new module_texture_parameter);
    case 13: return (vsx_module*)(new module_texture_effect_blur);
    case 14: return (vsx_module*)(new module_texture_visual_fader);
    case 15: return (vsx_module*)(new module_texture_render_surface_color_depth_buffer);
    case 16: return (vsx_module*)(new module_texture_render_surface_color_buffer);
    case 17: return (vsx_module*)(new module_texture_render_buffer);
    case 18: return (vsx_module*)(new module_texture_load_openexr);
  };
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: case 1: delete (module_texture_load_png*)m; break;
    case 2: case 3: delete (module_texture_load_jpeg*)m; break;
    case 4: case 5: delete (module_texture_load_jpeg_alpha*)m; break;
    case 6: delete (module_texture_load_bitmap2texture*)m; break;
    case 7: delete (module_texture_load_png_cubemap*)m; break;
    case 8: delete (vsx_module_rendered_texture_single*)m; break;
    case 9: delete (module_texture_translate*)m; break;
    case 10: delete (module_texture_scale*)m; break;
    case 11: delete (module_texture_rotate*)m; break;
    case 12: delete (module_texture_parameter*)m; break;
    case 13: delete (module_texture_effect_blur*)m; break;
    case 14: delete (module_texture_visual_fader*)m; break;
    case 15: delete (module_texture_render_surface_color_depth_buffer*)m; break;
    case 16: delete (module_texture_render_surface_color_buffer*)m; break;
    case 17: delete (module_texture_render_buffer*)m; break;
    case 18: delete (module_texture_load_openexr*)m; break;
  }
}

unsigned long get_num_modules() {
  return 19;

}

