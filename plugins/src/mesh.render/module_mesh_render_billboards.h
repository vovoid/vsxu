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


class module_mesh_render_billboards : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float4* base_color;
  vsx_module_param_float* dot_size;

  vsx_module_param_string* i_vertex_program;
  vsx_module_param_string* i_fragment_program;
  vsx_module_param_int* use_display_list;
  // out
  vsx_module_param_render* render_out;

  // internal
  vsx_mesh** mesh;
  vsx_matrix ma;
  vsx_vector upv;

  vsx_gl_state* gl_state;

  unsigned long mesh_timestamp;

  GLuint dlist;
  bool list_built;
  vsx_glsl shader;

  void param_set_notify(const vsx_string& name)
  {
    if ((name == "vertex_program" || name == "fragment_program"))
    {
      //printf("PARAM_SET_NOTIFY %s\n\n",name.c_str());
      shader.vertex_program = i_vertex_program->get();
      shader.fragment_program = i_fragment_program->get();
      message = shader.link();

      if (message.size() == 0)
      {
        redeclare_in = true;
        message = "module||ok";
      }
    }
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;mesh;mesh_dot_billboards";
    info->description = "Renders a texture billboard at each vertex in the mesh.";
    info->in_param_spec = "mesh_in:mesh,base_color:float4,dot_size:float,use_display_list:enum?no|yes,shader_params:complex{vertex_program:string,fragment_program:string"+shader.get_param_spec()+"}";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    loading_done = true;
    mesh_timestamp = 0;
  }

  void redeclare_in_params(vsx_module_param_list& in_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    base_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"base_color");
    base_color->set(1.0f,0);
    base_color->set(1.0f,1);
    base_color->set(1.0f,2);
    base_color->set(1.0f,3);
    dot_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"dot_size");
    dot_size->set(1.0f);
    use_display_list = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_display_list");
    use_display_list->set(0);

    i_fragment_program = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"fragment_program");
    i_fragment_program->set(shader.fragment_program);
    i_vertex_program = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"vertex_program");
    i_vertex_program->set(shader.vertex_program.c_str());

    shader.declare_params(in_parameters);
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    shader.vertex_program = ""
      "varying float particle_alpha;\n"
      "\n"
      "void main(void)\n"
      "{\n"
      "  // vertex id is stored in gl_Vertex.w, ugly hack!\n"
      "  // thus we need to reset it here\n"
      "  gl_Vertex.w = 1.0;\n"
      "  gl_Position = ftransform();\n"
      "  float vertDist = distance(vec3(gl_Position.x,gl_Position.y,gl_Position.z), vec3(0.0,0.0,0.0));\n"
      "  float dist_alpha;\n"
      "  dist_alpha = pow(1 / vertDist,1.1);\n"
      "  gl_PointSize = 460.155 * dist_alpha * (1.0+abs((sin(gl_Vertex.x*10.0) * sin (gl_Vertex.z*20.0))));\n"
      "  particle_alpha = pow(dist_alpha,1.2);\n"
      "  if (gl_PointSize < 1.0) particle_alpha = gl_PointSize;\n"
      "}"
    ;
    shader.fragment_program =
      "uniform sampler2D _tex;\n"
      "varying float particle_alpha;\n"
      "void main(void)\n"
      "{\n"
        "vec2 l_uv=gl_PointCoord;\n"
        "const vec2 l_offset = vec2(0.5,0.5);\n"
        "l_uv-=l_offset;\n"
        "l_uv=vec2(vec4(l_uv,0.0,1.0));\n"
        "l_uv+=l_offset;\n"
        "vec4 a = texture2D(_tex, l_uv);\n"
        "gl_FragColor = vec4(a.r, a.g, a.b, a.a * min(particle_alpha,1.0) * 0.1);\n"
      "}\n"
    ;

    loading_done = true;
    list_built = false;
    vsx_string h = shader.link();
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    redeclare_in_params(in_parameters);
    gl_state = vsx_gl_state::get_instance();
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    mesh = mesh_in->get_addr();
    if (!mesh) return;

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
      shader.begin();

      if (shader.uniform_map.find("_vx") != shader.uniform_map.end())
      {
        vsx_module_param_float* p = (vsx_module_param_float*)shader.uniform_map["_vx"]->module_param;
        if (p) p->set(gl_state->viewport_get_width());
      }

    shader.set_uniforms();
    glEnable( GL_POINT_SPRITE_ARB );
    glEnable(GL_POINT_SMOOTH);

    if ((!use_display_list->get()
      ||
      mesh_timestamp != (*mesh)->timestamp)
      &&
      list_built) {
      list_built = false;
      glDeleteLists(dlist,1);
    }

    if (!list_built)
    {
      mesh_timestamp = (*mesh)->timestamp;
      // init list -------------------------------------
      if (use_display_list->get() && list_built == false)
      {
        dlist = glGenLists(1);
        glNewList(dlist,GL_COMPILE);
      }
        // draw --------------------
        glBegin(GL_POINTS);
        // ugly hack to support vertex id
        float vid = 0.0f;
        float dvdi = 1.0f / (float)(*mesh)->data->vertices.size();
        for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i) {
          glVertex4f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z,vid);
          vid += dvdi;
        }
        glEnd();
        // -------------------------
      if (use_display_list->get() && list_built == false)
      {
        glEndList();
        list_built = true;
      }
    }
    else
    {
      glCallList(dlist);
    }
    // kill list -------------------------------------


    glDisable(GL_POINT_SMOOTH);
    glDisable( GL_POINT_SPRITE_ARB );
    shader.end();
    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

    render_out->set(1);
  }
};
