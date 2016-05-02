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


class module_mesh_grid : public vsx_module
{
  // in
  vsx_module_param_float* power_of_two_size;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  bool first_run;
  int l_param_updates;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;solid;mesh_grid";

    info->description = "";

    info->in_param_spec =
      "power_of_two_size:float";

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;
    power_of_two_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"power_of_two_size");
    power_of_two_size->set(5);

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
    if (!first_run && param_updates == 0) return;
    param_updates = 0;
    size_t width = (size_t)pow(2, (size_t)power_of_two_size->get());

    float onedivwidth = 1.0f / (float)width;
    float halfonedivwidth = -0.5f;
    for (size_t x = 0; x < width; x++)
    {
      for (size_t z = 0; z < width; z++)
      {
        size_t ipos = x + z*width;
        mesh->data->vertices[ipos].x = halfonedivwidth + ((float)x) * onedivwidth;
        mesh->data->vertices[ipos].z = halfonedivwidth + ((float)z) * onedivwidth;
        mesh->data->vertices[ipos].y = 0.0;
        mesh->data->vertex_normals[ipos].x = 0.0;
        mesh->data->vertex_normals[ipos].z = 0.0;
        mesh->data->vertex_normals[ipos].y = 1.0;
        mesh->data->vertex_tex_coords[ipos].s = ((float)x) * onedivwidth;
        mesh->data->vertex_tex_coords[ipos].t = ((float)z) * onedivwidth;
      }
    }
    for (size_t x = 1; x < width; x++)
    {
      for (size_t z = 1; z < width; z++)
      {
        vsx_face3 a;
        a.a = (GLuint)(x-1 +  (z - 1) * width);
        a.b = (GLuint)(x   +  (z - 1) * width);
        a.c = (GLuint)(x-1 +  (z    ) * width);
        mesh->data->faces.push_back(a);
        a.a = (GLuint)(x   +  (z - 1) * width);
        a.b = (GLuint)(x-1 +  (z    ) * width);
        a.c = (GLuint)(x   +  (z    ) * width);
        mesh->data->faces.push_back(a);
      }
    }

    first_run = false;
    mesh->timestamp++;
    result->set_p(mesh);
  }

};
