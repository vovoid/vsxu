#include <math/vsx_sequence.h>

class module_particlesystem_render : public vsx_module
{
  // in
  vsx_module_param_particlesystem* particles_in;
  vsx_module_param_texture* tex_inf;
  vsx_module_param_int* render_type;
  vsx_module_param_int* size_lifespan_type;
  vsx_module_param_int* color_lifespan_type;
  vsx_module_param_float_sequence* size_lifespan_sequence;
  vsx_module_param_float_sequence* alpha_lifespan_sequence;
  vsx_module_param_float_sequence* r_lifespan_sequence;
  vsx_module_param_float_sequence* g_lifespan_sequence;
  vsx_module_param_float_sequence* b_lifespan_sequence;
  vsx_module_param_string* i_vertex_program;
  vsx_module_param_string* i_fragment_program;
  vsx_module_param_int* ignore_particles_at_center;

  // out
  vsx_module_param_render* render_result;

  // internal
  vsx_particlesystem<>* particles;
  vsx_texture<>** tex;
  vsx::sequence::channel<vsx::sequence::value_float> seq_size;
  vsx::sequence::channel<vsx::sequence::value_float> seq_alpha;
  vsx::sequence::channel<vsx::sequence::value_float> seq_r;
  vsx::sequence::channel<vsx::sequence::value_float> seq_g;
  vsx::sequence::channel<vsx::sequence::value_float> seq_b;
  float sizes[8192];
  float alphas[8192];
  float rs[8192];
  float gs[8192];
  float bs[8192];

  vsx_float_array shader_sizes;
  vsx_ma_vector<float> shader_sizes_data;

  vsx_vector_array<> shader_colors;
  vsx_ma_vector< vsx_vector3<> > shader_colors_data;

  vsx_float_array shader_alphas;
  vsx_ma_vector<float> shader_alphas_data;

  vsx_glsl shader;

  vsx_gl_state* gl_state;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "renderers;particlesystems;simple";

    info->in_param_spec = ""
      "particlesystem:particlesystem,"
      "texture:texture,options:complex"
      "{"
        "render_type:enum?quads|point_sprites,"
        "size_lifespan_type:enum?normal|sequence,"
        "size_lifespan_sequence:sequence,"
        "alpha_lifespan_sequence:sequence,"
        "color_lifespan_type:enum?normal|sequence,"
        "r_lifespan_sequence:sequence,"
        "g_lifespan_sequence:sequence,"
        "b_lifespan_sequence:sequence,"
        "ignore_particles_at_center:enum?no|yes"
      "},"
      "shader_params:complex"
      "{"
        "vertex_program:string,"
        "fragment_program:string"
        +shader.get_param_spec()+
      "}"
    ;

    info->out_param_spec =
      "render_out:render";

    info->component_class =
      "render";
  }


  void redeclare_in_params(vsx_module_param_list& in_parameters)
  {
    loading_done = true;
    particles_in = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem",true,true);
    tex_inf = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture",true,true);

    render_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "render_type");
    size_lifespan_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "size_lifespan_type");
    color_lifespan_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "color_lifespan_type");

    ignore_particles_at_center = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "ignore_particles_at_center");

    size_lifespan_sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE,"size_lifespan_sequence");
    size_lifespan_sequence->set(seq_size);
    calc_sizes();

    alpha_lifespan_sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE,"alpha_lifespan_sequence");
    alpha_lifespan_sequence->set(seq_alpha);
    calc_alphas();

    r_lifespan_sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE,"r_lifespan_sequence");
    r_lifespan_sequence->set(seq_r);
    g_lifespan_sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE,"g_lifespan_sequence");
    g_lifespan_sequence->set(seq_g);
    b_lifespan_sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE,"b_lifespan_sequence");
    b_lifespan_sequence->set(seq_b);
    r_lifespan_sequence->updates = 1;
    g_lifespan_sequence->updates = 1;
    b_lifespan_sequence->updates = 1;
    calc_colors();

    i_fragment_program = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"fragment_program");
    i_fragment_program->set(shader.fragment_program);
    i_vertex_program = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"vertex_program");
    i_vertex_program->set(shader.vertex_program.c_str());
    shader.declare_params(in_parameters);
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
      //printf("SETTING DEFAULT PROGRAMS\n");
      shader.vertex_program = ""
          "attribute float _s;\n"
          "attribute float _a;\n"
          "attribute vec3  _c;\n"
          "uniform float _vx;\n"
          "varying float particle_alpha;\n"
          "varying vec3 particle_color;\n"
          "\n"
          "void main(void)\n"
          "{\n"
          "  particle_alpha = _a;\n"
          "  particle_color = _c;\n"
          "  gl_Position = ftransform();\n"
          "  float vertDist = distance(vec3(gl_Position.x,gl_Position.y,gl_Position.z), vec3(0.0,0.0,0.0));\n"
          "  float dist_alpha;\n"
          "  dist_alpha = pow(1 / vertDist,1.1);\n"
          "  gl_PointSize = _vx * 0.155 * dist_alpha * _s;\n"
          "  if (gl_PointSize < 1.0) particle_alpha = gl_PointSize;\n"
          "}";
      shader.fragment_program =
          "uniform sampler2D _tex;\n"
          "varying float particle_alpha;\n"
          "varying vec3 particle_color;\n"
          "void main(void)\n"
          "{\n"
            "vec2 l_uv=gl_PointCoord;\n"
            "const vec2 l_offset = vec2(0.5,0.5);\n"
            "l_uv-=l_offset;\n"
            "l_uv=vec2(vec4(l_uv,0.0,1.0));\n"
            "l_uv+=l_offset;\n"
            "vec4 a = texture2D(_tex, l_uv);\n"
            "gl_FragColor = vec4(a.r * particle_color.r, a.g * particle_color.g, a.b * particle_color.b, a.a * particle_alpha);\n"
          "}\n"
          ;

    loading_done = true;
    redeclare_in_params(in_parameters);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);

    gl_state = vsx_gl_state::get_instance();
  }


  void param_set_notify(const vsx_string<>& name) {
    if ((name == "vertex_program" || name == "fragment_program")) {
      //printf("PARAM_SET_NOTIFY %s\n\n",name.c_str());
      shader.vertex_program = i_vertex_program->get();
      shader.fragment_program = i_fragment_program->get();
      user_message = shader.link();//"module||"+shader.get_log();
      if (user_message.size() == 0) {
        redeclare_in = true;
        user_message = "module||ok";
      }
    }
  }


  void calc_sizes() {
    if (size_lifespan_sequence->updates)
    {
      seq_size = size_lifespan_sequence->get();
      size_lifespan_sequence->updates = 0;
      seq_size.reset();
      for (int i = 0; i < 8192; ++i) {
        sizes[i] = seq_size.execute(1.0f/8192.0f).get_float();
      }
    }
  }

  void calc_alphas() {
    if (alpha_lifespan_sequence->updates)
    {
      seq_alpha = alpha_lifespan_sequence->get();
      alpha_lifespan_sequence->updates = 0;
      seq_alpha.reset();
      for (int i = 0; i < 8192; ++i) {
        alphas[i] = seq_alpha.execute(1.0f/8192.0f).get_float();
      }
    }
  }

  void calc_colors() {
    if (r_lifespan_sequence->updates || g_lifespan_sequence->updates || b_lifespan_sequence->updates)
    {
      seq_r = r_lifespan_sequence->get();
      seq_g = g_lifespan_sequence->get();
      seq_b = b_lifespan_sequence->get();
      r_lifespan_sequence->updates = g_lifespan_sequence->updates = b_lifespan_sequence->updates = 0;
      seq_r.reset();
      seq_g.reset();
      seq_b.reset();
      for (int i = 0; i < 8192; ++i) {
        rs[i] = seq_r.execute(1.0f/8192.0f).get_float();
        gs[i] = seq_g.execute(1.0f/8192.0f).get_float();
        bs[i] = seq_b.execute(1.0f/8192.0f).get_float();
      }
    }
  }

  inline void set_color(GLfloat &r, GLfloat &g, GLfloat &b, GLfloat a, GLfloat &tt)
  {
    if (color_lifespan_type->get())
    {
      //int index = (int)round(8192.0f*tt);
      int index = (int)(8191.0f*tt);
      glColor4f(
          rs[index],
          gs[index],
          bs[index],
          a);
    }
    else
    {
      glColor4f(r,g,b,a);
    }
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    particles = particles_in->get_addr();
    tex = tex_inf->get_addr();
    if (!particles)
      return;

    if (!tex)
      return;


    glMatrixMode(GL_TEXTURE);
    glPushMatrix();

    if ((*tex)->get_transform())
      (*tex)->get_transform()->transform();

    (*tex)->bind();
    glColor4f(1,1,1,1);

    if (render_type->get() == 1)
    {
      if ( !shader.get_linked() )
      {
        vsx_string<>h = shader.link();
        #ifdef VSXU_DEBUG
          printf("vert = %s\n\n\n\n",shader.vertex_program.c_str());
          printf("frag = %s\n",shader.fragment_program.c_str());
          printf("link result:\n%s\n",h.c_str());
        #endif

      }
      glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
      #if defined(__linux__) || defined (WIN32)
        shader.begin();
        //printf("uniforms setting done\n");
      #else
        //shader.set_uniforms();
        //printf("uniforms setting done\n");
        //shader.begin();
      #endif


      //printf("pointsprite\n");
      // This is how will our point sprite's size will be modified by
          // distance from the viewer
        /*
          float quadratic[] =  { 1.0f, 0.0f, 0.01f };
          glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

          // Query for the max point size supported by the hardware
          float maxSize = 0.0f;
          glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );

          // Clamp size to 100.0f or the sprites could get a little too big on some
          // of the newer graphic cards. My ATI card at home supports a max point
          // size of 1024.0f!
          if( maxSize > 100.0f )
              maxSize = 100.0f;

          glPointSize( maxSize );

          // The alpha of a point is calculated to allow the fading of points
          // instead of shrinking them past a defined threshold size. The threshold
          // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to
          // the minimum and maximum point sizes.
          glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 6.0f );

          glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
          glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );

          // Specify point sprite texture coordinate replacement mode for each
          // texture unit
          glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
          */
      glEnable( GL_POINT_SPRITE_ARB );
      //glPointSize( 100.0f );
      glEnable(GL_POINT_SMOOTH);
      if (size_lifespan_type->get() == 0) {
        glBegin( GL_POINTS );
        for (size_t i = 0; i < particles->particles->size(); ++i) {
        if ((*particles->particles)[i].size > 0.0f) {
            float tt = ((*particles->particles)[i].time/(*particles->particles)[i].lifetime);
            float a = 1 - tt;
            if (a < 0.0f) a = 0.0f;
            //glPointSize( (*particles->particles)[i].size*a * 100.0f);
            set_color(
              (*particles->particles)[i].color.r,
              (*particles->particles)[i].color.g,
              (*particles->particles)[i].color.b,
              (*particles->particles)[i].color.a*a,
              tt
            );
            glVertex3f(
              (*particles->particles)[i].pos.x,
              (*particles->particles)[i].pos.y,
              (*particles->particles)[i].pos.z
            );
          }
        }
        glEnd();
      } else {
        calc_sizes();
        shader_sizes_data.allocate(particles->particles->size());
        float* shader_sizes_dp = shader_sizes_data.get_pointer();

        calc_colors();
        shader_colors_data.allocate(particles->particles->size());
        vsx_vector3<>* shader_colors_dp = shader_colors_data.get_pointer();

        calc_alphas();
        shader_alphas_data.allocate(particles->particles->size());
        float* shader_alphas_dp = shader_alphas_data.get_pointer();

        vsx_particle<>* particle_p = (*particles->particles).get_pointer();

        if (color_lifespan_type->get()) // color lifespan sequence
        {
          for (size_t i = 0; i < (*particles->particles).size(); ++i)
          {
            vsx_particle<>* pp = &particle_p[i];
            if (pp->lifetime != 0.0f) {
              float tt = pp->time/pp->lifetime;
              if (tt < 0.0f) tt = 0.0f;
              if (tt > 1.0f) tt = 1.0f;

              //int index8192 = (int)round(8192.0f*tt);
              //printf("tt: %f %f\n",tt,pp->lifetime);
              int index8192 = (int)(8191.0f*tt);

              float ss = sizes[index8192];
              shader_sizes_dp[i] = pp->size * ss;
              shader_alphas_dp[i] = pp->color.a * alphas[index8192];

              shader_colors_dp[i].x = rs[index8192];
              shader_colors_dp[i].y = gs[index8192];
              shader_colors_dp[i].z = bs[index8192];
            }
          }
        } else
        { // normal
          for (size_t i = 0; i < (*particles->particles).size(); ++i)
          {
            vsx_particle<>* pp = &particle_p[i];
            if (pp->lifetime != 0.0f) {
              float tt = pp->time/pp->lifetime;
              if (tt < 0.0f) tt = 0.0f;
              if (tt > 1.0f) tt = 1.0f;

              //int index8192 = (int)round(8192.0f*tt);
              int index8192 = (int)(8191.0f*tt);
              shader_sizes_dp[i] = pp->size*(float)sizes[index8192];
              shader_alphas_dp[i] = pp->color.a*(float)alphas[index8192];

              shader_colors_dp[i].x = pp->color.r;
              shader_colors_dp[i].y = pp->color.g;
              shader_colors_dp[i].z = pp->color.b;
            }
          }
        }
        shader_sizes.data = &shader_sizes_data;
        shader_colors.data = &shader_colors_data;
        shader_alphas.data = &shader_alphas_data;


        if (shader.uniform_map.find("_vx") != shader.uniform_map.end())
        {
          //printf("found vx\n");
          vsx_module_param_float* p = (vsx_module_param_float*)shader.uniform_map["_vx"]->module_param;
          if (p)
            p->set( gl_state->viewport_get_width() );
        }

        if (shader.attribute_map.find("_s") != shader.attribute_map.end())
        {
          //printf("found _s\n");
          vsx_module_param_float_array* p = (vsx_module_param_float_array*)shader.attribute_map["_s"]->module_param;
          //printf("found size1\n");
          if (p) p->set_p(shader_sizes);
        }

        if (shader.attribute_map.find("_c") != shader.attribute_map.end())
        {
          //printf("found _c\n");
          vsx_module_param_float3_array* p = (vsx_module_param_float3_array*)shader.attribute_map["_c"]->module_param;
          if (p) p->set_p(shader_colors);
        }

        if (shader.attribute_map.find("_a") != shader.attribute_map.end())
        {
          //printf("found _a\n");
          vsx_module_param_float_array* p = (vsx_module_param_float_array*)shader.attribute_map["_a"]->module_param;
          if (p) p->set_p(shader_alphas);
        }


        shader.set_uniforms();

        glColor4f(1.0,1.0,1.0,1.0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(vsx_particle<>), (*particles->particles).get_pointer());
        glDrawArrays(GL_POINTS,0,(*particles->particles).size()-1);
        glDisableClientState(GL_VERTEX_ARRAY);
      }
      glDisable(GL_POINT_SMOOTH);
      glDisable( GL_POINT_SPRITE_ARB );
      shader.end();
      glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
    } else
    {
      beginBlobs(gl_state);
      glBegin(GL_QUADS);
      if (size_lifespan_type->get() == 0) {
        for (unsigned long i = 0; i < particles->particles->size(); ++i) {
          if ((*particles->particles)[i].size > 0.0f) {
            bool run = true;
            if (ignore_particles_at_center->get()) {
              if (
                fabs((*particles->particles)[i].pos.x) < 0.001f &&
                fabs((*particles->particles)[i].pos.y) < 0.001f &&
                fabs((*particles->particles)[i].pos.z) < 0.001f
              ) {
                run = false;
              }
            }
            if (run) {
              float tt = ((*particles->particles)[i].time/(*particles->particles)[i].lifetime);
              float a = 1 - tt;
              if (a < 0.0f) a = 0.0f;
              set_color(
                (*particles->particles)[i].color.r,
                (*particles->particles)[i].color.g,
                (*particles->particles)[i].color.b,
                (*particles->particles)[i].color.a,
                tt
              );
              drawBlob(
                (*particles->particles)[i].pos.x,
                (*particles->particles)[i].pos.y,
                (*particles->particles)[i].pos.z,
                (*particles->particles)[i].size*a
              );
            }
          }
        }
      } else {
        calc_sizes();
        calc_alphas();
        calc_colors();
        for (unsigned long i = 0; i < particles->particles->size(); ++i) {
          if ((*particles->particles)[i].size > 0.0f) {
            bool run = true;
            if (ignore_particles_at_center->get()) {
              if (
                fabs((*particles->particles)[i].pos.x) < 0.001f &&
                fabs((*particles->particles)[i].pos.y) < 0.001f &&
                fabs((*particles->particles)[i].pos.z) < 0.001f
              ) {
                run = false;
              }
            }
            if (run) {
              float tt = ((*particles->particles)[i].time/(*particles->particles)[i].lifetime);
              if (tt < 0.0f) tt = 0.0f;
              if (tt > 1.0f) tt = 1.0f;
              set_color(
                (*particles->particles)[i].color.r,
                (*particles->particles)[i].color.g,
                (*particles->particles)[i].color.b,
                //(*particles->particles)[i].color.a*(float)alphas[(int)round(8192.0f*tt)],
                (*particles->particles)[i].color.a*(float)alphas[(int)(8191.0f*tt)],
                tt
              );
              drawBlob(
                (*particles->particles)[i].pos.x,
                (*particles->particles)[i].pos.y,
                (*particles->particles)[i].pos.z,
                //(*particles->particles)[i].size*(float)sizes[(int)round(8191.0f*tt)]
                (*particles->particles)[i].size*(float)sizes[(int)(8191.0f*tt)]
              );
            }
          }
        }
      }
      glEnd();
    }
    (*tex)->_bind();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    render_result->set(1);

  }
};
