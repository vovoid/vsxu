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


class module_mesh_translate_edge_wraparound : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* translation;
  vsx_module_param_float3* edge_min;
  vsx_module_param_float3* edge_max;

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
      "mesh;modifiers;transforms;mesh_translate_edge_wraparound";

    info->description =
      "Translates/moves mesh";

    info->in_param_spec =
      "mesh_in:mesh,"
      "translation:float3,"
      "edge_min:float3,"
      "edge_max:float3"
    ;

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    translation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "translation");
    edge_min = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "edge_min");
    edge_min->set(-1.0f, 0);
    edge_min->set(-1.0f, 1);
    edge_min->set(-1.0f, 2);
    edge_max = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "edge_max");
    edge_max->set(1.0f, 0);
    edge_max->set(1.0f, 1);
    edge_max->set(1.0f, 2);
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }
  unsigned long prev_timestamp;
  vsx_vector3<> v;
  void run() {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p)
    {
      prev_timestamp = 0xFFFFFFFF;
      return;
    }

    if (p && (param_updates || prev_timestamp != (*p)->timestamp))
    {
      prev_timestamp = (*p)->timestamp;
      v.x = translation->get(0);
      v.y = translation->get(1);
      v.z = translation->get(2);
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      unsigned long end = (*p)->data->vertices.size();
      vsx_vector3<>* vs_p = &(*p)->data->vertices[0];
      mesh->data->vertices.allocate(end);
      mesh->data->vertices.reset_used(end);
      vsx_vector3<>* vs_d = mesh->data->vertices.get_pointer();

      float eminx = edge_min->get(0);
      float eminy = edge_min->get(1);
      float eminz = edge_min->get(2);

      float emaxx = edge_max->get(0);
      float emaxy = edge_max->get(1);
      float emaxz = edge_max->get(2);

      float diffx = fabs(edge_max->get(0) - eminx);
      float diffy = fabs(edge_max->get(1) - eminy);
      float diffz = fabs(edge_max->get(2) - eminz);

      for (unsigned int i = 0; i < end; i++)
      {
        vs_d[i] = vs_p[i] + v;
        if (vs_d[i].x > emaxx) vs_d[i].x = eminx + fmod(vs_d[i].x-eminx,diffx);
        if (vs_d[i].y > emaxy) vs_d[i].y = eminy + fmod(vs_d[i].y-eminy,diffy);
        if (vs_d[i].z > emaxz) vs_d[i].z = eminz + fmod(vs_d[i].z-eminz,diffz);

        // -1.2 - 1
        if (vs_d[i].x < eminx) vs_d[i].x = emaxx + fmod(vs_d[i].x + eminx, diffx);
        if (vs_d[i].y < eminy) vs_d[i].y = emaxy + fmod(vs_d[i].y + eminy, diffy);
        if (vs_d[i].z < eminz) vs_d[i].z = emaxz + fmod(vs_d[i].z + eminz, diffz);
      }
/*
      vsx_ma_vector<vsx_vector> vertices;
      vsx_ma_vector<vsx_vector> vertex_normals;
      vsx_ma_vector<vsx_color> vertex_colors;
      vsx_ma_vector<vsx_tex_coord> vertex_tex_coords;
      vsx_ma_vector<vsx_face> faces;
*/

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
