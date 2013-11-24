const unsigned int tex_wrap[] = {
  GL_REPEAT,
#ifndef VSXU_OPENGL_ES
  GL_CLAMP,
#else
  GL_REPEAT,
#endif
  GL_CLAMP_TO_EDGE,
#ifndef VSXU_OPENGL_ES
  GL_CLAMP_TO_BORDER,
  GL_MIRRORED_REPEAT
#else
  GL_REPEAT,
  GL_OES_texture_mirrored_repeat
#endif
};

const unsigned int tex_filter[] = {
  GL_NEAREST,
  GL_LINEAR,
  GL_NEAREST_MIPMAP_NEAREST,
  GL_LINEAR_MIPMAP_NEAREST,
  GL_NEAREST_MIPMAP_LINEAR,
  GL_LINEAR_MIPMAP_LINEAR
};


class module_texture_parameter : public vsx_module {
  // in
  vsx_module_param_texture* texture_info_param_in;
  vsx_module_param_int* wrap_s;
  vsx_module_param_int* wrap_t;
  vsx_module_param_int* min_filter;
  vsx_module_param_int* mag_filter;
  vsx_module_param_int* anisotropic_filter;
  vsx_module_param_float4* border_color;

  // out
  vsx_module_param_texture* texture_result;
  // internal
  vsx_texture** texture_out;

public:

  void module_info(vsx_module_info* info) {
    info->identifier = "texture;modifiers;tex_parameters";
  #ifndef VSX_NO_CLIENT
    info->in_param_spec = "texture_in:texture,parameters:complex{"
    "wrap_s:enum?repeat|"
  #ifndef VSXU_OPENGL_ES
    "clamp|"
  #endif
    "clamp_to_edge"
    "|clamp_to_border|mirrored_repeat"
    ","
    "wrap_t:enum?repeat|clamp|clamp_to_edge|clamp_to_border|mirrored_repeat,"
    "border_color:float4,"
    "anisotropic_filtering:enum?no|yes,"
    "min_filter:enum?nearest|linear|nearest_mipmap_nearest|linear_mipmap_nearest|nearest_mipmap_linear|linear_mipmap_linear,"
    "mag_filter:enum?nearest|linear"
    "}";
    info->out_param_spec = "texture_rotate_out:texture";
    info->component_class = "texture";
  #endif
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    texture_info_param_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in");
    loading_done = true;

    wrap_t = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "wrap_s");
    wrap_s = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "wrap_t");
    min_filter = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "min_filter");
    mag_filter = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mag_filter");
    anisotropic_filter = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "anisotropic_filtering");
    border_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "border_color");
    border_color->set(0,0);
    border_color->set(0,1);
    border_color->set(0,2);
    border_color->set(0,3);
    texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_rotate_out");
  }

  void run() {
    texture_out = texture_info_param_in->get_addr();
    if (texture_out)
    {
     if (param_updates)
     {
        (*texture_out)->bind();
  #ifdef VSXU_OPENGL_ES
        if (GL_EXT_texture_filter_anisotropic)
  #endif
  #ifndef VSXU_OPENGL_ES
          if (GLEW_EXT_texture_filter_anisotropic)
  #endif
          {
          float rMaxAniso;
          glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &rMaxAniso);
          if (anisotropic_filter->get())
          glTexParameterf((*texture_out)->texture_info->ogl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, rMaxAniso);
          else
          glTexParameterf((*texture_out)->texture_info->ogl_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
        }

        float vals[4];
        vals[0] = border_color->get(0);
        vals[1] = border_color->get(1);
        vals[2] = border_color->get(2);
        vals[3] = border_color->get(3);

        glTexParameteri((*texture_out)->texture_info->ogl_type,GL_TEXTURE_MIN_FILTER, tex_filter[min_filter->get()]);
        glTexParameteri((*texture_out)->texture_info->ogl_type,GL_TEXTURE_MAG_FILTER, tex_filter[mag_filter->get()]);
  #ifndef VSXU_OPENGL_ES
        glTexParameterfv((*texture_out)->texture_info->ogl_type, GL_TEXTURE_BORDER_COLOR, vals);
  #endif
        glTexParameteri((*texture_out)->texture_info->ogl_type, GL_TEXTURE_WRAP_T, tex_wrap[wrap_t->get()]);
        glTexParameteri((*texture_out)->texture_info->ogl_type, GL_TEXTURE_WRAP_S, tex_wrap[wrap_s->get()]);
        (*texture_out)->_bind();

        --param_updates;
      }
      ((vsx_module_param_texture*)texture_result)->set(*texture_out);

    }	else {
      texture_result->valid = false;
    }
  }

  void start() {
    ++param_updates;
  }

};

