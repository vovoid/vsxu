/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
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

#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "marching_cubes.h"
#include "metaballs.h"
#include "vsx_math_3d.h"
#include "vsx_timer.h"


class vsx_module_metaballs : public vsx_module {
  // in
	vsx_module_param_float* grid_size;
	vsx_module_param_float4* center_color;
	CMetaballs balls;
	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
	bool first_run;
	int i_grid_size;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;metaballs";
    info->description = "Generates a metaballs mesh";
    info->in_param_spec = "grid_size:float?nc=1";
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    grid_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"grid_size");
    grid_size->set(20.0f);
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    loading_done = true;
    first_run = true;
  }

  bool init() {
    mesh = new vsx_mesh;
    balls.vertices = &(mesh->data->vertices);
    balls.vertex_normals = &(mesh->data->vertex_normals);
    balls.vertex_tex_coords = &(mesh->data->vertex_tex_coords);
    balls.faces = &(mesh->data->faces);
    CMarchingCubes::BuildTables();
    balls.SetGridSize(20);
    i_grid_size = 20;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void run() {
    int l_grid_size = (int)floor(grid_size->get());
    if (i_grid_size != l_grid_size) {
      balls.SetGridSize(l_grid_size);
      i_grid_size = l_grid_size;
    }
    if (engine->dtime != 0.0f) {
      float dd = engine->dtime;
      if (dd < 0) dd = 0;
    vsx_timer timer;
    timer.start();
    balls.Update(dd);

      //printf("update took: %f s\n", timer.dtime());
    timer.start();
    balls.Render();

    //printf("render took: %f s\n", timer.dtime());
    mesh->timestamp++;
    }
    result->set_p(mesh);
  }
};






//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}

vsx_module* create_new_module(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_metaballs);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_metaballs*)m; break;
  }
}


unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 1;
}


