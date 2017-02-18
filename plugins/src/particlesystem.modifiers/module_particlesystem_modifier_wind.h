class module_particlesystem_modifier_wind : public vsx_module
{
  float time;
  vsx_particlesystem<>* particles;
  vsx_module_param_particlesystem* in_particlesystem;
  vsx_module_param_float3* wind;
  // out
  vsx_module_param_particlesystem* result_particlesystem;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "particlesystems;modifiers;basic_wind_deformer";

    info->description =
      "Applies \"wind\" to particles.\n"
      "Moving all particles in one direction."
    ;

    info->out_param_spec =
      "particlesystem:particlesystem";

    info->in_param_spec =
      "in_particlesystem:particlesystem,"
      "wind:float3"
    ;

    info->component_class =
      "particlesystem";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");
  //  in_particlesystem->set(particles);

    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");
  //  result_particlesystem->set(particles);

    // oh, all these parameters.. ..
    wind = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"wind");
    // set the position of the particle spray emitter to the center
    wind->set(0,0);
    wind->set(0,1);
    wind->set(0,2);
    // default is 100 particles, should be enough for most effects (tm)
  }

  void run()
  {
    particles = in_particlesystem->get_addr();
    if (!particles) return;

    // get positions from the user
    float px = wind->get(0);
    float py = wind->get(1);
    float pz = wind->get(2);

    // go through all particles
    for (unsigned long i = 0; i <  particles->particles->size(); ++i)
    {
      // add the delta-time to the time of the particle
      (*particles->particles)[i].pos.x += px*engine_state->dtime;
      (*particles->particles)[i].pos.y += py*engine_state->dtime;
      (*particles->particles)[i].pos.z += pz*engine_state->dtime;
    }
    // in case some modifier has decided to base some mesh or whatever on the particle system
    // increase the timsetamp so that module can know that it has to copy the particle system all
    // over again.
    // bear in mind however that particlsystems are not commonly copied (unless you want to base something on it)
    // as a particle lives on to the next frame. So modifiers should just work with the values and increase or
    // decrease them. for instance, wind is applied adding or subtracting a value.. or possibly modifying the
    // speed component.

    // set the resulting value
    result_particlesystem->set_p(*particles);
    return;
    // now all left is to render this, that will be done one of the modules of the rendering branch
    result_particlesystem->valid = false;
  }
};
