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
#include <stdlib.h>

#include "application.h"

#include <GL/glew.h>
#include <vsx_gl_state.h>
#include <vsx_argvector.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <filesystem/vsx_filesystem.h>

#include <vsx_version.h>
#include "vsx_platform.h"


// SDL implementation
#include <SDL2/SDL.h>
#include "sdl_tools.h"
#include "sdl_event_handler.h"

// implementation of app externals
bool app_ctrl = false;
bool app_alt = false;
bool app_shift = false;
bool disable_randomizer = false;
bool option_preload_all = false;
bool no_overlay = false;

int app_argc = 0;
char** app_argv;

SDL_Window *window; /* Our window handle */


void set_modifiers()
{
  app_ctrl = sdl_event_handler::get_instance()->get_key(VSX_SCANCODE_LCTRL);
  app_shift = sdl_event_handler::get_instance()->get_key(VSX_SCANCODE_LSHIFT);
  app_alt = sdl_event_handler::get_instance()->get_key(VSX_SCANCODE_LALT);
}

long key_pressed = -1;
int key_character = -1;
float key_time;
float key_repeat_time;
float initial_key_delay = 0.04f;

/*void GLFWCALL key_char_event( int character, int action )
{
  if (action == GLFW_PRESS)
  {
    //printf("got key: %d\n",character);
    app_char(character);
    key_character = character;
  }
}*/


/*void GLFWCALL key_event(int key, int action)
{
  set_modifiers();
  if (action == GLFW_PRESS)
  {
    //printf("got keydown: %d\n",key);
    app_key_down((long)key);
    key_character = -1;
    key_time = 0.0f;
    key_repeat_time = 0.0f;
    key_pressed = key;
    initial_key_delay = 0.04f;
  }
  if (action == GLFW_RELEASE)
  {
    //printf("got key up: %d\n",key);
    app_key_up((long)key);
    key_pressed = -1;
  }
}*/

int last_x = 0, last_y = 0;
int mouse_state = 0;

/*void GLFWCALL mouse_button_event(int button, int action)
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
}*/

int mouse_pos_type = 0;

/*void GLFWCALL mouse_pos_event(int x, int y)
{
  VSX_UNUSED(x);
  VSX_UNUSED(y);
  set_modifiers();
  glfwGetMousePos(&last_x, &last_y);
  if (mouse_state) mouse_pos_type = 1;
  else mouse_pos_type = 2;
}*/

int mousewheel_prev_pos = 0;

/*void GLFWCALL mouse_wheel(int pos)
{
  app_mousewheel((float)(pos-mousewheel_prev_pos),last_x,last_y);
  mousewheel_prev_pos = pos;
}*/


void sdl_vsx_gl_update_viewport_size()
{
  int height = 0, width = 0;
  SDL_GL_GetDrawableSize(window, &width, &height);

  // Get window size (may be different than the requested size)
  height = height > 0 ? height : 1;

  // Set viewport
  vsx_gl_state::get_instance()->viewport_set( 0, 0, width, height );
}

//========================================================================
// main()
//========================================================================

int main(int argc, char* argv[])
{
  vsx_argvector::get_instance()->init_from_argc_argv(argc, argv);

  int     running, frames;

  char    titlestr[ 200 ];
  unsigned int is_borderless = 0;


  bool start_fullscreen = false;
  int x_res = 1280;
  int y_res = 720;
  bool manual_resolution_set = false;
  for (int i = 1; i < argc; i++)
  {
    vsx_string<> arg1 = argv[i];
    if (arg1 == "--help" || arg1 == "/?" || arg1 == "-help" || arg1 == "-?")
    {
      printf(
             "Usage:\n"
          "  vsxu_player [path_to_vsx_file]\n"
          "\n"
          "Flags: \n"
          "  -pl        Preload all visuals on start \n"
          "  -dr        Disable randomizer     \n"
          "  -p [x,y]   Set window position x,y \n"
          "  -s [x,y]   Set window size x,y \n\n\n"
            );
      exit(0);
    }

    if (arg1 == "-f") {
      start_fullscreen = true;
      continue;
    }

    if (arg1 == "-pl") {
      option_preload_all = true;
      continue;
    }

    if (arg1 == "-dr") {
      disable_randomizer = true;
      continue;
    }

    if (arg1 == "-no") {
      no_overlay = true;
      continue;
    }

    if (arg1 == "-s")
    {
      if (!(i+1 < argc))
        continue;

      i++;
      vsx_string<>arg2 = argv[i];
      vsx_nw_vector< vsx_string<> > parts;
      vsx_string<>deli = ",";
      vsx_string_helper::explode(arg2, deli, parts);
      if (parts.size() == 2)
      {
        x_res = vsx_string_helper::s2i(parts[0]);
        y_res = vsx_string_helper::s2i(parts[1]);
        manual_resolution_set = true;
      } else
      {
        deli = "x";
        vsx_string_helper::explode(arg2, deli, parts);
        if ( parts.size() == 2 )
        {
          x_res = vsx_string_helper::s2i(parts[0]);
          y_res = vsx_string_helper::s2i(parts[1]);
          manual_resolution_set = true;
        }
      }
      continue;
    }

    if (arg1 == "-bl")
      is_borderless = 1;
  }

  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    sprintf( titlestr, "Vovoid VSXu Player %s [GNU/Linux %d-bit]", vsxu_ver, PLATFORM_BITS);
  #endif
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    sprintf( titlestr, "Vovoid VSXu Player %s [Windows %d-bit]", vsxu_ver, PLATFORM_BITS);
  #endif

  // make sure we don't get spinning wheel
  #if (PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS)
  DisableProcessWindowsGhosting();
  #endif

  SDL_GLContext context; /* Our opengl context handle */

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
      sdl_tools::sdldie("Unable to initialize SDL"); /* Or die on error */

  SDL_DisableScreenSaver();

  /* Turn on double buffering with a 24bit Z buffer.
   * You may need to change this to 16 or 32 for your system */
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  if (start_fullscreen && !manual_resolution_set)
    window = SDL_CreateWindow(
      titlestr,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      x_res,
      y_res,
      SDL_WINDOW_OPENGL
      | SDL_WINDOW_ALLOW_HIGHDPI
      | SDL_WINDOW_FULLSCREEN_DESKTOP
      | SDL_WINDOW_SHOWN
    );

  if (start_fullscreen && manual_resolution_set)
    window = SDL_CreateWindow(
      titlestr,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      x_res,
      y_res,
      SDL_WINDOW_OPENGL
      | SDL_WINDOW_ALLOW_HIGHDPI
      | SDL_WINDOW_FULLSCREEN
      | SDL_WINDOW_SHOWN
    );

  if (!start_fullscreen)
    window = SDL_CreateWindow(
      titlestr,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      x_res,
      y_res,
      SDL_WINDOW_OPENGL
      | SDL_WINDOW_ALLOW_HIGHDPI
      | SDL_WINDOW_SHOWN
      | SDL_WINDOW_BORDERLESS * is_borderless
    );


  SDL_ShowCursor(0);

  sdl_tools::checkSDLError(__LINE__);

  if (vsx_argvector::get_instance()->has_param("gl_debug"))
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  /* Create our opengl context and attach it to our window */
  context = SDL_GL_CreateContext(window);
  sdl_tools::checkSDLError(__LINE__);



  SDL_GL_MakeCurrent( window,
                      context);

  /* This makes our buffer swap syncronized with the monitor's vertical refresh */
  SDL_GL_SetSwapInterval(1);

  glewInit();


  printf("INFO: app_init\n");
  app_init(0);
  printf("INFO: app_init done\n");


  for (int i = 1; i < argc; i++) {
    vsx_string<>arg1 = argv[i];
    if (arg1 == "-p") {
      if (i+1 < argc)
      {
        i++;
        vsx_string<>arg2 = argv[i];
        vsx_nw_vector< vsx_string<> > parts;
        vsx_string<>deli = ",";
        vsx_string_helper::explode(arg2, deli, parts);
        SDL_SetWindowPosition( window, vsx_string_helper::s2i(parts[0]), vsx_string_helper::s2i(parts[1]) );
      }
    }
  }


  // Main loop
  running = GL_TRUE;
  frames = 0;

  while( running )
  {
    sdl_vsx_gl_update_viewport_size();
    set_modifiers();

    if (mouse_pos_type)
    {
      if (mouse_pos_type == 1)
        app_mouse_move(last_x,last_y);
      else
        app_mouse_move_passive(last_x,last_y);
      mouse_pos_type = 0;
    }

    SDL_Event event;
    /* Check for events */
    while (SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_JOYAXISMOTION:
          if (event.caxis.value == -32768)
            event.caxis.value = -32767;
        break;

        case SDL_WINDOWEVENT:
          switch (event.window.event)
          {
            case SDL_WINDOWEVENT_CLOSE:
              running = false;
            break;
            case SDL_WINDOWEVENT_MAXIMIZED:
            case SDL_WINDOWEVENT_RESIZED:
              sdl_vsx_gl_update_viewport_size();
            break;

            default:
            break;

          }
        break;

        case SDL_KEYUP:
          switch (event.key.keysym.scancode)
          {
            case SDL_SCANCODE_F8:
              break;
            case SDL_SCANCODE_F9:
              break;
            default:
              break;
          }

        case SDL_KEYDOWN:
          switch (event.key.keysym.scancode)
          {
            case SDL_SCANCODE_5:
              break;

            case SDL_SCANCODE_4:
              break;

            default:
            break;
          }
        break;
      }
      sdl_event_handler::get_instance()->consume( event );
    }


    app_pre_draw();

    // Get time and mouse position
//    t = glfwGetTime();
//    glfwGetMousePos( &x, &y );
/*    float delta = t-t1;
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
    }*/
    frames ++;

    // Clear color buffer
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    app_draw(0);

    SDL_GL_SwapWindow(window);
  }

  // Close OpenGL window and terminate
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
