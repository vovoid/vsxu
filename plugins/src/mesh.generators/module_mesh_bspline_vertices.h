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


class module_mesh_bspline_vertices : public vsx_module
{
  // in
  vsx_module_param_mesh* source;
  vsx_module_param_float* density;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh* mesh;
  vsx_bspline spline0;
  bool first_run;

public:

  bool init()
  {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;vertices;bspline_vertices";
    info->description = "";
    info->out_param_spec =
      "mesh:mesh";
    info->in_param_spec =
      "source:mesh,"
      "density:float"
    ;
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    source = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH, "source" );

    density = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "density" );
    density->set(10);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH, "mesh" );

    first_run = true;
    spline0.init(vsx_vector(0), 0.7f, 0.3f, 0.6f);
  }

  void run()
  {
    vsx_mesh** in = source->get_addr();
    if (!in) return;
    spline0.points.set_volatile();
    spline0.points.set_data((*in)->data->vertices.get_pointer(), (*in)->data->vertices.size() );
    spline0.set_pos(0.0f);
    spline0.step(1);
    int idens = (int)floor(density->get());
    float step = 1.0f / (float)idens;
    if ((*in)->timestamp != mesh->timestamp) first_run = true;

    if (first_run || param_updates)
    {
      int i;
      for (i = 0; i < (int)((*in)->data->vertices.size()-1) * idens; ++i) {
        spline0.step(step);
        mesh->data->vertices[i] = spline0.calc_coord();
      }
      mesh->data->vertices.reset_used(i);
      first_run = false;
      param_updates = 0;
      mesh->timestamp = (*in)->timestamp;
      result->set(mesh);
    }
  }
};
