class vsx_module_render_gravity_ribbon_particles : public vsx_module
{
  // in
  vsx_module_param_float4* color0;
  vsx_module_param_float4* color1;
  vsx_module_param_float* friction;
  vsx_module_param_float* step_length;
  vsx_module_param_float* ribbon_width;
  vsx_module_param_float* length;
  vsx_particlesystem* particles;
  vsx_module_param_particlesystem* in_particlesystem;

  // out
  vsx_module_param_render* render_result;

  // internal
  vsx_avector<gravity_strip*> gr;
  gravity_strip* grp;
  float last_update;
  unsigned long prev_num_particles;

public:

  bool init()
  {
    last_update = 0;
    return true;
  }

  void on_delete()
  {
    for (unsigned long i = 0; i < gr.size(); i++)
    {
      delete gr[i];
    }
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;particlesystems;render_particle_ribbon";
    info->in_param_spec = "in_particlesystem:particlesystem,"
                          "params:complex"
                          "{"
                            "ribbon_width:float,"
                            "length:float,"
                            "friction:float,"
                            "step_length:float,"
                            "color0:float4,"
                            "color1:float4"
                          "}";

    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    prev_num_particles = 0;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");

    color0 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color0");
    color0->set(1.0f,0);
    color0->set(1.0f,1);
    color0->set(1.0f,2);
    color0->set(0.3f,3);

    color1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color1");
    color1->set(1.0f,0);
    color1->set(1.0f,1);
    color1->set(1.0f,2);
    color1->set(1.0f,3);

    // parameters for the effect
    friction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "friction");
    friction->set(1);

    step_length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_length");
    step_length->set(10);

    ribbon_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "ribbon_width");
    ribbon_width->set(0.2f);

    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "length");
    length->set(1.0f);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    particles = in_particlesystem->get_addr();
    if (!particles) return;

    if (prev_num_particles != particles->particles->size())
    {
      for (unsigned long i = prev_num_particles; i < particles->particles->size(); ++i)
      {
        gr[i] = new gravity_strip;
        gr[i]->init();
        gr[i]->init_strip();
      }
      prev_num_particles = particles->particles->size();
    }

    for (unsigned long i = 0; i < particles->particles->size(); ++i)
    {
      gr[i]->width = ribbon_width->get();
      gr[i]->length = length->get();
      gr[i]->friction = friction->get();
      float tt = ((*particles->particles)[i].time/(*particles->particles)[i].lifetime);
      if (tt < 0.0f) tt = 0.0f;
      if (tt > 1.0f) tt = 1.0f;
      gr[i]->color0[0] = color0->get(0)*tt;
      gr[i]->color0[1] = color0->get(1)*tt;
      gr[i]->color0[2] = color0->get(2)*tt;
      gr[i]->color0[3] = color0->get(3)*tt;

      gr[i]->color1[0] = color1->get(0);
      gr[i]->color1[1] = color1->get(1);
      gr[i]->color1[2] = color1->get(2);
      gr[i]->step_freq = 10.0f * step_length->get();
      gr[i]->update(engine->dtime, (*(particles->particles))[i].pos.x, (*(particles->particles))[i].pos.y, (*(particles->particles))[i].pos.z);
      gr[i]->render();
    }
    render_result->set(1);
  }
};
