#include <tools/vsx_req.h>

class module_render_gl_texture_bind : public vsx_module
{
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_texture* tex_in;

  // out
  vsx_module_param_render* render_out;

  // internal
  vsx_texture<>** t_tex;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "texture;opengl;texture_bind";

    info->description =
      "Uses OpenGL to bind a texture.\n"
      "All connected to it (unless \n"
      "it declares its own textures) \n"
      "will use the texture \n"
      "provided in the tex_in."
    ;

    info->in_param_spec =
      "render_in:render,"
      "tex_in:texture"
    ;

    info->out_param_spec =
      "render_out:render";

    info->component_class =
      "texture";

    loading_done = true;

    info->tunnel = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_in->set(0);
    render_in->run_activate_offscreen = true;
    render_out->set(0);
    tex_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"tex_in");
    //tex_in->set(&i_tex);
  }

  bool activate_offscreen() {
    t_tex = tex_in->get_addr();
    reqrv(t_tex, true);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    if ((*t_tex)->get_transform())
      (*t_tex)->get_transform()->transform();

    glMatrixMode(GL_MODELVIEW);

    (*t_tex)->bind();
    return true;
  }

  void deactivate_offscreen() {
    req(t_tex);
    (*t_tex)->_bind();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }
};


