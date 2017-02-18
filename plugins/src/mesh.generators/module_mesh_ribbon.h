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


class module_mesh_ribbon : public vsx_module
{
public:
  // in
  vsx_module_param_float3* start_point;
  vsx_module_param_float3* end_point;
  vsx_module_param_float3* up_vector;
  vsx_module_param_float* width;
  vsx_module_param_float* skew_amp;
  vsx_module_param_float* time_amp;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  int l_param_updates;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;generators;ribbon";

    info->description = "";

    info->in_param_spec =
      "start_point:float3,"
      "end_point:float3,"
      "up_vector:float3,"
      "width:float,"
      "skew_amp:float,"
      "time_amp:float"
    ;

    info->out_param_spec = "mesh:mesh";

    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;
    start_point = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"start_point");
    end_point = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"end_point");
    up_vector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"up_vector");
    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"width");
    skew_amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"skew_amp");
    skew_amp->set(1.0f);
    time_amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"time_amp");
    time_amp->set(1.0f);
    width->set(0.1f);
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    mesh = new vsx_mesh<>;
  }

  bool init()
  {
    return true;
  }

  void on_delete()
  {
    if (mesh)
    delete mesh;
    mesh = 0;
  }

  void run()
  {
    mesh->data->vertices[0] = vsx_vector3<>(0.0f);

    vsx_vector3<> a(start_point->get(0), start_point->get(1), start_point->get(2));
    vsx_vector3<> b(end_point->get(0), end_point->get(1), end_point->get(2));
    vsx_vector3<> up(up_vector->get(0), up_vector->get(1), up_vector->get(2));
    up *= width->get();


    vsx_vector3<> pos = a;
    vsx_vector3<> diff = b-a;
    vsx_vector3<> diff_n = diff;
    diff_n.normalize();

    vsx_vector3<> normal;
    vsx_vector3<> up_n = up;
    up_n.normalize();
    normal.cross(diff_n, up_n);

    vsx_vector3<> up_side = normal;
    up_side *= up.length();

    float t = engine_state->vtime * time_amp->get();

    #define COUNT 20.0f
    diff *= 1.0f / COUNT;
    float skew_amount = skew_amp->get();
    //     i=0   1   2   3   4   5   6   7   8   9
    // /\    0   2   4   6   8   10  12  14  16  18
    // ||    x---x---x---x---x---x---x---x---x---x
    // up   a|0/1|2/3| / | / | / | / | / | / | / |b  ----> diff
    //       x---x---x---x---x---x---x---x---x---x
    //       1   3   5   7   9   11  13  15  17  19

    float one_div_count = 1.0f / COUNT;
    mesh->data->faces.reset_used();
    for (int i = 0; i < (int)COUNT; i++)
    {
      int i2 = i << 2;
      float it = (float)i * one_div_count;
      float ft = sin(it * 3.14159f + t) * sin(-it * 5.18674f - t);
      float thick = sin(it * 3.14159f);
      vsx_vector3<> skew = up * ft * skew_amount * thick;

      mesh->data->vertices[i2    ] = pos + up * thick + skew;
      mesh->data->vertices[i2 + 1] = pos - up * thick + skew;

      mesh->data->vertices[i2 + 2] = pos + skew + up_side * thick;
      mesh->data->vertices[i2 + 3] = pos + skew - up_side * thick;

      mesh->data->vertex_normals[i2    ] = normal;
      mesh->data->vertex_normals[i2 + 1] = normal;
      mesh->data->vertex_normals[i2 + 2] = normal;
      mesh->data->vertex_normals[i2 + 3] = normal;

      pos += diff;

      mesh->data->vertex_colors[i2] = vsx_color<>(1, 1, 1, 1);
      mesh->data->vertex_colors[i2+1] = vsx_color<>(1, 1, 1, 1);
      mesh->data->vertex_colors[i2+2] = vsx_color<>(1, 1, 1, 1);
      mesh->data->vertex_colors[i2+3] = vsx_color<>(1, 1, 1, 1);

      mesh->data->vertex_tex_coords[i2]   = vsx_tex_coord2f(it, 0);
      mesh->data->vertex_tex_coords[i2+1] = vsx_tex_coord2f(it, 1);
      mesh->data->vertex_tex_coords[i2+2] = vsx_tex_coord2f(it, 0);
      mesh->data->vertex_tex_coords[i2+3] = vsx_tex_coord2f(it, 1);

      if (!i) continue;

      vsx_face3 a;
      a.a = i2;
      a.b = i2 - 3;
      a.c = i2 - 4;
      mesh->data->faces.push_back(a);

      a.a = i2;
      a.b = i2 + 1;
      a.c = i2 - 3;
      mesh->data->faces.push_back(a);

      a.a = i2 + 2;
      a.b = i2 - 1;
      a.c = i2 - 2;
      mesh->data->faces.push_back(a);

      a.a = i2 + 2;
      a.b = i2 + 3;
      a.c = i2 - 1;
      mesh->data->faces.push_back(a);

    }
    #undef COUNT

    mesh->timestamp++;
    result->set(mesh);
  }
};
