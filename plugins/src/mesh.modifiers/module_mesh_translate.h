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


class module_mesh_translate : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* translation;

  // out
  vsx_module_param_mesh* mesh_out;
  vsx_module_param_float3* translation_passthru;

  // internal
  vsx_mesh<>* mesh;
  unsigned long prev_timestamp;

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
      "mesh;modifiers;transforms;mesh_translate";

    info->description =
      "Translates/moves mesh";

    info->in_param_spec =
      "mesh_in:mesh,"
      "translation:float3"
    ;

    info->out_param_spec =
      "mesh_out:mesh,"
      "translation_passthru:float3"
    ;

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    translation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "translation");
    loading_done = true;

    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    translation_passthru = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"translation_passthru");
    prev_timestamp = 0xFFFF;
  }
  void run()
  {

    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p)
    {
      printf("error in vsx_module_mesh_translate: mesh_in is invalid\n");
      return;
    }

    if (param_updates || prev_timestamp != (*p)->timestamp || translation->updates)
    {
      translation->updates = 0;
      prev_timestamp = (*p)->timestamp;
      vsx_vector3<> v;
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


      for (unsigned int i = 0; i < end; i++)
      {
        vs_d[i] = vs_p[i] + v;
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

      translation_passthru->set(v.x, 0);
      translation_passthru->set(v.y, 1);
      translation_passthru->set(v.z, 2);

      param_updates = 0;
    }

  }
};
