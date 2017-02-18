/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2015
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
#include <math/vsx_math.h>
#include <math/quaternion/vsx_quaternion.h>
#include <math/vsx_rand.h>

class module_mesh_rays_uniform : public vsx_module
{
public:
  // in
  vsx_module_param_float* num_rays;
  vsx_module_param_float4* center_color;
  vsx_module_param_float* ray_width;
  vsx_module_param_int* put_angle_in_normal;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  bool first_run;
  int n_rays;
  vsx_rand rand;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;particles;mesh_rays_uniform";

    info->description =
      "A bunch of random triangles with edges alpha = 0";

    info->in_param_spec =
      "num_rays:float,"
      "center_color:float4,"
      "options:complex"
      "{"
        "ray_width:float,"
        "put_angle_in_normal:enum?no|yes"
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
    ray_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"ray_width");
    ray_width->set(1.0f);
    put_angle_in_normal = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "put_angle_in_normal");
    put_angle_in_normal->set(0);
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
    if (first_run || n_rays != (int)num_rays->get() || ray_width->updates)
    {
      ray_width->updates = 0;
      mesh->data->vertices.reset_used();
      mesh->data->vertex_normals.reset_used();
      mesh->data->faces.reset_used();

      float ray_w = 0.05f * ray_width->get();
      rand.srand(0);

      for (int i = 0; i < (int)num_rays->get(); ++i)
      {
        float s = rand.frand() * (float)(TWO_PI);
        float t = rand.frand() * (float)(TWO_PI);
        float vr = rand.frand() * (float)(TWO_PI);

        vsx_vector3<> srv(1.0, 0.0, 0.0);
        vsx_vector3<> trv(0.0, 0.0, 1.0);

        vsx_vector3<> v(0.0, 1.0, 0.0);
        vsx_vector3<> vp(1.0, 0.0, 0.0);

        vsx_quaternion<> q;
        q.from_axis_angle(srv, s);
        v = q.transform(v);
        vp = q.transform(vp);

        q.from_axis_angle(trv, t);
        v = q.transform(v);
        vp = q.transform(vp);

        mesh->data->vertices[i*3] = vsx_vector3f();
        mesh->data->vertices[i*3 + 1] = v;

        q.from_axis_angle( v, vr);
        vp = q.transform(vp);

        vp *= ray_w;

        mesh->data->vertices[i*3 + 2] = v + vp;

        if (put_angle_in_normal->get())
        {
          mesh->data->vertex_normals[i*3] = vsx_vector3f(s, t, 0.0);
          mesh->data->vertex_normals[i*3 + 1] = vsx_vector3f(s, t, 0.0);
          mesh->data->vertex_normals[i*3 + 2] = vsx_vector3f(s, t, 0.0);
        }


        mesh->data->vertex_colors[i*3] = vsx_color<>(0,0,0,0);
        mesh->data->vertex_colors[i*3 + 1] = vsx_color<>(0,0,0,0);
        mesh->data->vertex_colors[i*3 + 2] = vsx_color<>(0,0,0,0);

        mesh->data->vertex_tex_coords[i*3].s = 0.5f;
        mesh->data->vertex_tex_coords[i*3].t = 0.5f;
        mesh->data->vertex_tex_coords[i*3+1].s = 0.0f;
        mesh->data->vertex_tex_coords[i*3+1].t = 1.0f;
        mesh->data->vertex_tex_coords[i*3+2].s = 1.0f;
        mesh->data->vertex_tex_coords[i*3+2].t = 1.0f;

        mesh->data->faces[i].a = i*3;
        mesh->data->faces[i].b = i*3+1;
        mesh->data->faces[i].c = i*3+2;
        n_rays = (int)num_rays->get();
      }
      first_run = false;
      mesh->timestamp++;
      result->set_p(mesh);
    }
  }
};
