class module_particlesystem_modifier_floor : public vsx_module
{
public:

  // in
  vsx_module_param_particlesystem* in_particlesystem;
  vsx_module_param_int* x_floor;
  vsx_module_param_int* y_floor;
  vsx_module_param_int* z_floor;
  vsx_module_param_int* x_bounce;
  vsx_module_param_int* y_bounce;
  vsx_module_param_int* z_bounce;
  vsx_module_param_float* x_loss;
  vsx_module_param_float* y_loss;
  vsx_module_param_float* z_loss;
  vsx_module_param_int* refraction;
  vsx_module_param_float3* refraction_amount;
  vsx_module_param_float3* floor;

  // out
  vsx_module_param_particlesystem* result_particlesystem;

  // internal
  int i;
  vsx_particlesystem<>* particles;
  vsx_ma_vector<float> f_randpool;
  float* f_randpool_pointer;


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "particlesystems;modifiers;floor";

    info->description =
      "Stops particles when they reach either of the walls";

    info->out_param_spec =
      "particlesystem:particlesystem";

    info->in_param_spec =
      "in_particlesystem:particlesystem,"
      "axis:complex"
      "{"
        "x:complex"
        "{"
          "x_floor:enum?no|yes,"
          "x_bounce:enum?no|yes,"
          "x_loss:float"
        "},"
        "y:complex"
        "{"
          "y_floor:enum?no|yes,"
          "y_bounce:enum?no|yes,"
          "y_loss:float"
        "},"
        "z:complex"
        "{"
          "z_floor:enum?no|yes,"
          "z_bounce:enum?no|yes,"
          "z_loss:float"
        "},"
        "refraction:enum?no|yes,"
        "refraction_amount:float3"
      "},"
      "floor:float3"
    ;

    info->component_class =
      "particlesystem";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");

    x_floor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"x_floor");
    x_floor->set(0);
    y_floor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"y_floor");
    y_floor->set(0);
    z_floor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"z_floor");
    z_floor->set(0);

    x_bounce = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"x_bounce");
    x_bounce->set(0);
    y_bounce = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"y_bounce");
    y_bounce->set(0);
    z_bounce = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"z_bounce");
    z_bounce->set(0);

    x_loss = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_loss");
    x_loss->set(5.0f);
    y_loss = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_loss");
    y_loss->set(5.0f);
    z_loss = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"z_loss");
    z_loss->set(5.0f);

    refraction = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"refraction");
    refraction->set(0);
    refraction_amount = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"refraction_amount");
    refraction_amount->set(0.0f,0);
    refraction_amount->set(0.0f,1);
    refraction_amount->set(0.0f,2);

    floor = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"floor");
    floor->set(0.0f,0);
    floor->set(0.0f,1);
    floor->set(0.0f,2);

    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");
  }

  void run()
  {
    particles = in_particlesystem->get_addr();

    if (particles)
    {

      float fx = floor->get(0);
      float fy = floor->get(1);
      float fz = floor->get(2);
      bool xf = x_floor->get();
      bool yf = y_floor->get();
      bool zf = z_floor->get();
      bool xb = x_bounce->get();
      bool yb = y_bounce->get();
      bool zb = z_bounce->get();
      float xl = 1.0f-x_loss->get()*0.01f;
      float yl = 1.0f-y_loss->get()*0.01f;
      float zl = 1.0f-z_loss->get()*0.01f;

      unsigned long nump = particles->particles->size();
      if (nump != f_randpool.size() * 10)
      {
        for (unsigned long i = f_randpool.size() * 10; i < nump * 10; i++)
        {
          f_randpool[i] = ((float)(rand()%1000000)*0.000001f);
        }
      }
      f_randpool_pointer = f_randpool.get_pointer() + rand()%nump;

      vsx_particle<>* pp = particles->particles->get_pointer();
      for (unsigned long i = 0; i < nump; ++i) {
        if (xf) {
          if ((*pp).pos.x < fx) {
            (*pp).pos.x = fx;
            if (xb) {
              (*pp).speed.x = -(*particles->particles)[i].speed.x*xl*(*(f_randpool_pointer++));
              if (refraction->get()) {
                (*pp).speed.y += refraction_amount->get(1)*((  (*(f_randpool_pointer++)) - 0.5f));
                (*pp).speed.z += refraction_amount->get(2)*((  (*(f_randpool_pointer++)) - 0.5f));
              }
            } else {
              (*pp).speed.x = 0.0f;
            }
          }
        }
        if (yf) {
          if ((*pp).pos.y < fy)
          {
            (*pp).pos.y = fy;
            if (yb)
            {
              if ( fabs((*pp).speed.y) > 0.00001f )
              {
                (*pp).speed.y = -( (*pp).speed.y*yl)*(*(f_randpool_pointer++));
                if (refraction->get())
                {
                  (*pp).speed.x += refraction_amount->get(0)*(( (*(f_randpool_pointer++))-0.5f));
                  (*pp).speed.x *= (*pp).speed.y*0.1f;
                  (*pp).speed.z += refraction_amount->get(2)*(( (*(f_randpool_pointer++))-0.5f));
                  (*pp).speed.z *= (*pp).speed.y*0.1f;
                }
              }
            } else {
              (*pp).speed.y = 0.0f;
            }
          }
        }
        if (zf) {
          if ( (*pp).pos.z < fz) {
            (*pp).pos.z = fz;
            if (zb) {
              (*pp).speed.z = -(*pp).speed.z*zl*(*(f_randpool_pointer++));
              if (refraction->get()) {
                (*pp).speed.x += refraction_amount->get(0)*(( (*(f_randpool_pointer++))-0.5f));
                (*pp).speed.y += refraction_amount->get(1)*(( (*(f_randpool_pointer++))-0.5f));
              }
            } else {
              (*pp).speed.z = 0.0f;
            }
          }
        }
        ++pp;
      }
      result_particlesystem->set_p(*particles);
      return;
    }
    result_particlesystem->valid = false;
  }
};
