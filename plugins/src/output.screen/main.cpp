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
#include "vsx_module.h"
#ifdef _WIN32
#include "windows.h"
#include "wingdi.h"
#endif

#ifdef VSXU_MAC_XCODE
#include <syslog.h>
#endif

#include <vsx_tm.h>

typedef struct {
  unsigned short r[256];
  unsigned short g[256];
  unsigned short b[256];
} gamma_s;


class vsx_module_output_screen : public vsx_module {
  float internal_gamma;
	
  vsx_module_param_render* my_render;
  vsx_module_param_float* gamma_correction;
	vsx_module_param_float4* clear_color;

  float pre_material_colors[5][2][4];

  // don't mess with viewport or any opengl settings
  vsx_module_param_int* opengl_silent;
  
public:
	  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "outputs;screen";
    info->output = 1;
    info->description =
      "The screen is the visual output of VSXu.\n"
      "Components that are not in a chain connected\n"
      "in the end to the server will not be run.\n"
    ;
    info->in_param_spec = "screen:render,"
                          "gamma_correction:float?max=4&min=0&nc=1,"
                          "clear_color:float4?nc=1";
    info->component_class = "screen";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(out_parameters);
    loading_done = true;
    my_render = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER, "screen");

    opengl_silent = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "opengl_silent");
    opengl_silent->set(0);

    gamma_correction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"gamma_correction");
    gamma_correction->set(1.0f);
    internal_gamma = 1.0f;

    clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"clear_color");
    clear_color->set(0.0f,0);
    clear_color->set(0.0f,1);
    clear_color->set(0.0f,2);
    clear_color->set(1.0f,3);
  }

  void set_gamma(float mgamma)
  {
  #ifdef _WIN32
   HDC hdc = wglGetCurrentDC();
    gamma_s mygamma;
    gamma_s* mgp = &mygamma;
    
    for (int i = 0; i < 256; ++i) {
      int v = (int)round((double)255 * pow(((double)i)/(double)255, mgamma));
      if (v > 255) v = 255;
      // You can adjust each curve separately, but you usually shouldn't need to.
      mygamma.r[i] = v << 8;
      mygamma.g[i] = v << 8;
      mygamma.b[i] = v << 8;
    }  
    SetDeviceGammaRamp(hdc, (void*)mgp);
  #else
    VSX_UNUSED(mgamma);
  #endif
 }
 
  bool activate_offscreen()
  {
    if (opengl_silent->get() == 1) return true;
    if (internal_gamma != gamma_correction->get()) {
      internal_gamma = gamma_correction->get();
      set_gamma(internal_gamma);
    }

    // start by clearing the screen
    glClearColor(clear_color->get(0),clear_color->get(1),clear_color->get(2),clear_color->get(3));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    // identity matrices
    engine->gl_state->matrix_load_identities();

    // set up blending
    engine->gl_state->blend_set(1);
    engine->gl_state->blend_func(VSX_GL_SRC_ALPHA, VSX_GL_ONE_MINUS_SRC_ALPHA);

    // set up polygon mode
    engine->gl_state->polygon_mode_set(VSX_GL_FRONT, VSX_GL_FILL);
    engine->gl_state->polygon_mode_set(VSX_GL_BACK, VSX_GL_FILL);

    // set up depth test & mask
    engine->gl_state->depth_mask_set(0);
    engine->gl_state->depth_test_set(0);
    engine->gl_state->depth_function_set( VSX_GL_LESS );

    // set up line width
    engine->gl_state->line_width_set( 1.0f );

    const unsigned int lights[] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
    glDisable(lights[0]);
    glDisable(lights[1]);
    glDisable(lights[2]);
    glDisable(lights[3]);
    glDisable(lights[4]);
    glDisable(lights[5]);
    glDisable(lights[6]);
    glDisable(lights[7]);

    engine->gl_state->get_material_fv_all( &pre_material_colors[0][0][0] );

    // Implement default material settings
    engine->gl_state->material_set_default();


    return true;
  }

  void deactivate_offscreen()
  {
    if (opengl_silent->get() == 1) return;
    #ifndef VSXU_OPENGL_ES_2_0
      engine->gl_state->material_set_fv_all(&pre_material_colors[0][0][0]);
    #endif
    glClearColor(0.0f,0.0f,0.0f,0.0f);
  }

  void stop()
  {
    set_gamma(1.0f);
  }
};




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


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_output_screen);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_output_screen*)m; break;
  }
}

unsigned long MOD_NM() {
  return 1;
}

