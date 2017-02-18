/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#include <graphics/vsx_mesh.h>


// TODO: add options for interpolating between normals, vertices, texcoords
class module_mesh_interpolate_2p : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in_a;
  vsx_module_param_mesh* mesh_in_b;
  vsx_module_param_float* factor;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  float old_factor;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;modifiers;deformers;mesh_interpolate_2p";

    info->description =
      "Picks out a vertex from a mesh";

    info->in_param_spec =
      "mesh_in_a:mesh,"
      "mesh_in_b:mesh,"
      "factor:float"
    ;

    info->out_param_spec =
      "result:mesh"
    ;

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in_a = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in_a");
    mesh_in_b = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in_b");

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"result");

    factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"factor");
    factor->set(0.5f);

    old_factor = -1;


    loading_done = true;
  }

  bool init()
  {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }


  void run()
  {
    vsx_mesh<>** p_a = mesh_in_a->get_addr();
    if (!p_a)
      return;

    vsx_mesh<>** p_b = mesh_in_b->get_addr();
    if (!p_b)
      return;

    vsx_mesh<>& mesh_a = (*(*p_a));
    vsx_mesh<>& mesh_b = (*(*p_b));

    if (mesh_a.data->vertices.size() != mesh_b.data->vertices.size())
    {
      vsx_printf(L"Vertex count differs: %d vs %d\n", mesh_a.data->vertices.size(), mesh_b.data->vertices.size() );
      return;
    }

    if (mesh_a.data->vertex_normals.size() != mesh_b.data->vertex_normals.size())
    {
      vsx_printf(L"Vertex normal count differs: %d vs %d\n", mesh_a.data->vertex_normals.size(), mesh_b.data->vertex_normals.size() );
      return;
    }


    float i_factor = CLAMP(factor->get(), 0, 1);
    float one_minus_i_factor = 1.0 - i_factor;

    // prevent running all the time if no need
    if (fabs(i_factor - old_factor) < 0.0001)
      return;

    old_factor = i_factor;

    size_t num_vertices = mesh_a.data->vertices.size();

    mesh->data->vertices.allocate( num_vertices );
    mesh->data->vertex_normals.allocate( num_vertices );


    vsx_vector3<>* v_a = mesh_a.data->vertices.get_pointer();
    vsx_vector3<>* v_b = mesh_b.data->vertices.get_pointer();
    vsx_vector3<>* v_dest = mesh->data->vertices.get_pointer();

    for (size_t i = 0; i < num_vertices; i++)
    {
      v_dest->x = v_a->x * one_minus_i_factor + v_b->x * i_factor;
      v_dest->y = v_a->y * one_minus_i_factor + v_b->y * i_factor;
      v_dest->z = v_a->z * one_minus_i_factor + v_b->z * i_factor;
      v_a++;
      v_b++;
      v_dest++;
    }

    vsx_vector3<>* vn_a = mesh_a.data->vertex_normals.get_pointer();
    vsx_vector3<>* vn_b = mesh_b.data->vertex_normals.get_pointer();
    vsx_vector3<>* vn_dest = mesh->data->vertex_normals.get_pointer();

    size_t num_normals = mesh_a.data->vertex_normals.size();

    for (size_t i = 0; i < num_normals; i++)
    {
      vn_dest->x = vn_a->x * one_minus_i_factor + vn_b->x * i_factor;
      vn_dest->y = vn_a->y * one_minus_i_factor + vn_b->y * i_factor;
      vn_dest->z = vn_a->z * one_minus_i_factor + vn_b->z * i_factor;

      vn_dest->normalize();

      vn_a++;
      vn_b++;
      vn_dest++;
    }

//    mesh->data->vertex_normals.set_volatile();
//    mesh->data->vertex_normals.set_data((*p_a)->data->vertex_normals.get_pointer(), (*p_a)->data->vertex_normals.size());

    mesh->data->vertex_tex_coords.set_volatile();
    mesh->data->vertex_tex_coords.set_data((*p_a)->data->vertex_tex_coords.get_pointer(), (*p_a)->data->vertex_tex_coords.size());

    mesh->data->vertex_tangents.set_volatile();
    mesh->data->vertex_tangents.set_data((*p_a)->data->vertex_tangents.get_pointer(), (*p_a)->data->vertex_tangents.size());

    mesh->data->vertex_colors.set_volatile();
    mesh->data->vertex_colors.set_data((*p_a)->data->vertex_colors.get_pointer(), (*p_a)->data->vertex_colors.size());

    mesh->data->faces.set_volatile();
    mesh->data->faces.set_data((*p_a)->data->faces.get_pointer(), (*p_a)->data->faces.size());

    mesh->timestamp++;
    result->set_p(mesh);
  }
};
