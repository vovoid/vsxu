class module_particlesystem_modifier_2d_rotator : public vsx_module
{
  float time;
  vsx_particlesystem<>* particles;
  vsx_module_param_particlesystem* in_particlesystem;
  vsx_module_param_quaternion* rotation_dir;
  // out
  vsx_module_param_particlesystem* result_particlesystem;

  vsx_ma_vector< vsx_quaternion<> > rotation;
  vsx_ma_vector< vsx_quaternion<> > rotation_delta;
  vsx_quaternion<> q1;
  vsx_quaternion<>* q_out;
public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "particlesystems;modifiers;particle_2d_rotator";

    info->description =
      "";

    info->out_param_spec =
      "particlesystem:particlesystem";

    info->in_param_spec =
      "in_particlesystem:particlesystem,"
      "rotation_dir:quaternion"
    ;

    info->component_class =
      "particlesystem";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");

    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");

    rotation_dir = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"wind");
    // set the position of the particle spray emitter to the center
    rotation_dir->set(0.0f,0);
    rotation_dir->set(0.0f,1);
    rotation_dir->set(0.0f,2);
    rotation_dir->set(1.0f,3);
    // default is 100 particles, should be enough for most effects (tm)
  }

  void run() {
    particles = in_particlesystem->get_addr();
    if (particles) {
      if (rotation.size() != particles->particles->size())
      {
        rotation.reset_used();
        rotation_delta.reset_used();
        rotation.allocate(particles->particles->size());
        rotation_delta.allocate(particles->particles->size());
      }
      for (unsigned long i = 0; i < particles->particles->size(); ++i)
      {
        q_out = &(*particles->particles)[i].rotation;
        q1.x = 0.0f;
        q1.w = 1.0f;
        q1.y = rotation_dir->get(0) * engine_state->dtime;
        q1.z = 0.0f;
        q1.normalize();
        q_out->mul(*q_out, q1);

        q1.x = 0.0f;
        q1.w = 1.0f;
        q1.y = 0.0f;
        q1.z = rotation_dir->get(0) * engine_state->dtime;
        q1.normalize();
        q_out->mul(*q_out, q1);
      }
      // get positions from the user
      /*float px = wind->get(0);
      float py = wind->get(1);
      float pz = wind->get(2);

      // go through all particles
      for (unsigned long i = 0; i <  particles->particles->size(); ++i) {
        // add the delta-time to the time of the particle
        (*particles->particles)[i].pos.x += px*engine_state->dtime;
        (*particles->particles)[i].pos.y += py*engine_state->dtime;
        (*particles->particles)[i].pos.z += pz*engine_state->dtime;
      }*/
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
    }
    result_particlesystem->valid = false;
  }
};
