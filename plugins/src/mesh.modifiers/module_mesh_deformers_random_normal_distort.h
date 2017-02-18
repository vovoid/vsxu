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


class module_mesh_deformers_random_normal_distort : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* distortion_factor;
  vsx_module_param_int* distort_normals;
  vsx_module_param_int* distort_vertices;
  vsx_module_param_float* vertex_distortion_factor;

  // out
  vsx_module_param_mesh* mesh_out;

  // internal
  vsx_mesh<>* mesh;
  vsx_ma_vector< vsx_vector3<> > normals_dist_array;
  unsigned long int prev_timestamp;
  vsx_vector3<> v, v_;
  float vertex_distortion_factor_;

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
      "mesh;modifiers;deformers;mesh_normal_randistort";

    info->description =
        "Distorts the normals & vertices\n"
        "Makes fore some interesting effects.";

    info->in_param_spec =
      "mesh_in:mesh,"
      "distortion_factor:float3,"
      "distort_normals:enum?no|yes,"
      "distort_vertices:enum?no|yes,"
      "vertex_distortion_factor:float"
    ;

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    distortion_factor = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "distortion_factor");
    loading_done = true;
    distort_normals = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "distort_normals");
    distort_normals->set(1);
    distort_vertices = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "distort_vertices");
    distort_vertices->set(0);
    vertex_distortion_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "vertex_distortion_factor");
    vertex_distortion_factor->set(1.0f);
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    prev_timestamp = 0xFFFFFFFF;
    vertex_distortion_factor_ = 0.0f;
  }

  void run()
  {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p)
    {
      return;
    }
    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {

      if (normals_dist_array.size() != (*p)->data->vertices.size())
      {
        // inefficient, yes, but meshes don't change datasets that often..
        normals_dist_array.reset_used();
        for (size_t i = 0; i < (*p)->data->vertices.size(); i++)
        {
          normals_dist_array[i].x = rand()%1000 * 0.001;
          normals_dist_array[i].y = rand()%1000 * 0.001;
          normals_dist_array[i].z = rand()%1000 * 0.001;
        }
      }

      v.x = distortion_factor->get(0);
      v.y = distortion_factor->get(1);
      v.z = distortion_factor->get(2);

      float v1x = 1.0f - v.x;
      float v1y = 1.0f - v.y;
      float v1z = 1.0f - v.z;

      if (
          (
            vertex_distortion_factor->get() == vertex_distortion_factor_
          )
          &&
          (
            v_ == v
          )
          &&
          (
            prev_timestamp != 0xFFFFFFFF
          )
          &&
          (
           prev_timestamp == (*p)->timestamp
          )
        )
      {
        #ifdef VSXU_DEBUG
        //printf("return at line %d\n", __LINE__);
        #endif
        return;
      }
      #ifdef VSXU_DEBUG
        //printf("running meshdf\n");
      #endif

      v_ = v;
      vertex_distortion_factor_ = vertex_distortion_factor->get();
/*
      vsx_ma_vector<vsx_vector> vertices;
      vsx_ma_vector<vsx_vector> vertex_normals;
      vsx_ma_vector<vsx_color> vertex_colors;
      vsx_ma_vector<vsx_tex_coord> vertex_tex_coords;
      vsx_ma_vector<vsx_face> faces;
*/
      bool normal_transform_enabled = false;
      bool vertex_transform_enabled = false;

      if (1 == distort_normals->get())
      {
        vsx_vector3<>* ndap = normals_dist_array.get_pointer();
        vsx_vector3<>* vnp = (*p)->data->vertex_normals.get_pointer();
        mesh->data->vertex_normals.unset_volatile();
        mesh->data->vertex_normals.allocate( (*p)->data->vertex_normals.size() );
        vsx_vector3<>* vnd = mesh->data->vertex_normals.get_pointer();
        for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++)
        {
          // linear interpolation here
          vnd->x = ndap->x * v.x + v1x * vnp->x;
          vnd->y = ndap->y * v.y + v1y * vnp->y;
          vnd->z = ndap->z * v.z + v1z * vnp->z;
          vnd->normalize();
          vnp++;
          vnd++;
          ndap++;
        }
        if ((*p)->data->vertex_normals.size())
          normal_transform_enabled = true;
      }

      if (1 == distort_vertices->get())
      {
        v *= vertex_distortion_factor->get();
        vsx_vector3<>* ndap = normals_dist_array.get_pointer();
        vsx_vector3<>* vp = (*p)->data->vertices.get_pointer();
        mesh->data->vertices.unset_volatile();
        mesh->data->vertices.allocate( (*p)->data->vertices.size() );
        vsx_vector3<>* vd = mesh->data->vertices.get_pointer();

        for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
        {
          // similar linear interpolation here
          vd->x = ndap->x * v.x + vp->x;
          vd->y = ndap->y * v.y + vp->y;
          vd->z = ndap->z * v.z + vp->z;
          vp++;
          vd++;
          ndap++;
        }

        if ((*p)->data->vertices.size())
          vertex_transform_enabled = true;
      }
      {
        if (!vertex_transform_enabled)
        {
          mesh->data->vertices.set_volatile();
          mesh->data->vertices.set_data((*p)->data->vertices.get_pointer(), (*p)->data->vertices.size());
        }
        if (!normal_transform_enabled)
        {
          mesh->data->vertex_normals.set_volatile();
          mesh->data->vertex_normals.set_data((*p)->data->vertex_normals.get_pointer(), (*p)->data->vertex_normals.size());
        }

        mesh->data->vertex_tex_coords.set_volatile();
        mesh->data->vertex_tex_coords.set_data((*p)->data->vertex_tex_coords.get_pointer(), (*p)->data->vertex_tex_coords.size());

        mesh->data->vertex_tangents.set_volatile();
        mesh->data->vertex_tangents.set_data((*p)->data->vertex_tangents.get_pointer(), (*p)->data->vertex_tangents.size());

        mesh->data->vertex_colors.set_volatile();
        mesh->data->vertex_colors.set_data((*p)->data->vertex_colors.get_pointer(), (*p)->data->vertex_colors.size());

        mesh->data->faces.set_volatile();
        mesh->data->faces.set_data((*p)->data->faces.get_pointer(), (*p)->data->faces.size());
      }

      mesh->timestamp++;
      mesh_out->set(mesh);
      prev_timestamp = (*p)->timestamp;
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      param_updates = 0;
    }
  }
};
