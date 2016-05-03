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
#include <vector>
#include <map>
#include "vsx_glsl.h"
#include <texture/vsx_texture.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <texture/buffer/vsx_texture_buffer_color.h>
#include <texture/buffer/vsx_texture_buffer_color_depth.h>
#include <texture/buffer/vsx_texture_buffer_render.h>

typedef struct stat t_stat;


#include "module_texture_texgen_blob.h"
#include "module_texture_texgen_concentric_circles.h"
#include "module_texture_texgen_perlin_noise.h"
#include "module_texture_texgen_plasma.h"
#include "module_texture_texgen_subplasma.h"
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
#include "module_texture_load.h"
#include "module_texture_dummy.h"
#include "module_texture_screenshot_hires.h"


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
#define MOD_CM vsx_module_texture_cm
#define MOD_DM vsx_module_texture_dm
#define MOD_NM vsx_module_texture_nm
#endif


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch(module) {
    case 0: return (vsx_module*)(new module_texture_load("jpg_tex_load","jpg", "JPG"));
    case 1: return (vsx_module*)(new module_texture_load("tga_tex_load","tga", "TGA"));
    case 2: return (vsx_module*)(new module_texture_load("png_tex_load","png","PNG") );
    case 3: return (vsx_module*)(new module_texture_load("dds_tex_load", "dds", "DDS") );

    case 4: return (vsx_module*)(new module_texture_dummy);
    case 5: return (vsx_module*)(new module_texture_load_bitmap2texture);

    case 6: return (vsx_module*)(new vsx_module_rendered_texture_single);
    case 7: return (vsx_module*)(new module_texture_render_surface_color_depth_buffer);
    case 8: return (vsx_module*)(new module_texture_render_surface_color_buffer);
    case 9: return (vsx_module*)(new module_texture_render_buffer);
    case 10: return (vsx_module*)(new module_texture_effect_blur);

    case 11: return (vsx_module*)(new module_texture_translate);
    case 12: return (vsx_module*)(new module_texture_scale);
    case 13: return (vsx_module*)(new module_texture_rotate);

    case 14: return (vsx_module*)(new module_texture_parameter);
    case 15: return (vsx_module*)(new module_texture_visual_fader);
    case 16: return (vsx_module*)(new module_texture_screenshot_hires);

    case 17: return (vsx_module*)(new module_texture_texgen_blob);
    case 18: return (vsx_module*)(new module_texture_texgen_concentric_circles);
    case 19: return (vsx_module*)(new module_texture_texgen_perlin_noise);
    case 20: return (vsx_module*)(new module_texture_texgen_plasma);
    case 21: return (vsx_module*)(new module_texture_texgen_subplasma);
  };

  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0:
    case 1:
    case 2:
    case 3:
      delete (module_texture_load*)m; break;

    case 4: delete (module_texture_dummy*)m; break;
    case 5: delete (module_texture_load_bitmap2texture*)m; break;


    case 6: delete (vsx_module_rendered_texture_single*)m; break;
    case 7: delete (module_texture_render_surface_color_depth_buffer*)m; break;
    case 8: delete (module_texture_render_surface_color_buffer*)m; break;
    case 9: delete (module_texture_render_buffer*)m; break;
    case 10: delete (module_texture_effect_blur*)m; break;

    case 11: delete (module_texture_translate*)m; break;
    case 12: delete (module_texture_scale*)m; break;
    case 13: delete (module_texture_rotate*)m; break;

    case 14: delete (module_texture_parameter*)m; break;
    case 15: delete (module_texture_visual_fader*)m; break;
    case 16: delete (module_texture_screenshot_hires*)m; break;

    case 17: delete (module_texture_texgen_blob*)m; break;
    case 18: delete (module_texture_texgen_concentric_circles*)m; break;
    case 19: delete (module_texture_texgen_perlin_noise*)m; break;
    case 20: delete (module_texture_texgen_plasma*)m; break;
    case 21: delete (module_texture_texgen_subplasma*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment) {
  VSX_UNUSED(environment);

  // on windows glewInit has to be run per DLL
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    #ifndef VSXU_STATIC
      glewInit();
    #endif
  #endif

  return 22;
}

