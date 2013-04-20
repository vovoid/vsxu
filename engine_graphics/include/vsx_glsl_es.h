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

#ifndef VSX_GLSL_ES_H
#define VSX_GLSL_ES_H

#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = glGetError(); \
          if(glError != GL_NO_ERROR) { \
            fprintf(stderr, "glGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__); \
          } \
        }

typedef struct {
  vsx_module_param_abs* module_param;
  vsx_string name;
  vsx_string param_type;
  int param_type_id;
  int glsl_id;
  int glsl_type;
  GLint glsl_location;
} vsx_glsl_type_info;

class vsx_glsl {
  bool linked;

  GLint gl_get_shader_val(GLuint object, GLenum pname) {
    GLint val = 0;
    glGetShaderiv(object, pname, &val);
    return val;
  }

GLint gl_get_program_val(GLuint object, GLenum pname) {
    GLint val = 0;
    glGetProgramiv(object, pname, &val);
    return val;
  }

  vsx_avector<vsx_glsl_type_info> v_list;

  vsx_avector<vsx_glsl_type_info> a_list;

  void process_vars() {
    std::map<vsx_string,vsx_string> vars;
    std::map<vsx_string,vsx_string> attributes;

    int n;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n);
#if (VSXU_DEBUG)
    printf("maximum number of vertex attributes: %d\n", n);
#endif
    // step 1, clean up multiple spaces etc, stop at '{' as that is
    // the beginning of the shader, no uniforms there
    vsx_avector<vsx_string> parts;
    bool run = true;
    vsx_string ts;
    long i = 0;
    int cs = 0; // comment status, 1 = / found, 2 =
    while (run && i < (long)(fragment_program.size() - 1)) {
      if (i < (long)(fragment_program.size() - 1)) {
        if (fragment_program[i] == '/' && fragment_program[i+1] == '/') cs = 1;
        if (fragment_program[i] == '/' && fragment_program[i+1] == '*') cs = 2;
        if (cs == 1) {
          if (fragment_program[i] == 0x0D || fragment_program[i] == 0x0A) {
            cs = 0;
          }
        }
        if (cs == 2) {
          if (fragment_program[i] == '*' && fragment_program[i+1] == '/') {
            cs = 0;
            i+=2;
          }
        }
      }
      if (!cs) {
        // parse on newline
        switch(fragment_program[i]) {
          case '{':
            run = false;
          case ' ':
          case 0x09:
          case 0x0A:
          case 0x0D:
            if (ts.size())
            parts.push_back(ts);
            ts = "";
          break;
          default:
          ts.push_back(fragment_program[i]);
        }
      }
      ++i;
    }
    //for (unsigned int i = 0; i < parts.size(); ++i) {
     // printf("f_part: %s\n",parts[i].c_str());
    //}

    //vsx_avector<vsx_string> parts;
    run = true;
    i = 0;
    cs = 0; // comment status, 1 = / found, 2 =
    while (run && i < (long)(vertex_program.size() - 1)) {
      if (i < (long)(vertex_program.size() - 1)) {
        if (vertex_program[i] == '/' && vertex_program[i+1] == '/') cs = 1;
        if (vertex_program[i] == '/' && vertex_program[i+1] == '*') cs = 2;
        if (cs == 1) {
          if (vertex_program[i] == 0x0D || vertex_program[i] == 0x0A) {
            cs = 0;
          }
        }
        if (cs == 2) {
          if (vertex_program[i] == '*' && vertex_program[i+1] == '/') {
            cs = 0;
            i+=2;
          }
        }
      }
      if (!cs) {
        // parse on newline
        switch(vertex_program[i]) {
          case '{':
            run = false;
          case ' ':
          case 0x09:
          case 0x0A:
          case 0x0D:
            if (ts.size())
              parts.push_back(ts);
            ts = "";
          break;
          default:
          ts.push_back(vertex_program[i]);
        }
      }
      ++i;
    }
    //for (int i = 0; i < parts.size(); ++i) {
     // printf("v_part: %s\n",parts[i].c_str());
    //}




    i = 0;



    vsx_string vtype;
    while (i < (long)parts.size()) {
      if (parts[i] == "attribute"/* || parts[i] == "in"*/) {
        ++i;
        vtype = parts[i];
        vsx_string vp;
        ++i;
        while (parts[i].find(';') == -1) {
          vp = vp + parts[i];
          ++i;
        }
        vp = vp + parts[i];
        vsx_avector<vsx_string> pp;
        vsx_string deli = ',';
        vp = str_replace(";","",vp);
        explode(vp,deli,pp);
        if (pp.size()) {
          for (unsigned long j = 0; j < pp.size(); ++j) {
            attributes[pp[j]] = vtype;
#if (VSXU_DEBUG)
            printf("attribute: %s is of type %s\n",pp[j].c_str(),vtype.c_str());
#endif
          }
        }
      }
      if (parts[i] == "uniform") {
        ++i;
        vtype = parts[i];
        vsx_string vp;
        ++i;
        while (parts[i].find(';') == -1) {
          vp = vp + parts[i];
          ++i;
        }
        vp = vp + parts[i];
        vsx_avector<vsx_string> pp;
        vsx_string deli = ',';
        vp = str_replace(";","",vp);
        explode(vp,deli,pp);
        if (pp.size()) {
          for (unsigned long j = 0; j < pp.size(); ++j) {
            vars[pp[j]] = vtype;
            #if (VSXU_DEBUG)
            printf("uniform: %s is of type %s\n",pp[j].c_str(),vtype.c_str());
            #endif
          }
        }
      }
      ++i;
    }
    int num_uniforms = gl_get_program_val(prog,GL_ACTIVE_UNIFORMS);
    int num_attributes = gl_get_program_val(prog,GL_ACTIVE_ATTRIBUTES);
    #if (VSXU_DEBUG)
    printf("num uniforms (openGL): %d\n", num_uniforms);
    printf("num attributes (openGL): %d\n", num_attributes);
    #endif

    char name[1000];
    int length;
    GLint size;
    GLenum type;

    for (i = 0; i < num_uniforms; ++i) {
      glGetActiveUniform(prog,i,1000,&length,&size,&type,(char*)&name);
      vsx_string sn = name;

      if (vars.find(sn) != vars.end()) {
        vsx_glsl_type_info n_info;
        n_info.name = sn;
        //printf("sn: %s\n",sn.c_str());
        n_info.glsl_id = i;
        n_info.glsl_type = type;
        n_info.glsl_location = glGetUniformLocation(prog,name);
        n_info.module_param = 0;
        switch(type) {
          case GL_BOOL:

          case GL_FLOAT: {
            n_info.param_type = "float";
            n_info.param_type_id = VSX_MODULE_PARAM_ID_FLOAT;
          }
          break;
          case GL_BOOL_VEC2:
          case GL_BOOL_VEC3:
          case GL_FLOAT_VEC2:
          case GL_FLOAT_VEC3:
          case GL_INT_VEC2:
          case GL_INT_VEC3:
            n_info.param_type = "float3";
            n_info.param_type_id = VSX_MODULE_PARAM_ID_FLOAT3;
          break;
          case GL_INT_VEC4:
          case GL_FLOAT_VEC4:
          case GL_BOOL_VEC4:
            n_info.param_type = "float4";
            n_info.param_type_id = VSX_MODULE_PARAM_ID_FLOAT4;
          break;
          case GL_SAMPLER_CUBE:
          case GL_SAMPLER_2D:
            n_info.param_type = "texture";
            n_info.param_type_id = VSX_MODULE_PARAM_ID_TEXTURE;
          break;

          case GL_INT:
            if (vars[sn].find("sampler") != -1) {
              n_info.param_type = "texture";
              n_info.param_type_id = VSX_MODULE_PARAM_ID_TEXTURE;
            } else {
              n_info.param_type = "float";
              n_info.param_type_id = VSX_MODULE_PARAM_ID_FLOAT;
            }
          break;
          case GL_FLOAT_MAT2:
          case GL_FLOAT_MAT3:
          case GL_FLOAT_MAT4:
            n_info.param_type = "matrix";
            n_info.param_type_id = VSX_MODULE_PARAM_ID_MATRIX;
          break;
        }
        v_list.push_back(n_info);

        #if (VSXU_DEBUG)
        printf("GLSL uniform name: %s\n",sn.c_str());
        #endif
      }
    }

    for (i = 0; i < num_attributes; ++i) {
      glGetActiveAttrib(prog,i,1000,&length,&size,&type,(char*)&name);
      vsx_string sn = name;

      if (attributes.find(sn) != attributes.end()) {
        vsx_glsl_type_info n_info;
        n_info.name = sn;
        //printf("sn: %s\n",sn.c_str());
        n_info.glsl_id = i;
        n_info.glsl_type = type;
        n_info.glsl_location = glGetAttribLocation(prog,name);
        n_info.module_param = 0;
        switch(type) {
          case GL_BOOL:
          case GL_FLOAT: {
            n_info.param_type = "float";
            n_info.param_type_id = VSX_MODULE_PARAM_ID_FLOAT_ARRAY;
          }
          break;
          case GL_BOOL_VEC3:
          case GL_FLOAT_VEC3:
          case GL_INT_VEC3:
            n_info.param_type = "float3_array";
            n_info.param_type_id = VSX_MODULE_PARAM_ID_FLOAT3_ARRAY;
          break;
          case GL_BOOL_VEC4:
          case GL_FLOAT_VEC4:
          case GL_INT_VEC4:
#if (VSXU_DEBUG)
            printf("got quat array\n");
#endif
            n_info.param_type = "quaternion_array";
            n_info.param_type_id = VSX_MODULE_PARAM_ID_QUATERNION_ARRAY;
          break;
        }
        a_list.push_back(n_info);

#if (VSXU_DEBUG)
        printf("GLSL attribute name: %s\n",sn.c_str());
#endif
      }
    }
  }


public:
  GLuint vs,
              fs,
              prog;

  vsx_string vertex_program;
  vsx_string fragment_program;

  std::map<vsx_string,vsx_glsl_type_info*> v_map;
  std::map<vsx_string,vsx_glsl_type_info*> a_map;


  void init() {
  }

  bool get_linked()
  {
    return linked;
  }

  vsx_string link() {
    if (linked) {
      printf("clearing linked stuff\n");
      v_list.clear();
      a_list.clear();
      v_map.clear();
      a_map.clear();
      glDeleteShader(vs);
      glDeleteShader(fs);
      glDeleteProgram(prog);
      linked = false;
    }
    /*if (GL_FRAGMENT_SHADER)
    {
      printf("creating shaders\n");
    } else return "module||no shader support";
    */
    vs = glCreateShader(GL_VERTEX_SHADER);
    if (vs == 0) return "module||failed to create shader object 'vs'";
    printf("shader object id is: %d\n", vs);
    const char *vp = vertex_program.c_str();
    if (glIsShader(vs) == GL_FALSE) return "module||vs is not a shader object";
    glShaderSource(vs, 1, &vp, NULL);
    glCompileShader(vs);
    if (GL_FALSE == gl_get_shader_val(vs,GL_COMPILE_STATUS)) {
      glDeleteShader(vs);
      return "module||Vertex program compilation failed.\n\nThe message from OpenGL was:"+get_shader_log(vs);
    }
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fp = fragment_program.c_str();
    glShaderSource(fs, 1, &fp,NULL);
    glCompileShader(fs);
    if (GL_FALSE == gl_get_shader_val(fs,GL_COMPILE_STATUS)) {
      return "module||Fragment program compilation failed.\n\nThe message from OpenGL was:"+get_shader_log(fs);
    }

    prog = glCreateProgram();
    glAttachShader(prog,fs);
    glAttachShader(prog,vs);
    glLinkProgram(prog);
    if (gl_get_program_val(prog,GL_LINK_STATUS) == GL_FALSE) {
      return "module||Linking failed.\n\
The message from OpenGL was:\n"+get_program_log(prog);
    }

    linked = true;
    process_vars();
    return "";
    //return "vertex_program||Compilation successful.&&fragment_program||Compilation successful.";
  }

  vsx_string get_shader_log(GLuint handle = 0) {
    char infolog[2048];
    int length;
    if (handle == 0) handle = prog;
    glGetShaderInfoLog(handle,2048,&length,(char*)(&infolog));
    //vsx_string aa = infolog;
    return vsx_string(infolog);
  }

  vsx_string get_program_log(GLuint handle = 0) {
    char infolog[2048];
    int length;
    if (handle == 0) handle = prog;
    glGetProgramInfoLog(handle,2048,&length,(char*)(&infolog));
    //vsx_string aa = infolog;
    return vsx_string(infolog);
  }

  vsx_string get_param_spec()
  {
    if (!linked) return "";
    vsx_string res = ",uniforms:complex{";
    bool first = true;
    for (int i = v_list.size()-1; i >= 0; --i) {
      if (first) first = false;
      else res = res + ",";
      res = res +v_list[i].name+":"+v_list[i].param_type;
    }
    res = res + "},attributes:complex{";
    first = true;
#if (VSXU_DEBUG)
    printf("a_list size: %d\n", (int)a_list.size() );
#endif
    for (int i = a_list.size()-1; i >= 0; --i) {
      //if (a_list[i].name[0] != '_')
      {
        if (first) first = false;
        else res = res + ",";
        res = res +a_list[i].name+":"+a_list[i].param_type;
      }
    }
    res = res + "}";
    return res;
  }


  void declare_params(vsx_module_param_list& in_parameters) {
    if (!linked) return;

    for (unsigned long i = 0; i < v_list.size(); ++i) {
      v_map[v_list[i].name] = &v_list[i];

      v_list[i].module_param = 0;
      //if (v_list[i].name[0] != '_')
      switch(v_list[i].param_type_id) {
        case VSX_MODULE_PARAM_ID_FLOAT:
          v_list[i].module_param = in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,v_list[i].name.c_str());
        break;
        case VSX_MODULE_PARAM_ID_FLOAT3:
          v_list[i].module_param = in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,v_list[i].name.c_str());
        break;
        case VSX_MODULE_PARAM_ID_FLOAT4:
          v_list[i].module_param = in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,v_list[i].name.c_str());
        break;
        case VSX_MODULE_PARAM_ID_TEXTURE:
          v_list[i].module_param = in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,v_list[i].name.c_str());
        break;
        case VSX_MODULE_PARAM_ID_MATRIX:
          v_list[i].module_param = in_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,v_list[i].name.c_str());
        break;
      }
    }
    for (unsigned long i = 0; i < a_list.size(); ++i) {
      a_list[i].module_param = 0;
      a_map[a_list[i].name] = &a_list[i];
      //if (a_list[i].name[0] != '_')
      switch(a_list[i].param_type_id) {
        case VSX_MODULE_PARAM_ID_FLOAT_ARRAY:
          a_list[i].module_param = in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,a_list[i].name.c_str());
        break;
        case VSX_MODULE_PARAM_ID_FLOAT3_ARRAY:
          a_list[i].module_param = in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY,a_list[i].name.c_str());
        break;
        case VSX_MODULE_PARAM_ID_QUATERNION_ARRAY:
          a_list[i].module_param = in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION_ARRAY,a_list[i].name.c_str());
        break;
      }
    }
  }

  void set_uniforms() {
    int tex_i = 0;
    for (unsigned long i = 0; i < v_list.size(); ++i) {
      //printf("setting uniform %d\n",i);
      if (v_list[i].module_param) {
        switch(v_list[i].param_type_id) {
          case VSX_MODULE_PARAM_ID_FLOAT:
            glUniform1f(v_list[i].glsl_location,((vsx_module_param_float*)v_list[i].module_param)->get());
          break;
          case VSX_MODULE_PARAM_ID_FLOAT3:
          {
            switch (v_list[i].glsl_type)
            {
              case GL_BOOL_VEC2:
                break;
              case GL_FLOAT_VEC2:
                break;
              case GL_INT_VEC2:
                break;
              case GL_FLOAT_VEC3:
                //printf("setting %d uniform %s: %f\n",v_list[i].glsl_location,v_list[i].name.c_str(),((vsx_module_param_float3*)v_list[i].module_param)->get(0));

                glUniform3f(v_list[i].glsl_location,
                    ((vsx_module_param_float3*)v_list[i].module_param)->get(0),
                    ((vsx_module_param_float3*)v_list[i].module_param)->get(1),
                    ((vsx_module_param_float3*)v_list[i].module_param)->get(2)
                );

                //glUniform3fvARB(v_list[i].glsl_location,((vsx_module_param_float3*)v_list[i].module_param)->get_addr());
                break;
              case GL_BOOL_VEC3:
                break;
              case GL_INT_VEC3:
                break;
            }
          }
          break;
          case VSX_MODULE_PARAM_ID_FLOAT4:
            switch (v_list[i].glsl_type)
            {
              case GL_FLOAT_VEC4:
                //printf("setting %d uniform %s: %f\n",v_list[i].glsl_location,v_list[i].name.c_str(),((vsx_module_param_float3*)v_list[i].module_param)->get(0));

                glUniform4f(v_list[i].glsl_location,
                    ((vsx_module_param_float3*)v_list[i].module_param)->get(0),
                    ((vsx_module_param_float3*)v_list[i].module_param)->get(1),
                    ((vsx_module_param_float3*)v_list[i].module_param)->get(2),
                    ((vsx_module_param_float3*)v_list[i].module_param)->get(3)
                );

                //glUniform3fv(v_list[i].glsl_location,((vsx_module_param_float3*)v_list[i].module_param)->get_addr());
                break;
              case GL_BOOL_VEC4:
                break;
              case GL_INT_VEC4:
                break;
            }
          break;
          case VSX_MODULE_PARAM_ID_TEXTURE:
            vsx_texture* ba;
            //glBindTexture(GL_TEXTURE_2D, my_texture_object);
            ba = ((vsx_module_param_texture*)v_list[i].module_param)->get_addr();
            if (ba) {
              //printf("GLSL:binding texture\n");
              //if (GLEW_VERSION_1_3)
#if defined(__linux__)
              glActiveTexture(GL_TEXTURE0 + tex_i);
#else
              glActiveTexture(GL_TEXTURE0 + tex_i);
#endif
              ba->bind();
              glUniform1i(v_list[i].glsl_location,tex_i);
              tex_i++;
            }
          break;
          case VSX_MODULE_PARAM_ID_MATRIX:
            //v_list[i].module_param = in_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,v_list[i].name.c_str());
          break;
        }
      }
    }
    for (unsigned long i = 0; i < a_list.size(); ++i) {
      if (a_list[i].module_param) {
        #ifdef VSXU_DEBUG
              //printf("set attrib param_type_id %d\n",a_list[i].param_type_id);
        #endif
        switch(a_list[i].param_type_id) {
          case VSX_MODULE_PARAM_ID_FLOAT_ARRAY:
          {
            vsx_float_array* p = ((vsx_module_param_float_array*)a_list[i].module_param)->get_addr();
            if (p)
            {
              glVertexAttribPointer(a_list[i].glsl_location,1,GL_FLOAT,GL_FALSE,0, (GLvoid*)(p->data->get_pointer()));
              glEnableVertexAttribArray(a_list[i].glsl_location);
            } else
            glDisableVertexAttribArray(a_list[i].glsl_location);
          }
          break;
          case VSX_MODULE_PARAM_ID_FLOAT3_ARRAY:
          {
            vsx_float3_array* p = ((vsx_module_param_float3_array*)a_list[i].module_param)->get_addr();
            if (p)
            {
              #ifdef VSXU_DEBUG
              //printf("setting float3 attrib in shader\n");
              #endif
              glVertexAttribPointer(a_list[i].glsl_location,3,GL_FLOAT,GL_FALSE,0, (GLvoid*)(p->data->get_pointer()));
              glEnableVertexAttribArray(a_list[i].glsl_location);
            } else
            glDisableVertexAttribArray(a_list[i].glsl_location);
          }
          break;
        }
      }
    }
  }

  void unset_uniforms() {
    int tex_i = 0;
    for (unsigned long i = 0; i < v_list.size(); ++i) {
      if (v_list[i].module_param) {
        switch(v_list[i].param_type_id) {
          case VSX_MODULE_PARAM_ID_TEXTURE:
          vsx_texture* ba;
          //glBindTexture(GL_TEXTURE_2D, my_texture_object);
          ba = ((vsx_module_param_texture*)v_list[i].module_param)->get_addr();
          if (ba) {
          //if (GLEW_VERSION_1_3)
#if defined(__linux__)
            glActiveTexture(GL_TEXTURE0 + tex_i);
#else
            glActiveTextureARB(GL_TEXTURE0 + tex_i);
#endif
            //printf("unbinding tex\n");
            ba->_bind();
            ++tex_i;
          }
        }
      }
    }

    for (unsigned long i = 0; i < a_list.size(); ++i) {
      if (a_list[i].module_param) {
        switch(a_list[i].param_type_id) {
          case VSX_MODULE_PARAM_ID_FLOAT_ARRAY:
          {
            glDisableVertexAttribArray(a_list[i].glsl_location);
          }
          break;
          case VSX_MODULE_PARAM_ID_FLOAT3_ARRAY:
          {
            glDisableVertexAttribArray(a_list[i].glsl_location);
          }
          break;
        }
      }
    }

#if defined(__linux__)
    glActiveTexture(GL_TEXTURE0);
#else
    glActiveTextureARB(GL_TEXTURE0);
#endif

  }

  void begin() {
    if (!linked) return;
    if (linked)
    glUseProgram(prog);
  }

  void stop() {
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteProgram(prog);
    linked = false;
  }

  void end() {
    if (!linked) return;
    unset_uniforms();
    glUseProgram(0);
  }
  vsx_glsl() :
      linked(false),vs(0),
      fs(0),
      prog(0)
  {};
};

#undef GL_CHECK
#endif
