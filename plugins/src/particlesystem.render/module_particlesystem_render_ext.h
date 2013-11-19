#include "vsx_vbo_bucket.h"
#ifdef VSXU_TM
#include "vsx_tm.h"
#endif

class module_particlesystem_render_ext : public vsx_module
{
public:

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

  vsx_texture *texture_lookup_sizes;

  vsx_texture *texture_lookup_color;

  vsx_glsl shader;

  vsx_vbo_bucket<GLuint, 1, GL_POINTS, GL_STREAM_DRAW, vsx_quaternion> point_bucket;


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
        "vertex_program:string,"
        "fragment_program:string"
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
          "uniform float _vx; \n"
          "varying vec4 particle_color;\n"
          "uniform sampler1D _lookup_colors; \n"
          "uniform sampler1D _lookup_sizes; \n"
          "\n"
          "float rand(vec2 n)\n"
          "{\n"
          "  return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);\n"
          "}\n"
          "\n"
          "void main(void)\n"
          "{\n"
          "  float tt = gl_Vertex.w;\n"
          "  particle_color = texture1D(_lookup_colors, tt).rgba; \n  "
          "  gl_Vertex.w = 1.0;\n"
          "  gl_Position = ftransform();\n"
          "  float vertDist = distance(vec3(gl_Position.x,gl_Position.y,gl_Position.z), vec3(0.0,0.0,0.0));\n"
          "  float dist_alpha;\n"
          "  dist_alpha = pow(1 / vertDist,1.1);\n"
          "  gl_PointSize = _vx * 0.05 * dist_alpha * texture1D( _lookup_sizes, tt ).r;\n"
          "//  if (gl_PointSize < 1.0) particle_color.a = gl_PointSize;\n"
          "}";
      shader.fragment_program =
          "uniform sampler2D _tex;\n"
          "varying vec4 particle_color;\n"
          "void main(void)\n"
          "{\n"
            "vec2 l_uv=gl_PointCoord;\n"
            "const vec2 l_offset = vec2(0.5,0.5);\n"
            "l_uv-=l_offset;\n"
            "l_uv=vec2(vec4(l_uv,0.0,1.0));\n"
            "l_uv+=l_offset;\n"
            "vec4 a = texture2D(_tex, l_uv);\n"
            "gl_FragColor = vec4(a.r * particle_color.r, a.g * particle_color.g, a.b * particle_color.b, a.a * particle_color.a );\n"
          "}\n"
          ;

    // create and prepare the textures
    texture_lookup_sizes = new vsx_texture;
    texture_lookup_sizes->init_opengl_texture_1d();
    texture_lookup_color = new vsx_texture;
    texture_lookup_color->init_opengl_texture_1d();

    loading_done = true;
    redeclare_in_params(in_parameters);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);


  }

  bool init()
  {
  }

  void on_delete()
  {
    delete texture_lookup_sizes;
    delete texture_lookup_color;
  }


  void param_set_notify(const vsx_string& name) {
    if ((name == "vertex_program" || name == "fragment_program")) {
      shader.vertex_program = i_vertex_program->get();
      shader.fragment_program = i_fragment_program->get();
      message = shader.link();
      if (message.size() == 0)
      {
        redeclare_in = true;
        message = "module||ok";
      }
    }
  }


  vsx_array<float> texture_lookup_sizes_data;


  inline void calc_sizes()
  {
    if (!size_lifespan_sequence->updates) return;
    seq_size = size_lifespan_sequence->get();
    size_lifespan_sequence->updates = 0;
    seq_size.reset();
    for (int i = 0; i < 8192; ++i)
    {
      texture_lookup_sizes_data[i] = seq_size.execute(1.0f/8192.0f);
    }
    texture_lookup_sizes->bind();
    glTexParameteri(texture_lookup_sizes->texture_info->ogl_type, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(texture_lookup_sizes->texture_info->ogl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texture_lookup_sizes->texture_info->ogl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage1D(
      texture_lookup_sizes->texture_info->ogl_type,  // opengl type
      0, // mipmap level
      1, // internal format
      8191, // size
      0, // border
      GL_RED, // source data format
      GL_FLOAT, // source data type
      texture_lookup_sizes_data.get_pointer()
    );

    texture_lookup_sizes->_bind();
    texture_lookup_sizes->valid = true;
  }

  vsx_array<vsx_color> texture_lookup_color_data;

  inline void calc_alphas()
  {
    if (!alpha_lifespan_sequence->updates) return;
    seq_alpha = alpha_lifespan_sequence->get();
    alpha_lifespan_sequence->updates = 0;
    seq_alpha.reset();
    for (int i = 0; i < 8192; ++i)
    {
      texture_lookup_color_data[i].a = seq_alpha.execute(1.0f/8192.0f);
//      alphas[i] = seq_alpha.execute(1.0f/8192.0f);
    }
    texture_lookup_color->bind();
    glTexParameteri(texture_lookup_color->texture_info->ogl_type, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(texture_lookup_color->texture_info->ogl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texture_lookup_color->texture_info->ogl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage1D(
      texture_lookup_color->texture_info->ogl_type,  // opengl type
      0, // mipmap level
      4, // internal format
      8191, // size
      0, // border
      GL_RGBA, // source data format
      GL_FLOAT, // source data type
      texture_lookup_color_data.get_pointer()
    );
    texture_lookup_color->_bind();
    texture_lookup_color->valid = true;
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
    for (int i = 0; i < 8192; ++i)
    {
      texture_lookup_color_data[i].r = seq_r.execute(1.0f/8192.0f);
      texture_lookup_color_data[i].g = seq_g.execute(1.0f/8192.0f);
      texture_lookup_color_data[i].b = seq_b.execute(1.0f/8192.0f);
    }
    texture_lookup_color->bind();
    glTexParameteri(texture_lookup_color->texture_info->ogl_type, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(texture_lookup_color->texture_info->ogl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(texture_lookup_color->texture_info->ogl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage1D(
      texture_lookup_color->texture_info->ogl_type,  // opengl type
      0, // mipmap level
      4, // internal format
      8191, // size
      0, // border
      GL_RGBA, // source data format
      GL_FLOAT, // source data type
      texture_lookup_color_data.get_pointer()
    );
    texture_lookup_color->_bind();
    texture_lookup_color->valid = true;
  }


  size_t num_active_particles;

  void run()
  {
    #ifdef VSXU_TM
    ((vsx_tm*)engine->tm)->e( "particle_ext_run" );
    #endif

    num_active_particles = 0;
    particles = particles_in->get_addr();
    if (!particles)
    {
      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->l();
      #endif
      return;
    }

    calc_sizes();

    calc_colors();

    calc_alphas();



    vsx_particle* pp = (*particles->particles).get_pointer();

    if (point_bucket.faces.get_used() != (*particles->particles).size())
    {
      for (size_t i = 0; i < (*particles->particles).size(); ++i)
      {
        point_bucket.faces[i] = i;
      }
    }

    point_bucket.vertices.allocate( (*particles->particles).size() );

    float* pbv = (float*)point_bucket.vertices.get_pointer();

    for (size_t i = 0; i < (*particles->particles).size(); ++i)
    {
      if (pp->time < pp->lifetime)
      {
        float tt = pp->time * pp->one_div_lifetime;

        *pbv = pp->pos.x;
        pbv++;
        *pbv = pp->pos.y;
        pbv++;
        *pbv = pp->pos.z;
        pbv++;
        *pbv = tt;
        pbv++;
        num_active_particles++;
      }
      pp++;
    }
    point_bucket.invalidate_vertices();

    // set sizes into normals
    point_bucket.update();
    #ifdef VSXU_TM
    ((vsx_tm*)engine->tm)->l();
    #endif

  }

  void output(vsx_module_param_abs* param)
  {
#ifdef VSXU_TM
((vsx_tm*)engine->tm)->e( "particle_ext_output" );
#endif

    if (num_active_particles == 0)
    {
      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->l();
      #endif
      return;
    }
    VSX_UNUSED(param);
    particles = particles_in->get_addr();
    tex = tex_inf->get_addr();
    if (!particles || !tex)
    {
      render_result->set(0);
      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->l();
      #endif
      return;
    }
    if ( !((*tex)->valid))
    {
      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->l();
      #endif
      return;
    }


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


    if (shader.uniform_map.find("_vx") != shader.uniform_map.end())
    {
      vsx_module_param_float* p = (vsx_module_param_float*)shader.uniform_map["_vx"]->module_param;
      if (p) p->set( engine->gl_state->get_viewport_width() );
    }

    if (shader.uniform_map.find("_tex") != shader.uniform_map.end())
    {
      vsx_module_param_texture* t = (vsx_module_param_texture*)shader.uniform_map["_tex"]->module_param;
      if (t) t->set_p( *tex );
    }

    if (shader.uniform_map.find("_lookup_colors") != shader.uniform_map.end())
    {
      vsx_module_param_texture* t = (vsx_module_param_texture*)shader.uniform_map["_lookup_colors"]->module_param;
      if (t) t->set_p( texture_lookup_color );
    }

    if (shader.uniform_map.find("_lookup_sizes") != shader.uniform_map.end())
    {
      vsx_module_param_texture* t = (vsx_module_param_texture*)shader.uniform_map["_lookup_sizes"]->module_param;
      if (t) t->set_p( texture_lookup_sizes );
    }


    shader.set_uniforms();

    glEnable( GL_POINT_SPRITE_ARB );
    glEnable(GL_POINT_SMOOTH);

    point_bucket.output(num_active_particles);

    glDisable(GL_POINT_SMOOTH);
    glDisable( GL_POINT_SPRITE_ARB );

    shader.end();

    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

    (*tex)->_bind();
    (*tex)->end_transform();

    render_result->set(1);
    #ifdef VSXU_TM
    ((vsx_tm*)engine->tm)->l();
    #endif

  }
};
