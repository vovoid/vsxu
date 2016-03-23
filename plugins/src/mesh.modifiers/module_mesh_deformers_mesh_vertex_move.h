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


class module_mesh_deformers_mesh_vertex_move : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float* index;
  vsx_module_param_float3* offset;
  vsx_module_param_float* falloff_range;

  // out
  vsx_module_param_mesh* mesh_out;

  // internal
  float p_index;
  vsx_vector3<> p_offset;
  float p_falloff_range;
  unsigned long p_timestamp;
  vsx_mesh<>* mesh;
  vsx_quaternion<> q;
  vsx_nw_vector<unsigned long> moved_vertices;
  vsx_nw_vector<int> vertices_needing_normal_calc;
  float falloff;
  int first_index;
  vsx_mesh<>** p;

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
      "mesh;modifiers;deformers;mesh_vertex_move";

    info->description =
      "Moves a single vertex with falloff";

    info->in_param_spec =
      "mesh_in:mesh,"
      "index:float,"
      "offset:float3,"
      "falloff_range:float"
    ;

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    index = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "index");
    offset = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "offset");
    falloff_range = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "falloff_range");
    falloff_range->set(0.0f);
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }

  void do_falloff(float i_falloff, unsigned long ind, bool final = false)
  {
    if (ind > mesh->data->vertices.size()) return;
    float pp;
    if (falloff == 0.0f) pp = 0.0f;
    else
    {


      pp = i_falloff / falloff;
    }

    moved_vertices.push_back(ind);

    if (!final)
    {
      // find all faces connected to the index-vertex
      for (unsigned long i = 0; i < mesh->data->faces.size(); i++)
      {
        if (
            mesh->data->faces[i].a == ind ||
            mesh->data->faces[i].b == ind ||
            mesh->data->faces[i].c == ind)
        {
          unsigned long a = mesh->data->faces[i].a;
          unsigned long b = mesh->data->faces[i].b;
          unsigned long c = mesh->data->faces[i].c;
          // for each face, check all index points - if not already moved, move it and recurse on that index
          bool found = false;
          unsigned long j = 0;
          if (a != ind)
          {
            while (!found && j < moved_vertices.size())
            {
              if (moved_vertices[j] == a) found = true;
              j++;
            }
            if (!found) {
              vsx_vector3<> dist = (*p)->data->vertices[a] - (*p)->data->vertices[first_index];
              float len = dist.length();
              if (len > falloff)
              do_falloff(len, a,true);
              else
              do_falloff(len, a);
            }
          }

          if (b != ind)
          {
            found = false; j = 0;
            while (!found && j < moved_vertices.size())
            {
              if (moved_vertices[j] == b) found = true;
              j++;
            }
            if (!found) {
              vsx_vector3<> dist = (*p)->data->vertices[b] - (*p)->data->vertices[first_index];
              float len = dist.length();
              if (len > falloff)
              do_falloff(len, b,true);
              else
              do_falloff(len, b);
            }
          }

          found = false; j = 0;
          if (c != ind)
          {
            while (!found && j < moved_vertices.size())
            {
              if (moved_vertices[j] == c) found = true;
              j++;
            }
            if (!found) {
              vsx_vector3<> dist = (*p)->data->vertices[c] - (*p)->data->vertices[first_index];
              float len = dist.length();
              if (len > falloff)
              do_falloff(len, c,true);
              else
              do_falloff(len, c);
            }
          }
        } // if ind in face
      }
    }
    if (pp <= 1.0f)
    {
      // find distance between this index and center vertex

      mesh->data->vertices[ind] = (*p)->data->vertices[ind] + vsx_vector3<>(offset->get(0),offset->get(1),offset->get(2))*(1.0f-pp);
    }
    vertices_needing_normal_calc.push_back(ind);
  }

  void run()
  {
    p = mesh_in->get_addr();
    if (!p) return;
    bool run = false;
    if (p_index != index->get()) run = true;
    if (!run) if (p_offset != vsx_vector3<>(offset->get(0), offset->get(1), offset->get(2))) run = true;
    if (p_falloff_range != falloff_range->get()) run = true;

    if ((*p)->timestamp != p_timestamp || run) {
      p_timestamp = (*p)->timestamp;
      p_falloff_range = falloff_range->get();
      p_offset = vsx_vector3<>(offset->get(0), offset->get(1), offset->get(2));
      p_index = index->get();

      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);
      for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
      {
        mesh->data->vertices[i] = (*p)->data->vertices[i];
      }
      for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++)
      {
        mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
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
      unsigned long ind = (int)floor(index->get());
      if (ind > mesh->data->vertices.size()-1) ind = mesh->data->vertices.size()-1;
      // add to this vector now
      vertices_needing_normal_calc.clear();
      moved_vertices.clear();
      falloff = falloff_range->get();
      first_index = ind;
      do_falloff(0.0f, ind);

      // find all faces that affect this vertex, calculate their normals and add to compound normal
      for (unsigned long k = 0; k < vertices_needing_normal_calc.size(); k++)
      {
        unsigned long indi = vertices_needing_normal_calc[k];
        vsx_vector3<> norm_accum;
        for (unsigned long i = 0; i < mesh->data->faces.size(); i++)
        {
          if (
              mesh->data->faces[i].a == indi ||
              mesh->data->faces[i].b == indi ||
              mesh->data->faces[i].c == indi)
          {
            norm_accum = norm_accum + mesh->data->get_face_normal(i);
          }
        }
        norm_accum.normalize();
        mesh->data->vertex_normals[indi] = norm_accum;
      }
      mesh_out->set_p(mesh);
    }
  }
};
