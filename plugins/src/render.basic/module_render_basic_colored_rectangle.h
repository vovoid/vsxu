
class module_render_basic_colored_rectangle : public vsx_module
{
  // in
  vsx_module_param_float3* position;
  vsx_module_param_float3* size;
  vsx_module_param_float* angle;
  vsx_module_param_float3* rotation_axis;
  vsx_module_param_float4* color_rgb;
  vsx_module_param_int* border;
  vsx_module_param_float* border_width;
  vsx_module_param_float4* border_color;

  // out
  vsx_module_param_render* render_result;

  // internal
  vsx_matrix saved_modelview;

  vsx_gl_state* gl_state;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier =
    "renderers;basic;colored_rectangle"
    "||"
    "renderers;examples;simple";

    info->description =
      "Renders a filled rectangle\n"
      "with optional border.\n"
      "Supports size (x+y), rotating around an axis,\n"
      "position and color (rgba)."
    ;

    info->in_param_spec =
      "spatial:complex{"
        "position:float3,"
        "angle:float,"
        "rotation_axis:float3,"
        "size:float3"
      "},"
      "border:complex{"
        "border_enable:enum?no|yes,"
        "border_width:float,"
        "border_color:float4?default_controller=controller_col"
      "},"
      "color:float4?default_controller=controller_col"
    ;

    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "position");
    position->set(0.0f, 0);
    position->set(0.0f, 1);
    position->set(0.0f, 2);

    size = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "size");
    size->set(1.0f,0);
    size->set(0.3f,1);
    angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "angle");

    border = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "border_enable");
    border_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "border_width");
    border_width->set(1.5f);

    border_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "border_color");
    border_color->set(0.0f,0);
    border_color->set(0.0f,1);
    border_color->set(0.0f,2);
    border_color->set(1.0f,3);

    rotation_axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "rotation_axis");
    rotation_axis->set(1.0f, 0);
    rotation_axis->set(1.0f, 1);
    rotation_axis->set(0.0f, 2);
    color_rgb = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color");
    color_rgb->set(1.0f,0);
    color_rgb->set(1.0f,1);
    color_rgb->set(1.0f,2);
    color_rgb->set(1.0f,3);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);

    gl_state = vsx_gl_state::get_instance();
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    gl_state->matrix_get_v( VSX_GL_MODELVIEW_MATRIX, saved_modelview.m );
    gl_state->matrix_mode( VSX_GL_MODELVIEW_MATRIX);
    gl_state->matrix_translate_f( position->get(0),position->get(1),position->get(2) );
    gl_state->matrix_rotate_f( (float)angle->get()*360, rotation_axis->get(0), rotation_axis->get(1), rotation_axis->get(2) );
    gl_state->matrix_scale_f( size->get(0), size->get(1), size->get(2) );

    #ifndef VSXU_OPENGL_ES_2_0
      glColor4f(color_rgb->get(0),color_rgb->get(1),color_rgb->get(2),color_rgb->get(3));
    #endif
    #ifdef VSXU_OPENGL_ES
      const GLfloat square_vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f,   1.0f, 0.0f
      };
      const GLfloat square_colors[] = {
        color_rgb->get(0), color_rgb->get(1),color_rgb->get(2),color_rgb->get(3),
        color_rgb->get(0), color_rgb->get(1),color_rgb->get(2),color_rgb->get(3),
        color_rgb->get(0), color_rgb->get(1),color_rgb->get(2),color_rgb->get(3),
        color_rgb->get(0), color_rgb->get(1),color_rgb->get(2),color_rgb->get(3),
      };
      #ifdef VSXU_OPENGL_ES_1_0
        glDisableClientState(GL_COLOR_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, square_vertices);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      #endif
      #ifdef VSXU_OPENGL_ES_2_0
        vsx_es_begin();
        glEnableVertexAttribArray(0);
        vsx_es_set_default_arrays((GLvoid*)&square_vertices, (GLvoid*)&square_colors);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        vsx_es_end();
      #endif
    #endif

    #ifndef VSXU_OPENGL_ES
      glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0f,1.0f);
        glVertex3f(-1.0f,  1.0f, 0.0f);
        glTexCoord2f(1.0f,1.0f);
        glVertex3f( 1.0f,  1.0f, 0.0f);
        glTexCoord2f(1.0f,0.0f);
        glVertex3f( 1.0f, -1.0f, 0.0f);
      glEnd();

      if (border->get())
      {
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(border_width->get());
        glBegin(GL_LINE_STRIP);
          glColor4f(border_color->get(0),border_color->get(1),border_color->get(2),border_color->get(3));
          glVertex3f(-1, -1.0f, 0);
          glVertex3f(-1, 1.0f, 0);
          glVertex3f( 1, 1.0f, 0);
          glVertex3f( 1, -1.0f, 0);
          glVertex3f(-1, -1.0f, 0);
        glEnd();
      }
    #endif
    gl_state->matrix_load_identity();
    gl_state->matrix_mult_f( saved_modelview.m );
    render_result->set(1);
    loading_done = true;
  }
};
