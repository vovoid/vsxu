/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "main.h"
#include "vsx_math_3d.h"
#include "vsx_glsl.h"

GLfloat blobMat[16];

GLfloat blobVec0[4];
GLfloat blobVec1[4];


// this is the method used in starlight aurora vsx, quite handy actually
void beginBlobs() {
	glEnable(GL_TEXTURE_2D);
	GLfloat tmpMat[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, blobMat);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glGetFloatv(GL_PROJECTION_MATRIX, tmpMat);
	tmpMat[3] = 0;
	tmpMat[7] = 0;
	tmpMat[11] = 0;
	tmpMat[12] = 0;
	tmpMat[13] = 0;
	tmpMat[14] = 0;
	tmpMat[15] = 1;

	v_norm(tmpMat);
	v_norm(tmpMat + 4);
	v_norm(tmpMat + 8);

	glLoadIdentity();
	glMultMatrixf(tmpMat);

	blobMat[3] = 0;
	blobMat[7] = 0;
	blobMat[11] = 0;
	blobMat[12] = 0;
	blobMat[13] = 0;
	blobMat[14] = 0;
	blobMat[15] = 1;

	v_norm(blobMat);
	v_norm(blobMat + 4);
	v_norm(blobMat + 8);

	glMultMatrixf(blobMat);
	glGetFloatv(GL_PROJECTION_MATRIX, blobMat);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	//inv_mat(blobMat, blobMatInv);

	GLfloat upLeft[] = {-0.5f, 0.5f, 0.0f, 1.0f};
	GLfloat upRight[] = {0.5f, 0.5f, 0.0f, 1.0f};

	mat_vec_mult(blobMat, upLeft, blobVec0);
	mat_vec_mult(blobMat, upRight, blobVec1);
}

inline void drawBlob(GLfloat x, GLfloat y, GLfloat z, GLfloat size) {
	GLfloat tmpVec0[] = {blobVec0[0] * size, blobVec0[1] * size, blobVec0[2] * size};
	GLfloat tmpVec1[] = {blobVec1[0] * size, blobVec1[1] * size, blobVec1[2] * size};
	//glBegin(GL_QUADS);
		glTexCoord2f(1, 1);
		glVertex3f(x + tmpVec0[0], y + tmpVec0[1],  z + tmpVec0[2]);
		glTexCoord2f(1, 0);
		glVertex3f(x - tmpVec1[0], y - tmpVec1[1],  z - tmpVec1[2]);
		glTexCoord2f(0, 0);
		glVertex3f(x - tmpVec0[0], y - tmpVec0[1],  z - tmpVec0[2]);
		glTexCoord2f(0, 1);
		glVertex3f(x + tmpVec1[0], y + tmpVec1[1],  z + tmpVec1[2]);
	//glEnd();
}

inline void drawBlob_c(GLfloat x, GLfloat y, GLfloat z, GLfloat size, GLfloat dx, GLfloat dy, GLfloat dz) {
	GLfloat tmpVec0[] = {blobVec0[0] * size, blobVec0[1] * size, blobVec0[2] * size};
	GLfloat tmpVec1[] = {blobVec1[0] * size, blobVec1[1] * size, blobVec1[2] * size};
	//glBegin(GL_QUADS);
		glTexCoord2f(1, 1);
		glVertex3f(dx, dy, dz);
		glTexCoord2f(1, 0);
		glVertex3f(x - tmpVec1[0], y - tmpVec1[1],  z - tmpVec1[2]);
		glTexCoord2f(0, 0);
		glVertex3f(x - tmpVec0[0], y - tmpVec0[1],  z - tmpVec0[2]);
		glTexCoord2f(0, 1);
//		glVertex3f(dx, dy, dz);
		glVertex3f(dx + tmpVec1[0], dy+tmpVec1[1], dz+tmpVec1[2]);
	//glEnd();
}


class module_render_particlesystem : public vsx_module {
  // in
  vsx_module_param_particlesystem* particles_in;
	vsx_module_param_texture* tex_inf;
	vsx_module_param_int* render_type;
	vsx_module_param_int* size_lifespan_type;
  vsx_module_param_int* color_lifespan_type;
	//vsx_module_param_int* alpha_type;
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
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;particlesystems;simple";
    info->in_param_spec = ""
    "particlesystem:particlesystem,texture:texture,options:complex{"
      "render_type:enum?quads|point_sprites,"
      "size_lifespan_type:enum?normal|sequence,"
      "size_lifespan_sequence:sequence,"
      "alpha_lifespan_sequence:sequence,"
      "color_lifespan_type:enum?normal|sequence,"
      "r_lifespan_sequence:sequence,"
      "g_lifespan_sequence:sequence,"
      "b_lifespan_sequence:sequence,"
      "ignore_particles_at_center:enum?no|yes,"
      "point_sprite_shader:complex{"
      "}"
    "},shader_params:complex{vertex_program:string,fragment_program:string"
    +shader.get_param_spec()+"}";
    //printf("in_param_spec: %s\n", info->in_param_spec.c_str() );
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
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


#ifdef VSXU_DEBUG
    printf("vert = %s\n\n\n\n",shader.vertex_program.c_str());
    printf("frag = %s\n",shader.fragment_program.c_str());
#endif

    vsx_string h = shader.link();
#ifdef VSXU_DEBUG
    printf("link result:\n%s\n",h.c_str());
#endif

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


  void calc_sizes() {
    if (size_lifespan_sequence->updates)
    {
      seq_size = size_lifespan_sequence->get();
    	size_lifespan_sequence->updates = 0;
      seq_size.reset();
      for (int i = 0; i < 8192; ++i) {
        sizes[i] = seq_size.execute(1.0f/8192.0f);
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
        alphas[i] = seq_alpha.execute(1.0f/8192.0f);
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
        rs[i] = seq_r.execute(1.0f/8192.0f);
        gs[i] = seq_g.execute(1.0f/8192.0f);
        bs[i] = seq_b.execute(1.0f/8192.0f);
      }
    }
  }

  inline void set_color(GLfloat &r, GLfloat &g, GLfloat &b, GLfloat a, GLfloat &tt)
  {
    if (color_lifespan_type->get())
    {
      //int index = (int)round(8192.0f*tt);
      int index = (int)(8192.0f*tt);
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

  void output(vsx_module_param_abs* param) {
    particles = particles_in->get_addr();
    if (particles)
    {
      tex = tex_inf->get_addr();
      if (tex) {
        if (!((*tex)->valid)) {
          return;
        }
      	vsx_transform_obj texture_transform = (*tex)->get_transform();

      	glMatrixMode(GL_TEXTURE);
      	glPushMatrix();

      	if ((*tex)->get_transform()) texture_transform();


        (*tex)->bind();
        glColor4f(1,1,1,1);

        if (render_type->get() == 1)
        {

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
            for (unsigned long i = 0; i < particles->particles->size(); ++i) {
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
            vsx_vector* shader_colors_dp = shader_colors_data.get_pointer();

            calc_alphas();
            shader_alphas_data.allocate(particles->particles->size());
            float* shader_alphas_dp = shader_alphas_data.get_pointer();

            vsx_particle* particle_p = (*particles->particles).get_pointer();
            if (color_lifespan_type->get())
            {
              for (unsigned long i = 0; i < (*particles->particles).size(); ++i)
              {
                vsx_particle* pp = &particle_p[i];
                float tt = pp->time/pp->lifetime;
                if (tt < 0.0f) tt = 0.0f;
                if (tt > 1.0f) tt = 1.0f;

                //int index8192 = (int)round(8192.0f*tt);
                int index8192 = (int)(8192.0f*tt);
                shader_sizes_dp[i] = pp->size * sizes[index8192];
                shader_alphas_dp[i] = pp->color.a * alphas[index8192];

                shader_colors_dp[i].x = rs[index8192];
                shader_colors_dp[i].y = gs[index8192];
                shader_colors_dp[i].z = bs[index8192];
              }
            } else
            {
              for (unsigned long i = 0; i < (*particles->particles).size(); ++i)
              {
                vsx_particle* pp = &particle_p[i];
                float tt = pp->time/pp->lifetime;
                if (tt < 0.0f) tt = 0.0f;
                if (tt > 1.0f) tt = 1.0f;

                //int index8192 = (int)round(8192.0f*tt);
                int index8192 = (int)(8192.0f*tt);
                shader_sizes_dp[i] = pp->size*(float)sizes[index8192];
                shader_alphas_dp[i] = pp->color.a*(float)alphas[index8192];

                shader_colors_dp[i].x = pp->color.r;
                shader_colors_dp[i].y = pp->color.g;
                shader_colors_dp[i].z = pp->color.b;
              }
            }
            shader_sizes.data = &shader_sizes_data;
            shader_colors.data = &shader_colors_data;
            shader_alphas.data = &shader_alphas_data;

            glGetIntegerv (GL_VIEWPORT, viewport);
            float t_res_x = (float)abs(viewport[2] - viewport[0]);


            if (shader.v_map.find("_vx") != shader.v_map.end())
            {
              //printf("found vx\n");
              vsx_module_param_float* p = (vsx_module_param_float*)shader.v_map["_vx"]->module_param;
              if (p) p->set(t_res_x);
            }

            if (shader.a_map.find("_s") != shader.a_map.end())
            {
              //printf("found _s\n");
              vsx_module_param_float_array* p = (vsx_module_param_float_array*)shader.a_map["_s"]->module_param;
              //printf("found size1\n");
              if (p) p->set_p(shader_sizes);
            }

            if (shader.a_map.find("_c") != shader.a_map.end())
            {
              //printf("found _c\n");
              vsx_module_param_float3_array* p = (vsx_module_param_float3_array*)shader.a_map["_c"]->module_param;
              if (p) p->set_p(shader_colors);
            }

            if (shader.a_map.find("_a") != shader.a_map.end())
            {
              //printf("found _a\n");
              vsx_module_param_float_array* p = (vsx_module_param_float_array*)shader.a_map["_a"]->module_param;
              if (p) p->set_p(shader_alphas);
            }


            shader.set_uniforms();

            glColor4f(1.0,1.0,1.0,1.0);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(vsx_particle), (*particles->particles).get_pointer());
            glDrawArrays(GL_POINTS,0,(*particles->particles).size());
            glDisableClientState(GL_VERTEX_ARRAY);


/*            for (unsigned long i = 0; i < particles->particles->size(); ++i) {
              if ((*particles->particles)[i].size > 0.0f) {
                float tt = ((*particles->particles)[i].time/(*particles->particles)[i].lifetime);
                if (tt < 0.0f) tt = 0.0f;
                if (tt > 1.0f) tt = 1.0f;

                //vsx_array<float> shader_sizes_data;

                //glPointSize( (*particles->particles)[i].size*(float)sizes[(int)round(8191.0f*tt)] );
                set_color(
                  (*particles->particles)[i].color.r,
                  (*particles->particles)[i].color.g,
                  (*particles->particles)[i].color.b,
                  (*particles->particles)[i].color.a*(float)alphas[(int)round(8192.0f*tt)],
                  tt
                );
                glVertex3f(
                  (*particles->particles)[i].pos.x,
                  (*particles->particles)[i].pos.y,
                  (*particles->particles)[i].pos.z
                );
              }
            }*/
          }
          glDisable(GL_POINT_SMOOTH);
	        glDisable( GL_POINT_SPRITE_ARB );
	        shader.end();
	        glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
        } else
        {
          beginBlobs();
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
                    (*particles->particles)[i].color.a*(float)alphas[(int)(8192.0f*tt)],
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
    }
    render_result->set(0);
  }
};

class module_render_particlesystem_c : public vsx_module {
  unsigned long i;
  // in
  vsx_module_param_particlesystem* particles_in;
	vsx_module_param_texture* tex_inf;
	vsx_module_param_float3* position;
	vsx_module_param_float* alpha;
	vsx_module_param_float* size;
	// out
	vsx_module_param_render* render_result;
	// internal
	vsx_particlesystem* particles;
	vsx_texture** tex;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;particlesystems;render_particle_center";
    info->in_param_spec = "particlesystem:particlesystem,texture:texture,position:float3,alpha:float,size:float";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    particles_in = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem",true,true);
  	tex_inf = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture",true,true);
  	position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");
  	position->set(0.0f,0);
  	position->set(1.0f,1);
  	position->set(0.0f,2);
  	alpha = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"alpha");
  	alpha->set(1.0f);
  	size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"size");
  	size->set(1.0f);

  	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
  }

  void output(vsx_module_param_abs* param) {
    particles = particles_in->get_addr();
    if (particles)
    {
      tex = tex_inf->get_addr();
      float local_alpha = alpha->get();
      if (tex) {
        if (!((*tex)->valid)) {
          return;
        }
        float cx = position->get(0);
        float cy = position->get(1);
        float cz = position->get(2);
      	vsx_transform_obj texture_transform = (*tex)->get_transform();

      	glMatrixMode(GL_TEXTURE);
      	glPushMatrix();

      	if ((*tex)->get_transform()) texture_transform();

        (*tex)->bind();
        beginBlobs();
        glBegin(GL_QUADS);

        float sz = size->get();
        int count = 0;
        for (i = 0; i < particles->particles->size(); ++i) {
          //a = (1-((*particles->particles)[i].time/(*particles->particles)[i].lifetime));
          //if (a < 0.0f) a = 0.0f;
          if ((*particles->particles)[i].size > 0.0f) {
            glColor4f(
              (*particles->particles)[i].color.r*local_alpha,
              (*particles->particles)[i].color.g*local_alpha,
              (*particles->particles)[i].color.b*local_alpha,
              (*particles->particles)[i].color.a
            );
            drawBlob_c(
              (*particles->particles)[i].pos.x,
              (*particles->particles)[i].pos.y,
              (*particles->particles)[i].pos.z,
              (*particles->particles)[i].size*sz,
              cx,cy,cz
            );
            ++count;
          }
        }
        glEnd();
        (*tex)->_bind();
      	glMatrixMode(GL_TEXTURE);
      	glPopMatrix();
      	glMatrixMode(GL_MODELVIEW);
        render_result->set(1);
        //printf("rendered %d particles\n",count);
      }
    }
    render_result->set(0);
  }
};


#define MAX_PARTICLES 300
#define ARRAY_STEPS 20.0f

class module_render_particlesystem_sparks : public vsx_module {
  unsigned long i;
  // in
  vsx_module_param_particlesystem* particles_in;
  vsx_module_param_float_array* float_array_in;
  vsx_module_param_float* proximity_level;
  vsx_module_param_float4* color;

  // out
  vsx_module_param_render* render_result;
  // internal
  vsx_particlesystem* particles;
  vsx_float_array* data;
  bool charges_init;
  vsx_avector<float> charges;
  vsx_avector<float> delta_charges;
  vsx_avector<int> discharged_particles;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;particlesystems;sparks";
    info->in_param_spec = "particlesystem:particlesystem,"
                          "float_array_in:float_array,"
                          "proximity_level:float,"
                          "color:float4";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
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

  void output(vsx_module_param_abs* param) {
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

      vsx_vector perpendicular_mult;
      vsx_vector upv(0, 1.0f);
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
        vsx_vector last_dist;
        charges[i] += delta_charges[i] * engine->dtime;
        //if (i == 0) printf("%f\n", charges[i]);
        vsx_vector dist;
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
            charges[i] += -delta_charges[i] * engine->dtime * 0.5f;
            charges[found] += -delta_charges[found] * engine->dtime * 0.5f;
            //delta_charges[i] = ((float)(rand()%1000) * 0.001f - 0.5f) * 5.0f;
            //delta_charges[j] = ((float)(rand()%1000) * 0.001f - 0.5f) * 5.0f;
            //printf("%f\n", charges[i]);
            discharged_particles.push_back(i);
            //discharged_particles.push_back(found);
            // calculate perpendicular vector by cross product with up vector (0,1,0)
            perpendicular_mult.cross(last_dist, upv);
            perpendicular_mult.normalize();
            //perpendicular_mult *= 0.1f; // might use a parameter here?
            // ok, time to draw a line
            glBegin(GL_LINE_STRIP);
            //vsx_vector tv = (*particles->particles)[0].pos;
            for (int k = 0; k < ARRAY_STEPS; k++)
            {
              vsx_vector tv = (*particles->particles)[found].pos + last_dist * ((float)k / ARRAY_STEPS);
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

#if BUILDING_DLL
vsx_module* create_new_module(unsigned long module) {
  switch (module) {
    case 0: return (vsx_module*)(new module_render_particlesystem);
    case 1: return (vsx_module*)(new module_render_particlesystem_c);
    case 2: return (vsx_module*)(new module_render_particlesystem_sparks);
  }
  return 0;
}


void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (module_render_particlesystem*)m; break;
    case 1: delete (module_render_particlesystem_c*)m; break;
    case 2: delete (module_render_particlesystem_sparks*)m; break;
  }
}

unsigned long get_num_modules() {
  glewInit();
  // we have only one module. it's id is 0
  return 3;
}

#endif
