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


class module_mesh_box : public vsx_module {
  // in
  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh* mesh;
  bool first_run;
  int l_param_updates;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_box";
    info->description = "";
    info->in_param_spec = "";
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(in_parameters);
    l_param_updates = -1;
    loading_done = true;

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
    if (!first_run) return;
    vsx_face a;

    //right
    mesh->data->vertices[0] = vsx_vector( 0.5f,-0.5f, 0.5f);
    mesh->data->vertices[1] = vsx_vector( 0.5f, 0.5f, 0.5f);
    mesh->data->vertices[2] = vsx_vector( 0.5f, 0.5f,-0.5f);
    mesh->data->vertices[3] = vsx_vector( 0.5f,-0.5f,-0.5f);
    a.a = 2;
    a.b = 1;
    a.c = 0;
    mesh->data->faces.push_back(a);
    a.a = 3;
    a.b = 2;
    a.c = 0;
    mesh->data->faces.push_back(a);
    //left
    mesh->data->vertices[4] = vsx_vector(-0.5f,-0.5f, 0.5f);
    mesh->data->vertices[5] = vsx_vector(-0.5f, 0.5f, 0.5f);
    mesh->data->vertices[6] = vsx_vector(-0.5f, 0.5f,-0.5f);
    mesh->data->vertices[7] = vsx_vector(-0.5f,-0.5f,-0.5f);
    a.a = 4;
    a.b = 5;
    a.c = 6;
    mesh->data->faces.push_back(a);
    a.a = 4;
    a.b = 6;
    a.c = 7;
    mesh->data->faces.push_back(a);
    // bottom
    mesh->data->vertices[ 8] = vsx_vector(-0.5f,-0.5f,-0.5f);
    mesh->data->vertices[ 9] = vsx_vector(-0.5f,-0.5f, 0.5f);
    mesh->data->vertices[10] = vsx_vector( 0.5f,-0.5f, 0.5f);
    mesh->data->vertices[11] = vsx_vector( 0.5f,-0.5f,-0.5f);
    a.a = 10;
    a.b = 9;
    a.c = 8;
    mesh->data->faces.push_back(a);
    a.a = 11;
    a.b = 10;
    a.c = 8;
    mesh->data->faces.push_back(a);

    // top
    mesh->data->vertices[12] = vsx_vector(-0.5f, 0.5f,-0.5f);
    mesh->data->vertices[13] = vsx_vector(-0.5f, 0.5f, 0.5f);
    mesh->data->vertices[14] = vsx_vector( 0.5f, 0.5f, 0.5f);
    mesh->data->vertices[15] = vsx_vector( 0.5f, 0.5f,-0.5f);
    a.a = 12;
    a.b = 13;
    a.c = 14;
    mesh->data->faces.push_back(a);
    a.a = 12;
    a.b = 14;
    a.c = 15;
    mesh->data->faces.push_back(a);

    // near
    mesh->data->vertices[16] = vsx_vector(-0.5f,-0.5f,-0.5f);
    mesh->data->vertices[17] = vsx_vector(-0.5f, 0.5f,-0.5f);
    mesh->data->vertices[18] = vsx_vector( 0.5f, 0.5f,-0.5f);
    mesh->data->vertices[19] = vsx_vector( 0.5f,-0.5f,-0.5f);
    a.a = 16;
    a.b = 17;
    a.c = 18;
    mesh->data->faces.push_back(a);
    a.a = 16;
    a.b = 18;
    a.c = 19;
    mesh->data->faces.push_back(a);

    // far
    mesh->data->vertices[20] = vsx_vector(-0.5f,-0.5f, 0.5f);
    mesh->data->vertices[21] = vsx_vector(-0.5f, 0.5f, 0.5f);
    mesh->data->vertices[22] = vsx_vector( 0.5f, 0.5f, 0.5f);
    mesh->data->vertices[23] = vsx_vector( 0.5f,-0.5f, 0.5f);
    a.a = 22;
    a.b = 21;
    a.c = 20;
    mesh->data->faces.push_back(a);
    a.a = 23;
    a.b = 22;
    a.c = 20;
    mesh->data->faces.push_back(a);

    first_run = false;
    mesh->timestamp++;
    result->set_p(mesh);
  }
};
