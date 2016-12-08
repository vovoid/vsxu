/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <texture/buffer/vsx_texture_buffer_color.h>

class module_texture_effect_blur : public vsx_module
{
  // in
  vsx_module_param_texture* glow_source;
  vsx_module_param_float* start_value;
  vsx_module_param_float* attenuation;
  vsx_module_param_int* texture_size;
  vsx_module_param_int* passes;
  vsx_module_param_int* alpha;
  int tex_size_internal = -1;

  int res_x = 256;
  int res_y = 256;
  // out
  vsx_module_param_texture* texture_result;

  // internal
  vsx_texture<>* texture = 0x0;
  vsx_texture<>* texture2 = 0x0;
  vsx_texture_buffer_color buffer;
  vsx_texture_buffer_color buffer2;

  vsx_glsl shader;
  GLuint tex_id;
  GLuint glsl_offset_id,glsl_tex_id,glsl_attenuation;
  vsx_gl_state* gl_state;

public:

void module_info(vsx_module_specification* info)
{
  info->identifier =
    "texture;effects;blur";

  info->in_param_spec =
    "glow_source:texture,"
    "start_value:float,"
    "attenuation:float,"
    "texture_size:enum?2048x2048|1024x1024|512x512|256x256|128x128|64x64|32x32|16x16|8x8|4x4|VIEWPORT_SIZE|VIEWPORT_SIZE_DIV_2|VIEWPORT_SIZE_DIV_4|VIEWPORT_SIZEx2,"
    "passes:enum?ONE|TWO,"
    "alpha:enum?NO|YES"
  ;

  info->out_param_spec =
    "texture_out:texture";

  info->component_class =
    "texture";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  glow_source = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "glow_source",false,false);
  texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");

  texture_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"texture_size");
  texture_size->set(3);

  passes = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"passes");
  passes->set(0);

  gl_state = vsx_gl_state::get_instance();

  start_value = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"start_value");
  start_value->set(1.0f);
  attenuation = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
  attenuation->set(1.0f);

  alpha = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");
  alpha->set(0);


shader.vertex_program = "\
varying vec2 texcoord;\n\
void main()\n\
{\n\
  texcoord = gl_MultiTexCoord0.st;\n\
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
}\n\
";

shader.fragment_program = "\
uniform sampler2D GlowTexture;\n\
uniform vec2      texOffset;\n\
uniform float     attenuation;\n\
varying vec2 texcoord;\n\
void main(void)\n\
{\n\
  vec4  finalColor = vec4(0.0,0.0,0.0,0.0);\n\
\n\
  vec2 offset = vec2(-5.0,-5.0) * (texOffset) + texcoord;\n\
  \n\
  finalColor += 0.0097 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.0297 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.0439453 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.1171875 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.205078125 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.24609375 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.205078125 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.1171875 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.0439453 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.0297 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  \n\
  finalColor += 0.0097 * texture2D(GlowTexture, offset);\n\
  \n\
  gl_FragColor = finalColor * attenuation; \n\
}\n\
";

  vsx_string<>shader_res = shader.link();
//  printf("shader res: %s\n",shader_res.c_str());
  glsl_tex_id = glGetUniformLocationARB(shader.prog,"GlowTexture");
  glsl_offset_id = glGetUniformLocationARB(shader.prog,"texOffset");
  glsl_attenuation = glGetUniformLocationARB(shader.prog,"attenuation");

}

  void run() {
    if ( !glow_source->connected )
      return;

    bool rebuild = false;
    if (texture_size->get() >= 10)
    {
      int t_res_x = gl_state->viewport_get_width();
      int t_res_y = gl_state->viewport_get_height();

      if (texture_size->get() == 10) {
        if (t_res_x != res_x || t_res_y != res_y) rebuild = true;
      }

      if (texture_size->get() == 11) {
        if (t_res_x / 2 != res_x || t_res_y / 2 != res_y) rebuild = true;
      }

      if (texture_size->get() == 12) {
        if (t_res_x / 4 != res_x || t_res_y / 4 != res_y) rebuild = true;
      }

      if (texture_size->get() == 13) {
        if (t_res_x * 2 != res_x || t_res_y * 2 != res_y) rebuild = true;
      }
    }


    if (texture_size->get() != tex_size_internal || rebuild) {
      tex_size_internal = texture_size->get();
      switch (tex_size_internal) {
        case 0: res_y = res_x = 2048; break;
        case 1: res_y = res_x = 1024; break;
        case 2: res_y = res_x = 512; break;
        case 3: res_y = res_x = 256; break;
        case 4: res_y = res_x = 128; break;
        case 5: res_y = res_x = 64; break;
        case 6: res_y = res_x = 32; break;
        case 7: res_y = res_x = 16; break;
        case 8: res_y = res_x = 8; break;
        case 9: res_y = res_x = 4; break;
        case 10:
          res_x = gl_state->viewport_get_width();
          res_y = gl_state->viewport_get_height();
        break;
        case 11:
          res_x = gl_state->viewport_get_width() * 0.5;
          res_y = gl_state->viewport_get_height() * 0.5;
        break;
        case 12:
          res_x = gl_state->viewport_get_width() * 0.25;
          res_y = gl_state->viewport_get_height() * 0.25;
        break;
        case 13:
          res_x = gl_state->viewport_get_width() * 2.0;
          res_y = gl_state->viewport_get_height() * 2.0;
        break;
      };

      if (!texture)
        texture = new vsx_texture<>();

      buffer.reinit(texture, res_x, res_y, true, alpha->get() == 1, false, true, 0);

      if (!texture2)
        texture2 = new vsx_texture<>();
      buffer2.reinit(texture2, res_x, res_y, true, alpha->get() == 1, false, true, 0);
    }

    vsx_texture<>** ti = glow_source->get_addr();

    req(ti);
    req(texture);
    req(texture2);

    //
    (*ti)->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    (*ti)->_bind();

    texture->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture->_bind();

    texture2->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture2->_bind();

    float pixel_offset_size_x = start_value->get() * 0.1f / (float)res_x;
    float pixel_offset_size_y = start_value->get() * 0.1f / (float)res_y;

    buffer.begin_capture_to_buffer();
      loading_done = true;
      glColor4f(1,1,1,1);
      glDisable(GL_BLEND);

      if (GLEW_VERSION_1_3)
        glActiveTexture(GL_TEXTURE0);
      (*ti)->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        shader.begin();
          glUniform1iARB(glsl_tex_id,0);
          glUniform2fARB(glsl_offset_id,   pixel_offset_size_x,0.0f);
          glUniform1fARB(glsl_attenuation, attenuation->get());
          glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(0.0f,1.0f);
            glVertex3f(-1.0f,  1.0f, 0.0f);
            glTexCoord2f(1.0f,1.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
            glTexCoord2f(1.0f,0.0f);
            glVertex3f( 1.0f, -1.0f, 0.0f);
          glEnd();
        shader.end();
      (*ti)->_bind();
    buffer.end_capture_to_buffer();

    // -------------------------------

    buffer2.begin_capture_to_buffer();
      loading_done = true;
      glColor4f(1,1,1,1);
      glDisable(GL_BLEND);
      if (GLEW_VERSION_1_3)
      glActiveTexture(GL_TEXTURE0);
      texture->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

        shader.begin();
          glUniform1iARB(glsl_tex_id,0);
          glUniform2fARB(glsl_offset_id, 0.0f, pixel_offset_size_y);
          glUniform1fARB(glsl_attenuation, attenuation->get());
          glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(0.0f,1.0f);
            glVertex3f(-1.0f,  1.0f, 0.0f);
            glTexCoord2f(1.0f,1.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
            glTexCoord2f(1.0f,0.0f);
            glVertex3f( 1.0f, -1.0f, 0.0f);
          glEnd();
        shader.end();
      texture->_bind();
    buffer2.end_capture_to_buffer();

    if (passes->get() == 0)
    {
      texture_result->set(texture2);
      return;
    }

    // 2nd pass ***************************************************************


    pixel_offset_size_x = start_value->get() * 0.4f / (float)res_x;
    pixel_offset_size_y = start_value->get() * 0.4f / (float)res_y;

    buffer.begin_capture_to_buffer();
      loading_done = true;
      glColor4f(1,1,1,1);
      glDisable(GL_BLEND);

      if (GLEW_VERSION_1_3)
        glActiveTexture(GL_TEXTURE0);
      texture2->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        shader.begin();
          glUniform1iARB(glsl_tex_id,0);
          glUniform2fARB(glsl_offset_id,pixel_offset_size_x,0.0f);
          glUniform1fARB(glsl_attenuation, attenuation->get());
          glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(0.0f,1.0f);
            glVertex3f(-1.0f,  1.0f, 0.0f);
            glTexCoord2f(1.0f,1.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
            glTexCoord2f(1.0f,0.0f);
            glVertex3f( 1.0f, -1.0f, 0.0f);
          glEnd();
        shader.end();
      texture2->_bind();
    buffer.end_capture_to_buffer();

    buffer2.begin_capture_to_buffer();
      loading_done = true;
      glColor4f(1,1,1,1);
      glDisable(GL_BLEND);
      if (GLEW_VERSION_1_3)
      glActiveTexture(GL_TEXTURE0);
      texture->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

        shader.begin();
          glUniform1iARB(glsl_tex_id,0);
          glUniform2fARB(glsl_offset_id, 0.0f, pixel_offset_size_y);
          glUniform1fARB(glsl_attenuation, attenuation->get());
          glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(0.0f,1.0f);
            glVertex3f(-1.0f,  1.0f, 0.0f);
            glTexCoord2f(1.0f,1.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
            glTexCoord2f(1.0f,0.0f);
            glVertex3f( 1.0f, -1.0f, 0.0f);
          glEnd();
        shader.end();
      texture->_bind();
    buffer2.end_capture_to_buffer();



    // finally, set the output
    texture_result->set(texture2);
  }

  void stop() {
    shader.stop();
    if (texture) {
      buffer.deinit(texture);
      buffer2.deinit(texture2);
      delete texture;
      delete texture2;
      texture = 0;
      texture2 = 0;
    }
  }

  void start() {
    shader.link();
    glsl_offset_id = glGetUniformLocationARB(shader.prog,"texOffset");
    glsl_tex_id = glGetUniformLocationARB(shader.prog,"GlowTexture");
    glsl_attenuation = glGetUniformLocationARB(shader.prog,"attenuation");
  }

  void on_delete()
  {
    if (texture) {
      buffer.deinit(texture);
      buffer2.deinit(texture2);
      delete texture;
      delete texture2;
      texture = 0;
      texture2 = 0;
    }
  }
};
