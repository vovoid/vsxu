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


class module_mesh_scale_normalize : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in;

  // out
  vsx_module_param_mesh* mesh_out;

  // internal
  vsx_mesh<>* mesh;

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
      "mesh;modifiers;transforms;mesh_norm_scale";

    info->description =
      "Scales mesh to within 0..1 size";

    info->in_param_spec =
      "mesh_in:mesh";

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    prev_timestamp = 0xFFFFFFFF;
  }

  unsigned long prev_timestamp;
  vsx_vector3<> v;
  void run() {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p)
    {
      return;
    }

    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      // 1. find out the minima and maxima of the mesh
      vsx_vector3<> minima;
      vsx_vector3<> maxima;


      vsx_vector3<>* vs_p;
      unsigned long end = (*p)->data->vertices.size();
      vs_p = &(*p)->data->vertices[0];
      mesh->data->vertices.allocate(end);
      mesh->data->vertices.reset_used(end);
      vsx_vector3<>* vs_d = mesh->data->vertices.get_pointer();


      for (unsigned int i = 0; i < end; i++)
      {
        if (vs_p[i].x < minima.x) minima.x = vs_p[i].x;
        if (vs_p[i].x > maxima.x) maxima.x = vs_p[i].x;

        if (vs_p[i].y < minima.y) minima.y = vs_p[i].y;
        if (vs_p[i].y > maxima.y) maxima.y = vs_p[i].y;

        if (vs_p[i].z < minima.z) minima.z = vs_p[i].z;
        if (vs_p[i].z > maxima.z) maxima.z = vs_p[i].z;
      }

      // find largest diff
      float diff = fabs(maxima.x - minima.x);
      if (fabs(maxima.y - minima.y) > diff) diff = fabs(maxima.y - minima.y);
      if (fabs(maxima.z - minima.z) > diff) diff = fabs(maxima.z - minima.z);

      if (diff == 0.0f) diff = 1.0f;

      float scaling = 1.0f / diff;


      float xmove = -minima.x * scaling;
      float ymove = -minima.y * scaling;
      float zmove = -minima.z * scaling;


      vs_p = &(*p)->data->vertices[0];
      for (unsigned int i = 0; i < end; i++)
      {
        vs_d[i] = vs_p[i] * scaling + vsx_vector3<>(xmove, ymove, zmove);
      }
      //vsx_ma_vector<vsx_vector> vertices;
      //vsx_ma_vector<vsx_vector> vertex_normals;
      //vsx_ma_vector<vsx_color> vertex_colors;
      //vsx_ma_vector<vsx_tex_coord> vertex_tex_coords;
      //vsx_ma_vector<vsx_face> faces;


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
      mesh->timestamp++;
      mesh_out->set_p(mesh);
      param_updates = 0;
    }
  }
};
