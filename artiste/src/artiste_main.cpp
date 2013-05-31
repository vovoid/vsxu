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

#include <stdio.h>
#include "vsx_avector.h"
#include "vsx_string.h"
#include <vsx_argvector.h>
#include <GL/glew.h>
#include "GL/glfw.h"
#include "application.h"
#include "vsxfst.h"
#include "vsx_version.h"
#include "vsx_timer.h"
#if PLATFORM == PLATFORM_LINUX
#include <unistd.h>
#endif
#include <stdlib.h>
#include "vsx_platform.h"




// implementation of app externals
bool app_ctrl = false;
bool app_alt = false;
bool app_shift = false;
bool dual_monitor = false;

vsx_argvector app_argv;
vsx_tm* tm;

vsx_gl_state gl_state;

#include "vsx_tm.h"



/*
void app_char(long key);
void app_key_down(long key);
void app_key_up(long key);

void app_mouse_move_passive(int x, int y);
void app_mouse_move(int x, int y);

// buttons: 0 = left, 1 = middle, 2 = right
void app_mouse_down(unsigned long button,int x,int y);
void app_mouse_up(unsigned long button,int x,int y);

// -1 to -5 or whatever up to +1
void app_mousewheel(float diff,int x,int y);
*/

void set_modifiers()
{
  app_ctrl = (bool)glfwGetKey(GLFW_KEY_LCTRL);
  app_shift = (bool)glfwGetKey(GLFW_KEY_LSHIFT);
  app_alt = (bool)glfwGetKey(GLFW_KEY_LALT);
}

long key_pressed = -1;
int key_character = -1;
float key_time;
float key_repeat_time;
float initial_key_delay = 0.04f;

void GLFWCALL key_char_event( int character, int action )
{
  if (action == GLFW_PRESS)
  {
    app_char(character);
    key_character = character;

  }
}


void GLFWCALL key_event(int key, int action)
{
  set_modifiers();
  if (action == GLFW_PRESS)
  {
    app_key_down((long)key);
    key_character = -1;
    key_time = 0.0f;
    key_repeat_time = 0.0f;
    key_pressed = key;
    initial_key_delay = 0.04f;
  }
  if (action == GLFW_RELEASE)
  {
    app_key_up((long)key);
    key_pressed = -1;
  }
}

int last_x = 0, last_y = 0;
int mouse_state = 0;

void GLFWCALL mouse_button_event(int button, int action)
{
  glfwGetMousePos(&last_x, &last_y);
  set_modifiers();
  unsigned long i_but = 0;
  switch (button)
  {
    case GLFW_MOUSE_BUTTON_MIDDLE: i_but = 1; break;
    case GLFW_MOUSE_BUTTON_RIGHT: i_but = 2;
  }
  if (action == GLFW_PRESS)
  {
    mouse_state = 1; // pressed
    app_mouse_down(i_but, last_x, last_y);
  }
  else
  {
    mouse_state = 0; // depression
    app_mouse_up(i_but, last_x, last_y);
  }
}

int mouse_pos_type = 0;

void GLFWCALL mouse_pos_event(int x, int y)
{
  VSX_UNUSED(x);
  VSX_UNUSED(y);
  set_modifiers();
  glfwGetMousePos(&last_x, &last_y);
  if (mouse_state) mouse_pos_type = 1;
  else mouse_pos_type = 2;
}

int mousewheel_prev_pos = 0;

void GLFWCALL mouse_wheel(int pos)
{
  set_modifiers();
  app_mousewheel((float)(pos-mousewheel_prev_pos),last_x,last_y);
  mousewheel_prev_pos = pos;
}

void GLFWCALL window_size( int width, int height )
{
  vsx_printf("change viewport %d %d\n", width, height);
  gl_state.viewport_change(0,0,width, height);
}

void myErrorCallback
(
  GLenum _source,
  GLenum _type,
  GLuint _id,
  GLenum _severity,
  GLsizei _length,
  const char* _message,
  void* _user_param
)
{
  VSX_UNUSED(_source);
  VSX_UNUSED(_type);
  VSX_UNUSED(_severity);
  VSX_UNUSED(_length);
  VSX_UNUSED(_user_param);
  vsx_printf("GLDEBUG: %s\n", _message);
}

//========================================================================
// main()
//========================================================================


int main(int argc, char* argv[])
{
  tm = new vsx_tm();

  for (size_t i = 0; i < (size_t)argc; i++)
  {
    vsx_string arg = vsx_string(argv[i]);
    app_argv.push_back( arg );
  }

  if (app_argv.has_param("help"))
  {
    app_print_cli_help();
    exit(0);
  }

  // Initialise GLFW
  glfwInit();
  set_modifiers();


  int     width, height, running, frames, x, y;
  double  t, t1;
  unsigned long usleep_framelimit;
  char    titlestr[ 200 ];

  bool start_fullscreen = false;
  bool manual_resolution_set = false;
  int x_res = 1280;
  int y_res = 720;


  if (app_argv.has_param("f"))
  {
    start_fullscreen = true;
  }

  if (app_argv.has_param_with_value("s"))
  {
    vsx_string arg2 = app_argv.get_param_value("s");
    vsx_avector<vsx_string> parts;
    vsx_string deli = ",";
    explode(arg2, deli, parts);
    if (parts.size() == 2)
    {
      x_res = s2i(parts[0]);
      y_res = s2i(parts[1]);
      manual_resolution_set = true;
    } else
    {
      deli = "x";
      explode(arg2, deli, parts);
      if ( parts.size() == 2 )
      {
        x_res = s2i(parts[0]);
        y_res = s2i(parts[1]);
        manual_resolution_set = true;
      }
    }
  }

  if (app_argv.has_param_with_value("frame_limit"))
  {
    vsx_string arg = app_argv.get_param_value("frame_limit");
    usleep_framelimit = s2i(arg);
  }

  if (start_fullscreen && !manual_resolution_set)
  {
    // try to get the resolution from the desktop for fullscreen
    GLFWvidmode video_mode;
    glfwGetDesktopMode(&video_mode);
    x_res = video_mode.Height;
    y_res = video_mode.Width;
  }


  if (app_argv.has_param("gl_debug"))
  {
    printf("enabling GL DEBUG\n");
    glfwOpenWindowHint( GLFW_OPENGL_DEBUG_CONTEXT , GL_TRUE );
  }

  // OpenGL version
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

  if( !glfwOpenWindow( x_res, y_res, 0,0,0,0,16,0, start_fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW ) ) // GLFW_FULLSCREEN
  {
    printf("Error! Could not create an OpenGL context. Please check your GPU drivers...\n");
    glfwTerminate();
    return 0;
  }

  if (start_fullscreen) glfwEnable( GLFW_MOUSE_CURSOR );
  app_init(0);

  glfwEnable(GLFW_AUTO_POLL_EVENTS);

  for (int i = 1; i < argc; i++)
  {
    vsx_string arg1 = argv[i];
    if (arg1 == "-p") {
      if (i+1 < argc)
      {
        i++;
        vsx_string arg2 = argv[i];
        vsx_avector<vsx_string> parts;
        vsx_string deli = ",";
        explode(arg2, deli, parts);
        glfwSetWindowPos(s2i(parts[0]), s2i(parts[1]));
      }
    }
  }

  glfwSetKeyCallback(&key_event);
  glfwSetMouseButtonCallback(&mouse_button_event);
  glfwSetMousePosCallback(&mouse_pos_event);
  glfwSetCharCallback(&key_char_event);
  glfwSetMouseWheelCallback(&mouse_wheel);
  // set window size callback function
  glfwSetWindowSizeCallback(window_size);
  // Enable sticky keys
  glfwEnable( GLFW_STICKY_KEYS );


  // vsync handling
  bool vsync = true;
  if (app_argv.has_param("novsync"))
  {
    vsync = false;
    glfwSwapInterval(0);
  }
  else
  {
    glfwSwapInterval(1);
  }
  // Main loop
  running = GL_TRUE;
  frames = 0;

  if (app_argv.has_param("gl_debug"))
  {
    // enable debug callback
    if (__GLEW_ARB_debug_output)
    {
      glDebugMessageCallbackARB( myErrorCallback, NULL);
      GLuint unusedIds = 0;
      glDebugMessageControlARB(
          GL_DONT_CARE,
          GL_DONT_CARE,
          GL_DONT_CARE,
          0,
          &unusedIds,
          true
      );
    }
  }

  int display_gpu_vram_stats = 0;
  if (app_argv.has_param("gl_vram"))
  {
    display_gpu_vram_stats = 1;
  }


  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  sprintf( titlestr, "Vovoid VSXu Artiste %s [GNU/Linux %d-bit]", vsxu_ver, PLATFORM_BITS);
  #endif
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  sprintf( titlestr, "Vovoid VSXu Artiste %s [Windows %d-bit]", vsxu_ver, PLATFORM_BITS);
  #endif
  glfwSetWindowTitle( titlestr );


  vsx_timer frame_delay;
  int initial_vram_free = 0;
  while( running )
  {
    frame_delay.start();
    tm->z("main");

    if (mouse_pos_type)
    {
      if (mouse_pos_type == 1) app_mouse_move(last_x,last_y);
      else app_mouse_move_passive(last_x,last_y);
      mouse_pos_type = 0;
    }


    if (__GLEW_NVX_gpu_memory_info && display_gpu_vram_stats)
    {
    //      #define GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
    //      #define GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
    //      #define GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
    //      #define GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX 0x904A
    //      #define GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX 0x904B

          GLint total_memory;
          GLint total_available;
          GLint available_memory;
          GLint eviction_count;
          GLint eviction_size;
//          glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &total_memory);
//          glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_available);
//          glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &eviction_count);
//          glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &eviction_size);

          glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &available_memory);
//          float available_memory_f = (float)available_memory;

          if (initial_vram_free == 0) initial_vram_free = available_memory >> 10;

          vsx_printf("GPU MEMORY INFO: Before frame: available vram: %d MB\n", available_memory >> 10);
          vsx_printf("GPU MEMORY INFO: Probably used vram: %d MB\n", initial_vram_free - (available_memory >> 10));

          //if (gtm)
          //((vsx_tm*)gtm)->plot( available_memory_f, "gpu memory free" );

    }


    app_pre_draw();

    // Get time and mouse position
    t = glfwGetTime();
    glfwGetMousePos( &x, &y );
    float delta = t-t1;
    t1 = t;
    if (key_pressed != -1)
    {
      //printf("%f\n", delta);
      key_time += delta;
      if (key_time > 0.3f)
      {
        key_repeat_time += delta;
        if (key_repeat_time > initial_key_delay)
        {
          key_repeat_time = 0.0f;
          if (key_character != -1)
          app_char(key_character);
          app_key_down((long)key_pressed);
          initial_key_delay *= 0.99f;
          //printf("repeating key: %d\n", key_character);
        }
      }
    }

    frames ++;

    tm->e("window_management");
      // Get window size (may be different than the requested size)
      glfwGetWindowSize( &width, &height );
      height = height > 0 ? height : 1;

      tm->e("viewport");
      // Set viewport
      gl_state.viewport_set( 0, 0, width, height );
      tm->l();

      // Clear color buffer
      tm->e("clearcolor");
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      tm->l();
      tm->e("matrices");
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();											// Reset The Modelview Matrix
      tm->l();
    tm->l();

    tm->e("app_draw");
    app_draw(0);
    tm->l();

    tm->e("swapbuffers");
    glfwSwapBuffers();
    tm->l();

    if (!vsync)
    {
      tm->e("frame_zzz", 0x0002);

      float dtime = frame_delay.dtime();

      if (dtime < 1.0f/60.0f)
      {
        float sleeptime = (1.0f / 60.0f - dtime)*1000000.0f;
        usleep( (useconds_t) sleeptime );
      }
      tm->l();
    }

    tm->t();

    // Check if the ESC key was pressed or the window was closed
    running = /*!glfwGetKey( GLFW_KEY_ESC ) &&*/
    glfwGetWindowParam( GLFW_OPENED );
  }

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}

