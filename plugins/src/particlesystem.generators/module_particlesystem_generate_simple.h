#include <math/vsx_rand.h>

class module_particlesystem_generate_simple : public vsx_module
{
  int i;
  float time;
  bool first;
  float px, py, pz, rr, gg, bb, aa;
  float nump;
  float size_base, size_random_weight;
  float lifetime_base, lifetime_random_weight;
  vsx_quaternion<> q1;
  vsx_quaternion<>* q_out;
  vsx_rand rand;

  float random_numbers[8192];
  size_t random_number_index;


  vsx_particlesystem<> particles;

  vsx_module_param_float* particles_per_second;
  float particles_to_go;
  vsx_module_param_float3* spray_pos;

  vsx_module_param_float* speed_x;
  vsx_module_param_float* speed_y;
  vsx_module_param_float* speed_z;
  float spd_x, spd_y, spd_z;
  vsx_module_param_int* speed_type;

  vsx_module_param_int* time_source;

  vsx_module_param_float4* color;
  vsx_module_param_float* particles_count;
  vsx_module_param_float* particle_size_base;
  vsx_module_param_float* particle_size_random_weight;

  vsx_module_param_float* particle_lifetime_base;
  vsx_module_param_float* particle_lifetime_random_weight;

  vsx_module_param_quaternion* particle_rotation_dir;
  // out
  vsx_module_param_particlesystem* result_particlesystem;

public:
  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "particlesystems;generators;basic_spray_emitter";

    info->description =
      "Generates a particlesystem emitting\n"
      "from one point/vertex.\n"
      "With num_particles you set how many\n"
      "particles you want. Default is 100,\n"
      "but you usually want a lot more."
    ;

    info->out_param_spec =
      "particlesystem:particlesystem";

    info->in_param_spec =
      "num_particles:float?nc=1,"
      "particles_per_second:float,"
      "spatial:complex"
      "{"
        "emitter_position:float3,"
        "speed:complex"
        "{"
          "speed_x:float,"
          "speed_y:float,"
          "speed_z:float"
        "},"
        "speed_type:enum?random_balanced|directional,"
        "size:complex"
        "{"
          "particle_size_base:float,particle_size_random_weight:float"
        "},"
        "time_source:enum?sequencer|real,"
        "particle_rotation_dir:quaternion,"
        "enable_rotation:enum?true|false"
      "},"
      "appearance:complex"
      "{"
        "color:float4?default_controller=controller_col,"
        "time:complex"
        "{"
          "particle_lifetime_base:float,"
          "particle_lifetime_random_weight:float"
        "}"
      "}"
    ;

    info->component_class =
      "particlesystem";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");

    // oh, all these parameters.. ..
    spray_pos = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"emitter_position");

    speed_x = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed_x");
    speed_y = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed_y");
    speed_z = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed_z");
    speed_x->set(1);
    speed_y->set(1);
    speed_z->set(1);
    speed_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"speed_type");
    time_source = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_source");

    color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    // set the color to all white. white is clean baby!
    color->set(1,0);
    color->set(1,1);
    color->set(1,2);
    color->set(1,3);

    particles_per_second = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particles_per_second");
    particles_per_second->set(-1.0f);
    particles_to_go = 0.0f;

    particles_count = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_particles");
    particle_size_base = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particle_size_base");
    particle_size_base->set(0.1f);
    particle_size_random_weight = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particle_size_random_weight");
    particle_size_random_weight->set(0.01f);

    particle_lifetime_base = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particle_lifetime_base");
    particle_lifetime_base->set(2.0f);
    particle_lifetime_random_weight = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particle_lifetime_random_weight");
    particle_lifetime_random_weight->set(1.0f);

    particle_rotation_dir = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"particle_rotation_dir");
    particle_rotation_dir->set(0.0f, 0);
    particle_rotation_dir->set(0.0f, 1);
    particle_rotation_dir->set(0.0f, 2);
    particle_rotation_dir->set(1.0f, 3);
    // out

    // set the position of the particle spray emitter to the center
    spray_pos->set(0,0);
    spray_pos->set(0,1);
    spray_pos->set(0,2);
    // default is 100 particles, should be enough for most effects (tm)
    particles_count->set(100);
    particles.timestamp = 0;
    particles.particles = new vsx_ma_vector< vsx_particle<> >;
    //result_particlesystem->set_p(particles);
    first = true;

    for (size_t i = 0; i < 8192; i++)
    {
      random_numbers[i] = rand.frand();
    }
    random_number_index = 0;
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

    if (first || (ddtime < 0))
    {
      for (i = 0; i < particles_count->get(); ++i)
      {
        (*particles.particles)[i].color = vsx_color<>(1,1,1,1);
        (*particles.particles)[i].orig_size = (*particles.particles)[i].size = 0;
        (*particles.particles)[i].pos.x = 0;
        (*particles.particles)[i].pos.y = 0;
        (*particles.particles)[i].pos.z = 0;
        (*particles.particles)[i].creation_pos = (*particles.particles)[i].pos;
        (*particles.particles)[i].speed.x = 0;
        (*particles.particles)[i].speed.y = 0;
        (*particles.particles)[i].speed.z = 0;
        (*particles.particles)[i].time = 3;
        (*particles.particles)[i].lifetime = 2;
        (*particles.particles)[i].rotation_dir = vsx_quaternion<>(0,0,0,0);
      }
      first = false;
      return;
    }

    // calculate how many particles to render
    if (particles_per_second->get() < 1) particles_to_go = -1.0f;
    else
    particles_to_go += particles_per_second->get()*ddtime;
    // this code does not render the particle system, it only defines it and
    // manages the positions of the particles, run each frame.
    size_base = particle_size_base->get();
    size_random_weight = particle_size_random_weight->get();
    lifetime_base = particle_lifetime_base->get();
    lifetime_random_weight = particle_lifetime_random_weight->get();
    spd_x = speed_x->get();
    spd_y = speed_y->get();
    spd_z = speed_z->get();

    float half_spd_x = spd_x * 0.5f;
    float half_spd_y = spd_y * 0.5f;
    float half_spd_z = spd_z * 0.5f;
    float half_lifetime_random_weight = lifetime_random_weight*0.5f;

    // get positions from the user
    px = spray_pos->get(0);
    py = spray_pos->get(1);
    pz = spray_pos->get(2);
    // get colors from the user
    rr = color->get(0);
    gg = color->get(1);
    bb = color->get(2);
    aa = color->get(3);
    // get number of active particles from the user
    nump = (long)particles_count->get();
    // some out of bounds-checks
    if (nump < 0) nump = 0;
  //  if (nump > 2000) nump = 2000;
    // update the particle system with the new count so the renderer (and modifiers) can read it
    if (nump < particles.particles->size())
    particles.particles->reset_used((unsigned long)ceil(nump));
    long p_to_go;
    if (particles_per_second->get() < 0.0f)
    p_to_go = 100000000;
    else
    p_to_go = (long)round(particles_per_second->get()*ddtime);

    vsx_quaternion<> c_rotation_dir;
    c_rotation_dir.x = particle_rotation_dir->get(0);
    c_rotation_dir.y = particle_rotation_dir->get(1);
    c_rotation_dir.z = particle_rotation_dir->get(2);
    c_rotation_dir.w = particle_rotation_dir->get(3);
    c_rotation_dir.normalize();

    float c_lifetime = lifetime_base - half_lifetime_random_weight;

    // go through all particles
    for (i = 0; i < nump; ++i)
    {
      // add the delta-time to the time of the particle
      (*particles.particles)[i].time += ddtime;
      // if the time got over the maximum lifetime of the particle, re-initialize it
      if (
          p_to_go > 1
          &&
          (*particles.particles)[i].time > (*particles.particles)[i].lifetime)
      {
        (*particles.particles)[i].size = (*particles.particles)[i].orig_size = size_base+rand.frand()*size_random_weight-size_random_weight*0.5f;
        (*particles.particles)[i].color = vsx_color<>(rr,gg,bb,aa);
        switch (speed_type->get()) {
          case 0:
            (*particles.particles)[i].speed.x = spd_x * rand.frand() - half_spd_x;
            (*particles.particles)[i].speed.y = spd_y * rand.frand() - half_spd_y;
            (*particles.particles)[i].speed.z = spd_z * rand.frand() - half_spd_z;
          break;
          case 1:
            (*particles.particles)[i].speed.x = spd_x;
            (*particles.particles)[i].speed.y = spd_y;
            (*particles.particles)[i].speed.z = spd_z;
          break;
        } // switch

        (*particles.particles)[i].rotation.x = rand.frand()*2.0f-1.0f;
        (*particles.particles)[i].rotation.y = rand.frand()*2.0f-1.0f;
        (*particles.particles)[i].rotation.z = rand.frand()*2.0f-1.0f;
        (*particles.particles)[i].rotation.w = rand.frand()*2.0f-1.0f;
        (*particles.particles)[i].rotation.normalize();

        (*particles.particles)[i].rotation_dir = c_rotation_dir;

        (*particles.particles)[i].pos.x = px;
        (*particles.particles)[i].pos.y = py;
        (*particles.particles)[i].pos.z = pz;
        (*particles.particles)[i].creation_pos = (*particles.particles)[i].pos;
        (*particles.particles)[i].time = 0.0f;
        (*particles.particles)[i].lifetime = c_lifetime + rand.frand() * lifetime_random_weight;
        (*particles.particles)[i].one_div_lifetime = 1.0f / (*particles.particles)[i].lifetime;
        --p_to_go;
      }
      // add the speed component to the particles
      (*particles.particles)[i].pos.x += (*particles.particles)[i].speed.x * ddtime;
      (*particles.particles)[i].pos.y += (*particles.particles)[i].speed.y * ddtime;
      (*particles.particles)[i].pos.z += (*particles.particles)[i].speed.z * ddtime;

      if (random_number_index > 8000) random_number_index = 0;

      q_out = &(*particles.particles)[i].rotation;
      q1 = (*particles.particles)[i].rotation_dir;
      q1.normalize();
      q_out->mul(*q_out, q1);
    }

    (*particles.particles)[particles.particles->size()-1].color.a = nump-(float)floor(nump);


    // in case some modifier has decided to base some mesh or whatever on the particle system
    // increase the timsetamp so that module can know that it has to copy the particle system all
    // over again.
    ++particles.timestamp;
    // bear in mind however that particlsystems are not commonly copied (unless you want to base something on it)
    // as a particle lives on to the next frame. So modifiers should just work with the values and increase or
    // decrease them. for instance, wind is applied adding or subtracting a value.. or possibly modifying the
    // speed component.

    // set the resulting value
    result_particlesystem->set_p(particles);
    // now all left is to render this, that will be done one of the modules of the rendering branch
  }

  void on_delete() {
    delete particles.particles;
  }
};
