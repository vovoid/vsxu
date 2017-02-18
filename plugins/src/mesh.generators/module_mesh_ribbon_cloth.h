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


class module_mesh_ribbon_cloth : public vsx_module
{
  // in
  vsx_module_param_float3* start_point;
  vsx_module_param_float3* end_point;
  vsx_module_param_float3* up_vector;
  vsx_module_param_float* width;
  vsx_module_param_float* skew_amp;
  vsx_module_param_float* time_amp;
  vsx_module_param_float* damping_factor;
  vsx_module_param_float* step_size;
  vsx_module_param_float* stiffness;
  vsx_module_param_float* floor_y;
  vsx_module_param_float* gravity_amount;
  vsx_module_param_float* length;
  vsx_module_param_float* edge_treshold;
  vsx_module_param_int* reinit;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  int l_param_updates;
  bool regen;
  vsx_ma_vector< vsx_vector3<> > face_lengths;
  vsx_ma_vector< vsx_vector3<> > vertices_speed;
  vsx_ma_vector< vsx_vector3<> > vertices_orig;
  int num_runs;
  vsx_vector3<> prev_pos;

public:

  bool init()
  {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;generators;ribbon_cloth";

    info->description = "";

    info->in_param_spec =
      "start_point:float3,"
      "end_point:float3,"
      "options:complex{"
        "up_vector:float3,"
        "width:float,"
        "skew_amp:float,"
        "time_amp:float,"
        "damping_Factor:float,"
        "step_size:float,"
        "stiffness:float,"
        "floor_y:float,"
        "gravity_amount:float,"
        "length:float,"
        "edge_treshold:float"
      "},"
      "reinit:enum?no|yes"
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

    damping_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"damping_Factor");
    damping_factor->set(0.5f);

    step_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"step_size");
    step_size->set(1.0f);

    stiffness = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"stiffness");
    stiffness->set(0.8f);

    up_vector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"up_vector");
    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"width");
    skew_amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"skew_amp");
    skew_amp->set(1.0f);

    floor_y = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"floor_y");
    floor_y->set(0.0f);

    time_amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"time_amp");
    time_amp->set(1.0f);
    width->set(0.1f);


    gravity_amount = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"gravity_amount");
    gravity_amount->set(1.0f);

    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"length");
    length->set(20.0f);

    edge_treshold = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"edge_treshold");
    edge_treshold->set(0.04f);


    reinit = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reinit");
    reinit->set(0);


    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    regen = true;
    num_runs = 0;
  }



  void run()
  {
    mesh->data->vertices[0] = vsx_vector3<>(0.0f);

    if (reinit->get())
    {
      regen = true;
      reinit->set(0);
    }

    vsx_vector3<> a(start_point->get(0), start_point->get(1), start_point->get(2));
    vsx_vector3<> b(end_point->get(0), end_point->get(1), end_point->get(2));


    //     i=0   1   2   3   4   5   6   7   8   9
    // /\    0   2   4   6   8   10  12  14  16  18
    // ||    x---x---x---x---x---x---x---x---x---x
    // up   a|0/1|2/3| / | / | / | / | / | / | / |b  ----> diff
    //       x---x---x---x---x---x---x---x---x---x
    //       1   3   5   7   9   11  13  15  17  19


    if (regen)
    {
      vsx_vector3<> up(up_vector->get(0), up_vector->get(1), up_vector->get(2));
      up *= width->get();

      vsx_vector3<> pos = vsx_vector3<>(0.0f,0.0f,0.0f);
      vsx_vector3<> diff = b-a;
      vsx_vector3<> diff_n = diff;
      diff_n.normalize();

      vsx_vector3<> normal;
      vsx_vector3<> up_n = up;
      up_n.normalize();
      normal.cross(diff_n, up_n);

      float t = engine_state->vtime * time_amp->get();

      diff *= width->get() * (0.4f*0.1f) / length->get();
      float skew_amount = skew_amp->get();


      prev_pos = a;
      vertices_speed.allocate((int)length->get()*4);
      for (int i = 0; i < (int)length->get()*4; i++)
      {
        vertices_speed[i].x = 0.0f;
        vertices_speed[i].y = -0.04f;
        vertices_speed[i].z = 0.0f;
      }

      regen = false;
      mesh->data->faces.reset_used();
      for (int i = 0; i < (int)length->get(); i++)
      {
        int i2 = i << 1;
        float it = (float)i / length->get();
        float ft = sin(it * 3.14159f + t) * sin(-it * 5.18674f - t);
        float thick = 0.11f * width->get();
        vsx_vector3<> skew = up * ft * skew_amount * thick;

        mesh->data->vertices[i2    ] = pos + up * thick + skew;
        mesh->data->vertices[i2 + 1] = pos - up * thick + skew;
        vertices_orig[i2] = mesh->data->vertices[i2    ];
        vertices_orig[i2+1] = mesh->data->vertices[i2   +1];

        vsx_vector3<> norm_a;
        norm_a.cross(up_n, pos);
        mesh->data->vertex_normals[i2    ] = norm_a;
        mesh->data->vertex_normals[i2 + 1] = norm_a;

        pos += diff;

        mesh->data->vertex_colors[i2] = vsx_color<>(1, 1, 1, 1);
        mesh->data->vertex_colors[i2+1] = vsx_color<>(1, 1, 1, 1);

        mesh->data->vertex_tex_coords[i2]   = vsx_tex_coord2f(it, 0);
        mesh->data->vertex_tex_coords[i2+1] = vsx_tex_coord2f(it, 1);

        vsx_vector3<> len;
        if (i>1)
        {
          vsx_face3 f;
          f.a = i2;
          f.b = i2 - 1;
          f.c = i2 - 2;
          mesh->data->faces.push_back(f);

          vsx_vector3<> v0 = mesh->data->vertices[f.a];
          vsx_vector3<> v1 = mesh->data->vertices[f.b];
          vsx_vector3<> v2 = mesh->data->vertices[f.c];

          len.x = fabs( (v1 - v0).length()/*+(float)(rand()%1000)*0.0001f*/);
          len.y = fabs( (v2 - v1).length()/*+(float)(rand()%1000)*0.0001f*/);
          len.z = fabs( (v0 - v2).length()/*+(float)(rand()%1000)*0.00005f*/);
          if (len.x < edge_treshold->get()) len.x = edge_treshold->get();
          if (len.y < edge_treshold->get()) len.y = edge_treshold->get();
          if (len.z < edge_treshold->get()) len.z = edge_treshold->get();

          face_lengths.push_back(len);

          f.a = i2-1;
          f.b = i2;
          f.c = i2+1;
          mesh->data->faces.push_back(f);

          v0 = mesh->data->vertices[f.a];
          v1 = mesh->data->vertices[f.b];
          v2 = mesh->data->vertices[f.c];

          len.x = fabs( (v1 - v0).length()/*+(float)(rand()%1000)*0.0001f */);
          len.y = fabs( (v2 - v1).length()/*+(float)(rand()%1000)*0.0001f */);
          len.z = fabs( (v0 - v2).length()/*+(float)(rand()%1000)*0.00005f */);
          if (len.x < edge_treshold->get()) len.x = edge_treshold->get();
          if (len.y < edge_treshold->get()) len.y = edge_treshold->get();
          if (len.z < edge_treshold->get()) len.z = edge_treshold->get();


          face_lengths.push_back(len);
        }
      }
    }

    vsx_vector3<> addpos;
    float fcount = 1.0f / (float)mesh->data->faces.size();
    float dirx = -b.x*0.05f;
    float dirz = -b.z*0.05f;
    float stepsizemultiplier = 0.02f * step_size->get();
    //float gravity_pull = -0.01f;
    float gravity_pull = -0.003f * gravity_amount->get();
    vsx_face3* face_p = mesh->data->faces.get_pointer();
    vsx_vector3<>* vertices_speed_p = vertices_speed.get_pointer();
    vsx_vector3<>* faces_length_p = face_lengths.get_pointer();
    vsx_vector3<>* vertex_p = mesh->data->vertices.get_pointer();

    for (int j = 0; j < 8; j++)
    {
      vsx_face3* face_p_it = face_p;
      for(unsigned int i = 0; i < mesh->data->faces.size(); i++)
      {
        // face fetching
        unsigned long fa = (*face_p_it).a;
        unsigned long fb = (*face_p_it).b;
        unsigned long fc = (*face_p_it).c;
        face_p_it++;
        // ---

        vsx_vector3<> v0 = vertex_p[fa];
        vsx_vector3<> v1 = vertex_p[fb];
        vsx_vector3<> v2 = vertex_p[fc];

        vsx_vector3<> edgeA = (v1 - v0);
        vsx_vector3<> edgeB = (v2 - v1);
        vsx_vector3<> edgeC = (v0 - v2);

        float lenA = edgeA.length();
        float lenB = edgeB.length();
        float lenC = edgeC.length();

        if (lenA < 0.0001f) lenA = 0.0001f;
        if (lenB < 0.0001f) lenB = 0.0001f;
        if (lenC < 0.0001f) lenC = 0.0001f;
        float edgeForceA = (face_lengths[i].x - lenA) / faces_length_p[i].x;
        float edgeForceB = (face_lengths[i].y - lenB) / faces_length_p[i].y;
        float edgeForceC = (face_lengths[i].z - lenC) / faces_length_p[i].z;
        float edgeAccA = edgeForceA / lenA;
        float edgeAccB = edgeForceB / lenB;
        float edgeAccC = edgeForceC / lenC;
        vsx_vector3<> accA = edgeA * edgeAccA;
        vsx_vector3<> accB = edgeB * edgeAccB;
        vsx_vector3<> accC = edgeC * edgeAccC;

        float ii = 0.5f - (float)i * fcount*0.5f;

        vertices_speed_p[fa] -= (accA - accC)*stiffness->get();
        vertices_speed_p[fb] -= (accB - accA)*stiffness->get();
        vertices_speed_p[fc] -= (accC - accB)*stiffness->get();

        vertices_speed_p[fa].y += gravity_pull;
        vertices_speed_p[fb].y += gravity_pull;
        vertices_speed_p[fc].y += gravity_pull;
        float sp2 = pow(sin(ii*1.57f),3.0f)*2.0f;

        vertices_speed_p[fa].x -= dirx*sp2;
        vertices_speed_p[fb].x -= dirx*sp2;
        vertices_speed_p[fc].x -= dirx*sp2;

        vertices_speed_p[fa].z -= dirz*sp2;
        vertices_speed_p[fb].z -= dirz*sp2;
        vertices_speed_p[fc].z -= dirz*sp2;
      }
      vsx_vector3<> mdist = a-prev_pos;  // prev_pos-------->a
      float mdl = mdist.length();
      if (mdl > 0.07f)
      {
        // prev_pos    <---0.1f a
        mdist.normalize();
        mdist.x = -mdist.x;
        mdist.y = -mdist.y;
        mdist.z = -mdist.z;
        addpos = mdist*(mdl-0.07f);
        for(unsigned long i = 4; i < mesh->data->vertices.size(); i++) {
          vertex_p[i] -= addpos;
        }
      }
      prev_pos = a;
      for(unsigned long i = 0; i < 4; i++)
      {
        mesh->data->vertices[i] = a;
      }
      float floor = floor_y->get();
      for(unsigned long i = 4; i < mesh->data->vertices.size(); i++)
      {
        //mesh->data->vertices[i] -= addpos;
        vertex_p[i] += (vertices_speed[i] * stepsizemultiplier);
        if (mesh->data->vertices[i].y < floor) mesh->data->vertices[i].y = floor;
        vertices_speed[i] = vertices_speed[i] * damping_factor->get();
      }
    }
    for(unsigned long i = 0; i < mesh->data->faces.size(); i++)
    {
      vsx_vector3<> a = mesh->data->vertices[mesh->data->faces[i].b] - mesh->data->vertices[mesh->data->faces[i].a];
      vsx_vector3<> b = mesh->data->vertices[mesh->data->faces[i].c] - mesh->data->vertices[mesh->data->faces[i].a];
      vsx_vector3<> normal;
      normal.cross(a,b);

      normal = -normal;
      normal.normalize();
      mesh->data->vertex_normals[mesh->data->faces[i].a] = normal;
      mesh->data->vertex_normals[mesh->data->faces[i].b] = normal;
      mesh->data->vertex_normals[mesh->data->faces[i].c] = normal;
    }

    mesh->timestamp++;
    result->set(mesh);
    num_runs++;
  }
};
