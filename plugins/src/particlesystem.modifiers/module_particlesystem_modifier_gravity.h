class module_particlesystem_modifier_gravity : public vsx_module
{
  float time;
  vsx_particlesystem<>* particles;
  vsx_module_param_particlesystem* in_particlesystem;
  vsx_module_param_float3* center;
  vsx_module_param_float3* amount;
  vsx_module_param_float3* friction;
  vsx_module_param_int* mass_type;
  vsx_module_param_int* time_source;
  vsx_module_param_float* uniform_mass;
  // out
  vsx_module_param_particlesystem* result_particlesystem;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "particlesystems;modifiers;basic_gravity";

    info->description =
      "";

    info->out_param_spec =
      "particlesystem:particlesystem";

    info->in_param_spec =
      "in_particlesystem:particlesystem,"
      "basic_parameters:complex"
      "{"
        "center:float3,"
        "amount:float3,"
        "friction:float3,"
        "time_source:enum?sequencer|real"
      "},"
      "mass_calculations:complex"
      "{"
        "mass_type:enum?individual|uniform,"
        "uniform_mass:float"
      "}"
    ;

    info->component_class =
      "particlesystem";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");
    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");
    mass_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"mass_type");
    time_source = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_source");
    uniform_mass = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"uniform_mass");
    uniform_mass->set(0.1f);
    center = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"center");
    center->set(0.0f,0);
    center->set(0.0f,1);
    center->set(0.0f,2);
    amount = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"amount");
    amount->set(0.0f,0);
    amount->set(1.0f,1);
    amount->set(0.0f,2);
    friction = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"friction");
    friction->set(1.5f,0);
    friction->set(1.5f,1);
    friction->set(1.5f,2);
  }

  void run()
  {
    float ddtime;
    if (time_source->get())
    {
       ddtime = engine_state->real_dtime;
    }
    else
    {
      ddtime = engine_state->dtime;
    }
    particles = in_particlesystem->get_addr();

    if (particles)
    {

      // get positions from the user
      float cx = center->get(0);
      float cy = center->get(1);
      float cz = center->get(2);

      // go through all particles
      float orig_size;
      float fricx = 1.0f-friction->get(0) * ddtime;
      float fricy = 1.0f-friction->get(1) * ddtime;
      float fricz = 1.0f-friction->get(2) * ddtime;
      float ax = amount->get(0)*ddtime;
      float ay = amount->get(1)*ddtime;
      float az = amount->get(2)*ddtime;
      if (mass_type->get() == 0) {
        unsigned long nump = particles->particles->size();
        vsx_particle<>* pp = particles->particles->get_pointer();
        for (unsigned long i = 0; i <  nump; ++i) {
          if ((*pp).time < (*pp).lifetime) {
            // add the delta-time to the time of the particle
            orig_size = 1.0f / (*pp).orig_size;
            //printf("orig size: %f\n", orig_size);
            (*pp).speed.x += ax*((cx - (*pp).pos.x) * orig_size);
            (*pp).speed.x *= fricx;

            (*pp).speed.y += ay*((cy - (*pp).pos.y) * orig_size);
            (*pp).speed.y *= fricy;

            (*pp).speed.z += az*((cz - (*pp).pos.z) * orig_size);
            (*pp).speed.z *= fricz;
          }
          pp++;
        }
      } else
      {
        orig_size = 1.0f / uniform_mass->get();
        unsigned long nump = particles->particles->size();
        vsx_particle<>* pp = particles->particles->get_pointer();
        for (unsigned long i = 0; i <  nump; ++i) {
          if ((*pp).time < (*pp).lifetime) {
            // add the delta-time to the time of the particle
            (*pp).speed.x += ax*((cx - (*pp).pos.x) * orig_size);
            (*pp).speed.x *= fricx;

            (*pp).speed.y += ay*((cy - (*pp).pos.y) * orig_size);
            (*pp).speed.y *= fricy;

            (*pp).speed.z += az*((cz - (*pp).pos.z) * orig_size);
            (*pp).speed.z *= fricz;
          }
          pp++;
        }
      }

      // set the resulting value
      result_particlesystem->set_p(*particles);

      return;
      // now all left is to render this, that will be done one of the modules of the rendering branch
    }
    result_particlesystem->valid = false;
  }
};
