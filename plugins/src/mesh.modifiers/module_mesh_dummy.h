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


class module_mesh_dummy : public vsx_module
{
  // in
  vsx_module_param_mesh* mesh_in;

  // out
  vsx_module_param_mesh* mesh_out;

  // internal

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;dummies;mesh_dummy"
      "||"
      "dummies;mesh_dummy"
    ;

    info->description =
      "dummy for mesh (useful in macros)";

    info->in_param_spec =
      "mesh_in:mesh";

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    loading_done = true;
  }

  void run() {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (p)
    {
      mesh_out->set_p(*p);
    }
  }
};
