/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB - Copyright (C) 2003-2014
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


class module_mesh_mirror : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_int* axis;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh<>* mesh;
  unsigned long prev_timestamp;
  vsx_vector3<> v;

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
      "mesh;modifiers;transforms;mesh_mirror";

    info->description =
      "Mirrors mesh along one axis - x, y or z";

    info->in_param_spec =
      "mesh_in:mesh,"
      "axis:enum?x|y|z"
    ;

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    axis = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "axis");
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    prev_timestamp = 0xFFFFFFFF;
  }

  void run()
  {
    vsx_mesh<>** p = mesh_in->get_addr();

    req(p);

    // check if there's work to do, otherwise return
    if
    (
      !(
        param_updates
        ||
        prev_timestamp != (*p)->timestamp
      )
    )
    return;

    prev_timestamp = (*p)->timestamp;

    v.x = 1.0;
    v.y = 1.0;
    v.z = 1.0;

    if (0 == axis->get())
      v.x = -1.0;

    if (1 == axis->get())
      v.y = -1.0;

    if (2 == axis->get())
      v.z = -1.0;

    mesh->data->vertices.reset_used(0);
    mesh->data->vertex_normals.reset_used(0);
    mesh->data->vertex_tex_coords.reset_used(0);
    mesh->data->vertex_colors.reset_used(0);
    mesh->data->faces.reset_used(0);

    // mirror vertices
    unsigned long v_end = (*p)->data->vertices.size();
    vsx_vector3<>* vs_p = &(*p)->data->vertices[0];
    mesh->data->vertices.allocate(v_end);
    mesh->data->vertices.reset_used(v_end);
    vsx_vector3<>* vs_d = mesh->data->vertices.get_pointer();

    for (unsigned int i = 0; i < v_end; i++)
    {
      vs_d[i] = vs_p[i] * v;
    }

    // mirror normals
    unsigned long n_end = (*p)->data->vertex_normals.size();
    vsx_vector3<>* ns_p = &(*p)->data->vertex_normals[0];
    mesh->data->vertex_normals.allocate(n_end);
    mesh->data->vertex_normals.reset_used(n_end);
    vsx_vector3<>* ns_d = mesh->data->vertex_normals.get_pointer();

    for (unsigned int i = 0; i < n_end; i++)
    {
      ns_d[i] = ns_p[i] * v;
    }

    // mirror tangents
    unsigned long t_end = (*p)->data->vertex_tangents.size();
    vsx_quaternion<>* ts_p = &(*p)->data->vertex_tangents[0];
    mesh->data->vertex_tangents.allocate(t_end);
    mesh->data->vertex_tangents.reset_used(t_end);
    vsx_quaternion<>* ts_d = mesh->data->vertex_tangents.get_pointer();

    for (unsigned int i = 0; i < t_end; i++)
    {
      ts_d[i] = ts_p[i] * v;
    }


    // mirror faces
    unsigned long f_end = (*p)->data->faces.size();
    vsx_face3* fs_p = &(*p)->data->faces[0];
    mesh->data->faces.allocate(f_end);
    mesh->data->faces.reset_used(f_end);
    vsx_face3* fs_d = mesh->data->faces.get_pointer();

    for (unsigned int i = 0; i < f_end; i++)
    {
      fs_d[i].a = fs_p[i].c;
      fs_d[i].b = fs_p[i].b;
      fs_d[i].c = fs_p[i].a;
    }

    mesh->data->vertex_tex_coords.set_volatile();
    mesh->data->vertex_tex_coords.set_data((*p)->data->vertex_tex_coords.get_pointer(), (*p)->data->vertex_tex_coords.size());

    mesh->data->vertex_colors.set_volatile();
    mesh->data->vertex_colors.set_data((*p)->data->vertex_colors.get_pointer(), (*p)->data->vertex_colors.size());

    mesh->timestamp++;
    mesh_out->set_p(mesh);
    param_updates = 0;
  }
};
