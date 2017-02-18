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


class module_mesh_planes : public vsx_module
{
public:
  // in
  vsx_module_param_float* num_planes;
  vsx_module_param_float* space_between;
  vsx_module_param_float* diameter;
  vsx_module_param_float3* normals;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  bool first_run;
  int n_segs;
  int l_param_updates;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;solid;mesh_planes";

    info->description =
      "Multiple planes layered on top of each other";

    info->in_param_spec =
      "num_planes:float,"
      "space_between:float,"
      "diameter:float,"
      "normals:float3"
    ;

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;
    num_planes = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_planes");
    num_planes->set(20.0f);
    space_between = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"space_between");
    space_between->set(0.05f);
    diameter = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"diameter");
    diameter->set(1.0f);

    normals = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"normals");
    normals->set(0.0f,0);
    normals->set(1.0f,1);
    normals->set(0.0f,2);

    n_segs = 20;

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
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
    if (l_param_updates != param_updates)
      first_run = true;
    mesh->data->vertices[0] = vsx_vector3<>(0.0f);

    if (first_run || n_segs != num_planes->get())
    {
      l_param_updates = param_updates;

      mesh->data->vertices.reset_used();
      mesh->data->faces.reset_used();
      float inc = space_between->get();
      vsx_vector3<> vertex_normals = vsx_vector3<>(normals->get(0),normals->get(1),normals->get(2));
      float ip = 0.0f;
      for (int i = 0; i < (int)num_planes->get(); ++i)
      {
        int i4 = i*4;
        mesh->data->vertices[i4].x = -1.0f*diameter->get();
        mesh->data->vertices[i4].y = -1.0f*diameter->get();
        mesh->data->vertices[i4].z = ip;
        mesh->data->vertex_colors[i4] = vsx_color<>(1,1,1,1);
        mesh->data->vertex_tex_coords[i4] = vsx_tex_coord2f(0,0);
        mesh->data->vertex_normals[i4] = vertex_normals;

        mesh->data->vertices[i4+1].x = 1.0f*diameter->get();
        mesh->data->vertices[i4+1].y = -1.0f*diameter->get();
        mesh->data->vertices[i4+1].z = ip;
        mesh->data->vertex_colors[i4+1] = vsx_color<>(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+1] = vsx_tex_coord2f(1,0);
        mesh->data->vertex_normals[i4+1] = vertex_normals;

        mesh->data->vertices[i4+2].x = 1.0f*diameter->get();
        mesh->data->vertices[i4+2].y = 1.0f*diameter->get();
        mesh->data->vertices[i4+2].z = ip;
        mesh->data->vertex_colors[i4+2] = vsx_color<>(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+2] = vsx_tex_coord2f(1,1);
        mesh->data->vertex_normals[i4+2] = vertex_normals;

        mesh->data->vertices[i4+3].x = -1.0f*diameter->get();
        mesh->data->vertices[i4+3].y = 1.0f*diameter->get();
        mesh->data->vertices[i4+3].z = ip;
        mesh->data->vertex_colors[i4+3] = vsx_color<>(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+3] = vsx_tex_coord2f(0,1);
        mesh->data->vertex_normals[i4+3] = vertex_normals;

        vsx_face3 a;
        a.a = i4+2; a.b = i4; a.c = i4+1;
        mesh->data->faces.push_back(a);
        a.a = i4+2; a.b = i4+3; a.c = i4;
        mesh->data->faces.push_back(a);
        ip += inc;
      }
      mesh->timestamp++;
      first_run = false;
      n_segs = (int)num_planes->get();
    }
    result->set_p(mesh);
  }
};
