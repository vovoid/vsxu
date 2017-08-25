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

#include <math/vsx_bspline.h>
#include <math/quaternion/vsx_quaternion.h>
#include <math/quaternion/vsx_quaternion_helper.h>


class module_mesh_plasma_tree: public vsx_module
{
public:
  // in
  vsx_module_param_float* recursion_levels;
  vsx_module_param_quaternion* base_rotation;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  bool first_run;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;solid;plasma_tree";

    info->description =
      "Plasma tree";

    info->in_param_spec =
      "recursion_levels:float,"
      "base_rotation:quaternion"
    ;

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    recursion_levels = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "recursion_levels");
    recursion_levels->set(2);

    base_rotation = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "base_rotation");

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH, "mesh");
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

  void generate_stem()
  {
    vsx_bspline<vsx_vector3f> spline;
    spline.points.push_back(vsx_vector3f(0,0,0));
    spline.points.push_back(vsx_vector3f(0,0,0));
    vsx_quaternion<float> base_rot_quat(base_rotation->get(0), base_rotation->get(1), base_rotation->get(2), base_rotation->get(3));
    vsx_vector3f axis;
    float angle;
    base_rot_quat.to_axis_angle( axis, angle );



  }


  void run()
  {
    mesh->data->vertices[0] = vsx_vector3<>(0.0f);
    mesh->data->vertex_colors[0] = vsx_color<>(1,1,1,1);

    req(first_run || recursion_levels->updates);

    mesh->data->vertex_tex_coords[0].s = 0;
    mesh->data->vertex_tex_coords[0].t = 0;
    mesh->data->vertices.reset_used();
    mesh->data->faces.reset_used();



    //printf("generating random points\n");
//    for (int i = 1; i < (int)num_rays->get(); ++i)
//    {
//      mesh->data->vertices[i*2].x = (rand()%10000)*0.0001f-0.5f;
//      mesh->data->vertices[i*2].y = (rand()%10000)*0.0001f-0.5f;
//      mesh->data->vertices[i*2].z = (rand()%10000)*0.0001f-0.5f;
//      mesh->data->vertex_colors[i*2] = vsx_color<>(0,0,0,0);
//      mesh->data->vertex_tex_coords[i*2].s = 0.0f;
//      mesh->data->vertex_tex_coords[i*2].t = 1.0f;
//      if (limit_ray_size->get() > 0.0f )
//      {
//        mesh->data->vertices[i*2+1].x = mesh->data->vertices[i*2].x+((rand()%10000)*0.0001f-0.5f)*limit_ray_size->get();
//        mesh->data->vertices[i*2+1].y = mesh->data->vertices[i*2].y+((rand()%10000)*0.0001f-0.5f)*limit_ray_size->get();
//        mesh->data->vertices[i*2+1].z = mesh->data->vertices[i*2].z+((rand()%10000)*0.0001f-0.5f)*limit_ray_size->get();
//      }
//      else
//      {
//        mesh->data->vertices[i*2+1].x = (rand()%10000)*0.0001f-0.5f;
//        mesh->data->vertices[i*2+1].y = (rand()%10000)*0.0001f-0.5f;
//        mesh->data->vertices[i*2+1].z = (rand()%10000)*0.0001f-0.5f;
//      }

//      mesh->data->vertex_colors[i*2+1] = vsx_color<>(0,0,0,0);
//      mesh->data->vertex_tex_coords[i*2+1].s = 1.0f;
//      mesh->data->vertex_tex_coords[i*2+1].t = 0.0f;
//      mesh->data->faces[i-1].a = 0;
//      mesh->data->faces[i-1].b = i*2;
//      mesh->data->faces[i-1].c = i*2+1;
//      n_rays = (int)num_rays->get();
//    }
    first_run = false;
    mesh->timestamp++;
    result->set_p(mesh);
  }
};
