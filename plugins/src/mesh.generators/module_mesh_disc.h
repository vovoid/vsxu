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


class module_mesh_disc : public vsx_module
{
public:

  // in
  vsx_module_param_float* num_segments;
  vsx_module_param_float* width;
  vsx_module_param_float* diameter;

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
      "mesh;particles;mesh_disc";

    info->description =
      "A disc. simple as that.";

    info->in_param_spec =
      "num_segments:float,"
      "diameter:float,"
      "border_width:float"
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
    num_segments = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_segments");
    num_segments->set(20);

    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"border_width");
    width->set(1);

    diameter = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"diameter");
    diameter->set(1);

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
    if (l_param_updates != param_updates) first_run = true;
    mesh->data->vertices[0] = vsx_vector3<>(0.0f);

    if (first_run || n_segs != num_segments->get())
    {
      l_param_updates = param_updates;
      mesh->data->vertices.reset_used();
      mesh->data->faces.reset_used();
      float inc = (float)(PI*2/(double)((int)num_segments->get()));
      float t_inc = 1.0f/(float)((int)num_segments->get());
      float t = 0.0f;
      float ip = 0.0f;
      float dia = diameter->get();
      for (int i = 0; i < (int)num_segments->get(); ++i) {
        int i4 = i*4;
        mesh->data->vertices[i4].x = (float)cos(ip)*dia;
        mesh->data->vertices[i4].y = 0.0f;
        mesh->data->vertices[i4].z = (float)sin(ip)*dia;
        mesh->data->vertex_colors[i4] = vsx_color<>(1,1,1,1);
        mesh->data->vertex_tex_coords[i4].s = t;
        mesh->data->vertex_tex_coords[i4].t = 0;

        mesh->data->vertices[i4+1].x = (float)cos(ip)*(dia+0.15f*width->get());
        mesh->data->vertices[i4+1].y = 0.0f;
        mesh->data->vertices[i4+1].z = (float)sin(ip)*(dia+0.15f*width->get());
        mesh->data->vertex_colors[i4+1] = vsx_color<>(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+1].s = t;
        mesh->data->vertex_tex_coords[i4+1].t = 1;

        mesh->data->vertices[i4+2].x = (float)cos(ip+inc)*(dia+0.15f*width->get());
        mesh->data->vertices[i4+2].y = 0.0f;
        mesh->data->vertices[i4+2].z = (float)sin(ip+inc)*(dia+0.15f*width->get());
        mesh->data->vertex_colors[i4+2] = vsx_color<>(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+2].s = t+t_inc;
        mesh->data->vertex_tex_coords[i4+2].t = 1;

        mesh->data->vertices[i4+3].x = (float)cos(ip+inc)*dia;
        mesh->data->vertices[i4+3].y = 0.0f;
        mesh->data->vertices[i4+3].z = (float)sin(ip+inc)*dia;
        mesh->data->vertex_colors[i4+3] = vsx_color<>(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+3].s = t+t_inc;
        mesh->data->vertex_tex_coords[i4+3].t = 0;

        vsx_face3 a;
        a.a = i4+2; a.b = i4; a.c = i4+1;
        mesh->data->faces.push_back(a);
        a.a = i4+2; a.b = i4+3; a.c = i4;
        mesh->data->faces.push_back(a);
        ip += inc;
        t += t_inc;
      }
      first_run = false;
      n_segs = (int)num_segments->get();
      mesh->timestamp++;
      result->set_p(mesh);
    }
  }
};
