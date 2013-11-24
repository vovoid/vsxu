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


class module_mesh_render_face_id : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float4* base_color;
  vsx_module_param_float3* min_box;
  vsx_module_param_float3* max_box;
  vsx_module_param_float* font_size;
  vsx_module_param_float* font_align;

  // out
  vsx_module_param_render* render_out;

  // internal
  vsx_mesh** mesh;
  vsx_matrix ma;
  vsx_vector upv;
  vsx_font* myf;

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;mesh;mesh_face_id_render";
    info->description = "";
    info->in_param_spec = "mesh_in:mesh,base_color:float4,font_size:float,min_box:float3,max_box:float3,font_align:float";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    loading_done = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    base_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"base_color");
    base_color->set(1.0f,0);
    base_color->set(1.0f,1);
    base_color->set(1.0f,2);
    base_color->set(1.0f,3);
    min_box = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"min_box");
    max_box = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"max_box");
    min_box->set(-1.0f,0);
    min_box->set(-1.0f,1);
    min_box->set(-1.0f,2);

    max_box->set( 1.0f,0);
    max_box->set( 1.0f,1);
    max_box->set( 1.0f,2);
    font_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"font_size");
    font_align = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"font_align");
    font_size->set(1.0f);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    myf = new vsx_font();
    myf->init("/home/jaw/vsxu-dev/vsxu/share/font/font-ascii.png");
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    mesh = mesh_in->get_addr();
    if (!mesh) return;

    float minx = min_box->get(0);
    float miny = min_box->get(1);
    float minz = min_box->get(2);

    float maxx = max_box->get(0);
    float maxy = max_box->get(1);
    float maxz = max_box->get(2);
    float fs = font_size->get();

    myf->align = font_align->get();
    glColor4f(base_color->get(0),base_color->get(1),base_color->get(2),base_color->get(3));



    for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i)
    {
      if ( !( (*mesh)->data->vertices[i].x > minx && (*mesh)->data->vertices[i].x < maxx ) ) continue;
      if ( !( (*mesh)->data->vertices[i].y > miny && (*mesh)->data->vertices[i].y < maxy ) ) continue;
      if ( !((*mesh)->data->vertices[i].z > minz && (*mesh)->data->vertices[i].z < maxz) ) continue;

      myf->print(
          (*mesh)->data->vertices[i],
          i2s(i), 0.007f * fs
      );
    }

    render_out->set(1);
  }

};
