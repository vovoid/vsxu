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


class module_mesh_abstract_hand : public vsx_module
{
  // in
  vsx_module_param_float* num_sectors;
  vsx_module_param_float* num_stacks;
  vsx_module_param_float* z_length;
  vsx_module_param_sequence* param_x_shape;
  vsx_module_param_sequence* param_y_shape;
  vsx_module_param_sequence* param_z_shape;
  vsx_module_param_sequence* param_size_shape_x;
  vsx_module_param_sequence* param_size_shape_y;
  vsx_module_param_float* x_shape_multiplier;
  vsx_module_param_float* y_shape_multiplier;
  vsx_module_param_float* z_shape_multiplier;
  vsx_module_param_float* size_shape_x_multiplier;
  vsx_module_param_float* size_shape_y_multiplier;

  // out
  vsx_module_param_mesh* result;
  vsx_module_param_float* last_vertex_index;

  // internal
  vsx_mesh* mesh;
  int l_param_updates;
  int current_num_stacks;
  int current_num_sectors;

  // x_shape
  vsx_sequence seq_x_shape;
  float x_shape[8192];
  // y_shape
  vsx_sequence seq_y_shape;
  float y_shape[8192];
  // z_shape
  vsx_sequence seq_z_shape;
  float z_shape[8192];
  // size_shape_x
  vsx_sequence seq_size_shape_x;
  float size_shape_x[8192];
  // size_shape_y
  vsx_sequence seq_size_shape_y;
  float size_shape_y[8192];


  void calc_shapes()
  {
    #define CALCS(var_name) \
    if (param_##var_name->updates)\
    {\
      seq_##var_name = param_##var_name->get();\
      param_##var_name->updates = 0;\
      seq_##var_name.reset();\
      for (int i = 0; i < 8192; ++i) {\
        var_name[i] = seq_##var_name.execute(1.0f/8192.0f);\
      }\
    }

    CALCS(size_shape_x);
    CALCS(size_shape_y);
    CALCS(x_shape);
    CALCS(y_shape);
    CALCS(z_shape);

    #undef CALCS
  }


public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_super_banana";

    info->description = "";

    info->in_param_spec =
      "num_sectors:float?min=2,"
      "num_stacks:float?min=2,"
      "shape:complex"
      "{"
        "x_shape:sequence,"
        "x_shape_multiplier:float,"
        "y_shape:sequence,"
        "y_shape_multiplier:float,"
        "z_shape:sequence,"
        "z_shape_multiplier:float"
      "},"
      "size:complex"
      "{"
        "size_shape_x:sequence,"
        "size_shape_x_multiplier:float,"
        "size_shape_y:sequence,"
        "size_shape_y_multiplier:float"
      "}"
    ;

    info->out_param_spec =
      "mesh:mesh,"
      "last_vertex_index:float"
    ;
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    param_x_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "x_shape");
    param_x_shape->set(seq_x_shape);

    param_y_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "y_shape");
    param_y_shape->set(seq_y_shape);

    param_z_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "z_shape");
    param_z_shape->set(seq_z_shape);


    x_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "x_shape_multiplier"); x_shape_multiplier->set(1.0f);
    y_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "y_shape_multiplier"); y_shape_multiplier->set(1.0f);
    z_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "z_shape_multiplier"); z_shape_multiplier->set(1.0f);

    size_shape_x_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size_shape_x_multiplier"); size_shape_x_multiplier->set(1.0f);
    size_shape_y_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size_shape_y_multiplier"); size_shape_y_multiplier->set(1.0f);

    param_size_shape_x = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "size_shape_x");
    param_size_shape_x->set(seq_size_shape_x);

    param_size_shape_y = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "size_shape_y");
    param_size_shape_y->set(seq_size_shape_y);

    num_sectors = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_sectors");
    num_sectors->set(6.0f);

    num_stacks = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_stacks");
    num_stacks->set(4.0f);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");

    last_vertex_index = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "last_vertex_index");

    current_num_stacks = 0;
    current_num_sectors = 0;
  }

  bool init()
  {
    mesh = new vsx_mesh;
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

    if (param_updates == 0)
      return;

    param_updates = 0;

    mesh->data->reset();

    calc_shapes();

    current_num_sectors = new_num_sectors;
    current_num_stacks = new_num_stacks;

    float x_shape_multiplier_f = x_shape_multiplier->get();
    float y_shape_multiplier_f = y_shape_multiplier->get();
    float z_shape_multiplier_f = z_shape_multiplier->get();

    float size_shape_x_multiplier_f = size_shape_x_multiplier->get();
    float size_shape_y_multiplier_f = size_shape_y_multiplier->get();


    int vi = 0; // vertex index

    mesh->data->faces.reset_used();

    float one_div_num_stacks = 1.0f / (float)current_num_stacks;

    float one_div_num_sectors_minus_one = 1.0f / (float)(current_num_sectors - 1);

    for(int i = 0; i < current_num_stacks; i++)
    {
      // banana extends in z direction
      // x and y are the roundness
      float ip = (float)i * one_div_num_stacks;
      int index8192 = (int)round(8192.0f*ip);

      vsx_vector circle_base_pos = vsx_vector
      (
        x_shape[index8192] * x_shape_multiplier_f,
        y_shape[index8192] * y_shape_multiplier_f,
        z_shape[index8192] * z_shape_multiplier_f
      );

      int j;
      for(j = 0; j < current_num_sectors; j++)
      {
        double j1 = (float)j * one_div_num_sectors_minus_one;
        vsx_vector tmp_vec
        (
          circle_base_pos.x + cos(j1 * TWO_PI) * size_shape_x[index8192] * size_shape_x_multiplier_f,
          circle_base_pos.y + sin(j1 * TWO_PI) * size_shape_y[index8192] * size_shape_y_multiplier_f,
          circle_base_pos.z
        );
        mesh->data->vertices[vi] = tmp_vec;
        mesh->data->vertex_normals[vi] = tmp_vec - circle_base_pos;
        mesh->data->vertex_normals[vi].normalize();
        mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
        mesh->data->vertex_tex_coords[vi].s = j1;
        mesh->data->vertex_tex_coords[vi].t = ip;

        if (i && j)
        {
          vsx_face a;
          a.a = vi - current_num_sectors;
          a.b = vi-1;
          a.c = vi - current_num_sectors-1;
          mesh->data->faces.push_back(a);

          a.a = vi - current_num_sectors;
          a.b = vi;
          a.c = vi-1;
          mesh->data->faces.push_back(a);
        }
        vi++;
      }
    }
    last_vertex_index->set( (float)vi );
    mesh->timestamp++;
    result->set_p(mesh);
  }
};
