/**
* Written by Alastair Cota for:
* 
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2014
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
#include <bitmap/vsx_bitmap.h>
#include <texture/vsx_texture.h>

#define VERTEX_PROGRAM \
       "varying vec2 texcoord;\n" \
       "void main()\n" \
       "{\n" \
       "  texcoord = gl_MultiTexCoord0.st;\n" \
       "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n" \
       "}\n"

#define FRAGMENT_PROGRAM \
       "uniform sampler2D A_tex;\n" \
       "uniform sampler2D B_tex;\n" \
       "uniform float A_mix;\n" \
       "uniform float B_mix;\n" \
       "varying vec2 texcoord;\n" \
       "vec4 Acolorvec;\n" \
       "vec4 Bcolorvec;\n" \
       "void main(void)\n" \
       "{\n" \
       "  Acolorvec = texture2D(A_tex, texcoord);\n" \
       "  Acolorvec = vec4((Acolorvec[0] * A_mix)," \
                          "(Acolorvec[1] * A_mix)," \
                          "(Acolorvec[2] * A_mix)," \
                          "1.0);\n" \
       "\n" \
       "  Bcolorvec = texture2D(B_tex, texcoord);\n" \
       "  Bcolorvec = vec4((Bcolorvec[0] * B_mix)," \
                          "(Bcolorvec[1] * B_mix)," \
                          "(Bcolorvec[2] * B_mix)," \
                          "1.0);\n" \
       "\n" \
       "  gl_FragColor = vec4(Acolorvec[0] + Bcolorvec[0]," \
                             "Acolorvec[1] + Bcolorvec[1]," \
                             "Acolorvec[2] + Bcolorvec[2]," \
                             "1.0);\n" \
       "}\n"

#include <string>
#include <sstream>

#define SEQ_RESOLUTION 8192

class module_texture_selector : public vsx_module
{
//---DATA-STORAGE---------------------------------------------------------------

  // in
  vsx_module_param_float* index;
  vsx_module_param_int* inputs;
  std::vector<vsx_module_param_texture*> texture_x;
  
  vsx_module_param_int* wrap;
  vsx_module_param_int* blend_type;
  vsx_module_param_float_sequence* sequence;
  vsx_module_param_int* reverse;
  vsx_module_param_int* reset_seq_to_default;
  
  vsx_module_param_int* blend_size;
  vsx_module_param_float_sequence* A_sequence;
  vsx_module_param_int* A_reverse;
  vsx_module_param_int* A_reset_seq_to_default;
  vsx_module_param_float_sequence* B_sequence;
  vsx_module_param_int* B_reverse;
  vsx_module_param_int* B_reset_seq_to_default;
  vsx_module_param_float4* clear_color;

  // out
  vsx_module_param_texture* result;

  // internal
  vsx_gl_state* gl_state;
  GLuint glsl_A_tex;
  GLuint glsl_B_tex;
  GLuint glsl_A_mix;
  GLuint glsl_B_mix;

  vsx_texture<>* i_tex_blank;
  vsx_texture_buffer_color buf_blank;
  vsx_texture<>** i_tex_A;
  vsx_texture<>** i_tex_B;
  vsx_texture<>* i_tex_output;
  vsx_texture_buffer_color buf_output;

  int i_prev_inputs;
  int i_curr_inputs;

  float i_index;
  int i_index_x;
  int i_index_x0;
  int i_index_x1;
  bool i_underRange;
  bool i_overRange;
  
  int i_wrap;
  int i_blend_type;
  vsx::sequence::channel<vsx::sequence::value_float> i_sequence;
  vsx::sequence::channel<vsx::sequence::value_float> i_seq_default;
  vsx_ma_vector<float> i_sequence_data;
  long i_seq_index;
  int i_reverse;

  int i_new_size;
  int i_tex_size;

  vsx::sequence::channel<vsx::sequence::value_float> i_A_sequence;
  vsx::sequence::channel<vsx::sequence::value_float> i_A_seq_default;
  vsx_ma_vector<float> i_A_sequence_data;
  long i_A_seq_index;
  int i_A_reverse;

  vsx::sequence::channel<vsx::sequence::value_float> i_B_sequence;
  vsx::sequence::channel<vsx::sequence::value_float> i_B_seq_default;
  vsx_ma_vector<float> i_B_sequence_data;
  long i_B_seq_index;
  int i_B_reverse;
  
  float i_A_mixLevel;
  float i_B_mixLevel;

  vsx_bitmap i_clear_bmp;
  uint32_t* i_clear_bmp_data;
  long i_clear_color[4];
  float i_prev_clear_color[4];
 
  std::stringstream i_paramString;
  std::stringstream i_paramName;
  vsx_string<>i_in_param_string;

  bool i_am_ready;

//---INITIALISATION-------------------------------------------------------------

public:

  module_texture_selector()
    :
      i_tex_blank(0x0),
      i_tex_output(0x0)
  {}

  //Initialise Data

  vsx_glsl shader;
    
  //Initialise Module & GUI
  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "selectors;texture_selector";

    info->description =
      "[result] is equal to the\n"
      "[texture_x] chosen by [index]\n"
      "\n"
      "The number of inputs is\n"
      "controlled by [inputs]\n"
      "\n"
      "A variety of blend options\n"
      "are available in [options]\n"
      "\n";

    info->out_param_spec =
      "result:texture";

    info->in_param_spec =
      "index:float,"
      "inputs:enum?0|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16&nc=1,"
      + i_in_param_string +
      "options:complex"
      "{"
        "wrap:enum?None_Zero|None_Freeze|Wrap,"
        "blend_type:enum?Snap|Linear|Sequence,"
        "blend_options:complex"
        "{"
          "blend_size:enum?"
          "8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024|2048x2048,"
          "A_crossfade:complex"
          "{"
            "A_sequence:sequence,"
            "A_reverse:enum?Off|On,"
            "A_reset_seq_to_default:enum?ok?nc=1"
          "},"
          "B_crossfade:complex"
          "{"
            "B_sequence:sequence,"
            "B_reverse:enum?Off|On,"
            "B_reset_seq_to_default:enum?ok?nc=1"
          "},"
          "clear_color:float4?default_controller=controller_col"
        "},"
        "sequence:sequence,"
        "reverse:enum?Off|On,"
        "reset_seq_to_default:enum?ok?nc=1"
      "}";

    info->component_class = "texture";

    info->output = 1; // Always Running
  }

  //Build Interface
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    i_am_ready = false; //Don't do operations if the data isn't ready

    gl_state = vsx_gl_state::get_instance();

    i_prev_inputs = 15; //"16" for loading
    i_curr_inputs = 2;  //"3" for default

    i_index = 0.0;
    i_index_x = 0;
    i_index_x0 = 0;
    i_index_x1 = 1;
    i_underRange = false;
    i_overRange = false;

    i_wrap = 2;          //Wrap
    i_blend_type = 1;    //Linear Blend
    i_reverse = 0;       //Off
    i_seq_index = 0;

    i_seq_default.get_item_by_index(0).value = 0.0;       //Default Items for Sequences
    i_seq_default.get_item_by_index(0).delay = 1.0;
    i_seq_default.get_item_by_index(0).interpolation = vsx::sequence::linear;

    i_A_seq_default.get_item_by_index(0).value = 1.0;
    i_A_seq_default.get_item_by_index(0).delay = 1.0;
    i_A_seq_default.get_item_by_index(1).value = 0.0;
    i_A_seq_default.get_item_by_index(0).interpolation = vsx::sequence::linear;

    i_B_seq_default.get_item_by_index(0).value = 0.0;
    i_B_seq_default.get_item_by_index(0).delay = 1.0;
    i_B_seq_default.get_item_by_index(0).interpolation = vsx::sequence::linear;

    i_A_mixLevel = 1.0;  //Default values to be sent to the shader
    i_A_reverse = 0;
    i_A_seq_index = 0;

    i_B_mixLevel = 0.0;
    i_B_reverse = 0;
    i_B_seq_index = 0;

    i_tex_size = 5;      //256x256
    i_new_size = 5;

    i_clear_color[0] =  0;   //R
    i_clear_color[1] =  0;   //G
    i_clear_color[2] =  0;   //B
    i_clear_color[3] =  255; //A

    i_prev_clear_color[0] = 0.0;
    i_prev_clear_color[1] = 0.0;
    i_prev_clear_color[2] = 0.0;
    i_prev_clear_color[3] = 1.0;

    i_clear_bmp.width = 1;
    i_clear_bmp.height = 1;

    i_clear_bmp.data_set( malloc( sizeof(uint32_t) ) );
    i_clear_bmp_data = (uint32_t*)i_clear_bmp.data_get();
    *i_clear_bmp_data = 0xff000000;


    i_in_param_string = "";

    loading_done = true; //Allows main sequencer to start playing
    
    //Give the programs to the shader and retrieve id's for its uniform variables
    shader.vertex_program = VERTEX_PROGRAM;     
    shader.fragment_program = FRAGMENT_PROGRAM;
    shader.link(); //return from this shows glsl compiler errors if any
    glsl_A_tex = glGetUniformLocationARB(shader.prog, "A_tex");
    glsl_B_tex = glGetUniformLocationARB(shader.prog, "B_tex");
    glsl_A_mix = glGetUniformLocationARB(shader.prog, "A_mix");
    glsl_B_mix = glGetUniformLocationARB(shader.prog, "B_mix");

    //Priority parameters - inputs must get loaded before texture_x in the state file
    index = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "index");
    index->set(i_index);
    inputs = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "inputs");
    inputs->set((float)(i_curr_inputs + 1)); //converts 0-based index to 1-based index!

    texture_x.clear();     //Create the array and the param_string for texture_x
    i_paramString.str("");
    i_paramString << "texture_x:complex{";

    for(int i = 0; i < 16; ++i)
    {
      if(i > 0) i_paramString << ",";
      i_paramName.str("");
      i_paramName << "texture_" << i;
      i_paramString << i_paramName.str().c_str() << ":texture";

      texture_x.push_back((vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, i_paramName.str().c_str()));
      texture_x[i]->set(i_tex_blank);
    }

    i_paramString << "},";
    i_in_param_string = i_paramString.str().c_str();

    //Options
    wrap = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "wrap");
    wrap->set(i_wrap);
    blend_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "blend_type");
    blend_type->set(i_blend_type);
    sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE, "sequence");
    sequence->set(i_seq_default);
    reverse = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reverse");
    reverse->set(i_reverse);
    reset_seq_to_default = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reset_seq_to_default");
    reset_seq_to_default->set(-1);
    
    //Blend options
    blend_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "blend_size");
    blend_size->set(i_tex_size);
    clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "clear_color");
    clear_color->set(i_clear_color[0], 0);
    clear_color->set(i_clear_color[1], 1);
    clear_color->set(i_clear_color[2], 2);
    clear_color->set(i_clear_color[3], 3);

    //Crossfade options
    A_sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE, "A_sequence");
    A_sequence->set(i_A_seq_default);
    A_reverse = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "A_reverse");
    A_reverse->set(i_A_reverse);
    A_reset_seq_to_default = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "A_reset_seq_to_default");
    A_reset_seq_to_default->set(-1);

    B_sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE, "B_sequence");
    B_sequence->set(i_B_seq_default);
    B_reverse = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "B_reverse");
    B_reverse->set(i_B_reverse);
    B_reset_seq_to_default = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "B_reset_seq_to_default");
    B_reset_seq_to_default->set(-1);

    //Outputs
    result = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"result");
    result->set(i_tex_output);
  }

  //Rebuild Inputs
  void redeclare_in_params(vsx_module_param_list& in_parameters)
  {
    loading_done = true;

    shader.vertex_program = VERTEX_PROGRAM;
    shader.fragment_program = FRAGMENT_PROGRAM;
    shader.link();
    glsl_A_tex = glGetUniformLocationARB(shader.prog, "A_tex");
    glsl_B_tex = glGetUniformLocationARB(shader.prog, "B_tex");
    glsl_A_mix = glGetUniformLocationARB(shader.prog, "A_mix");
    glsl_B_mix = glGetUniformLocationARB(shader.prog, "B_mix");

    index = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "index");
    inputs = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "inputs");

    texture_x.clear();
    i_paramString.str("");
    i_paramString << "texture_x:complex{";

    for(int i = 0; i <= i_prev_inputs; ++i)
    {
        if(i > 0) i_paramString << ",";
        i_paramName.str("");
        i_paramName << "texture_" << i;
        i_paramString << i_paramName.str().c_str() << ":texture";

        texture_x.push_back((vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, i_paramName.str().c_str()));
        texture_x[i]->set(i_tex_blank);
    }

    i_paramString << "},";
    i_in_param_string = i_paramString.str().c_str();

    wrap = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "wrap");
    blend_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "blend_type");
    sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE, "sequence");
    reverse = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reverse");
    reset_seq_to_default = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reset_seq_to_default");
    
    blend_size = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "blend_size");
    clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "clear_color");

    A_sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE, "A_sequence");
    A_reverse = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "A_reverse");
    A_reset_seq_to_default = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "A_reset_seq_to_default");

    B_sequence = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE, "B_sequence");
    B_reverse = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "B_reverse");
    B_reset_seq_to_default = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "B_reset_seq_to_default");
    
    i_am_ready = true; //Data is ready, we can start running properly!
  }

//---SHADER-FUNCTIONS-----------------------------------------------------------
 
  //Set GL parameters for a texture
  void BindTexture(vsx_texture<>* tex)
  {
    tex->bind();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    tex->_bind();
  }

  //Send the textures and parameters to the shader, returning a shaded texture
  void BlendTexture(vsx_texture<>* texInA, vsx_texture<>* texInB,
                    float Amix, float Bmix, vsx_texture_buffer_color* bufOut)
  {
    bufOut->begin_capture_to_buffer();
      loading_done = true;
      glColor4f(1,1,1,1);
      glDisable(GL_BLEND);
      
      if(GLEW_VERSION_1_3) glActiveTexture(GL_TEXTURE0); //multitexturing
      texInA->bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
      if(GLEW_VERSION_1_3) glActiveTexture(GL_TEXTURE1);
      texInB->bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    
        shader.begin();
          glUniform1iARB(glsl_A_tex, 0); //0 and 1 are GL_TEXTURE numbers
          glUniform1iARB(glsl_B_tex, 1);
          glUniform1fARB(glsl_A_mix, Amix); //binding of parameter id's to their
          glUniform1fARB(glsl_B_mix, Bmix); //values for this run of the shader
        
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
      texInA->_bind();
      texInB->_bind();
    bufOut->end_capture_to_buffer();
  }

  //Sequence to delete any texture
  void DeleteTexture(vsx_texture<>* tex)
  {
    if(tex)
    {
      delete tex;
      tex = 0;
    }
  }

//---DATA-SETUP-FUNCTIONS-------------------------------------------------------

  //See /vsxu/engine/include/vsx_math.h for definitions of:
    //FLOAT_MOD(V, M)
    //FLOAT_CLAMP(V, MN, MX)

  //Cache the data of any sequence
  void CacheSequenceData(vsx::sequence::channel<vsx::sequence::value_float>* seq, vsx_ma_vector<float>* seqdata,
                         vsx_module_param_float_sequence* mpseq)
  {
    *seq = mpseq->get();
    mpseq->updates = 0;
    seq->reset();

    for(int i = 0; i < SEQ_RESOLUTION; ++i)
      (*seqdata)[i] = seq->execute(1.0f / (float)(SEQ_RESOLUTION - 1)).get_float();
  }

  //Calculate mix levels based on A/B Crossfade Curves
  void CalculateMixLevels(float ix, float ix0, float ix1)
  {
    CacheSequenceData(&i_A_sequence, &i_A_sequence_data, A_sequence);
    CacheSequenceData(&i_B_sequence, &i_B_sequence_data, B_sequence);

    i_A_reverse = A_reverse->get();
    i_B_reverse = B_reverse->get();

    if(i_A_reverse == 0) //Reverse A Off
      i_A_seq_index = (ix - ix0) * (float)SEQ_RESOLUTION;

    if(i_A_reverse == 1) //Reverse A On
      i_A_seq_index = (ix1 - ix) * (float)SEQ_RESOLUTION;

    if(i_B_reverse == 0) //Reverse B Off
      i_B_seq_index = (ix - ix0) * (float)SEQ_RESOLUTION;

    if(i_B_reverse == 1) //Reverse B On
      i_B_seq_index = (ix1 - ix) * (float)SEQ_RESOLUTION;
     
    i_A_seq_index = (long)FLOAT_CLAMP((float)i_A_seq_index, 0.0, 
                                      (float)(SEQ_RESOLUTION - 1));
    i_B_seq_index = (long)FLOAT_CLAMP((float)i_B_seq_index, 0.0,
                                      (float)(SEQ_RESOLUTION - 1));
     
    i_A_mixLevel = i_A_sequence_data[i_A_seq_index]; //had to do it twice or
    i_B_mixLevel = i_B_sequence_data[i_B_seq_index]; //it'd be argument overkill!
  }
  
  //Set-Up Data for Linear Blend
  void SetupLinearBlend()
  {
    i_wrap = wrap->get();
    switch(i_wrap)
    {
      case 0: //Don't Wrap Input- Zero Ends
        i_index = FLOAT_CLAMP(index->get(), -1.0, (float)(i_prev_inputs + 1));
        i_index_x = (int)i_index;
        i_underRange = i_index < 0.0;
        i_overRange = i_index > (float)i_prev_inputs;
        i_index_x0 = (i_underRange) ? -1
                   :((i_overRange) ? i_prev_inputs
                   :  FLOAT_CLAMP(i_index_x, -1, i_prev_inputs + 1));
        i_index_x1 = (i_underRange) ? 0
                   :((i_overRange) ? i_prev_inputs + 1
                   :  FLOAT_CLAMP(i_index_x + 1, 0, i_prev_inputs + 2));
      break;
      case 1: //Don't Wrap Input- Freeze Ends
        i_index = FLOAT_CLAMP(index->get(), 0.0, (float)i_prev_inputs);
        i_index_x = (int)i_index;
        i_index_x0 = FLOAT_CLAMP(i_index_x, 0, i_prev_inputs);
        i_index_x1 = FLOAT_CLAMP(i_index_x + 1, 0, i_prev_inputs);
      break;
      case 2: //Wrap Input
        i_index = FLOAT_MOD(index->get(), (float)(i_prev_inputs + 1));
        i_index_x = (int)i_index;
        i_index_x0 = i_index_x % (i_prev_inputs + 1);
        i_index_x1 = (i_index_x0 < i_prev_inputs) ? i_index_x0 + 1 : 0;
      break;
    }
    CalculateMixLevels(i_index, (float)i_index_x0, (float)i_index_x1);
  }

  //Set-Up Data for Mixing Using the Sequence Graph
  void SetupSequenceBlend()
  {
    CacheSequenceData(&i_sequence, &i_sequence_data, sequence);

    i_reverse = reverse->get();
    if(i_reverse == 0) //Reverse Off
      i_seq_index = (i_index - (float)i_index_x0) * (float)SEQ_RESOLUTION;

    if(i_reverse == 1) //Reverse On
      i_seq_index = ((float)i_index_x1 - i_index) * (float)SEQ_RESOLUTION;

    i_seq_index = (long)FLOAT_CLAMP((float)i_seq_index, 0.0,
                                    (float)(SEQ_RESOLUTION - 1));

    CalculateMixLevels(i_sequence_data[i_seq_index], 0.0, 1.0);
  }

//---BLEND-METHODS--------------------------------------------------------------

  //See /vsxu/engine/include/vsx_math.h for definitions of:
    //FLOAT_MOD(V, M)
    //FLOAT_CLAMP(V, MN, MX)

  //Send Chosen Texture Directly to Output
  void Snap()
  {
    i_wrap = wrap->get();
    switch(i_wrap)
    {
      case 0: //Don't Wrap Index - Zero Ends
        i_index = FLOAT_CLAMP(index->get() + 0.5, -0.5, (float)(i_prev_inputs) + 1.5);
        i_index_x0 = FLOAT_CLAMP((int)i_index, 0, i_prev_inputs);
      break;
      case 1: //Don't Wrap Index - Freeze Ends
        i_index = FLOAT_CLAMP(index->get() + 0.5, 0.5, (float)(i_prev_inputs) + 0.5);
        i_index_x0 = FLOAT_CLAMP((int)i_index, 0, i_prev_inputs);
      break;
      case 2: //Wrap Index
        i_index = FLOAT_MOD(index->get(), (float)(i_prev_inputs + 1));
        i_index_x0 = (i_index > (float)i_prev_inputs)
                   ? ((i_index > ((float)i_prev_inputs + 0.5)) ? 0 : i_prev_inputs)
                   : (int)(i_index + 0.5);
      break;
    }

    result->set(i_tex_blank);
    if(texture_x[i_index_x0]->connected)
      result->set(*(texture_x[i_index_x0]->get_addr()));
  }

  //Blend two Textures to Interpolate between them
  void Blend()
  {
    i_tex_A = &i_tex_blank;
    i_tex_B = &i_tex_blank;

    if(texture_x[i_index_x0]->connected)
      i_tex_A = texture_x[i_index_x0]->get_addr();
    if(texture_x[i_index_x1]->connected)
      i_tex_B = texture_x[i_index_x1]->get_addr();
    
    if((i_tex_A) && (i_tex_B) && (i_tex_output))
    {
      BindTexture(*i_tex_A);
      BindTexture(*i_tex_B);
      BindTexture(i_tex_output);
      
      BlendTexture(*i_tex_A,
                   *i_tex_B,
                   FLOAT_CLAMP(i_A_mixLevel, 0.0, 1.0),
                   FLOAT_CLAMP(i_B_mixLevel, 0.0, 1.0),
                   &buf_output);

      result->set(i_tex_output);
    }
  }

//---CORE-FUNCTIONS-------------------------------------------------------------

  //See /vsxu/engine/include/vsx_math.h for definitions of:
    //FLOAT_CLAMP(V, MN, MX)

  //Update Number of Inputs
  void UpdateInputs()
  {
    i_curr_inputs = inputs->get() - 1;
    if(i_prev_inputs != i_curr_inputs)
    {
      i_am_ready = false;
      i_prev_inputs = i_curr_inputs;
      redeclare_in = true;
    }
  }

  //Check for Reset to Default flag on any sequence
  void ResetSequence(vsx::sequence::channel<vsx::sequence::value_float>* seq, vsx::sequence::channel<vsx::sequence::value_float>* defseq,
       vsx_module_param_float_sequence* mpseq, vsx_module_param_int* mpflag)
  {
    if(mpflag->get() == 0) //ok
    {
      *seq = *defseq;
      mpseq->set(*seq);
      mpflag->set(-1);
    }
  }

  //Set Clear Color for Blank Textures
  void SetClearColor()
  {
    if((i_prev_clear_color[0] != clear_color->get(0))
    || (i_prev_clear_color[1] != clear_color->get(1))
    || (i_prev_clear_color[2] != clear_color->get(2))
    || (i_prev_clear_color[3] != clear_color->get(3)))
    {
      i_clear_color[0] = (long)FLOAT_CLAMP(255.0 * clear_color->get(0), 0.0, 255.0);
      i_clear_color[1] = (long)FLOAT_CLAMP(255.0 * clear_color->get(1), 0.0, 255.0);
      i_clear_color[2] = (long)FLOAT_CLAMP(255.0 * clear_color->get(2), 0.0, 255.0);
      i_clear_color[3] = (long)FLOAT_CLAMP(255.0 * clear_color->get(3), 0.0, 255.0);
  
      *i_clear_bmp_data = 0x01000000 * i_clear_color[3]
                                     |
                          0x00010000 * i_clear_color[2]
                                     |
                          0x00000100 * i_clear_color[1]
                                     |
                          0x00000001 * i_clear_color[0];
      vsx_texture_gl_loader::upload_bitmap_2d(i_tex_blank->texture, &i_clear_bmp, true);

      i_prev_clear_color[0] = clear_color->get(0);
      i_prev_clear_color[1] = clear_color->get(1);
      i_prev_clear_color[2] = clear_color->get(2);
      i_prev_clear_color[3] = clear_color->get(3);
    }
  }

  //Update Textures with new size
  void UpdateTextureSize()
  {
    i_new_size = blend_size->get();
    if(i_tex_size != i_new_size)
    { 
      i_tex_size = i_new_size;
      buf_output.reinit(i_tex_output, 8 << i_tex_size, 8 << i_tex_size, true, true, false, true, 0);
      buf_blank.reinit(i_tex_blank, 8 << i_tex_size, 8 << i_tex_size, true, true, false, true, 0);
      vsx_texture_gl_loader::upload_bitmap_2d(i_tex_blank->texture, &i_clear_bmp, true);
    }
  }

//---MODULE-MANAGEMENT---------------------------------------------------------

  void start()
  {
    shader.link();
    glsl_A_tex = glGetUniformLocationARB(shader.prog, "A_tex");
    glsl_B_tex = glGetUniformLocationARB(shader.prog, "B_tex");
    glsl_A_mix = glGetUniformLocationARB(shader.prog, "A_mix");
    glsl_B_mix = glGetUniformLocationARB(shader.prog, "B_mix");
  }

  void stop()
  {
    shader.stop();
    buf_output.deinit(i_tex_output);
    DeleteTexture(i_tex_output);
  }

  void on_delete()
  {
    buf_output.deinit(i_tex_output);
    buf_blank.deinit(i_tex_blank);
    DeleteTexture(i_tex_output);
    DeleteTexture(i_tex_blank);
  }

//---MAIN-PROGRAM-LOOP---------------------------------------------------------

  void run()
  {
    if (!i_tex_output)
    {
      i_tex_output = new vsx_texture<>; //Output Texture from the shader
      i_tex_blank = new vsx_texture<>; //Blank Texture for default and clear_color

      buf_output.reinit(i_tex_output, 8 << i_tex_size, 8 << i_tex_size, true, true, false, true, 0);
      buf_blank.reinit(i_tex_blank, 8 << i_tex_size, 8 << i_tex_size, true, true, false, true, 0);

      vsx_texture_gl_loader::upload_bitmap_2d(i_tex_blank->texture, &i_clear_bmp, true);

      i_tex_A = &i_tex_blank;
      i_tex_B = &i_tex_blank;
    }

    UpdateInputs();
    ResetSequence(&i_sequence, &i_seq_default,
                     sequence, reset_seq_to_default);
    ResetSequence(&i_A_sequence, &i_A_seq_default,
                     A_sequence, A_reset_seq_to_default);
    ResetSequence(&i_B_sequence, &i_B_seq_default,
                     B_sequence, B_reset_seq_to_default);

    if(i_am_ready)
    {
      SetClearColor();
      UpdateTextureSize();

      i_blend_type = blend_type->get();
      if(i_blend_type == 0) Snap();
      if(i_blend_type >  0) SetupLinearBlend();
      if(i_blend_type == 2) SetupSequenceBlend();
      if(i_blend_type >  0) Blend();
    }
  }
};
