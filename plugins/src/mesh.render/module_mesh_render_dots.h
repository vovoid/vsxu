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


class module_mesh_render_dots : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float4* base_color;
  vsx_module_param_float* dot_size;

  // out
  vsx_module_param_render* render_out;

  // internal
  vsx_mesh** mesh;
  vsx_matrix ma;
  vsx_vector upv;

  float prev_psize;
  bool list_built;
  vsx_gl_state* gl_state;

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;mesh;mesh_dot_render";
    info->description = "Renders a dot at each vertex in the mesh.";
    info->in_param_spec = "mesh_in:mesh,base_color:float4,dot_size:float";
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
    dot_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"dot_size");
    dot_size->set(1.0f);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

    gl_state = vsx_gl_state::get_instance();
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);

    prev_psize = gl_state->point_size_get();

    gl_state->point_size_set( dot_size->get() );

    glEnable(GL_POINT_SMOOTH);
    mesh = mesh_in->get_addr();
    if (mesh)
    {
      glColor4f(base_color->get(0),base_color->get(1),base_color->get(2),base_color->get(3));

      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, sizeof(vsx_vector), (*mesh)->data->vertices.get_pointer());
      glDrawArrays(GL_POINTS,0,(*mesh)->data->vertices.size());
      glDisableClientState(GL_VERTEX_ARRAY);
    }
    gl_state->point_size_set(prev_psize);

    render_out->set(1);
  }
};
