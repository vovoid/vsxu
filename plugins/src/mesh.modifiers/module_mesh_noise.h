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


class module_mesh_noise : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* noise_amount;

  // out
  vsx_module_param_mesh* mesh_out;

  // internal
  vsx_mesh<>* mesh;

  vsx_nw_vector< vsx_vector3<> > random_distort_points;

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
      "mesh;modifiers;deformers;mesh_noise";

    info->description =
      "Trashes / applies noise to mesh";

    info->in_param_spec =
      "mesh_in:mesh,"
      "noise_amount:float3"
    ;

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    noise_amount = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "noise_amount");
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }
  unsigned long prev_timestamp;
  vsx_vector3<> v;
  void run() {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;
      v.x = noise_amount->get(0);
      v.y = noise_amount->get(1);
      v.z = noise_amount->get(2);
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      if (random_distort_points.size() != (*p)->data->faces.size())
      {
        // inefficient, yes, but meshes don't change datasets that often..
        for (size_t i = 0; i < (*p)->data->faces.size(); i++)
        {
          random_distort_points[i].x = rand()%1000 * 0.001-0.5f;
          random_distort_points[i].y = rand()%1000 * 0.001-0.5f;
          random_distort_points[i].z = rand()%1000 * 0.001-0.5f;
          // thought of normalizing here but we'll do that later so doesn't matter really
        }
      }

      // we need to decouple the faces
      size_t i_vertex_iter = 0;
      size_t i_face_iter = 0;
      for (size_t face_iterator = 0; face_iterator < (*p)->data->faces.size(); face_iterator++)
      {
        mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].a] + random_distort_points[face_iterator] * v;
        mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].a];
        mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].a];
        mesh->data->faces[i_face_iter].a = i_vertex_iter;
        i_vertex_iter++;
        mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].b] + random_distort_points[face_iterator] * v;
        mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].b];
        mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].b];
        mesh->data->faces[i_face_iter].b = i_vertex_iter;
        i_vertex_iter++;
        mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].c] + random_distort_points[face_iterator] * v;
        mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].c];
        mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].c];
        mesh->data->faces[i_face_iter].c = i_vertex_iter;
        i_vertex_iter++;
        i_face_iter++;
      }

      mesh->timestamp++;
      mesh_out->set_p(mesh);
      param_updates = 0;
    }
  }
};
