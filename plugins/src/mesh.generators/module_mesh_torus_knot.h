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


class module_mesh_torus_knot : public vsx_module
{
public:
  // in
  vsx_module_param_float* num_sectors;
  vsx_module_param_float* num_stacks;
  vsx_module_param_float* p;
  vsx_module_param_float* q;
  vsx_module_param_float* phi_offset;
  vsx_module_param_float* z_length;
  vsx_module_param_sequence* param_x_shape;
  vsx_module_param_sequence* param_y_shape;
  vsx_module_param_sequence* param_z_shape;
  vsx_module_param_sequence* param_size_shape_x;
  vsx_module_param_sequence* param_size_shape_y;
  vsx_module_param_float* size_shape_x_multiplier;
  vsx_module_param_float* size_shape_y_multiplier;
  // out
  vsx_module_param_mesh* result;
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



  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_torus_knot";
    info->description = "";
    info->in_param_spec = "num_sectors:float?min=2,"
        "num_stacks:float?min=2,"
        "p:float,"
        "q:float,"
        "phi_offset:float,"
        "size:complex{"
        "size_shape_x:sequence,"
        "size_shape_x_multiplier:float,"
        "size_shape_y:sequence,"
        "size_shape_y_multiplier:float"
        "}"
        ;
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    param_x_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "x_shape");
    param_y_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "y_shape");
    param_z_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "z_shape");
    param_x_shape->set(seq_x_shape);
    param_y_shape->set(seq_y_shape);
    param_z_shape->set(seq_z_shape);

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

    p = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"p");
    p->set(4.0f);
    q = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"q");
    q->set(3.0f);
    phi_offset = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"phi_offset");
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");

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

    if (param_updates == 0)
      return;

    param_updates = 0;

    int new_num_stacks = (int)num_stacks->get();
    int new_num_sectors = (int)num_sectors->get();

    mesh->data->reset();
    calc_shapes();

    current_num_sectors = new_num_sectors;
    current_num_stacks = new_num_stacks;

    float size_shape_x_multiplier_f = size_shape_x_multiplier->get();
    float size_shape_y_multiplier_f = size_shape_y_multiplier->get();


    int vi = 0; // vertex index

    float Q = q->get();
    float P = p->get();
    float phiofs = phi_offset->get();

    mesh->data->faces.reset_used();
    float one_div_num_stacks = 1.0f / (float)(current_num_stacks);

    for(int i = 0; i < current_num_stacks; i++)
    {
      // banana extends in z direction
      // x and y are the roundness
      float ip = (float)i * one_div_num_stacks;
      float ip2 = (float)(i+1) * one_div_num_stacks;
      float phi = TWO_PI * ip;
      float phi2 = TWO_PI * ip2;

      // knot vertex pos

      float r = 0.5f * (2.0f + sin( Q * phi ) );



      int index8192 = (int)round(8192.0f*ip) % 8192;

      vsx_vector circle_base_pos = vsx_vector(
                                                r * cos ( P * phi + phiofs),
                                                r * cos ( Q * phi + phiofs),
                                                r * sin ( P * phi + phiofs)
                                                );
      vsx_vector circle_base_pos_phi2 = vsx_vector(
                                                r * cos ( P * phi2 +phiofs),
                                                r * cos ( Q * phi2 +phiofs),
                                                r * sin ( P * phi2 +phiofs)
                                                );

      // rotation calculation
      vsx_vector T = circle_base_pos_phi2 - circle_base_pos;
      vsx_vector N = circle_base_pos_phi2 + circle_base_pos;
      vsx_vector B;
      B.cross(T, N);
      N.cross(B, T);
      B.normalize();
      N.normalize();


      float j1_div_num_sectors = 1.0f / (float)(current_num_sectors);
      int j;
      for(j = 0; j < current_num_sectors; j++)
      {
        double j1 = (float)j * j1_div_num_sectors;

        float px = cos(j1 * TWO_PI) * size_shape_x[index8192] * size_shape_x_multiplier_f;
        float py = sin(j1 * TWO_PI) * size_shape_y[index8192] * size_shape_y_multiplier_f;

        vsx_vector tmp_vec(
            circle_base_pos.x,
            circle_base_pos.y,
            circle_base_pos.z
        );

        tmp_vec += N * px + B * py;

        mesh->data->vertices[vi] = tmp_vec;
        mesh->data->vertex_normals[vi] = tmp_vec - circle_base_pos;
        mesh->data->vertex_normals[vi].normalize();
        mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);

        if (i && j)
        {
          vsx_face a;

          a.c = vi - current_num_sectors;
          a.b = vi - current_num_sectors-1;
          a.a = vi-1;

          mesh->data->faces.push_back(a);

          a.c = vi - current_num_sectors;
          a.b = vi-1;
          a.a = vi;
          mesh->data->faces.push_back(a);
        }
        vi++;
      }
      if (i > 1 && i < current_num_stacks-1)
      {
        vsx_face a;
       {
          a.c = vi - current_num_sectors ;
          a.b = vi - current_num_sectors - 1;
          a.a = vi - 1;
          mesh->data->faces.push_back(a);
        }
        {
          a.b = vi - current_num_sectors;
          a.c = vi;
          a.a = vi - 1;
          mesh->data->faces.push_back(a);
        }
      }
    }

    for(int j = 0; j < current_num_sectors-1; j++)
    {
      if (j)
      {
        vsx_face a;
        a.c = vi - current_num_sectors;
        a.b = vi - current_num_sectors-1;
        a.a = j-1;
        mesh->data->faces.push_back(a);

        a.c = vi - current_num_sectors;
        a.b = j-1;
        a.a = j;
        mesh->data->faces.push_back(a);
      }
      vi++;
    }
    {
      vsx_face a;
      a.c = vi - current_num_sectors ;
      a.b = vi - current_num_sectors - 1;
      a.a = current_num_sectors - 1;
      mesh->data->faces.push_back(a);
    }

    mesh->timestamp++;
    result->set_p(mesh);
  }

};
