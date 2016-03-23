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


class module_mesh_rays : public vsx_module
{
public:
  // in
  vsx_module_param_float* num_rays;
  vsx_module_param_float* limit_ray_size;
  vsx_module_param_float4* center_color;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  bool first_run;
  int n_rays;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;particles;mesh_rays";

    info->description =
      "A bunch of random triangles with edges alpha = 0";

    info->in_param_spec =
      "num_rays:float,"
      "center_color:float4,"
      "options:complex"
      "{"
        "limit_ray_size:float"
      "}"
    ;

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    num_rays = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_rays");
    num_rays->set(40);
    n_rays = 40;
    limit_ray_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"limit_ray_size");
    limit_ray_size->set(-1.0f);

    center_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"center_color");
    center_color->set(0.5f,0);
    center_color->set(0.5f,1);
    center_color->set(0.5f,2);
    center_color->set(1,3);
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
    mesh->data->vertices[0] = vsx_vector3<>(0.0f);
    mesh->data->vertex_colors[0] = vsx_color<>(center_color->get(0),center_color->get(1),center_color->get(2),center_color->get(3));

    if (first_run || n_rays != (int)num_rays->get() || limit_ray_size->updates)
    {
      limit_ray_size->updates = 0;
      mesh->data->vertex_tex_coords[0].s = 0;
      mesh->data->vertex_tex_coords[0].t = 0;
      mesh->data->vertices.reset_used();
      mesh->data->faces.reset_used();
      //printf("generating random points\n");
      for (int i = 1; i < (int)num_rays->get(); ++i)
      {
        mesh->data->vertices[i*2].x = (rand()%10000)*0.0001f-0.5f;
        mesh->data->vertices[i*2].y = (rand()%10000)*0.0001f-0.5f;
        mesh->data->vertices[i*2].z = (rand()%10000)*0.0001f-0.5f;
        mesh->data->vertex_colors[i*2] = vsx_color<>(0,0,0,0);
        mesh->data->vertex_tex_coords[i*2].s = 0.0f;
        mesh->data->vertex_tex_coords[i*2].t = 1.0f;
        if (limit_ray_size->get() > 0.0f )
        {
          mesh->data->vertices[i*2+1].x = mesh->data->vertices[i*2].x+((rand()%10000)*0.0001f-0.5f)*limit_ray_size->get();
          mesh->data->vertices[i*2+1].y = mesh->data->vertices[i*2].y+((rand()%10000)*0.0001f-0.5f)*limit_ray_size->get();
          mesh->data->vertices[i*2+1].z = mesh->data->vertices[i*2].z+((rand()%10000)*0.0001f-0.5f)*limit_ray_size->get();
        }
        else
        {
          mesh->data->vertices[i*2+1].x = (rand()%10000)*0.0001f-0.5f;
          mesh->data->vertices[i*2+1].y = (rand()%10000)*0.0001f-0.5f;
          mesh->data->vertices[i*2+1].z = (rand()%10000)*0.0001f-0.5f;
        }

        mesh->data->vertex_colors[i*2+1] = vsx_color<>(0,0,0,0);
        mesh->data->vertex_tex_coords[i*2+1].s = 1.0f;
        mesh->data->vertex_tex_coords[i*2+1].t = 0.0f;
        mesh->data->faces[i-1].a = 0;
        mesh->data->faces[i-1].b = i*2;
        mesh->data->faces[i-1].c = i*2+1;
        n_rays = (int)num_rays->get();
      }
      first_run = false;
      mesh->timestamp++;
      result->set_p(mesh);
    }
  }
};
