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


class module_mesh_quat_rotate_around_vertex : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_quaternion* quat_in;
  vsx_module_param_float* vertex_rot_id;
  vsx_module_param_float3* offset_pos;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh<>* mesh;
  vsx_quaternion<> q;
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
      "mesh;modifiers;transforms;mesh_rotate_quat_around_vertex";

    info->description =
      "Rotates mesh around vertex id by Quaternion";

    info->in_param_spec =
      "mesh_in:mesh,"
      "quat_in:quaternion,"
      "vertex_rot_id:float,"
      "offset_pos:float3"
    ;

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    quat_in = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_in");
    quat_in->set(0.0,0);
    quat_in->set(0.0,1);
    quat_in->set(0.0,2);
    quat_in->set(1.0,3);
    vertex_rot_id = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "vertex_rot_id");
    offset_pos = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "offset_pos");
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");

    prev_timestamp = 0xFFFFFFFF;
  }
  unsigned long prev_timestamp;
  void run() {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p)
    {
      return;
    }

    if (p && (param_updates || prev_timestamp != (*p)->timestamp) && (*p)->data->vertices.size()) {
      q.x = quat_in->get(0);
      q.y = quat_in->get(1);
      q.z = quat_in->get(2);
      q.w = quat_in->get(3);
      float len = (float)(sqrt(
        q.x*q.x +
        q.y*q.y +
        q.z*q.z +
        q.w*q.w
      ));
      if (len != 0.0f)
      {
        len = 1.0f / len;
        q.x *= len;
        q.y *= len;
        q.z *= len;
        q.w *= len;
      }
      vsx_matrix<float> mat = q.matrix();


      vsx_vector3<> neg_vec;
      int n_i = (int)floor( vertex_rot_id->get() ) % (*p)->data->vertices.size();
      neg_vec = (*p)->data->vertices[n_i];
      vsx_vector3<> ofs_vec;
      ofs_vec.x = offset_pos->get(0);
      ofs_vec.y = offset_pos->get(1);
      ofs_vec.z = offset_pos->get(2);
      ofs_vec += neg_vec;

      unsigned long end = (*p)->data->vertices.size();
      vsx_vector3<>* vs_p = &(*p)->data->vertices[0];
      mesh->data->vertices.allocate(end);
      mesh->data->vertices.reset_used(end);
      vsx_vector3<>* vs_d = mesh->data->vertices.get_pointer();



      for (unsigned long i = 0; i < end; i++)
      {
        vs_d[i].multiply_matrix_other_vec(&mat.m[0],vs_p[i] - neg_vec);// = mat.multiply_vector((*p)->data->vertices[i]);
        vs_d[i] += ofs_vec;
      }

      end = (*p)->data->vertex_normals.size();
      mesh->data->vertex_normals.allocate(end);
      mesh->data->vertex_normals.reset_used(end);
      vs_d = mesh->data->vertex_normals.get_pointer();
      vs_p = (*p)->data->vertex_normals.get_pointer();

      for (unsigned long i = 0; i < end; i++)
      {
        vs_d[i].multiply_matrix_other_vec(&mat.m[0],vs_p[i]);// = mat.multiply_vector((*p)->data->vertex_normals[i]);
      }
/*
      vsx_ma_vector<vsx_vector> vertices;
      vsx_ma_vector<vsx_vector> vertex_normals;
      vsx_ma_vector<vsx_color> vertex_colors;
      vsx_ma_vector<vsx_tex_coord> vertex_tex_coords;
      vsx_ma_vector<vsx_face> faces;
*/
      if (prev_timestamp != (*p)->timestamp)
      {
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
      mesh_out->set_p(mesh);
      prev_timestamp = (*p)->timestamp;
      param_updates = 0;
    }
  }
};
