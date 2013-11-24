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


class module_mesh_vertices_rand_points : public vsx_module
{
public:
  // in
  vsx_module_param_float* rand_seed;
  vsx_module_param_float* num_points;
  vsx_module_param_float3* scaling;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh* mesh;
  vsx_vector old_scaling;
  bool first_run;
  vsx_rand rand;

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;vertices;random_vertices";
    info->description = "Generates [num_points] vertices\nconfined within the box\ndefined by [scaling]\ndelivered as a mesh.\n"
      "Colors, faces etc. are not genarated.\n"
      "Primarily for use with particle systems\n"
      "and point renderers and data source for\n"
      "the b-spline generator.";
    info->out_param_spec = "mesh:mesh";
    info->in_param_spec =
  "\
  rand_seed:float,\
  num_points:float,\
  scaling:float3?nc=1\
  ";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    rand_seed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    num_points = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_points");
    num_points->set(100);
    scaling = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"scaling");
    scaling->set(1,0);
    scaling->set(1,1);
    scaling->set(1,2);
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
  }

  bool init()
  {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void run()
  {
    if (first_run || param_updates) {
      rand.srand( (int)rand_seed->get() );
      int i;
      for (i = 0; i < (int)num_points->get(); ++i) {
        mesh->data->vertices[i].x = (rand.frand()-0.5f)*scaling->get(0);
        mesh->data->vertices[i].y = (rand.frand()-0.5f)*scaling->get(1);
        mesh->data->vertices[i].z = (rand.frand()-0.5f)*scaling->get(2);
      }
      mesh->data->vertices.reset_used(i);
      first_run = false;
      param_updates = 0;
      mesh->timestamp++;
      result->set_p(mesh);
    }
  }
};
