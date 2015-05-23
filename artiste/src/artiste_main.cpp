/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Public License (GPL)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <stdio.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <vsx_argvector.h>
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

#include "artiste_application.h"
#include "vsxfst.h"
#include "vsx_version.h"
#include "vsx_timer.h"
#if PLATFORM == PLATFORM_LINUX
#include <unistd.h>
#endif
#include <stdlib.h>
#include "vsx_platform.h"
#include "vsx_profiler_manager.h"



// implementation of app externals
bool app_ctrl = false;
bool app_alt = false;
bool app_shift = false;
bool dual_monitor = false;


GLFWwindow* mainWindow = NULL;

VSXP_CLASS_DECLARE;


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
  app_ctrl = (bool)glfwGetKey(mainWindow, GLFW_KEY_LEFT_CONTROL);
  app_shift = (bool)glfwGetKey(mainWindow, GLFW_KEY_LEFT_SHIFT);
  app_alt = (bool)glfwGetKey(mainWindow, GLFW_KEY_LEFT_ALT);
}

long key_pressed = -1;
int key_character = -1;
float key_time;
float key_repeat_time;
float initial_key_delay = 0.04f;

void key_char_event( GLFWwindow* window, unsigned int character )
{
  VSX_UNUSED(window);
//  if (action == GLFW_PRESS)
//  {
    app_char(character);
    key_character = character;
//  }
}

void app_close_window()
{
  glfwDestroyWindow(mainWindow);
}

void key_event(GLFWwindow* window, int key, int scancode, int action, int modifier)
{
  VSX_UNUSED(window);
  VSX_UNUSED(scancode);
  VSX_UNUSED(modifier);

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

double last_x = 0, last_y = 0;
int mouse_state = 0;

void mouse_button_event(GLFWwindow* window, int button, int action, int modifiers)
{
  VSX_UNUSED(window);
  VSX_UNUSED(modifiers);

  glfwGetCursorPos(mainWindow, &last_x, &last_y);
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

void mouse_pos_event(GLFWwindow* window, double x, double y)
{
  VSX_UNUSED(window);
  VSX_UNUSED(x);
  VSX_UNUSED(y);
  set_modifiers();
  glfwGetCursorPos(mainWindow, &last_x, &last_y);
  if (mouse_state) mouse_pos_type = 1;
  else mouse_pos_type = 2;
}

int mousewheel_prev_pos = 0;

void mouse_wheel(GLFWwindow* window, double xoffset, double yoffset)
{
  VSX_UNUSED(window);
  VSX_UNUSED(xoffset);
  set_modifiers();
  //int pos = yoffset;
  //app_mousewheel((float)(pos-mousewheel_prev_pos),last_x,last_y);
  app_mousewheel(yoffset,last_x,last_y);
  //mousewheel_prev_pos = pos;
}

void window_size(GLFWwindow* window, int width, int height )
{
  VSX_UNUSED(window);
  vsx_gl_state::get_instance()->viewport_change(0,0,width, height);
}

void myErrorCallback
(
  GLenum _source,
  GLenum _type,
  GLuint _id,
  GLenum _severity,
  GLsizei _length,
  const GLchar* _message,
  GLvoid* _user_param
)
{
  VSX_UNUSED(_source);
  VSX_UNUSED(_type);
  VSX_UNUSED(_severity);
  VSX_UNUSED(_length);
  VSX_UNUSED(_user_param);
  VSX_UNUSED(_id);
  vsx_printf(L"GLDEBUG: %s\n", _message);
}

//========================================================================
// main()
//========================================================================


int main(int argc, char* argv[])
{
  vsx_argvector::get_instance()->init_from_argc_argv(argc, argv);

  if (vsx_argvector::get_instance()->has_param("help"))
  {
    glewInit();
    app_print_cli_help();
    exit(0);
  }

  VSXP_CLASS_CONSTRUCTOR;
  VSXP_CLASS_LOCAL_INIT;

  vsx_printf(L"");

  // Initialise GLFW
  glfwInit();

  int     width, height, frames;
  double  x, y;
  double  t, t1;
  char    titlestr[ 200 ];

  bool start_fullscreen = false;
  bool manual_resolution_set = false;
  int x_res = 1280;
  int y_res = 720;

  if (vsx_argvector::get_instance()->has_param("f"))
  {
    start_fullscreen = true;
  }

  if (vsx_argvector::get_instance()->has_param_with_value("s"))
  {
    vsx_string<>arg2 = vsx_argvector::get_instance()->get_param_value("s");
    vsx_nw_vector< vsx_string<> > parts;
    vsx_string<>deli = ",";
    explode(arg2, deli, parts);
    if (parts.size() == 2)
    {
      x_res = vsx_string_helper::s2i(parts[0]);
      y_res = vsx_string_helper::s2i(parts[1]);
      manual_resolution_set = true;
    } else
    {
      deli = "x";
      explode(arg2, deli, parts);
      if ( parts.size() == 2 )
      {
        x_res = vsx_string_helper::s2i(parts[0]);
        y_res = vsx_string_helper::s2i(parts[1]);
        manual_resolution_set = true;
      }
    }
  }


  if (start_fullscreen && !manual_resolution_set)
  {
    // try to get the resolution from the desktop for fullscreen
    const GLFWvidmode* video_mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
    x_res = video_mode->height;
    y_res = video_mode->width;
  }


  if (vsx_argvector::get_instance()->has_param("gl_debug"))
  {
    vsx_printf(L"enabling GL DEBUG\n");
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT , GL_TRUE );
  }

  // OpenGL version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  mainWindow = glfwCreateWindow( x_res, y_res, "VSXu Artiste", start_fullscreen? glfwGetPrimaryMonitor() : NULL, NULL );
  if( !mainWindow ) // GLFW_FULLSCREEN
  {
    vsx_printf(L"Error! Could not create an OpenGL context. Please check your GPU drivers...\n");
    glfwTerminate();
    return 0;
  }
  glfwMakeContextCurrent(mainWindow);

  glewInit();


  if (start_fullscreen)
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL );

  app_load(0);

  //glfwEnable(GLFW_AUTO_POLL_EVENTS);

  for (int i = 1; i < argc; i++)
  {
    vsx_string<>arg1 = argv[i];
    if (arg1 == "-p") {
      if (i+1 < argc)
      {
        i++;
        vsx_string<>arg2 = argv[i];
        vsx_nw_vector< vsx_string<> > parts;
        vsx_string<>deli = ",";
        explode(arg2, deli, parts);
        glfwSetWindowPos( mainWindow, vsx_string_helper::s2i(parts[0]), vsx_string_helper::s2i(parts[1]) );
      }
    }
  }

  set_modifiers();
  glfwSetCharCallback(mainWindow, &key_char_event);
  glfwSetKeyCallback(mainWindow, &key_event);
  glfwSetMouseButtonCallback(mainWindow, &mouse_button_event);
  glfwSetCursorPosCallback(mainWindow, &mouse_pos_event);
  glfwSetScrollCallback(mainWindow, &mouse_wheel);
  // set window size callback function
  glfwSetWindowSizeCallback(mainWindow, window_size);
  // Enable sticky keys
  glfwSetInputMode(mainWindow, GLFW_STICKY_KEYS, GL_TRUE );


  // vsync handling
  if (vsx_argvector::get_instance()->has_param("novsync"))
    glfwSwapInterval(0);
  else
    glfwSwapInterval(1);

  // Main loop
  frames = 0;

  if (vsx_argvector::get_instance()->has_param("gl_debug"))
  {
    // enable debug callback
    if (__GLEW_ARB_debug_output)
    {
      glDebugMessageCallbackARB( (GLDEBUGPROCARB)myErrorCallback, NULL);
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
  if (vsx_argvector::get_instance()->has_param("gl_vram"))
    display_gpu_vram_stats = 1;


  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  sprintf( titlestr, "Vovoid VSXu Artiste %s [GNU/Linux %d-bit]", vsxu_ver, PLATFORM_BITS);
  #endif
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  sprintf( titlestr, "Vovoid VSXu Artiste %s [Windows %d-bit]", vsxu_ver, PLATFORM_BITS);
  #endif
  glfwSetWindowTitle( mainWindow, titlestr );


  vsx_timer frame_delay;
  int initial_vram_free = 0;
  while( !glfwWindowShouldClose(mainWindow) )
  {
    VSXP_M_BEGIN;

    frame_delay.start();

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

//          GLint total_memory;
//          GLint total_available;
          GLint available_memory;
//          GLint eviction_count;
//          GLint eviction_size;
//          glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &total_memory);
//          glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_available);
//          glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &eviction_count);
//          glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &eviction_size);

          glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &available_memory);
//          float available_memory_f = (float)available_memory;

          if (initial_vram_free == 0) initial_vram_free = available_memory >> 10;

          vsx_printf(L"GPU MEMORY INFO: Before frame: available vram: %d MB\n", available_memory >> 10);
          vsx_printf(L"GPU MEMORY INFO: Probably used vram: %d MB\n", initial_vram_free - (available_memory >> 10));

          //if (gtm)
          //((vsx_tm*)gtm)->plot( available_memory_f, "gpu memory free" );

    }


    app_pre_draw();

    // Get time and mouse position
    t = glfwGetTime();
    glfwGetCursorPos( mainWindow, &x, &y );
    float delta = t-t1;
    t1 = t;
    if (key_pressed != -1)
    {
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
        }
      }
    }

    frames ++;

      // Get window size (may be different than the requested size)
    // TODO: verify if glfwGetFramebufferSize(mainWindow, &width, &height); is more apt here
      glfwGetWindowSize( mainWindow, &width, &height );
      height = height > 0 ? height : 1;

      // Set viewport
      vsx_gl_state::get_instance()->viewport_set( 0, 0, width, height );

      // Clear color buffer
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();	// Reset The Modelview Matrix

    app_draw(0);

    glfwSwapBuffers(mainWindow);
    glfwPollEvents();

//#if (PLATFORM != PLATFORM_WINDOWS)
//    if (!vsync)
//    {
//      float dtime = frame_delay.dtime();

//      if (dtime < 1.0f/60.0f)
//      {
//        float sleeptime = (1.0f / 60.0f - dtime)*1000000.0f;
//        usleep( (useconds_t) sleeptime );
//      }
//    }
//#endif


    VSXP_M_END;
  }

  app_unload();

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}

