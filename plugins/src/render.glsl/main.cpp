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
#include "vsx_gl_global.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_math_3d.h"
#include "default_shader.h"
#include <map>
#include "vsx_glsl.h"
#include <vsx_platform.h>


vsx_engine_environment* engine_environment = 0;

// stuff for loading pre-defined shaders from the file system
typedef struct {
  vsx_string name;
  vsx_string module_name;
} shader_info;

vsx_avector<shader_info> ext_shaders;

vsx_avector<unsigned long> init_run; // to keep track of the first time the module_info
// is queried, no need to link shaders in that step because the param_spec won't be read anyway.



void load_shader(vsx_glsl &shader, vsx_string filename) {
  //printf("loading shader %s\n",filename.c_str());
  FILE* fp = fopen(filename.c_str(), "r");
  if (fp) {
    vsx_string s;
    vsx_string vert, frag;
    char line[4096];
    int state = 0;
    while (fgets(line,4096,fp))
    {
      if (vsx_string(line).find("*****") != -1) {
        ++state;
        //printf("*****INCREASING STATE\n");
      } else
      {
        if (!state) {
          vert = vert + vsx_string(line);
        } else {
          frag = frag + vsx_string(line);
        }
      }
    }
    shader.vertex_program = vert;
    shader.fragment_program = frag;
    fclose(fp);
  }
}

/*
int explode(vsx_string& input, vsx_string& delimiter, vsx_avector<vsx_string>& results, int max_parts) {
  results.clear();
  if (input == delimiter) {
    results.push_back(input);
    return 1;
  }
  //printf("splitting vsx_string: %s\n",input.c_str());
  vsx_string res;
  int fpos = 0;
  int count = 0;
  char lastchar = 0;
  for (int i = 0; i < input.size(); ++i)
  {
    if (input[i] == delimiter[fpos] && lastchar != '\\') {
      ++fpos;
    } else {
      res.push_back(input[i]);
      fpos = 0;
    }
    if (fpos == delimiter.size() || i == input.size()-1)
    {
      fpos = 0;
      results.push_back(res);
      res = "";
      //res.clear();
      ++count;
    }
    lastchar = input[i];
  }
  if (count == 0) results.push_back(input);
  return count;
}


vsx_string str_replace(vsx_string search, vsx_string replace, vsx_string subject, int max_replacements, int required_pos) {
  //printf("------------\nsubject coming in: %s\n",subject.c_str());
  vsx_string n = subject;
  if (search == "") return n;
  int loc = 1;
  int replacements = 0;
  while ((loc = n.find(search, loc-1)) != -1) {
//      printf("loc: %d\n",loc);
    if (loc <= required_pos || required_pos == -1) {
//      printf("subject changed 0: %s\n",subject.c_str());
      if (replace.size()) {
        n = n.substr(0,loc) + replace + n.substr(loc+search.size());
        loc += replace.size();
      }
      else
      {
        n = n.substr(0,loc) + n.substr(loc+search.size());
        ++loc;
      }
//        printf("subject changed 1: %s\n",n.c_str());

    } else return n;
    if (max_replacements) {
      replacements++;
      if (replacements >= (int)max_replacements) {
        return n;
      }
    }
  }
//  printf("reached end %s\n",n.c_str());
  return n;
}
*/


class vsx_module_glsl : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* reset_params;
  vsx_module_param_string* i_vertex_program;
  vsx_module_param_string* i_fragment_program;
  // out
  vsx_module_param_render* render_result;

  float tax, tay, tbx, tby;
  // internal
  std::map<vsx_string, vsx_module_param_abs*> shader_params;
  bool first;
  vsx_color cm;
  int cc;
public:
  vsx_glsl shader;
  long shader_source; // -1 for default, > 0 for filesystem loading of shaders



vsx_module_glsl() {
  first = true;
  shader_source = 0;
/*shader.vertex_program = "\
void main()\n\
{\n\
  gl_TexCoord[0] = gl_MultiTexCoord0;\n\
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
}\n\
";*/


/*"\
    void main (void)\n\
    {\n\
        vec4 color;\n\
\n\
        color = gl_Color;\n\
\n\
        gl_FragColor = color;\n\
    }\n\
";*/

/*shader.fragment_program = "\
uniform sampler2D texture;\n\
uniform float alpha;\n\
const vec3 coef= vec3(0.2125, 0.7154, 0.0721);\n\
void main()\n\
{\n\
  vec4 color = texture2D(texture,gl_TexCoord[0].st);  \n\
  vec4 intensity = vec4(dot(color.rgb,coef));  \n\
  gl_FragColor  =  mix(intensity, color,alpha);\n\
}\n\
";*/


// PHONG CoR
/*  shader.vertex_program = "\
varying vec3 normal, lightDir;\n\
\n\
void main()\n\
{\n\
  lightDir = normalize(vec3(gl_LightSource[0].position));\n\
  normal = normalize(gl_NormalMatrix * gl_Normal);\n\
  gl_FrontColor = gl_Color;\n\
  gl_Position = ftransform();\n\
}\n\
";
  shader.fragment_program = "\n\
varying vec3 normal, lightDir;\n\
uniform vec3 foobar;\n\
\n\
void main()\n\
{\n\
  float intensity;\n\
  vec3 n;\n\
  vec4 color;\n\
  vec3 phongvec;\n\
  float phong;\n\
\n\
  n = normalize(normal);\n\
  intensity = max(dot(lightDir,n),0.0);\n\
\n\
  phongvec = (n * intensity - normalize(lightDir)) * 2.0 + lightDir;\n\
  vec3 extra;\n\
  phong = max(dot(phongvec,vec3(0.0, 0.0, 1.0)) * 30.0 - 29.0, 0.0);\n\
\n\
  color = max(vec4(phong, phong, phong, gl_Color.a), vec4(0.0, 0.0, 0.0, 0.0)) +\n\
          vec4(gl_Color.rgb * intensity, gl_Color.a);\n\
\n\
  gl_FragColor = vec4(foobar,1.0);\n\
//color;\n\
}\n\
";*/

}

void module_info(vsx_module_info* info)
{
  if (shader_source == 0)
  {
    info->identifier = "renderers;shaders;glsl_loader";
  } else {
    //info->identifier = "renderers;shaders;glsl_loader2";
    info->identifier = "renderers;shaders;"+ext_shaders[shader_source-1].module_name;
  }

  //if (init_run[shader_source] == 0) {
//		init_run[shader_source] = 1;
//	} else
//	{
    //if (shader_source == 0)
    //{
//			shader.vertex_program = default_vert;
      //shader.fragment_program = default_frag;
    /*} else {
      load_shader(shader,ext_shaders[shader_source-1].name);
    }*/

    //vsx_string h = shader.link();

    //printf("vert = %s\n",shader.vertex_program.c_str());
    //printf("frag = %s\n",shader.fragment_program.c_str());

  //if (override_name == "") override_name = "glsl_loader";
  //info->identifier = "renderers;shaders;"+override_name;
  //if (first)
  //info->in_param_spec = "render_in:render,reset_params:enum?no|yes,vertex_program:string,fragment_program:string";
  //else
    info->in_param_spec = "render_in:render,vertex_program:string,fragment_program:string" + shader.get_param_spec();
    info->out_param_spec = "render_out:render";
  //}
  //printf("inparamspec: %s\n",info->in_param_spec.c_str());
  info->component_class = "render";
  info->tunnel = false;
}



void redeclare_in_params(vsx_module_param_list& in_parameters) {
  //if (init_run[shader_source] == 0) {
//		init_run[shader_source] = 1;
//	} else
//	{
  render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  render_in->set(0);
  i_fragment_program = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"fragment_program");
  i_fragment_program->set(shader.fragment_program);
  i_vertex_program = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"vertex_program");
  i_vertex_program->set(shader.vertex_program.c_str());
    //printf("setting shader's vertex program to: %s\n", shader.vertex_program.c_str());

  shader.declare_params(in_parameters);
}

void param_set_notify(const vsx_string& name)
{
  if ((name == "vertex_program" || name == "fragment_program")) {
    //printf("PARAM_SET_NOTIFY %s\n\n",name.c_str());
    shader.vertex_program = i_vertex_program->get();
    shader.fragment_program = i_fragment_program->get();
    message = shader.link();//"module||"+shader.get_log();
    if (message.size() == 0)
    {
      redeclare_in = true;
      message = "module||ok";
    }
  }
  /*if (name == "reset_params") {
    if (reset_params->get()) {
      message = "module||running";
      printf("foobar\n");
      redeclare_in = true;
      first = false;
    }
  }*/
  //printf("psn: %s\n",name.c_str());
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  if (shader_source == 0)
  {
#if (VSXU_DEBUG)
    printf("SETTING DEFAULT PROGRAMS\n");
#endif
    shader.vertex_program = default_vert;
    shader.fragment_program = default_frag;
  } else {
    load_shader(shader,ext_shaders[shader_source-1].name);
  }
  printf("vert = %s\n",shader.vertex_program.c_str());
  printf("frag = %s\n",shader.fragment_program.c_str());

  vsx_string h = shader.link();
#if (VSXU_DEBUG)
  printf("link result:\n%s\n",h.c_str());
#endif
//	}
  //printf("redeclare params\n");
  //printf("%d\n",__LINE__);

  //printf("shader messages: %s\n",shader.link().c_str());
  //printf("%d\n",__LINE__);
  loading_done = true;
  redeclare_in_params(in_parameters);
  //reset_params = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"reset_params");
  render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  //printf("%d\n",__LINE__);
  render_result->set(0);
  //printf("%d\n",__LINE__);

}

bool activate_offscreen() {
  //printf("GLSL::activate offscreen %f\n",engine->vtime);

#if PLATFORM == PLATFORM_LINUX || PLATFORM == PLATFORM_WINDOWS
  shader.begin();
  shader.set_uniforms();
  //printf("uniforms setting done\n");
#else
  shader.set_uniforms();
  //printf("uniforms setting done\n");
  shader.begin();
#endif
  return true;
}

void run() {
/*  if (reset_params->get() == 1) {
    printf("foobar\n");
    redeclare_in = true;
    first = false;
  }*/
  //printf("GLSL RUN\n");
    render_result->set(0);
}

void deactivate_offscreen() {
  //printf("GLSL::deactivate offscreen %f\n",engine->vtime);
  shader.end();
}

bool init() {
  #ifdef VSXU_OPENGL_ES_2_0
  if (!(GL_VERTEX_SHADER)) {
  #else
  if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)) {
  #endif
#ifdef _WIN32
    if (MessageBox(0,"Error, no GLSL present. Do you still want to run?","Fatal error!",MB_YESNO|MB_ICONQUESTION)) {

      loading_done = true;
      return true;
    } else
#else
#endif
    return false;
  }
  return true;
}

};



//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
__declspec(dllexport) void set_environment_info(vsx_engine_environment* environment);
}


vsx_module* MOD_CM(unsigned long module) {
  if (module) {
    vsx_module* v = (vsx_module*)(new vsx_module_glsl());
    ((vsx_module_glsl*)v)->shader_source = (long)module;
//		load_shader(((vsx_module_glsl*)v)->shader,"_plugins/render.glsl/blenders/test.glsl");
    return v;
  } else
  return (vsx_module*)(new vsx_module_glsl()); // module 0

  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  delete (vsx_module_glsl*)m;
}



unsigned long MOD_NM() {
  #ifndef VSXU_OPENGL_ES
  glewInit();
  #endif
  // run once when vsxu starts
  init_run.push_back(0);
  std::list<vsx_string> i_shaders;
  vsx_string base_path;
  if (engine_environment)
  {
    base_path = engine_environment->engine_parameter[0];
#if (VSXU_DEBUG)
    printf("------%s-------\n",(base_path+"render.glsl").c_str());
#endif
    get_files_recursive(base_path+"render.glsl",&i_shaders,".glsl",".svn");
  }
  else
  {
    return 1;
  }
#if (VSXU_DEBUG)
  printf("list size: %lu\n",i_shaders.size());
#endif

  unsigned long num_shaders = 0;
  for (std::list<vsx_string>::iterator it = i_shaders.begin(); it != i_shaders.end(); ++it) {
    vsx_string filename = *it;
    filename = str_replace(base_path, "", filename);

    shader_info info;
    info.name = *it;
    vsx_avector<vsx_string> parts;

    vsx_string deli = "/";
    explode(filename, deli, parts);
    vsx_avector<vsx_string> name_result;

    if (parts.size() > 1) {
      for (unsigned long i = 1; i < parts.size(); ++i) {
        name_result.push_back(parts[i]);
      }
      vsx_string deli_semi = ";";
      //str_replace(vsx_string search, vsx_string replace, vsx_string subject, int max_replacements = 0, int required_pos = -1);
      info.module_name = str_replace(".glsl","",implode(name_result, deli_semi));
      //printf("*********************** shader: %s\n",(*it).c_str());
      //printf("*********************** shader_name: %s\n",info.module_name.c_str());
      ext_shaders.push_back(info);
      ++num_shaders;
      init_run.push_back(0);
    }
  }
  return 1 + num_shaders;
}

void set_environment_info(vsx_engine_environment* environment)
{
  engine_environment = environment;
}
