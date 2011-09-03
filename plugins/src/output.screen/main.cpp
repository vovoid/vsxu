#include "_configuration.h"
//#include <string>
#include "vsx_param.h"
#include "vsx_module.h"
//#include "vsx_timer.h"
#include "main.h"
//#include "vsx_math_3d.h"
#ifdef _WIN32
#include "windows.h"
#include "wingdi.h"
#endif

#ifdef VSXU_MAC_XCODE
#include <syslog.h>
#endif

typedef struct {
  unsigned short r[256];
  unsigned short g[256];
  unsigned short b[256];
} gamma_s;


class vsx_module_output_screen : public vsx_module {
  float internal_gamma;
	
  vsx_module_param_render* my_render;
  vsx_module_param_float* gamma_correction;
	vsx_module_param_float4* clear_color;
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat emission[4];
  GLfloat spec_exp;
	
  GLfloat default_ambient[4];
  GLfloat default_diffuse[4];
  GLfloat default_specular[4];
  GLfloat default_emission[4];
  GLfloat default_spec_exp;
  
public:
	

void module_info(vsx_module_info* info)
{
  info->identifier = "outputs;screen";
  info->output = 1;
  info->description = "The screen is the visual output of VSXu.\n\
Components that are not in a chain connected\n\
in the end to the server will not be run.\n\
";
  info->in_param_spec = "screen:render,\
  gamma_correction:float?max=4&min=0&nc=1,clear_color:float4";
  info->component_class = "screen";
}

	
void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  //printf("vsx_module_screen_pbuffer::declare_params\n");
	my_render = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER, "screen");
//	primary_offscreen = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "offscreen");
//  primary_offscreen->set(0);
  
  /*window_sx = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "window_sx");
  window_sy = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "window_sy");
  window_ex = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "window_ex");
  window_ey = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "window_ey");*/
  
//	res_x = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "screen_width");
//	res_y = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "screen_height");
//	res_x->set(1024);
//	res_y->set(768);
//	res_x_old = 0;
//	res_y_old = 0;

	//reset_time = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reset_time");
	//reset_time->set(1);

  gamma_correction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"gamma_correction");
  gamma_correction->set(1.0f);
  internal_gamma = 1.0f;

  clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"clear_color");
  clear_color->set(0.0f,0);
  clear_color->set(0.0f,1);
  clear_color->set(0.0f,2);
  clear_color->set(1.0f,3);
  
  ambient[0] = default_ambient[0] = 0.2f;
  ambient[1] = default_ambient[1] = 0.2f;
  ambient[2] = default_ambient[2] = 0.2f;
  ambient[3] = default_ambient[3] = 1.0f;
  
  diffuse[0] = default_diffuse[0] = 0.8f;
  diffuse[1] = default_diffuse[1] = 0.8f;
  diffuse[2] = default_diffuse[2] = 0.8f;
  diffuse[3] = default_diffuse[3] = 1.0f;
  
  specular[0] = default_specular[0] = 0.0f;
  specular[1] = default_specular[1] = 0.0f;
  specular[2] = default_specular[2] = 0.0f;
  specular[3] = default_specular[3] = 1.0f;

  emission[0] = default_emission[0] = 0.0f;
  emission[1] = default_emission[1] = 0.0f;
  emission[2] = default_emission[2] = 0.0f;
  emission[3] = default_emission[3] = 1.0f;
  spec_exp = default_spec_exp = 0.0f;
  

  
  
	//time_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"time_multiplier");
	//time_multiplier->set(1);
	//m_timer.start();
	
  //d_time = 0;
  //v_time = 0;

	
//	pbuffer = 0;
//''	texture_result = (vsx_module_param_texture*)out_parameters.create("texture","offscreen_result");
//''	texture = new vsx_texture;

//''	texture_result->set(*texture);
}	

void set_gamma(float mgamma) {
#ifdef _WIN32
   HDC hdc = wglGetCurrentDC();
    gamma_s mygamma;
    gamma_s* mgp = &mygamma;
    
    for (int i = 0; i < 256; ++i) {
      int v = (int)round((double)255 * pow(((double)i)/(double)255, mgamma));
      if (v > 255) v = 255;
      // You can adjust each curve separately, but you usually shouldn't need to.
      mygamma.r[i] = v << 8;
      mygamma.g[i] = v << 8;
      mygamma.b[i] = v << 8;
    }  
    SetDeviceGammaRamp(hdc, (void*)mgp);
#endif
 }
 
 
 
 
 
 
 
 
 
 

    



 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

bool activate_offscreen() {
  if (internal_gamma != gamma_correction->get()) {
    internal_gamma = gamma_correction->get();
    set_gamma(internal_gamma);
  }  
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  //printf("viewport x: %d viewport y %d\n", viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();											// Reset The Modelview Matrix
 	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifndef VSXU_OPENGL_ES
	glPolygonMode(GL_FRONT, GL_FILL);
  glPolygonMode(GL_BACK, GL_FILL);
  glDisable(GL_DEPTH_TEST);
#endif
  glLineWidth(1.0f);
  glClearColor(clear_color->get(0),clear_color->get(1),clear_color->get(2),clear_color->get(3));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
  #ifndef VSXU_OPENGL_ES_2_0
    const unsigned int lights[] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};
    glDisable(lights[0]);
    glDisable(lights[1]);
    glDisable(lights[2]);
    glDisable(lights[3]);
    glDisable(lights[4]);
    glDisable(lights[5]);
    glDisable(lights[6]);
    glDisable(lights[7]);
    // default material
    #define ff GL_FRONT_AND_BACK
    glGetMaterialfv(ff,GL_AMBIENT,&ambient[0]);
    glGetMaterialfv(ff,GL_DIFFUSE,&diffuse[0]);
    glGetMaterialfv(ff,GL_SPECULAR,&specular[0]);
    glGetMaterialfv(ff,GL_EMISSION,&emission[0]);
    glGetMaterialfv(ff,GL_SHININESS,&spec_exp);

    glMaterialfv(ff,GL_AMBIENT,default_ambient);
    glMaterialfv(ff,GL_DIFFUSE,default_diffuse);
    glMaterialfv(ff,GL_SPECULAR,default_specular);
    glMaterialfv(ff,GL_EMISSION,default_emission);
    glMaterialfv(ff,GL_SHININESS,&default_spec_exp);
    #undef ff
  #endif

  #ifdef VSXU_OPENGL_ES_2_0
  //glMatrixMode(GL_PROJECTION);
  //gluPerspective(90,1.0f,0.0001f,120.0f);
	#endif
  return true;
}

void deactivate_offscreen() {
  #ifndef VSXU_OPENGL_ES_2_0
    #define ff GL_FRONT_AND_BACK
    glMaterialfv(ff,GL_AMBIENT    ,&ambient[0]);
    glMaterialfv(ff,GL_DIFFUSE    ,&diffuse[0]);
    glMaterialfv(ff,GL_SPECULAR   ,&specular[0]);
    glMaterialfv(ff,GL_EMISSION   ,&emission[0]);
    glMaterialfv(ff,GL_SHININESS  ,&spec_exp);
    #undef ff
  #endif
	glClearColor(0.0f,0.0f,0.0f,0.0f);
}

void stop() {
  set_gamma(1.0f);
}
};




vsx_module* MOD_CM(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_output_screen);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_output_screen*)m; break;
  }
}

unsigned long MOD_NM() {
  return 1;
}

