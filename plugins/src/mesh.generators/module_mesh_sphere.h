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


class module_mesh_sphere : public vsx_module
{
public:
  // in
  vsx_module_param_float* num_sectors;
  vsx_module_param_float* num_stacks;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  int l_param_updates;
  int current_num_stacks;
  int current_num_sectors;


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;solid;mesh_sphere";

    info->description =
      "U/V Sphere generatr";

    info->in_param_spec =
      "num_sectors:float?min=2,"
      "num_stacks:float?min=2"
    ;

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    num_sectors = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_sectors");
    num_sectors->set(6.0f);

    num_stacks = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_stacks");
    num_stacks->set(4.0f);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    current_num_stacks = 0;
    current_num_sectors = 0;
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
    int new_num_stacks = (int)num_stacks->get();
    int new_num_sectors = (int)num_sectors->get();

    if (current_num_stacks == new_num_stacks && current_num_sectors == new_num_sectors) return;

    mesh->data->reset();

    current_num_sectors = new_num_sectors;
    current_num_stacks = new_num_stacks;

    int vi = 0; // vertex index

    for(int i = 1; i < current_num_stacks; i++) {
      double angle_stack = (double)i / current_num_stacks * PI;
      float rad = (float)sin(angle_stack);
      float y = (float)cos(angle_stack);
      for(int j = 0; j < current_num_sectors; j++) {
        double angle = (double)j / current_num_sectors * 2 * PI;
        vsx_vector3<> tmp_vec((float)sin(angle) * rad, y, (float)cos(angle) * rad);
        mesh->data->vertices[vi] = tmp_vec;
        mesh->data->vertex_normals[vi] = tmp_vec;
        mesh->data->vertex_colors[vi] = vsx_color<>(1, 1, 1, 1);
        vi++;
      }
    }

    vsx_vector3<> tmp_vec = vsx_vector3<>(0, 1, 0);
    mesh->data->vertices[vi] = tmp_vec;
    mesh->data->vertex_normals[vi] = tmp_vec;
    mesh->data->vertex_colors[vi] = vsx_color<>(1, 1, 1, 1);
    vi++;
    tmp_vec = vsx_vector3<>(0, -1, 0);
    mesh->data->vertices[vi] = tmp_vec;
    mesh->data->vertex_normals[vi] = tmp_vec;
    mesh->data->vertex_colors[vi] = vsx_color<>(1, 1, 1, 1);
    vi++;

    for(int i = 0; i < current_num_stacks - 2; i++) {
      for(int j = 0; j < current_num_sectors; j++) {
        vsx_face3 a;
        a.a = i * current_num_sectors + j;
        a.b = (i + 1) * current_num_sectors + j;
        a.c = i * current_num_sectors + ((j + 1) % current_num_sectors);
        mesh->data->faces.push_back(a);
        a.a = i * current_num_sectors + ((j + 1) % current_num_sectors);
        a.b = (i + 1) * current_num_sectors + j;
        a.c = (i + 1) * current_num_sectors + ((j + 1) % current_num_sectors);
        mesh->data->faces.push_back(a);
      }
    }

    for(int j = 0; j < current_num_sectors; j++) {
      vsx_face3 a;
      a.a = vi - 2;
      a.b = 0 * current_num_sectors + j;
      a.c = 0 * current_num_sectors + ((j + 1) % current_num_sectors);
      mesh->data->faces.push_back(a);
      a.a = vi - 1;
      a.b = (current_num_stacks - 2) * current_num_sectors + ((j + 1) % current_num_sectors);
      a.c = (current_num_stacks - 2) * current_num_sectors + j;
      mesh->data->faces.push_back(a);
    }

    mesh->timestamp++;
    result->set_p(mesh);
  }
};
