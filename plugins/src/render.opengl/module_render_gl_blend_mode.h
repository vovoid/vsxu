const unsigned int sfactors[] =
  {
    VSX_GL_ZERO,
    VSX_GL_ONE,
    VSX_GL_DST_COLOR,
    VSX_GL_ONE_MINUS_DST_COLOR,
    VSX_GL_SRC_ALPHA,
    VSX_GL_ONE_MINUS_SRC_ALPHA,
    VSX_GL_DST_ALPHA,
    VSX_GL_ONE_MINUS_DST_ALPHA,
    VSX_GL_CONSTANT_COLOR,
    VSX_GL_ONE_MINUS_CONSTANT_COLOR,
    VSX_GL_CONSTANT_ALPHA,
    VSX_GL_ONE_MINUS_CONSTANT_ALPHA,
    VSX_GL_SRC_ALPHA_SATURATE
  }
;

const unsigned int dfactors[] =
  {
    VSX_GL_ZERO,
    VSX_GL_ONE,
    VSX_GL_SRC_COLOR,
    VSX_GL_ONE_MINUS_SRC_COLOR,
    VSX_GL_SRC_ALPHA,
    VSX_GL_ONE_MINUS_SRC_ALPHA,
    VSX_GL_DST_ALPHA,
    VSX_GL_ONE_MINUS_DST_ALPHA,
    VSX_GL_CONSTANT_COLOR,
    VSX_GL_ONE_MINUS_CONSTANT_COLOR,
    VSX_GL_CONSTANT_ALPHA,
    VSX_GL_ONE_MINUS_CONSTANT_ALPHA
  }
;


class module_render_gl_blend_mode : public vsx_module
{
  GLint prev_src, prev_dst;
  GLboolean isblend;
  // in
  vsx_module_param_int* sfactor;
  vsx_module_param_int* dfactor;
  vsx_module_param_float4* blend_color;
  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  // internal
  GLfloat prev_blend_col[4];

  vsx_gl_state* gl_state;
public:



void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;blend_mode";
  info->description = "Changes the way pixels are blended.\n\
Default is normal transparency:\n\
  GL_SRC_ALPHA,\n\
  GL_ONE_MINUS_SRC_ALPHA\n\
To get sat adding, choose:\n\
  GL_SRC_ALPHA,\n\
  GL_ONE";

  info->in_param_spec =
"render_in:render,"
"source_blend:enum?ZERO|ONE|DST_COLOR|ONE_MINUS_DST_COLOR|SRC_ALPHA|ONE_MINUS_SRC_ALPHA|DST_ALPHA|ONE_MINUS_DST_ALPHA|CONSTANT_COLOR_EXT|ONE_MINUS_CONSTANT_COLOR_EXT|CONSTANT_ALPHA_EXT|ONE_MINUS_CONSTANT_ALPHA_EXT|SRC_ALPHA_SATURATE&nc=1,"
"dest_blend:enum?ZERO|ONE|SRC_COLOR|ONE_MINUS_SRC_COLOR|SRC_ALPHA|ONE_MINUS_SRC_ALPHA|DST_ALPHA|ONE_MINUS_DST_ALPHA|CONSTANT_COLOR_EXT|ONE_MINUS_CONSTANT_COLOR_EXT|CONSTANT_ALPHA_EXT|ONE_MINUS_CONSTANT_ALPHA_EXT&nc=1,"
"blend_color:float4?default_controller=controller_col"
;
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  sfactor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"source_blend");
  sfactor->set(4);
  dfactor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"dest_blend");
  dfactor->set(5);
  blend_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"blend_color");
  blend_color->set(1,0);
  blend_color->set(1,1);
  blend_color->set(1,2);
  blend_color->set(1,3);
  render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  render_in->set(0);
  render_in->run_activate_offscreen = true;
  render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  gl_state = vsx_gl_state::get_instance();
}


bool activate_offscreen()
{
  prev_src = gl_state->blend_src_get();
  prev_dst = gl_state->blend_dst_get();
  gl_state->blend_color_v(prev_blend_col);
  gl_state->blend_func(
    sfactors[sfactor->get()],
    dfactors[dfactor->get()]
  );
  gl_state->blend_color_set(
    blend_color->get(0),
    blend_color->get(1),
    blend_color->get(2),
    blend_color->get(3)
  );
  isblend = gl_state->blend_get();
  gl_state->blend_set(1);

    return true;
}

void deactivate_offscreen()
{
  gl_state->blend_func(
    prev_src,
    prev_dst
  );
  gl_state->blend_color_set(
    prev_blend_col[0],
    prev_blend_col[1],
    prev_blend_col[2],
    prev_blend_col[3]
  );
  gl_state->blend_set(isblend);
}


void output(vsx_module_param_abs* param)
{
  VSX_UNUSED(param);
  render_result->set(render_in->get());
}

};
