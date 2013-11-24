class module_particlesystem_render_c : public vsx_module
{
public:

  // in
  vsx_module_param_particlesystem* particles_in;
  vsx_module_param_texture* tex_inf;
  vsx_module_param_float3* position;
  vsx_module_param_float* alpha;
  vsx_module_param_float* size;

  // out
  vsx_module_param_render* render_result;

  // internal
  vsx_particlesystem* particles;
  vsx_texture** tex;
  unsigned long i;

  vsx_gl_state* gl_state;

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;particlesystems;render_particle_center";
    info->in_param_spec = "particlesystem:particlesystem,texture:texture,position:float3,alpha:float,size:float";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    particles_in = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem",true,true);
    tex_inf = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture",true,true);
    position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");
    position->set(0.0f,0);
    position->set(1.0f,1);
    position->set(0.0f,2);
    alpha = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"alpha");
    alpha->set(1.0f);
    size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"size");
    size->set(1.0f);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);

    gl_state = vsx_gl_state::get_instance();
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    particles = particles_in->get_addr();
    if (particles)
    {
      tex = tex_inf->get_addr();
      float local_alpha = alpha->get();
      if (tex)
      {
        if (!((*tex)->valid))
        {
          return;
        }
        float cx = position->get(0);
        float cy = position->get(1);
        float cz = position->get(2);
        vsx_transform_obj texture_transform = (*tex)->get_transform();

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();

        if ((*tex)->get_transform()) texture_transform();

        (*tex)->bind();
        beginBlobs( gl_state );
        glBegin(GL_QUADS);

        float sz = size->get();
        int count = 0;
        for (i = 0; i < particles->particles->size(); ++i)
        {
          if ((*particles->particles)[i].size > 0.0f)
          {
            glColor4f(
              (*particles->particles)[i].color.r*local_alpha,
              (*particles->particles)[i].color.g*local_alpha,
              (*particles->particles)[i].color.b*local_alpha,
              (*particles->particles)[i].color.a
            );
            drawBlob_c(
              (*particles->particles)[i].pos.x,
              (*particles->particles)[i].pos.y,
              (*particles->particles)[i].pos.z,
              (*particles->particles)[i].size*sz,
              cx,cy,cz
            );
            ++count;
          }
        }
        glEnd();
        (*tex)->_bind();
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        render_result->set(1);
      }
    }
    render_result->set(0);
  }
};

