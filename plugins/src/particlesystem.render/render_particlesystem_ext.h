#include "vsx_vbo_bucket.h"

class module_render_particlesystem_ext : public vsx_module {
  // in
  vsx_module_param_particlesystem* particles_in;
  vsx_module_param_texture* tex_inf;
  vsx_module_param_sequence* size_lifespan_sequence;
  vsx_module_param_sequence* alpha_lifespan_sequence;
  vsx_module_param_sequence* r_lifespan_sequence;
  vsx_module_param_sequence* g_lifespan_sequence;
  vsx_module_param_sequence* b_lifespan_sequence;

  vsx_module_param_string* i_vertex_program;
  vsx_module_param_string* i_fragment_program;

  vsx_module_param_int* ignore_particles_at_center;
  // out
  vsx_module_param_render* render_result;
  // internal
  vsx_particlesystem* particles;
  vsx_texture** tex;

  // sequences
  vsx_sequence seq_size;
  vsx_sequence seq_alpha;
  vsx_sequence seq_r;
  vsx_sequence seq_g;
  vsx_sequence seq_b;
  float sizes[8192];
  float alphas[8192];
  float rs[8192];
  float gs[8192];
  float bs[8192];
  GLint viewport[4];

  vsx_float_array shader_sizes;
  vsx_array<float> shader_sizes_data;

  vsx_float3_array shader_colors;
  vsx_array<vsx_vector> shader_colors_data;

  vsx_float_array shader_alphas;
  vsx_array<float> shader_alphas_data;

  vsx_glsl shader;

  vsx_vbo_bucket<GLuint, 1, GL_POINTS> point_bucket;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;particlesystems;render_particlesystem_ext";

    info->in_param_spec =
      "particlesystem:particlesystem,texture:texture,options:complex{"
        "size_lifespan_sequence:sequence,"
        "alpha_lifespan_sequence:sequence,"
        "r_lifespan_sequence:sequence,"
        "g_lifespan_sequence:sequence,"
        "b_lifespan_sequence:sequence,"
        "ignore_particles_at_center:enum?no|yes"
      "},"
      "shader_params:complex"
      "{"
        "vertex_program:string,"
        "fragment_program:string"
        + shader.get_param_spec() +
      "}"
    ;

    info->out_param_spec = "render_out:render";

    info->component_class = "render";
  }


  void redeclare_in_params(vsx_module_param_list& in_parameters)
  {
    loading_done = true;
    particles_in = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem",true,true);
    tex_inf = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture",true,true);

    ignore_particles_at_center = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "ignore_particles_at_center");

    size_lifespan_sequence = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE,"size_lifespan_sequence");
    size_lifespan_sequence->set(seq_size);
    calc_sizes();

    alpha_lifespan_sequence = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE,"alpha_lifespan_sequence");
    alpha_lifespan_sequence->set(seq_alpha);
    calc_alphas();

    r_lifespan_sequence = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE,"r_lifespan_sequence");
    r_lifespan_sequence->set(seq_r);
    g_lifespan_sequence = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE,"g_lifespan_sequence");
    g_lifespan_sequence->set(seq_g);
    b_lifespan_sequence = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE,"b_lifespan_sequence");
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

  }


  void param_set_notify(const vsx_string& name) {
    if ((name == "vertex_program" || name == "fragment_program")) {
      //printf("PARAM_SET_NOTIFY %s\n\n",name.c_str());
      shader.vertex_program = i_vertex_program->get();
      shader.fragment_program = i_fragment_program->get();
      message = shader.link();//"module||"+shader.get_log();
      if (message.size() == 0) {
        redeclare_in = true;
        message = "module||ok";
      }
    }
  }


  inline void calc_sizes()
  {
    if (!size_lifespan_sequence->updates) return;
    seq_size = size_lifespan_sequence->get();
    size_lifespan_sequence->updates = 0;
    seq_size.reset();
    for (int i = 0; i < 8192; ++i) {
      sizes[i] = seq_size.execute(1.0f/8192.0f);
    }
  }

  inline void calc_alphas()
  {
    if (!alpha_lifespan_sequence->updates) return;
    seq_alpha = alpha_lifespan_sequence->get();
    alpha_lifespan_sequence->updates = 0;
    seq_alpha.reset();
    for (int i = 0; i < 8192; ++i) {
      alphas[i] = seq_alpha.execute(1.0f/8192.0f);
    }
  }

  inline void calc_colors()
  {
    if (
        !r_lifespan_sequence->updates &&
        !g_lifespan_sequence->updates &&
        !b_lifespan_sequence->updates
    )
    return;

    seq_r = r_lifespan_sequence->get();
    seq_g = g_lifespan_sequence->get();
    seq_b = b_lifespan_sequence->get();
    r_lifespan_sequence->updates = g_lifespan_sequence->updates = b_lifespan_sequence->updates = 0;
    seq_r.reset();
    seq_g.reset();
    seq_b.reset();
    for (int i = 0; i < 8192; ++i) {
      rs[i] = seq_r.execute(1.0f/8192.0f);
      gs[i] = seq_g.execute(1.0f/8192.0f);
      bs[i] = seq_b.execute(1.0f/8192.0f);
    }
  }

  void run()
  {
    particles = particles_in->get_addr();
    if (!particles)
    {
      return;
    }

    calc_sizes();
    shader_sizes_data.allocate(particles->particles->size());
    float* shader_sizes_dp = shader_sizes_data.get_pointer();

    calc_colors();
    shader_colors_data.allocate(particles->particles->size());
    vsx_vector* shader_colors_dp = shader_colors_data.get_pointer();

    calc_alphas();
    shader_alphas_data.allocate(particles->particles->size());
    float* shader_alphas_dp = shader_alphas_data.get_pointer();



    vsx_particle* pp = (*particles->particles).get_pointer();

    if (point_bucket.faces.get_used() != (*particles->particles).size())
    {
      for (size_t i = 0; i < (*particles->particles).size(); ++i)
      {
        point_bucket.faces[i] = i;
      }
    }

    for (size_t i = 0; i < (*particles->particles).size(); ++i)
    {
      if (pp->lifetime > 0.0f)
      {
        float tt = pp->time/pp->lifetime;
        if (tt < 0.0f) tt = 0.0f;
        if (tt > 1.0f) tt = 1.0f;

        int index8192 = (int)(8191.0f*tt);

        shader_sizes_dp[i] = pp->size * sizes[index8192];
        shader_alphas_dp[i] = pp->color.a * alphas[index8192];

        shader_colors_dp[i].x = rs[index8192];
        shader_colors_dp[i].y = gs[index8192];
        shader_colors_dp[i].z = bs[index8192];

        point_bucket.vertices[i] = pp->pos;
      }
      pp++;
    }
    point_bucket.invalidate_vertices();
    point_bucket.update();

  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    particles = particles_in->get_addr();
    tex = tex_inf->get_addr();
    if (!particles || !tex)
    {
      render_result->set(0);
      return;
    }
    if ( !((*tex)->valid)) return;


    (*tex)->begin_transform();
    (*tex)->bind();

    if ( !shader.get_linked() )
    {
      vsx_string h = shader.link();
      #ifdef VSXU_DEBUG
        printf("vert = %s\n\n\n\n",shader.vertex_program.c_str());
        printf("frag = %s\n",shader.fragment_program.c_str());
        printf("link result:\n%s\n",h.c_str());
      #endif
    }

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    shader.begin();


    shader_sizes.data = &shader_sizes_data;
    shader_colors.data = &shader_colors_data;
    shader_alphas.data = &shader_alphas_data;

    if (shader.uniform_map.find("_vx") != shader.uniform_map.end())
    {
      //printf("found vx\n");
      vsx_module_param_float* p = (vsx_module_param_float*)shader.uniform_map["_vx"]->module_param;
      if (p) p->set( engine->gl_state->get_viewport_width() );
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

    glEnable( GL_POINT_SPRITE_ARB );
    glEnable(GL_POINT_SMOOTH);

    point_bucket.output();

    glDisable(GL_POINT_SMOOTH);
    glDisable( GL_POINT_SPRITE_ARB );

    shader.end();

    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

    (*tex)->_bind();
    (*tex)->end_transform();

    render_result->set(1);
  }
};
