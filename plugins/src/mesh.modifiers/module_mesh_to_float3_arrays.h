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


class module_mesh_to_float3_arrays : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in;
  // out
  vsx_module_param_float3_array* vertices;
  vsx_module_param_float3_array* vertex_normals;
  vsx_module_param_float3_array* vertex_tangents;
  vsx_module_param_float3_array* face_normals;
  vsx_module_param_float3_array* face_centers;
  // internal
  vsx_ma_vector< vsx_vector3<> > int_empty_array;
  vsx_vector_array<> int_vertices;
  vsx_vector_array<> int_vertex_normals;
  vsx_vector_array<> int_vertex_tangents;
  vsx_vector_array<> int_face_normals;
  vsx_vector_array<> int_face_centers;
public:
  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;modifiers;converters;mesh_to_float3_arrays";

    info->description =
      "Pick out the vector data from a mesh";

    info->in_param_spec =
      "mesh_in:mesh";

    info->out_param_spec =
      "vertices:float3_array,"
      "vertex_normals:float3_array,"
      "face_normals:float3_array,"
      "face_centers:float3_array"
    ;

    info->component_class =
      "mesh";
  }
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    vertices = (vsx_module_param_float3_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY, "vertices");
    vertex_normals = (vsx_module_param_float3_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY, "vertex_normals");
    face_normals = (vsx_module_param_float3_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY, "face_normals");
    face_centers = (vsx_module_param_float3_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY, "face_centers");

    int_vertices.data = &int_empty_array;
    vertices->set_p(int_vertices);

    int_vertex_normals.data = &int_empty_array;
    vertex_normals->set_p(int_vertex_normals);

    int_face_normals.data = &int_empty_array;
    face_normals->set_p(int_face_normals);

    int_face_centers.data = &int_empty_array;
    face_centers->set_p(int_face_centers);
  }
  void run() {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p) return;

    int_vertices.data = &((*p)->data->vertices);
    vertices->set_p(int_vertices);

    int_vertex_normals.data = &((*p)->data->vertex_normals);
    vertex_normals->set_p(int_vertex_normals);

    int_face_normals.data = &((*p)->data->face_normals);
    face_normals->set_p(int_face_normals);

    int_face_normals.data = &((*p)->data->face_normals);
    face_normals->set_p(int_face_normals);

    int_face_centers.data = &((*p)->data->face_centers);
    face_centers->set_p(int_face_centers);
  }
};
