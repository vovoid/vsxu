#include <particlesystem/vsx_particlesystem.h>

class module_particlesystem_generate_mesh : public vsx_module
{
public:

  size_t i;
  float time;

  float px, py, pz, rr, gg, bb, aa;
  long nump;
  float size_base, size_random_weight;
  float lifetime_base, lifetime_random_weight;

  bool first;

  unsigned long meshcoord;

  vsx_rand rand;

  vsx_module_param_mesh* mesh_in;

  vsx_particlesystem<> particles;
  vsx_module_param_float* particles_per_second;
  float particles_to_go;

  vsx_module_param_float* speed_multiplier;
  vsx_module_param_float* speed_random_value;

  vsx_module_param_float* speed_x;
  vsx_module_param_float* speed_y;
  vsx_module_param_float* speed_z;
  float spd_x, spd_y, spd_z;
  vsx_module_param_int* speed_type;

  float center_[3];
  float spread_[3];
  vsx_module_param_float3* center;
  vsx_module_param_float3* spread;
  vsx_module_param_float3* random_deviation;
  vsx_module_param_float3* add_vector;

  vsx_module_param_int* pick_type;

  vsx_module_param_float4* color;
  vsx_module_param_float* particles_count;
  vsx_module_param_float* particle_size_base;
  vsx_module_param_float* particle_size_random_weight;
  vsx_module_param_int* time_source;

  vsx_module_param_float* particle_lifetime_base;
  vsx_module_param_float* particle_lifetime_random_weight;
  vsx_module_param_int* initial_lifetime_random;

  // out
  vsx_module_param_particlesystem* result_particlesystem;
  vsx_ma_vector<float> f_randpool;
  float* f_randpool_pointer;


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "particlesystems;generators;particles_mesh_spray";

    info->description =
      "Uses the vertices of a mesh \n"
      "as origins to spray particles.\n"
      "The locations can be picked randomly or\n"
      "in sequence.\n"
    ;


    info->in_param_spec =
      "mesh_in:mesh,"
      "num_particles:float,"
      "particles_per_second:float,"
      "mesh_properties:complex"
      "{"
        "pick_type:enum?sequential|random,"
        "center:float3,"
        "spread:float3,"
        "random_deviation:float3"
      "},"
      "spatial:complex"
      "{"
        "speed_type:enum?random_balanced|directional|mesh_beam,"
        "speed_multiplier:float,"
        "speed_random_value:float,"
        "speed:complex"
        "{"
          "speed_x:float,"
          "speed_y:float,"
          "speed_z:float"
        "},"
        "add_vector:float3,"
        "size:complex"
        "{"
          "particle_size_base:float,"
          "particle_size_random_weight:float"
        "}"
      "},"
      "appearance:complex"
      "{"
        "color:float4?default_controller=controller_col,"
        "time:complex"
        "{"
          "particle_lifetime_base:float,"
          "particle_lifetime_random_weight:float,"
          "initial_lifetime_random:enum?no|yes"
        "},"
        "time_source:enum?sequencer|real"
      "}"
    ;

    info->out_param_spec =
      "particlesystem:particlesystem";

    info->component_class =
      "particlesystem";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");

    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");

    center = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"center");
    spread = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"spread");
    spread->set(1.0f,0);
    spread->set(1.0f,1);
    spread->set(1.0f,2);
    add_vector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"add_vector");

    random_deviation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"random_deviation");
    time_source = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_source");

    speed_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed_multiplier");
    speed_multiplier->set(1.0f);

    // for directional type
    speed_random_value = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed_random_value");
    speed_random_value->set(0.0f);

    speed_x = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed_x");
    speed_y = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed_y");
    speed_z = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"speed_z");
    speed_x->set(1);
    speed_y->set(1);
    speed_z->set(1);
    speed_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"speed_type");


    pick_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"pick_type");
    pick_type->set(0);

    particles_per_second = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particles_per_second");
    particles_per_second->set(-1.0f);
    particles_to_go = 0.0f;

    color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    // set the color to all white. white is clean baby!
    color->set(1,0);
    color->set(1,1);
    color->set(1,2);
    color->set(1,3);

    particles_count = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_particles");
    particle_size_base = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particle_size_base");
    particle_size_base->set(0.1f);
    particle_size_random_weight = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particle_size_random_weight");
    particle_size_random_weight->set(0.01f);

    particle_lifetime_base = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particle_lifetime_base");
    particle_lifetime_base->set(2.0f);
    particle_lifetime_random_weight = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"particle_lifetime_random_weight");
    particle_lifetime_random_weight->set(1.0f);

    initial_lifetime_random = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "initial_lifetime_random");
    initial_lifetime_random->set(1);

    // out

    meshcoord = 0;

    // default is 100 particles, should be enough for most effects (tm)
    particles_count->set(100);
    //particles.num_particles = 100;
    particles.particles = new vsx_ma_vector<vsx_particle<> >;
    //particles.particles->allocation_increment = 1000;
    particles.timestamp = 0;

    first = true;
  }

  void on_delete()
  {
    delete particles.particles;
  }

  void run() {
    float ddtime;
    if (time_source->get()) {
      ddtime = engine_state->real_dtime;
    } else ddtime = engine_state->dtime;

    if (ddtime < 0)
      first = true;

    float dtime = ddtime;
    // get the mesh
    vsx_mesh<>** our_mesh;
    our_mesh = mesh_in->get_addr();
    if (our_mesh) {

      size_t particle_count = (size_t)particles_count->get();

      center_[0] = center->get(0);
      center_[1] = center->get(1);
      center_[2] = center->get(2);

      spread_[0] = spread->get(0);
      spread_[1] = spread->get(1);
      spread_[2] = spread->get(2);
      // this code does not render the particle system, it only defines it and
      // manages the positions of the particles, run each frame.
      size_base = particle_size_base->get();
      size_random_weight = particle_size_random_weight->get();
      lifetime_base = particle_lifetime_base->get();
      lifetime_random_weight = particle_lifetime_random_weight->get();
      float half_lifetime_random_weight = 0.5f * lifetime_random_weight;

      if (particle_count+1 != (*particles.particles).size())
        first = true;

      if (first)
      {
        //printf("first %d %d\n",(int)particles_count->get(),(int)(*particles.particles).size());
        (*particles.particles).allocate(particle_count);
        (*particles.particles).reset_used(particle_count+1);
        (*particles.particles).memory_clear();
        f_randpool.allocate(particle_count*10);
        f_randpool.memory_clear();
        float* fpp = f_randpool.get_pointer();

        for (i = 0; i < (size_t)particles_count->get()*10; ++i) {
          *fpp = rand.frand();
          fpp++;
        }
        f_randpool_pointer = f_randpool.get_pointer();

        vsx_particle<>* pp =(*particles.particles).get_pointer();
        for (i = 0; i < particle_count; ++i) {
          /*vsx_vector pos; // current position
          vsx_vector speed; // current speed
          vsx_color color; // color it starts out as (including alpha)
          vsx_color color_end; // color it interpolates to (including alpha)
          vsx_quaternion rotation; // rotation vector
          vsx_quaternion rotation_dir; // rotation vector
          float orig_size; // size upon creation (also used for weight)
          float size; // rendering size
          float time; // how long it has lived
          float lifetime; // how long it can live
          int grounded; // if a particle is grounded it shouldn't move or rotate anymore, lying on the floor
          */
          (*pp).pos.x = 0;
          (*pp).pos.y = 0;
          (*pp).pos.z = 0;
          (*pp).creation_pos = (*pp).pos;
          (*pp).speed.x = 0;//((float)(rand()%1000)/1000.0)*0.01-0.005;
          (*pp).speed.y = 0;//((float)(rand()%1000)/1000.0)*0.01-0.005;
          (*pp).speed.z = 0;//((float)(rand()%1000)/1000.0)*0.01-0.005;
          (*pp).color = vsx_color<>(0,0,0,0);
          (*pp).color_end = vsx_color<>(0,0,0,0);
          (*pp).rotation.x = 0.0f;
          (*pp).rotation.y = 0.0f;
          (*pp).rotation.z = 0.0f;
          (*pp).rotation.w = 1.0f;
          (*pp).rotation_dir.x = 0.0f;
          (*pp).rotation_dir.y = 0.0f;
          (*pp).rotation_dir.z = 0.0f;
          (*pp).rotation_dir.w = 1.0f;
          (*pp).orig_size = 0.01f;
          (*pp).size = 0.01f;

          if (initial_lifetime_random->get())
          {
            (*pp).lifetime = lifetime_base + (*(f_randpool_pointer++))*lifetime_random_weight-lifetime_random_weight*0.5f;
            (*pp).one_div_lifetime = 1.0f / (*pp).lifetime;
            (*pp).time = (*(f_randpool_pointer++)) * (*pp).lifetime;
          } else
          {
            (*pp).lifetime = 0.0001;
            (*pp).one_div_lifetime = 1.0f / (*pp).lifetime;
            (*pp).time = 0.0;
          }
          (*pp).grounded = 0;
          pp++;
        }
        first = false;
        dtime = 0.0f;
        f_randpool_pointer = f_randpool.get_pointer();
      }

      spd_x = speed_x->get();
      spd_y = speed_y->get();
      spd_z = speed_z->get();

      float half_spd_x = spd_x * 0.5f;
      float half_spd_y = spd_y * 0.5f;
      float half_spd_z = spd_z * 0.5f;

      // get positions from the user
      // get colors from the user
      rr = color->get(0);
      gg = color->get(1);
      bb = color->get(2);
      aa = color->get(3);
      // get number of active particles from the user
      nump = (long)particles_count->get();
      // some out of bounds-checks
      if (nump < 0) nump = 0;
      //if (nump < (long)(*particles.particles).size()) (*particles.particles).reset_used(nump);
      (*particles.particles).allocate((int)particles_count->get());
      //if (nump > 2000) nump = 2000;
      // update the particle system with the new count so the renderer (and modifiers) can read it
      //particles.num_particles = ceil(nump);
      //long p_to_go;
      if (particles_per_second->get() < 0)
        particles_to_go = 1000000000.0f;
      else
        {
          particles_to_go += particles_per_second->get()*dtime;
          if (particles_to_go > particles_per_second->get())
            particles_to_go = particles_per_second->get();
        }

      // set the rand pool pointer to something nice and random
      f_randpool_pointer = f_randpool.get_pointer() + rand.rand()%(nump+1);

      float avx = add_vector->get(0);
      float avy = add_vector->get(1);
      float avz = add_vector->get(2);
      unsigned long num_vertices = (*our_mesh)->data->vertices.size();
      if (num_vertices) {
        vsx_vector3<>* vertex_pool = (*our_mesh)->data->vertices.get_pointer();
        vsx_vector3<>* vertex_cur = &vertex_pool[meshcoord];
          //printf("something to do\n");
        // go through all particles
        float speed_multv = speed_multiplier->get();
        float speed_rvalue = speed_random_value->get();
        float rda = random_deviation->get(0);
        float rdb = random_deviation->get(1);
        float rdc = random_deviation->get(2);
        vsx_particle<>* pp =(*particles.particles).get_pointer();
        for (i = 0; i < (size_t)nump; ++i)
        {
          // add the delta-time to the time of the particle
          (*pp).time+=dtime;
          // is the time got over the maximum lifetime of the particle, re-initialize it
          if ((*pp).time > (*pp).lifetime)
          {
            if (particles_to_go >= 1.0f)
            {
              (*pp).size = size_base;
              (*pp).orig_size = size_base;//+((float)(rand()%1000)/1000.0f)*size_random_weight-size_random_weight*0.5;
              if ((*pp).size == 0.0f) {
                (*pp).size = (*pp).orig_size = size_base+0.0001f;
              }
              (*pp).color.r = rr;
              (*pp).color.g = gg;
              (*pp).color.b = bb;
              (*pp).color.a = aa;
              (*pp).color_end.r = rr;
              (*pp).color_end.g = gg;
              (*pp).color_end.b = bb;
              (*pp).color_end.a = aa;
              //= vsx_color__(rr,gg,bb,aa);//vsx_color__(our_mesh.data->vertex_colors[meshcoord].r,our_mesh.data->vertex_colors[meshcoord].g,our_mesh.data->vertex_colors[meshcoord].b,our_mesh.data->vertex_colors[meshcoord].a);
              //(*pp).color_end = vsx_color__(rr,gg,bb,aa);

              float speed_mult =  speed_multv + ((*(f_randpool_pointer++)) - 0.5f) * speed_multv * speed_rvalue;

              switch (speed_type->get()) {
                case 0: // normal - random direction..
                  (*pp).speed.x = speed_mult*spd_x*(*(f_randpool_pointer++))-half_spd_x+avx;
                  (*pp).speed.y = speed_mult*spd_y*(*(f_randpool_pointer++))-half_spd_y+avy;
                  (*pp).speed.z = speed_mult*spd_z*(*(f_randpool_pointer++))-half_spd_z+avz;
                break;
                case 1:  // fixed vector direction

                  (*pp).speed.x = speed_mult*spd_x;
                  (*pp).speed.y = speed_mult*spd_y;
                  (*pp).speed.z = speed_mult*spd_z;
                break;
                case 2:  // fixed vector direction
                  vsx_vector3<> dir = (*our_mesh)->data->vertices[meshcoord];
                  dir.normalize();
                  (*pp).speed = dir * speed_mult;
                  (*pp).speed.x += avx;
                  (*pp).speed.y += avy;
                  (*pp).speed.z += avz;
                  /*(*particles.particles)[i].speed.x = spd_x;
                  (*particles.particles)[i].speed.y = spd_y;
                  (*particles.particles)[i].speed.z = spd_z;*/
                break;
              } // switch


              //(*particles.particles)[i].speed.x = spd_x*((float)(rand()%1000)/1000.0f)-spd_x*0.5;
              //(*particles.particles)[i].speed.y = spd_y*((float)(rand()%1000)/1000.0f)-spd_y*0.5;
              //(*particles.particles)[i].speed.z = spd_z*((float)(rand()%1000)/1000.0f)-spd_z*0.5;
              //printf("aaa%d",meshcoord);
              (*pp).pos.x = center_[0]+(*vertex_cur).x*spread_[0]+rda*((*(f_randpool_pointer++))-0.5f);
              (*pp).pos.y = center_[1]+(*vertex_cur).y*spread_[1]+rdb*((*(f_randpool_pointer++))-0.5f);
              (*pp).pos.z = center_[2]+(*vertex_cur).z*spread_[2]+rdc*((*(f_randpool_pointer++))-0.5f);
              (*pp).creation_pos = (*pp).pos;
              (*pp).time = 0.0f;
              (*pp).lifetime = lifetime_base+(*(f_randpool_pointer++))*lifetime_random_weight-half_lifetime_random_weight;
              (*pp).one_div_lifetime = 1.0f / (*pp).lifetime;
              if (pick_type->get() == 0) {
                ++meshcoord;
              } else {
                meshcoord = (*(f_randpool_pointer++))*(num_vertices-1)+1;
              }//vertex_cur
              if (meshcoord >= num_vertices-1) meshcoord = 0;
              vertex_cur = &vertex_pool[meshcoord];
              particles_to_go -= 1.0f;
            }
          }
          // add the speed component to the particles
          (*pp).pos.x += (*pp).speed.x*dtime;
          (*pp).pos.y += (*pp).speed.y*dtime;
          (*pp).pos.z += (*pp).speed.z*dtime;

          pp++;

        }
        //(*particles.particles)[particles.particles->size()-1].color.a = nump-floor(nump);
      }

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
    } else
    result_particlesystem->valid = false;
  }
};
