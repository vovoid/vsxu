
#define MAX_PARTICLES 300
#define ARRAY_STEPS 20.0f

class module_particlesystem_render_sparks : public vsx_module
{
public:

  // in
  vsx_module_param_particlesystem* particles_in;
  vsx_module_param_float_array* float_array_in;
  vsx_module_param_float* proximity_level;
  vsx_module_param_float4* color;

  // out
  vsx_module_param_render* render_result;

  // internal
  unsigned long i;
  bool charges_init;

  vsx_particlesystem<>* particles;
  vsx_float_array* data;

  vsx_nw_vector<float> charges;
  vsx_nw_vector<float> delta_charges;
  vsx_nw_vector<int> discharged_particles;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "renderers;particlesystems;sparks";

    info->in_param_spec =
      "particlesystem:particlesystem,"
      "float_array_in:float_array,"
      "proximity_level:float,"
      "color:float4"
    ;

    info->out_param_spec =
      "render_out:render";

    info->component_class =
      "render";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    particles_in = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem",true,true);
    proximity_level = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"proximity_level");
    float_array_in = (vsx_module_param_float_array*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY, "float_array_in");
    color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color");
    color->set(1.0f, 0 );
    color->set(1.0f, 1 );
    color->set(1.0f, 2 );
    color->set(1.0f, 3 );

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
    charges_init = false;
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    particles = particles_in->get_addr();
    if (particles)
    {
      data = float_array_in->get_addr();
      if (!data) {
        render_result->set(0);
        return;
      }

      // warning, this algorithm brings pretty much as n*n calculations
      // many to many relation, go through ALL relations.
      // set hard limit to MAX_PARTICLES particles
      int run_particles = particles->particles->size();
      if (run_particles > MAX_PARTICLES) run_particles = MAX_PARTICLES;

      int array_pos = 0;

      vsx_vector3<> perpendicular_mult;
      vsx_vector3<> upv(0, 1.0f);
      glColor4f(color->get(0), color->get(1), color->get(2), color->get(3) );
      if (!charges_init) {
        for (int i = 0; i < run_particles; ++i) {
          delta_charges[i] = ((float)(rand()%1000) * 0.001f - 0.5f) * 9.0f;
          charges[i] = 0.0f;
        }
        charges_init = true;
      }

      for (int i = 0; i < run_particles; ++i)
      {
        int found = -1;
        float last_length = 1000.0f;
        vsx_vector3<> last_dist;
        charges[i] += delta_charges[i] * engine_state->dtime;
        //if (i == 0) printf("%f\n", charges[i]);
        vsx_vector3<> dist;
        for (int j = 0; j < run_particles; j++)
        {
          if (j != i)
          {
             dist = (*particles->particles)[i].pos - (*particles->particles)[j].pos;

            //printf("%f\n", dist.length() * 0.1f);

            float dle = dist.length();

            if (dle < proximity_level->get() && dle < last_length)
            {
              bool lrun = true;
              for (int dpi = 0; dpi < (int)discharged_particles.size(); dpi++)
              {
                if (discharged_particles[dpi] == i
                    ||
                    discharged_particles[dpi] == j
                    )
                {
                  lrun = false;
                }
              }
              if (lrun)
              {
                last_length = dle;
                found = j;
                last_dist = dist;
              }
            }
          }
        } // for j
        {
          bool lrun = true;
          if (found == -1) lrun = false;
          //printf("charge delta: %f\n", charges[i] + charges[j]);
          if (lrun)
          //if (((*particles->particles)[i].size * (*particles->particles)[found].size) * (charges[i] + charges[found]) < 0.0f) run = false;
          if ((charges[i] + charges[found]) < 0.0f) lrun = false;
          if (lrun)
          {
            charges[i] += charges[found];
            glLineWidth(charges[i]);
            charges[i] = 0;
            charges[i] += -delta_charges[i] * engine_state->dtime * 0.5f;
            charges[found] += -delta_charges[found] * engine_state->dtime * 0.5f;
            //delta_charges[i] = ((float)(rand()%1000) * 0.001f - 0.5f) * 5.0f;
            //delta_charges[j] = ((float)(rand()%1000) * 0.001f - 0.5f) * 5.0f;
            //printf("%f\n", charges[i]);
            discharged_particles.push_back(i);
            //discharged_particles.push_back(found);
            // calculate perpendicular vector by cross product with up vector (0,1,0)
            perpendicular_mult.cross(last_dist, upv);
            perpendicular_mult.normalize();
            //perpendicular_mult *= 0.1f; // might use a parameter here?

            // ok, time to draw
            glBegin(GL_LINE_STRIP);
            //vsx_vector tv = (*particles->particles)[0].pos;
            for (int k = 0; k < ARRAY_STEPS; k++)
            {
              vsx_vector3<> tv = (*particles->particles)[found].pos + last_dist * ((float)k / ARRAY_STEPS);
              tv += perpendicular_mult * (*(data->data))[array_pos] * (float)sin(PI * ((float)k / ARRAY_STEPS)) * 0.1f;
              glVertex3f(tv.x, tv.y, tv.z);
              if (array_pos++ > (int)data->data->size()-2) array_pos = 0;
            }
            glEnd();
          }
        }
      }
      discharged_particles.reset_used();
      render_result->set(1);
    } else
    render_result->set(0);
  }
}; // class
