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

class module_texture_effect_blur : public vsx_module
{
  // in
  vsx_module_param_texture* glow_source;
  vsx_module_param_float* start_value;
  vsx_module_param_float* attenuation;
  vsx_module_param_int* texture_size;
  vsx_module_param_int* passes;
  int tex_size_internal;

  int res_x, res_y;
  // out
  vsx_module_param_texture* texture_result;

  // internal
  vsx_texture* texture;
  vsx_texture* texture2;
  vsx_glsl shader;
  GLuint tex_id;
  GLuint glsl_offset_id,glsl_tex_id,glsl_attenuation;
  GLint	viewport[4];
  vsx_gl_state* gl_state;

public:


  ~module_texture_effect_blur()
  {
  }

void module_info(vsx_module_info* info) {
  glewInit();
  info->identifier = "texture;effects;blur";
#ifndef VSX_NO_CLIENT
  info->in_param_spec =
      "glow_source:texture,"
      "start_value:float,"
      "attenuation:float,"
      "texture_size:enum?2048x2048|1024x1024|512x512|256x256|128x128|64x64|32x32|16x16|8x8|4x4|VIEWPORT_SIZE|VIEWPORT_SIZE_DIV_2|VIEWPORT_SIZE_DIV_4|VIEWPORT_SIZEx2,"
      "passes:enum?ONE|TWO"
  ;
  info->out_param_spec = "texture_out:texture";
  info->component_class = "texture";
#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
  glewInit();
  glow_source = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "glow_source",false,false);
  texture_result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");

  texture_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"texture_size");
  texture_size->set(3);
  tex_size_internal = 3;

  passes = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"passes");
  passes->set(0);

  gl_state = vsx_gl_state::get_instance();

  texture = new vsx_texture;
  texture->set_gl_state( gl_state );
  res_x = res_y = 256;
  texture->reinit_color_buffer(res_x,res_y,true,false);
  texture->valid = true;

  texture2 = new vsx_texture;
  texture2->set_gl_state( gl_state );
  texture2->reinit_color_buffer(res_x,res_y,true,false);
  texture2->valid = true;


  start_value = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"start_value");
  start_value->set(1.0f);
  attenuation = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
  attenuation->set(1.0f);

  texture_result->set(texture);



#ifdef __APPLE__
  return;
  shader.vertex_program = "\
  void main()\n\
  {\n\
    gl_TexCoord[0] = gl_MultiTexCoord0;\n\
    gl_Position = ftransform();\n\
  }\n\
  ";

  shader.fragment_program = "\
  uniform sampler2D GlowTexture;\n\
  uniform vec2      texOffset;\n\
  uniform float     attenuation;\n\
  void main(void)\n\
  {\n\
    vec4  finalColor = vec4(0.0,0.0,0.0,0.0);\n\
  \n\
    vec2 offset = vec2(-4.0,-4.0) * (texOffset) + gl_TexCoord[0].xy;\n\
    finalColor += 0.0217 * texture2D(GlowTexture, offset);\n\
    offset += texOffset;\n\
    finalColor += 0.0434 * texture2D(GlowTexture, offset);\n\
    offset += texOffset;\n\
    finalColor += 0.0869 * texture2D(GlowTexture, offset);\n\
    offset += texOffset;\n\
    finalColor += 0.1739 * texture2D(GlowTexture, offset);\n\
    offset += texOffset;\n\
    finalColor += 0.3478 * texture2D(GlowTexture, offset);\n\
    offset += texOffset;\n\
    finalColor += 0.1739 * texture2D(GlowTexture, offset);\n\
    offset += texOffset;\n\
    finalColor += 0.0869 * texture2D(GlowTexture, offset);\n\
    offset += texOffset;\n\
    finalColor += 0.0434 * texture2D(GlowTexture, offset);\n\
    offset += texOffset;\n\
    finalColor += 0.0217 * texture2D(GlowTexture, offset);\n\
    gl_FragColor = finalColor * attenuation; \n\
  }\n\
  ";
#else
shader.vertex_program = "\
varying vec2 texcoord;\n\
void main()\n\
{\n\
  texcoord = gl_MultiTexCoord0.st;\n\
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
}\n\
";

/*shader.fragment_program = "\
uniform sampler2D GlowTexture;\n\
uniform vec2      texOffset;\n\
uniform float     attenuation;\n\
varying vec2 texcoord;\n\
void main(void)\n\
{\n\
  vec4  finalColor = vec4(0.0,0.0,0.0,0.0);\n\
\n\
  vec2 offset = vec2(-4.0,-4.0) * (texOffset) + texcoord;\n\
  finalColor += 0.0217 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0434 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0869 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.1739 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.3478 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.1739 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0869 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0434 * texture2D(GlowTexture, offset);\n\
  offset += texOffset;\n\
  finalColor += 0.0217 * texture2D(GlowTexture, offset);\n\
  gl_FragColor = finalColor * attenuation; \n\
}\n\
";
*/

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

/*
    shader.fragment_program =
    "uniform float sigma;     // The sigma value for the gaussian function: higher value means more blur\n"
    "                        // A good value for 9x9 is around 3 to 5\n"
    "                        // A good value for 7x7 is around 2.5 to 4\n"
    "                        // A good value for 5x5 is around 2 to 3.5\n"
    "                             // ... play around with this based on what you need :)\n"
    "    \n"
    "    uniform float blurSize;  // This should usually be equal to\n"
    "                             // 1.0f / texture_pixel_width for a horizontal blur, and\n"
    "                             // 1.0f / texture_pixel_height for a vertical blur.\n"
    "\n"
    "    uniform sampler2D GlowTexture;  // Texture that will be blurred by this shader\n"
    "\n"
    "    const float pi = 3.14159265f;\n"
    "    \n"
    "    // The following are all mutually exclusive macros for various \n"
    "    // seperable blurs of varying kernel size\n"
    "    #if defined(VERTICAL_BLUR_9)\n"
    "    const float numBlurPixelsPerSide = 4.0f;\n"
    "    const vec2  blurMultiplyVec      = vec2(0.0f, 1.0f);\n"
    "    #elif defined(HORIZONTAL_BLUR_9)\n"
    "    const float numBlurPixelsPerSide = 4.0f;\n"
    "    const vec2  blurMultiplyVec      = vec2(1.0f, 0.0f);\n"
    "    #elif defined(VERTICAL_BLUR_7)\n"
    "    const float numBlurPixelsPerSide = 3.0f;\n"
    "    const vec2  blurMultiplyVec      = vec2(0.0f, 1.0f);\n"
    "    #elif defined(HORIZONTAL_BLUR_7)\n"
    "    const float numBlurPixelsPerSide = 3.0f;\n"
    "    const vec2  blurMultiplyVec      = vec2(1.0f, 0.0f);\n"
    "    #elif defined(VERTICAL_BLUR_5)\n"
    "    const float numBlurPixelsPerSide = 2.0f;\n"
    "    const vec2  blurMultiplyVec      = vec2(0.0f, 1.0f);\n"
    "    #elif defined(HORIZONTAL_BLUR_5)\n"
    "    const float numBlurPixelsPerSide = 2.0f;\n"
    "    const vec2  blurMultiplyVec      = vec2(1.0f, 0.0f);\n"
    "    #else\n"
    "    // This only exists to get this shader to compile when no macros are defined\n"
    "    const float numBlurPixelsPerSide = 0.0f;\n"
    "    const vec2  blurMultiplyVec      = vec2(0.0f, 0.0f);\n"
    "    #endif\n"
    "    \n"
    "    void main() {\n"
    "    \n"
    "      // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)\n"
    "      vec3 incrementalGaussian;\n"
    "      incrementalGaussian.x = 1.0f / (sqrt(2.0f * pi) * sigma);\n"
    "      incrementalGaussian.y = exp(-0.5f / (sigma * sigma));\n"
    "      incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;\n"
    "    \n"
    "      vec4 avgValue = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
    "      float coefficientSum = 0.0f;\n"
    "    \n"
    "      // Take the central sample first...\n"
    "      avgValue += texture2D(blurSampler, gl_TexCoord[0].xy) * incrementalGaussian.x;\n"
    "      coefficientSum += incrementalGaussian.x;\n"
    "      incrementalGaussian.xy *= incrementalGaussian.yz;\n"
    "    \n"
    "      // Go through the remaining 8 vertical samples (4 on each side of the center)\n"
    "      for (float i = 1.0f; i <= numBlurPixelsPerSide; i++) { \n"
    "        avgValue += texture2D(blurSampler, gl_TexCoord[0].xy - i * blurSize * \n"
    "                              blurMultiplyVec) * incrementalGaussian.x;         \n"
    "        avgValue += texture2D(blurSampler, gl_TexCoord[0].xy + i * blurSize * \n"
    "                              blurMultiplyVec) * incrementalGaussian.x;         \n"
    "        coefficientSum += 2 * incrementalGaussian.x;\n"
    "        incrementalGaussian.xy *= incrementalGaussian.yz;\n"
    "      }\n"
    "       \n"
    "      gl_FragColor = avgValue / coefficientSum;\n"
    "    }\n"
    "    \n"
    ;

*/
#endif

/*shader.fragment_program = "\
uniform sampler2D GlowTexture;\n\
uniform vec2      texOffset;\n\
void main(void)\n\
{\n\
  vec4  blurFilter[9],\n\
        finalColor = vec4(0.0,0.0,0.0,0.0);\n\
\n\
  float xOffset    = texOffset.x,\n\
        yOffset    = texOffset.y;\n\
\n\
  blurFilter[0]  = vec4( 4.0*xOffset, 4.0*yOffset, 0.0, 0.0217);\n\
  blurFilter[1]  = vec4( 3.0*xOffset, 3.0*yOffset, 0.0, 0.0434);\n\
  blurFilter[2]  = vec4( 2.0*xOffset, 2.0*yOffset, 0.0, 0.0869);\n\
  blurFilter[3]  = vec4( 1.0*xOffset, 1.0*yOffset, 0.0, 0.1739);\n\
  blurFilter[4]  = vec4(         0.0,         0.0, 0.0, 0.3478);\n\
  blurFilter[5]  = vec4(-1.0*xOffset,-1.0*yOffset, 0.0, 0.1739);\n\
  blurFilter[6]  = vec4(-2.0*xOffset,-2.0*yOffset, 0.0, 0.0869);\n\
  blurFilter[7]  = vec4(-3.0*xOffset,-3.0*yOffset, 0.0, 0.0434);\n\
  blurFilter[8]  = vec4(-4.0*xOffset,-4.0*yOffset, 0.0, 0.0217);\n\
\n\
  for (int i = 0;i< 9;i++)\n\
    finalColor += texture2D(GlowTexture, gl_TexCoord[0].xy + blurFilter[i].xy) * blurFilter[i].w;\n\
\n\
  gl_FragColor = finalColor*1.15;\n\
}\n\
";
*/
  vsx_string shader_res = shader.link();
//  printf("shader res: %s\n",shader_res.c_str());
  glsl_tex_id = glGetUniformLocationARB(shader.prog,"GlowTexture");
  glsl_offset_id = glGetUniformLocationARB(shader.prog,"texOffset");
  glsl_attenuation = glGetUniformLocationARB(shader.prog,"attenuation");

}

  void run() {
    if
    (
      !glow_source->connected
    )
    {
      return;
    }
    if (texture == 0)
    {
      //printf("############################# texture is 0 and creating new one...\n");
      texture = new vsx_texture;
      texture->set_gl_state( gl_state );

      tex_size_internal = 3;
      texture->reinit_color_buffer(res_x,res_y,true,false);
      //texture->valid = false;

      texture2 = new vsx_texture;
      texture2->set_gl_state( gl_state );
      texture2->reinit_color_buffer(res_x,res_y,true,false);
      //texture2->valid = false;
    }
    bool rebuild = false;
    if (texture_size->get() >= 10)
    {
      gl_state->viewport_get(viewport);
      int t_res_x = abs(viewport[2] - viewport[0]);
      int t_res_y = abs(viewport[3] - viewport[1]);

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
        case 10: res_x = abs(viewport[2] - viewport[0]); res_y = abs(viewport[3] - viewport[1]); break;
        case 11: res_x = abs(viewport[2] - viewport[0]) / 2; res_y = abs(viewport[3] - viewport[1]) / 2; break;
        case 12: res_x = abs(viewport[2] - viewport[0]) / 4; res_y = abs(viewport[3] - viewport[1]) / 4; break;
        case 13: res_x = abs(viewport[2] - viewport[0]) * 2; res_y = abs(viewport[3] - viewport[1]) * 2; break;
      };
      texture->reinit_color_buffer(res_x,res_y,true,false);
      texture2->reinit_color_buffer(res_x,res_y,true,false);
    }


    vsx_texture** ti = glow_source->get_addr();

    if (!ti) return;
    if (!texture) return;
    if (!texture2) return;

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

    texture->begin_capture_to_buffer();
      glViewport(0,0,res_x,res_y);
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
    texture->end_capture_to_buffer();
    texture->valid = true;

    //
    texture2->begin_capture_to_buffer();
      glViewport(0,0,res_x,res_y);
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
    texture2->end_capture_to_buffer();
    texture2->valid = true;

    if (passes->get() == 0)
    {
      texture_result->set(texture2);
      return;
    }



//    pixel_offset_size_x = 1.0f / (float)res_x;
//    pixel_offset_size_y = 1.0f / (float)res_y;
    pixel_offset_size_x = start_value->get() * 0.4f / (float)res_x;
    pixel_offset_size_y = start_value->get() * 0.4f / (float)res_y;

    // 2nd pass
    texture->begin_capture_to_buffer();
      glViewport(0,0,res_x,res_y);
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
    texture->end_capture_to_buffer();
    texture->valid = true;

    texture2->begin_capture_to_buffer();
      glViewport(0,0,res_x,res_y);
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
    texture2->end_capture_to_buffer();
    texture2->valid = true;



    // finally, set the output
    texture_result->set(texture2);
  }

  void stop() {
  #ifdef __APPLE__
    return;
  #endif
    shader.stop();
    if (texture) {
      texture->deinit_buffer();
      texture2->deinit_buffer();
      delete texture;
      delete texture2;
      texture = 0;
      texture2 = 0;
    }
  }

  void start() {
  #ifdef __APPLE__
    return;
  #endif
    shader.link();
    glsl_offset_id = glGetUniformLocationARB(shader.prog,"texOffset");
    glsl_tex_id = glGetUniformLocationARB(shader.prog,"GlowTexture");
    glsl_attenuation = glGetUniformLocationARB(shader.prog,"attenuation");
  }

  void on_delete()
  {
    if (texture) {
      texture->deinit_buffer();
      texture2->deinit_buffer();
      delete texture;
      delete texture2;
      texture = 0;
      texture2 = 0;
    }
  }
};
