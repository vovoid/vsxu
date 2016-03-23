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


class module_mesh_rain_down : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float* start;
  vsx_module_param_float* floor_level;
  vsx_module_param_float* explosion_factor;
  vsx_module_param_float* landing_fluffiness;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh<>* mesh;
  unsigned long prev_timestamp;
  vsx_vector3<> v;
  float prev_start;
  vsx_ma_vector<float> vertex_weight_array;
  vsx_ma_vector<float> vertex_explosion_array_x;
  vsx_ma_vector<float> vertex_explosion_array_z;
public:
  bool init() {
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
      "mesh;modifiers;deformers;mesh_rain_down";

    info->description =
      "Slowly rains the mesh down on the floor";

    info->in_param_spec =
      "mesh_in:mesh,"
      "start:float,"
      "floor_level:float,"
      "explosion_factor:float,"
      "landing_fluffiness:float"
    ;

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    start = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "start");
    start->set(0.0f);
    landing_fluffiness = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "landing_fluffiness");
    landing_fluffiness->set(1.0f);
    explosion_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "explosion_factor");
    explosion_factor->set(0.0f);
    floor_level = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "floor_level");
    floor_level->set(-1.0f);
    prev_start = -1.0f;
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    prev_timestamp = 0xFFFFFFFF;
  }

  void run() {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p)
    {
      return;
    }
    if (((start->get() > 0.0f))) {
      if (prev_start <= 0.0f) // should cover all instances
      {
        // splice the mesh into faces for our personal use.
        // we need to decouple the faces
        size_t i_vertex_iter = 0;
        size_t i_face_iter = 0;
        size_t i_vertex_weight_iter = 0;
        mesh->data->vertices.unset_volatile();
        mesh->data->vertex_normals.unset_volatile();
        mesh->data->vertex_tex_coords.unset_volatile();
        mesh->data->faces.unset_volatile();
        for (size_t face_iterator = 0; face_iterator < (*p)->data->faces.size(); face_iterator++)
        {
          vsx_vector3<> a,b,c,ab,ac;
          mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].a];
          a = mesh->data->vertices[i_vertex_iter];
          mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].a];
          mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].a];
          mesh->data->faces[i_face_iter].a = i_vertex_iter;
          i_vertex_iter++;
          mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].b];
          b = mesh->data->vertices[i_vertex_iter];
          mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].b];
          mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].b];
          mesh->data->faces[i_face_iter].b = i_vertex_iter;
          i_vertex_iter++;
          mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].c];
          c = mesh->data->vertices[i_vertex_iter];
          mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].c];
          mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].c];
          mesh->data->faces[i_face_iter].c = i_vertex_iter;
          ab = b-a;
          ac = c-a;
          a.cross(ab,ac);
          float weight=pow(a.length(),0.2f);
          vsx_vector3<> exp_dist;

          exp_dist.x = (float)(rand()%1000) * 0.001f-0.5f;
          exp_dist.z = (float)(rand()%1000) * 0.001f-0.5f;
          exp_dist.normalize();
          exp_dist *= (float)(rand()%1000) * 0.001f-0.5f;
          float explosion_x = exp_dist.x;
          float explosion_z = exp_dist.z;
          vertex_weight_array[i_vertex_weight_iter] = weight;
          vertex_explosion_array_x[i_vertex_weight_iter] = explosion_x;
          vertex_explosion_array_z[i_vertex_weight_iter] = explosion_z;
          i_vertex_weight_iter++;
          vertex_weight_array[i_vertex_weight_iter] = weight;
          vertex_explosion_array_x[i_vertex_weight_iter] = explosion_x;
          vertex_explosion_array_z[i_vertex_weight_iter] = explosion_z;
          i_vertex_weight_iter++;
          vertex_weight_array[i_vertex_weight_iter] = weight;
          vertex_explosion_array_x[i_vertex_weight_iter] = explosion_x;
          vertex_explosion_array_z[i_vertex_weight_iter] = explosion_z;
          i_vertex_weight_iter++;

          i_vertex_iter++;
          i_face_iter++;
        }
      }
      vsx_vector3<>* vp = mesh->data->vertices.get_pointer();
      float* v_weight_p = vertex_weight_array.get_pointer();
      float* v_ex_p = vertex_explosion_array_x.get_pointer();
      float* v_ez_p = vertex_explosion_array_z.get_pointer();
      float dtx = engine_state->dtime;
      float speed = start->get()*2.0f;
      float spdtx = speed * dtx;
      float spdtx_exp = spdtx*explosion_factor->get();
      float fluffiness = landing_fluffiness->get()*10.0f;
      float fl = floor_level->get();
      for (size_t i = 0; i < mesh->data->vertices.size(); i++)
      {
        float floor = fl-(*v_weight_p) * fluffiness;
        (*vp).y += spdtx * ( (floor - (*vp).y * (*v_weight_p)));

        if ((*vp).y < floor) (*vp).y = floor; else
        {
          (*vp).x += (*v_ex_p)*spdtx_exp;
          (*vp).z += (*v_ez_p)*spdtx_exp;
          (*v_ex_p)*=1.0f-0.01f*spdtx;
          (*v_ez_p)*=1.0f-0.01f*spdtx;
        }
        vp++;
        v_weight_p++;
        v_ex_p++;
        v_ez_p++;
      }
      mesh->timestamp++;
      param_updates = 0;
    } else
    {
      mesh->data->vertices.set_volatile();
      mesh->data->vertices.set_data((*p)->data->vertices.get_pointer(), (*p)->data->vertices.size());

      mesh->data->vertex_normals.set_volatile();
      mesh->data->vertex_normals.set_data((*p)->data->vertex_normals.get_pointer(), (*p)->data->vertex_normals.size());

      mesh->data->vertex_tex_coords.set_volatile();
      mesh->data->vertex_tex_coords.set_data((*p)->data->vertex_tex_coords.get_pointer(), (*p)->data->vertex_tex_coords.size());

      mesh->data->vertex_tangents.set_volatile();
      mesh->data->vertex_tangents.set_data((*p)->data->vertex_tangents.get_pointer(), (*p)->data->vertex_tangents.size());

      mesh->data->vertex_colors.set_volatile();
      mesh->data->vertex_colors.set_data((*p)->data->vertex_colors.get_pointer(), (*p)->data->vertex_colors.size());

      mesh->data->faces.set_volatile();
      mesh->data->faces.set_data((*p)->data->faces.get_pointer(), (*p)->data->faces.size());
      mesh->timestamp = (*p)->timestamp;
    }
    mesh_out->set_p(mesh);
    prev_start = start->get();
    prev_timestamp = (*p)->timestamp;
  }
};
