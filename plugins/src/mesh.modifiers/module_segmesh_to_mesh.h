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


class module_mesh_segmesh_to_mesh : public vsx_module
{
  vsx_module_param_segment_mesh* seg_mesh_in;
  vsx_module_param_mesh* mesh_out;
  vsx_2dgrid_mesh* mesh_in;
  vsx_mesh<>* mesh;
public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;segmesh;segmesh_to_mesh";

    info->in_param_spec =
      "seg_mesh_in:segment_mesh";

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    loading_done = true;
    seg_mesh_in = (vsx_module_param_segment_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"seg_mesh_in",true);
    //vsx_2dgrid_mesh foomesh;
    //seg_mesh_in->set(foomesh);
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    mesh_out->set(mesh);
  }

  bool init() {
    mesh = new vsx_mesh<>;
    return true;
  }
  void on_delete()
  {
    delete mesh;
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    mesh_in = seg_mesh_in->get_addr();
    if (mesh_in) {
      mesh_in->calculate_face_normals();
      mesh_in->calculate_vertex_normals();
      mesh_in->dump_vsx_mesh(mesh);
      mesh_out->set(mesh);
    }
  }
};
