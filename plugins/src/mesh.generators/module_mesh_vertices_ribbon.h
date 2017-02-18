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


class module_mesh_vertices_ribbon : public vsx_module
{
public:
  // in
  vsx_module_param_float3* start_point;
  vsx_module_param_float3* end_point;
  vsx_module_param_float3* up_vector;
  vsx_module_param_float* num_segments;
  vsx_module_param_float* particle_scale;
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
      "mesh;vertices;ribbon_vertices";

    info->description =
      "Useful for feeding particle cloud.";

    info->in_param_spec =
      "start_point:float3,"
      "end_point:float3,"
      "up_vector:float3,"
      "num_segments:float,"
      "particle_scale:float,"
      "width:float,"
      "skew_amp:float,"
      "time_amp:float"
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
    start_point = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"start_point");
    end_point = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"end_point");
    up_vector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"up_vector");
    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"width");

    num_segments = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_segments");
    num_segments->set(20.0);
    particle_scale = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particle_scale");
    particle_scale->set(1.0);

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

    float p_scale = particle_scale->get();

    float COUNT = num_segments->get();
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
      int i2 = i;
      float it = (float)i * one_div_count;
      float ft = sin(it * 3.14159f + t) * sin(-it * 5.18674f - t);
      float thick = (float)fabs(sin(it * 3.14159f + t * 0.5));
      vsx_vector3<> skew = up * ft * skew_amount * thick;
      mesh->data->vertices[i2    ] = pos + up * thick + skew;
      mesh->data->vertex_normals[i2    ] = normal;
      pos += diff;
      mesh->data->vertex_colors[i2] = vsx_color<>(thick * p_scale, thick * p_scale, thick * p_scale, 1.0);
      mesh->data->vertex_tex_coords[i2]   = vsx_tex_coord2f(it, 0);
    }

    mesh->timestamp++;
    result->set(mesh);
  }
};
