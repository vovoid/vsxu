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


class module_mesh_vertex_picker : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float* id;
  // out
  vsx_module_param_float3* vertex;
  vsx_module_param_float3* normal;
  vsx_module_param_float4* color;
  vsx_module_param_float3* texcoords;
  vsx_module_param_mesh* passthru;
  // internal
public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;modifiers;pickers;mesh_vertex_picker";

    info->description =
      "Picks out a vertex from a mesh";

    info->in_param_spec =
      "mesh_in:mesh,"
      "id:float"
    ;

    info->out_param_spec =
      "vertex:float3,"
      "normal:float3,"
      "color:float4,"
      "texcoords:float3,"
      "passthru:mesh"
    ;

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    passthru = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"passthru");
    id = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"id");
    id->set(0.0f);
    loading_done = true;
    vertex = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"vertex");
    normal = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"normal");

    color = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    texcoords = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"texcoords");
    set_default_values();
  }

  void set_default_values() {
    vertex->set(0.0f,0);
    vertex->set(0.0f,1);
    vertex->set(0.0f,2);
    normal->set(0.0f,0);
    normal->set(0.0f,1);
    normal->set(0.0f,2);
    color->set(0.0f,0);
    color->set(0.0f,1);
    color->set(0.0f,2);
    color->set(0.0f,3);
    texcoords->set(0.0f,0);
    texcoords->set(0.0f,1);
    texcoords->set(0.0f,2);
  }

  void run() {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p) return;
    passthru->set(*p);
    long id_ = (unsigned long)floor(id->get());
    if (!(*p)->data) return;

    if (id_ < 0) id_ += (*p)->data->vertices.size();

    if ((unsigned long)id_ < (*p)->data->vertices.size())
    {
      vertex->set((*p)->data->vertices[id_].x,0);
      vertex->set((*p)->data->vertices[id_].y,1);
      vertex->set((*p)->data->vertices[id_].z,2);

      if ((*p)->data->vertex_normals.size() > (unsigned long)id_)
      {
        //printf("id: %d\n",(int)id_);
        normal->set((*p)->data->vertex_normals[id_].x,0);
        normal->set((*p)->data->vertex_normals[id_].y,1);
        normal->set((*p)->data->vertex_normals[id_].z,2);
      }
      if ((*p)->data->vertex_colors.size() > (unsigned long)id_)
      {
        color->set((*p)->data->vertex_colors[id_].r,0);
        color->set((*p)->data->vertex_colors[id_].g,1);
        color->set((*p)->data->vertex_colors[id_].b,2);
        color->set((*p)->data->vertex_colors[id_].a,3);
      }

      if ((*p)->data->vertex_tex_coords.size() > (unsigned long)id_)
      {
        texcoords->set((*p)->data->vertex_tex_coords[id_].s,0);
        texcoords->set((*p)->data->vertex_tex_coords[id_].t,1);
      }
    } else {
      set_default_values();
    }
  }
};
