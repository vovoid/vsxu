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


class module_mesh_quat_rotate : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_quaternion* quat_in;
  vsx_module_param_int* invert_rotation;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh<>* mesh;
  vsx_quaternion<> q;
  unsigned long prev_timestamp;
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
      "mesh;modifiers;transforms;mesh_rotate_quat";

    info->description =
      "Rotates mesh by Quaternion";

    info->in_param_spec =
      "mesh_in:mesh,"
      "quat_in:quaternion,"
      "invert_rotation:enum?no|yes"
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
    invert_rotation = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "invert_rotation");
    invert_rotation->set(0);
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    prev_timestamp = 0xffff;
  }

  void run()
  {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p) { printf("error in vsx_module_mesh_quat_rotate: mesh_in is invalid\n"); return; }
    if (
        param_updates
        ||
        prev_timestamp != (*p)->timestamp
        )
    {
      prev_timestamp = (*p)->timestamp;
      q.x = quat_in->get(0);
      q.y = quat_in->get(1);
      q.z = quat_in->get(2);
      q.w = quat_in->get(3);
      q.normalize();
      vsx_matrix<float> mat;
      if ( invert_rotation->get())
      {
        vsx_matrix<float> mat2 = q.matrix();
        mat.assign_inverse(&mat2);
      } else
      {
        mat = q.matrix();
      }
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
      {
        mesh->data->vertices[i] = mat.multiply_vector((*p)->data->vertices[i]);
      }
      for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++)
      {
        mesh->data->vertex_normals[i] = mat.multiply_vector((*p)->data->vertex_normals[i]);
      }
/*
      vsx_ma_vector<vsx_vector> vertices;
      vsx_ma_vector<vsx_vector> vertex_normals;
      vsx_ma_vector<vsx_color> vertex_colors;
      vsx_ma_vector<vsx_tex_coord> vertex_tex_coords;
      vsx_ma_vector<vsx_face> faces;
*/
      for (unsigned int i = 0; i < (*p)->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = (*p)->data->vertex_tex_coords[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = (*p)->data->vertex_tangents[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = (*p)->data->vertex_colors[i];
      for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) mesh->data->faces[i] = (*p)->data->faces[i];
      mesh->timestamp++;
      mesh_out->set_p(mesh);
      param_updates = 0;
    }
  }
};
