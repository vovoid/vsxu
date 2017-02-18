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


class module_mesh_needle : public vsx_module {
  // in
  vsx_module_param_float* num_points;
  vsx_module_param_float* size;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;

public:
  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;vertices;needle";

    info->description = "";

    info->out_param_spec = "mesh:mesh";

    info->in_param_spec =
      "num_points:float,"
      "size:float"
    ;

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    num_points = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_points");
    size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"size");
    num_points->set(5);
    size->set(1);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
  }

  bool init()
  {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void run()
  {
    if (!param_updates) return;  param_updates = 0;
    float istart = 0;
    for (int i = 0; i < 5; ++i) {
      mesh->data->vertices[i].x = istart;
      mesh->data->vertices[i].y = 0;
      mesh->data->vertices[i].z = 0;
      mesh->data->vertex_colors[i].r = 1;
      mesh->data->vertex_colors[i].g = size->get()/5.0f;
      mesh->data->vertex_colors[i].b = 1;
      mesh->data->vertex_colors[i].a = 1;
      istart += size->get()/5.0f;
    }
    mesh->timestamp++;

  }
};
