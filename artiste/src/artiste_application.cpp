/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vsx_gl_global.h"
#include "pthread.h"
#include "vsx_command.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_timer.h"
#include "vsxfst.h"
#include "vsx_font.h"
#include <vsx_version.h>
#include <vsx_engine.h>
#include <vsx_module_list_factory.h>
#include <vsx_error.h>

#include "log/vsx_log_a.h"

#include <vsx_widget_base.h>

#include "vsx_widget_window.h"
#include "artiste_desktop.h"
#include <vsx_command_client_server.h>
#include "vsx_widget/server/vsx_widget_server.h"
#include "vsx_widget/module_choosers/vsx_widget_module_chooser.h"
#include "vsx_widget/helpers/vsx_widget_object_inspector.h"
#include "vsx_widget/helpers/vsx_widget_preview.h"
#include "logo_intro.h"
//#define NO_INTRO
#include "artiste_application.h"
#include "vsxg.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <time.h>
#endif




// global vars
vsx_string fpsstring = "VSX Ultra "+vsx_string(vsxu_version)+" - 2012 Vovoid";
vsx_engine* vxe = 0x0;

// from the perspective (both for gui/server) from here towards the tcp thread
vsx_command_list system_command_queue;
vsx_command_list internal_cmd_in;
vsx_command_list internal_cmd_out;
vsx_widget_desktop *desktop = 0;
bool prod_fullwindow = false;
bool take_screenshot = false;
bool record_movie = false;
bool *gui_prod_fullwindow;
bool gui_prod_fullwindow_helptext = true;
bool reset_time_measurements = false;
vsx_font myf;

unsigned long frame_counter = 0;
unsigned long delta_frame_counter = 0;
float delta_frame_time = 0.0f;
float delta_fps;
float total_time = 0.0f;

void (*app_set_fullscreen)(int,bool) = 0;
bool (*app_get_fullscreen)(int) = 0;

float global_time;
vsx_timer time2;

void load_desktop_a(vsx_string state_name = "");

// draw-related variables
class vsxu_draw {
public:
  bool first;
  vsx_string current_fps;
  vsx_timer gui_t;
  vsx_timer engine_render_time;
  int frame_count;
  int movie_frame_count;
  float gui_g_time;
  double dt;
  double gui_f_time;
  double gui_fullscreen_fpstimer;
  double max_fps;
  double min_fps;
  double max_render_time;
  double min_render_time;
  vsx_command_s pgv;
  vsx_logo_intro *intro;
  vsxu_draw() :
    first(true),
    frame_count(0),
    gui_g_time(0),
    gui_f_time(0),
    gui_fullscreen_fpstimer(0),
    max_fps(0),
    min_fps(1000000),
    max_render_time(-1),
    min_render_time(1000)
  {}
  ~vsxu_draw() {}

  void draw() {
    if (record_movie) {
      vxe->set_constant_frame_progression(1.0f / 60.0f);
      vxe->time_play();
    }

    if (desktop)
    {
      desktop->vsx_command_process_f();
    }

    if (first) {
      intro = new vsx_logo_intro;
    }
    dt = gui_t.dtime();
    gui_f_time += dt;
    gui_g_time += dt;

    float f_wait;
    bool run_always = false;
    if (desktop)
    {
      if (desktop->global_framerate_limit == -1)
      {
        run_always = true;
      }
      else
      {
        f_wait = 1.0f/desktop->global_framerate_limit;
      }
    }
    else
    {
      f_wait = 1.0f/100.0f;
    }

    if (run_always || gui_f_time > f_wait)
    {
      ++frame_count;

      vsx_widget_time::get_instance()->set_dtime( dt );
      vsx_widget_time::get_instance()->increase_time( dt );

      if (desktop) {
        desktop->frames = frame_count;
      }
      gui_fullscreen_fpstimer += gui_f_time;
      current_fps = f2s(round(1.0f/gui_f_time),2);
      if (gui_fullscreen_fpstimer > 1)
      {
        vsx_string h = fpsstring + " @ "+ current_fps+ "fps";
        gui_fullscreen_fpstimer = 0;
      }

      gui_f_time = 0;
      if (!*gui_prod_fullwindow)
      {
        if (desktop)
        {
          desktop->init_frame();
          desktop->draw();
          desktop->draw_2d();
        }
      }
      if (!dual_monitor)
      {
        vxe->process_message_queue(&internal_cmd_in,&internal_cmd_out,false,false,60.0f);
      }

      if (*gui_prod_fullwindow)
      {

        glDepthMask(GL_TRUE);
        glClearColor(0.0f,0.0f,0.0f,1.0f);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        if (vxe && !dual_monitor) {
          engine_render_time.start();
          vxe->render();
          float frame_time = engine_render_time.dtime();
          glDisable(GL_DEPTH_TEST);

          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          glMatrixMode(GL_MODELVIEW);
          glLoadIdentity();
          glEnable(GL_BLEND);
          ++frame_counter;
          ++delta_frame_counter;

          delta_frame_time+= dt;
          total_time += dt;
          if (delta_frame_counter == 100) {
            delta_fps = 100.0f/delta_frame_time;
            delta_frame_counter = 0;
            delta_frame_time = 0.0f;
          }

          if (gui_prod_fullwindow_helptext)
          {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0,0,0,0.4f);
            glBegin(GL_QUADS);                  // Draw A Quad
              glVertex3f(-1.0f, 1.0f, 0.0f);          // Top Left
              glVertex3f( 1.0f, 1.0f, 0.0f);          // Top Right
              glVertex3f( 1.0f,0.92f, 0.0f);          // Bottom Right
              glVertex3f(-1.0f,0.92f, 0.0f);          // Bottom Left
            glEnd();                      // Done Drawing The Quad
            if (reset_time_measurements)
            {
              max_fps = 0.0f;
              min_fps = 1000.0f;
              max_render_time = 0.0f;
              min_render_time = 1000.0f;
              reset_time_measurements = false;
            }
            if (delta_fps > max_fps) max_fps = delta_fps;
            if (delta_fps < min_fps) min_fps = delta_fps;
            if (frame_time > max_render_time) max_render_time = frame_time;
            if (frame_time < min_render_time) min_render_time = frame_time;
            myf.print(vsx_vector<>(-0.99f,0.92f),"VSXu (c) 2003-2013 Vovoid - Alt+T=toggle this text, Ctrl+Alt+P=screenshot (data dir), Alt+F=performance mode || FrameCounter "+i2s(frame_counter) + "   Elapsed time: "+f2s(total_time)+"   Module Count: "+i2s(vxe->get_num_modules())+"Average FPS "+f2s(frame_counter/total_time),0.025f);
            myf.print
            (
              vsx_vector<>(
                -0.99f,
                0.88f
              ),
              "[Cur/Min/Max] "
              "FPS: ("+f2s(delta_fps)+"/"+f2s(min_fps)+"/"+f2s(max_fps)+") "
              "Frame render time: ("+f2s(frame_time)+"/"+f2s(min_render_time)+"/"+f2s(max_render_time)+") "
              "Ctrl+T to reset"
              ,0.025f
            );
            //"("+f2s(frame_time)+")
          }
        }
        if (desktop && desktop->performance_mode)
        {
          glClear(GL_DEPTH_BUFFER_BIT);
          desktop->init_frame();
          desktop->draw();
          desktop->draw_2d();
        }
      }
      #ifndef NO_INTRO
        intro->draw();
      #endif
      if (!first && !desktop)
      {
        pgv.iterations = -1;
      }
      pgv.process_garbage();
      if (first)
      {
        if (!dual_monitor) {
          vxe->start();
        }
        load_desktop_a();
        first = false;
      }
    } else
    {
      int zz = (int)((f_wait-gui_f_time)*1000.0f);
      if (zz < 0) zz = 0;
      //printf("zz%d ",zz);
      //Sleep(zz);
    }
    //------------------------------------------------------------------
    // movie recording
    //------------------------------------------------------------------
    if (record_movie)
    {
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      char* pixeldata = (char*)malloc( viewport[2] * viewport[3] * 3 );
      char* pixeldata_flipped = (char*)malloc( viewport[2] * viewport[3] * 3 );
      take_screenshot = false;
      glReadPixels(0,0,viewport[2],viewport[3],GL_RGB,GL_UNSIGNED_BYTE, (GLvoid*)pixeldata);
      int x3 = viewport[2]*3;
      int hi = viewport[3];
      for (int y = 0; y < hi; y++)
      {
        for (int x = 0; x < x3; x++)
        {
          pixeldata_flipped[y*x3+x] = pixeldata[ (hi-y)*x3+x];
        }
      }

      CJPEGTest jpeg;
      jpeg.m_nResX = viewport[2];
      jpeg.m_nResY = viewport[3];
      jpeg.m_pBuf = (unsigned char*)pixeldata_flipped;

      //char filename[32768];
      #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
      if (access((vsx_get_data_path()+"videos").c_str(),0) != 0) mkdir((vsx_get_data_path()+"/videos").c_str(),0700);
        //sprintf(filename, "%sscreenshots/%d_%d_%d_rgb.jpg",vsx_get_data_path().c_str(),(int)time(0),viewport[2],viewport[3]);
      #endif
      vsx_string err;
      char mfilename[32];
      sprintf(mfilename, "%05d", movie_frame_count);

      jpeg.SaveJPEG( vsx_get_data_path()+"videos"+DIRECTORY_SEPARATOR+vsx_string(mfilename)+"_"+ i2s(viewport[2]) + "_" + i2s(viewport[3])+".jpg", err, 100 );
      jpeg.m_pBuf = 0;
      free(pixeldata);
      free(pixeldata_flipped);
      movie_frame_count++;
    }

    //------------------------------------------------------------------
    // screenshots
    //------------------------------------------------------------------

    if (take_screenshot)
    {
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      char* pixeldata = (char*)malloc( viewport[2] * viewport[3] * 3 );
      char* pixeldata_flipped = (char*)malloc( viewport[2] * viewport[3] * 3 );
      take_screenshot = false;
      glReadPixels(0,0,viewport[2],viewport[3],GL_RGB,GL_UNSIGNED_BYTE, (GLvoid*)pixeldata);

      int x3 = viewport[2]*3;
      int hi = viewport[3];
      for (int y = 0; y < hi; y++)
      {
        for (int x = 0; x < x3; x++)
        {
          pixeldata_flipped[y*x3+x] = pixeldata[ (hi-y)*x3+x];
        }
      }

      CJPEGTest jpeg;
      jpeg.m_nResX = viewport[2];
      jpeg.m_nResY = viewport[3];
      jpeg.m_pBuf = (unsigned char*)pixeldata_flipped;

      //char filename[32768];
      #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
      if (access((vsx_get_data_path()+"screenshots").c_str(),0) != 0) mkdir((vsx_get_data_path()+"/screenshots").c_str(),0700);
        //sprintf(filename, "%sscreenshots/%d_%d_%d_rgb.jpg",vsx_get_data_path().c_str(),(int)time(0),viewport[2],viewport[3]);
      #endif
      /*FILE* fp = fopen(filename,"wb");
      fwrite(pixeldata_flipped, 1, viewport[2] * viewport[3] * 3, fp);
      fclose(fp);*/
      vsx_string err;
      jpeg.SaveJPEG( vsx_get_data_path()+"screenshots"+DIRECTORY_SEPARATOR+i2s(time(0))+"_"+ i2s(viewport[2]) + "_" + i2s(viewport[3])+".jpg", err, 100 );
      jpeg.m_pBuf = 0;
      free(pixeldata);
      free(pixeldata_flipped);
    }
  } // ::draw()
};

vsxu_draw* my_draw = 0x0;

void load_desktop_a(vsx_string state_name)
{
  //printf("{CLIENT} creating desktop:");
  desktop = new vsx_widget_desktop;
  //printf(" [DONE]\n");
  internal_cmd_in.clear();
  internal_cmd_out.clear();
  // connect server widget to command lists
  ((vsx_widget_server*)desktop->find("desktop_local"))->cmd_in = &internal_cmd_out;
  ((vsx_widget_server*)desktop->find("desktop_local"))->cmd_out = &internal_cmd_in;
  if (state_name != "") ((vsx_widget_server*)desktop->find("desktop_local"))->state_name = str_replace("/",";",str_replace("//",";",str_replace("_states/","",state_name)));
  internal_cmd_out.add_raw(vsx_string("vsxu_welcome ")+vsx_string(vsxu_ver)+" 0");
  desktop->system_command_queue = &system_command_queue;
  vsx_widget* t_viewer = desktop->find("vsxu_preview");
  if (t_viewer)
  {
    gui_prod_fullwindow = ((vsx_window_texture_viewer*)desktop->find("vsxu_preview"))->get_fullwindow_ptr();
    LOG_A("found vsxu_preview widget")
  }
  if (!dual_monitor)
  ((vsx_widget_server*)desktop->find("desktop_local"))->engine = (void*)vxe;
  desktop->front(desktop->find("vsxu_preview"));

  //printf("{CLIENT} starting desktop:");
  desktop->init();
  //printf(" [DONE]\n");
}


void app_init(int id)
{
  if (dual_monitor && id == 0)
    return;


  vxe = new vsx_engine();

  my_draw = new vsxu_draw();

  gui_prod_fullwindow = &prod_fullwindow;
  //---------------------------------------------------------------------------
  vsxf filesystem;
  myf.init( vsx_string(PLATFORM_SHARED_FILES) + "font/font-ascii_output.png", &filesystem);

  if (dual_monitor) {
    vxe->start();
  }
}

/*
 print out help texts
*/
void app_print_cli_help()
{
  printf(
         "VSXu Artiste command syntax:\n"
         "  -f             fullscreen mode\n"
         "  -s 1920,1080   screen/window size\n"
         "  -p 100,100     window posision\n"
         "\n"
        );

  vsx_module_list_factory_create()->print_help();
}


void app_pre_draw() {
  //printf("app_pre_draw\n");

  vsx_command_s *c = 0;
  while ( (c = system_command_queue.pop()) )
  {
    //printf("c->cmd: %s\n",c->cmd.c_str());
    vsx_string cmd = c->cmd;
    vsx_string cmd_data = c->cmd_data;
    if (cmd == "system.shutdown") {
      vxe->stop();
      delete vxe;
      desktop->stop();
      delete desktop;
      exit(0);
    } else
    if (cmd == "fullscreen_toggle") {
    } else
    if (cmd == "fullscreen") {
      if (desktop)
      desktop->stop();
      internal_cmd_in.addc(c);
    }
    c = 0;
  }
}

// id is 0 for first monitor, 1 for the next etc.

bool app_draw(int id)
{
  if (id == 0) {
    if (!my_draw)
      ERROR_RETURN_V("my draw is 0x0", false);
    my_draw->draw();
  } else
  {
    if (dual_monitor) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
      vxe->process_message_queue(&internal_cmd_in,&internal_cmd_out);
      vxe->render();
      glDisable(GL_DEPTH_TEST);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();											// Reset The Modelview Matrix
      glEnable(GL_BLEND);
      ++frame_counter;
      ++delta_frame_counter;
      float dt = time2.dtime();
      delta_frame_time+= dt;
      total_time += dt;
      if (delta_frame_counter == 100) {
        delta_fps = 100.0f/delta_frame_time;
        delta_frame_counter = 0;
        delta_frame_time = 0.0f;
      }
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(0,0,0,0.4f);
      glBegin(GL_QUADS);									// Draw A Quad
        glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
        glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
        glVertex3f( 1.0f,0.92f, 0.0f);					// Bottom Right
        glVertex3f(-1.0f,0.92f, 0.0f);					// Bottom Left
      glEnd();											// Done Drawing The Quad
      myf.print(vsx_vector<>(-1.0f,0.92f)," Fc "+i2s(frame_counter)+" Fps "+f2s(delta_fps)+" T "+f2s(total_time)+" Tfps "+f2s(frame_counter/total_time)+" MC "+i2s(vxe->get_num_modules())+" VSX Ultra (c) Vovoid",0.07);
    }
  }
  return true;
}

void app_char(long key)
{
  #ifdef VSXU_DEBUG
  printf("char code: %d\n", key);
  printf("alt: %d\n", (int)app_alt);
  printf("ctrl: %d\n", (int)app_ctrl);
  printf("shift: %d\n", (int)app_shift);
  printf("\n\n");
  #endif

  if (desktop) {
    if
      (
        !( key == 102 && (app_alt || app_ctrl) )
        &&
        !desktop->performance_mode
        &&
        *gui_prod_fullwindow
      )
    {
      vsx_engine_input_event eie;
      eie.type = VSX_ENGINE_INPUT_EVENT_KEY_DOWN;
      eie.key = key;
      eie.ctrl = app_ctrl;
      eie.alt = app_alt;
      eie.shift = app_shift;
      if (vxe)
      {
        vxe->input_event(eie);
      }

      //printf("would send char to engine %d\n", key);
      return;
    }
    desktop->key_down(key,app_alt, app_ctrl, app_shift);
  }
}

void app_key_down(long key)
{
  #ifdef VSXU_DEBUG
    printf("key code: %d\n", key);
    printf("alt: %d\n", (int)app_alt);
    printf("ctrl: %d\n", (int)app_ctrl);
    printf("shift: %d\n", (int)app_shift);
    printf("\n\n");
  #endif
  if (desktop) {
    if (app_alt && app_ctrl && app_shift && key == 80) {
      if (record_movie == false)
      {
        my_draw->movie_frame_count = 0;
      }
      record_movie = !record_movie;
    }
    else
    if (app_alt && app_ctrl && key == 80) take_screenshot = true;

    if
      (
        !( key == 'F' && (app_alt || app_ctrl) )
        &&
        !( key == 'T' && (app_alt || app_ctrl) )
        &&
        !desktop->performance_mode
        &&
        *gui_prod_fullwindow
      )
    {
      vsx_engine_input_event eie;
      eie.type = VSX_ENGINE_INPUT_EVENT_KEY_DOWN;
      eie.key = -key;
      eie.ctrl = app_ctrl;
      eie.alt = app_alt;
      eie.shift = app_shift;
      if (vxe)
      {
        vxe->input_event(eie);
      }
      //printf("would send key to engine %d\n", key);
      return;
    }

    if (*gui_prod_fullwindow && app_alt && !app_ctrl && !app_shift && key == 'T') gui_prod_fullwindow_helptext = !gui_prod_fullwindow_helptext;
    if (*gui_prod_fullwindow && !app_alt && app_ctrl && !app_shift && key == 'T') reset_time_measurements = true;
    desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
    desktop->key_down(-key, app_alt, app_ctrl, app_shift);
  }
}

void app_key_up(long key)
{
  //if (desktop)
  if (desktop) {
    if (
      !desktop->performance_mode
      &&
      *gui_prod_fullwindow
    )
    {
      vsx_engine_input_event eie;
      eie.type = VSX_ENGINE_INPUT_EVENT_KEY_UP;
      eie.key = key;
      eie.ctrl = app_ctrl;
      eie.alt = app_alt;
      eie.shift = app_shift;
      if (vxe)
      {
        vxe->input_event(eie);
      }

      //printf("would send key up to engine %d\n", key);
      return;
    }
    //printf("key up %d\n",key);
    desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
    desktop->key_up(key,app_alt, app_ctrl, app_shift);
  }
  //printf("key up: %d\n",key);
}

void app_mouse_move_passive(int x, int y) {
  if (desktop) {
    if (
      !desktop->performance_mode
      &&
      *gui_prod_fullwindow
    )
    {
      //glfwDisable(GLFW_MOUSE_CURSOR);

      vsx_engine_input_event eie;
      eie.type = VSX_ENGINE_INPUT_EVENT_MOUSE_HOVER;
      eie.x = (float)x;
      eie.y = (float)y;
      eie.ctrl = app_ctrl;
      eie.alt = app_alt;
      eie.shift = app_shift;
      if (vxe)
      {
        vxe->input_event(eie);
      }

      //printf("would send mouse passive move to engine\n");
      return;
    } else
    {
      //glfwEnable(GLFW_MOUSE_CURSOR);
    }

    desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
    desktop->mouse_move_passive((float)x,(float)y);
  }
  //printf("mouse passive pos: %d :: %d\n",x,y);
}

void app_mouse_move(int x, int y)
{
  GLint	viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  int xx = x;
  int yy = y;
  if (xx < 0) xx = 0;
  if (yy < 0) yy = 0;
  if (xx > viewport[2]) xx = viewport[2]-1;
  if (yy > viewport[3]) yy = viewport[3]-1;
  if (desktop) {

    if (
      !desktop->performance_mode
      &&
      *gui_prod_fullwindow
    )
    {
      vsx_engine_input_event eie;
      eie.type = VSX_ENGINE_INPUT_EVENT_MOUSE_MOVE;
      eie.x = (float)x;
      eie.y = (float)y;
      eie.ctrl = app_ctrl;
      eie.alt = app_alt;
      eie.shift = app_shift;
      if (vxe)
      {
        vxe->input_event(eie);
      }
      return;
    }

    desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
    desktop->mouse_move(xx,yy);
  }
}

void app_mouse_down(unsigned long button,int x,int y)
{
  if (desktop) {
    if (
      !desktop->performance_mode
      &&
      *gui_prod_fullwindow
    )
    {
      vsx_engine_input_event eie;
      eie.type = VSX_ENGINE_INPUT_EVENT_MOUSE_DOWN;
      eie.key = button;
      eie.x = (float)x;
      eie.y = (float)y;
      eie.ctrl = app_ctrl;
      eie.alt = app_alt;
      eie.shift = app_shift;
      if (vxe)
      {
        vxe->input_event(eie);
      }

      //printf("would send mouse button down to engine \n");
      return;
    }

    desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
    desktop->mouse_down(x,y,button);
  }
}

void app_mouse_up(unsigned long button,int x,int y)
{
  if (desktop) {
    if (
      !desktop->performance_mode
      &&
      *gui_prod_fullwindow
    )
    {
      vsx_engine_input_event eie;
      eie.type = VSX_ENGINE_INPUT_EVENT_MOUSE_UP;
      eie.key = button;
      eie.x = (float)x;
      eie.y = (float)y;
      eie.ctrl = app_ctrl;
      eie.alt = app_alt;
      eie.shift = app_shift;
      if (vxe)
      {
        vxe->input_event(eie);
      }

      //printf("would send mouse button up to engine \n");
      return;
    }
    desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
    desktop->mouse_up(x,y,button);
  }
}

void app_mousewheel(float diff,int x,int y)
{
  if (desktop)
  {
    if (
      !desktop->performance_mode
      &&
      *gui_prod_fullwindow
    )
    {
      vsx_engine_input_event eie;
      eie.type = VSX_ENGINE_INPUT_EVENT_MOUSE_WHEEL;
      eie.x = (float)x;
      eie.y = (float)y;
      eie.w = diff;
      eie.ctrl = app_ctrl;
      eie.alt = app_alt;
      eie.shift = app_shift;
      if (vxe)
      {
        vxe->input_event(eie);
      }

      //printf("would send mouse wheel to engine \n");
      return;
    }
    desktop->mouse_wheel(diff);
  }
}

