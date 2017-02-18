#include <math/vsx_rand.h>

class module_particlesystem_modifier_size_noise : public vsx_module
{
  int i;
  vsx_particlesystem<>* particles;
  vsx_module_param_particlesystem* in_particlesystem;
  vsx_module_param_float* strength;
  vsx_module_param_int* size_type;
  // out
  vsx_module_param_particlesystem* result_particlesystem;
  vsx_rand rand;
  vsx_ma_vector<float> f_randpool;
  float* f_randpool_pointer;
public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "particlesystems;modifiers;size_noise";

    info->description =
      "Makes particles randomly differ in size.";

    info->out_param_spec =
      "particlesystem:particlesystem";

    info->in_param_spec =
      "in_particlesystem:particlesystem,"
      "strength:float?min=0,"
      "size_type:enum?multiply|add"
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
    strength = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"strength");
    size_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size_type");
    // set the position of the particle spray emitter to the center
    strength->set(1);
    // default is 100 particles, should be enough for most effects (tm)
  }

  void run() {
    particles = in_particlesystem->get_addr();
    if (particles) {
      float sx = strength->get(0);

      unsigned long nump = particles->particles->size();
      if (nump != f_randpool.size()<<1)
      {
        for (unsigned long i = f_randpool.size()<<1; i < nump<<1; i++)
        {
          f_randpool[i] = rand.frand();
        }
      }
      f_randpool_pointer = f_randpool.get_pointer() + rand.rand()%nump;
      if (size_type->get()) {
        vsx_particle<>* pp = particles->particles->get_pointer();

        for (unsigned long i = 0; i <  nump; ++i) {
          (*pp).size = (*pp).orig_size + ( (*(f_randpool_pointer++)) *sx);
        }
      } else {
        vsx_particle<>* pp = particles->particles->get_pointer();
        for (unsigned long i = 0; i <  nump; ++i) {
          (*pp).size = (*pp).orig_size * ( (*(f_randpool_pointer++)) *sx);
          pp++;
        }
      }
      result_particlesystem->set_p(*particles);
      return;
    }
    result_particlesystem->valid = false;
  }
};
