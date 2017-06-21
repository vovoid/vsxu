#pragma once

#include <vsx_platform.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include <SDL.h>
#include <windows.h>
#else
#include <SDL2/SDL.h>
#endif
#include <vsx_application_control.h>
#include <time/vsx_timer.h>
#include <time/vsx_time_manager.h>
#include <filesystem/vsx_filesystem_helper.h>
#include "vsx_application_sdl_tools.h"
#include "vsx_application_sdl_input_event_handler.h"
#include <vsx_gl_state.h>
#include <string/vsx_string_helper.h>
#include "vsx_application_sdl_window_holder.h"
#include <vsx_application_display.h>

void gl_debug_callback
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
  req(_type != 33361 && _type != 33360);

  VSX_UNUSED(_source);
  VSX_UNUSED(_type);
  VSX_UNUSED(_severity);
  VSX_UNUSED(_length);
  VSX_UNUSED(_user_param);
  VSX_UNUSED(_id);
  vsx_printf(L"GLDEBUG: %hs\n", _message);
}


class vsx_application_sdl
{
  SDL_GLContext context;
  vsx_application_sdl_input_event_handler event_handler;
  vsx_timer frame_timer;

  void update_viewport_size()
  {
    int height = 0, width = 0;
    SDL_GL_GetDrawableSize(vsx_application_sdl_window_holder::get_instance()->window, &width, &height);

    // Get window size (may be different than the requested size)
    height = height > 0 ? height : 1;

    // Set viewport
    vsx_gl_state::get_instance()->viewport_set( 0, 0, width, height );
  }

  void setup()
  {
    // on Windows, handle the console window
    #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
      if (!vsx_argvector::get_instance()->has_param("console"))
        ShowWindow( GetConsoleWindow(), SW_HIDE );
    #endif

    // initialization performed before SDL / graphics has been initialized
    vsx_application_manager::get_instance()->get()->init_no_graphics();

    if (vsx_argvector::get_instance()->has_param("-help") || vsx_argvector::get_instance()->has_param("help"))
    {
      vsx_application_manager::get_instance()->get()->print_help();
      exit(0);
    }

    // make sure we don't get spinning wheel
    #if (PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS)
    DisableProcessWindowsGhosting();
    #endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
        sdl_tools::sdldie("Unable to initialize SDL");

    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    vsx_printf(L"VSX Application: \n    %hs\n    Build Date: %hs, %hs\n", vsx_application_manager::get_instance()->get()->window_title_get().c_str(), __DATE__, __TIME__);
    vsx_printf(L"    SDL Version: %d.%d.%d\n", linked.major, linked.minor, linked.patch);

    //Get number of displays
    int num_displays = SDL_GetNumVideoDisplays();
    SDL_Rect* display_bounds = 0x0;
    display_bounds = new SDL_Rect[ num_displays ];
    for( int i = 0; i < num_displays; ++i )
    {
        SDL_GetDisplayBounds( i, &display_bounds[ i ] );
        vsx_application_display::get()->displays.push_back( vsx_application_display::display_info(display_bounds[i].x, display_bounds[i].y, display_bounds[i].w, display_bounds[i].h) );
    }




    size_t chosen_display = 0;
    if (vsx_argvector::get_instance()->has_param("d"))
    {
      chosen_display = vsx_string_helper::s2i(vsx_argvector::get_instance()->get_param_value("d"));
      if (!chosen_display)
        chosen_display = 1;
      chosen_display--;
      if (chosen_display > num_displays - 1)
        chosen_display = num_displays - 1;
    }

    if (vsx_argvector::get_instance()->has_param("dq"))
    {
      vsx_printf(L"Number of displays attached: %d\n", num_displays);
      exit(0);
    }

    SDL_DisableScreenSaver();

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // borderless window, taking up one full desktop
    if (
      vsx_argvector::get_instance()->has_param("d")
      &&
      vsx_argvector::get_instance()->has_param("f")
      &&
      !vsx_argvector::get_instance()->has_param_with_value("s")
      &&
      !vsx_argvector::get_instance()->has_param_with_value("p")
    )
    {
      vsx_printf(L"Reported screen bounds: x: %d, y: %d, width: %d, height: %d", display_bounds[chosen_display].x, display_bounds[chosen_display].y, display_bounds[chosen_display].w, display_bounds[chosen_display].h);
      vsx_application_sdl_window_holder::get_instance()->window = SDL_CreateWindow(
        vsx_application_manager::get_instance()->get()->window_title_get().c_str(),
        display_bounds[chosen_display].x,
        display_bounds[chosen_display].y,
        display_bounds[chosen_display].w,
        display_bounds[chosen_display].h,
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_ALLOW_HIGHDPI
        | SDL_WINDOW_SHOWN
        | SDL_WINDOW_BORDERLESS
      );
    }

    // real fullscreen, main desktop resolution
    if (
      !vsx_argvector::get_instance()->has_param("d")
      &&
      vsx_argvector::get_instance()->has_param("f")
      &&
      !vsx_argvector::get_instance()->has_param_with_value("s")
    )
      vsx_application_sdl_window_holder::get_instance()->window = SDL_CreateWindow(
        vsx_application_manager::get_instance()->get()->window_title_get().c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1,
        1,
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_ALLOW_HIGHDPI
        | SDL_WINDOW_FULLSCREEN_DESKTOP
        | SDL_WINDOW_SHOWN
      );

    // real fullscreen, custom resolution
    if (
      !vsx_argvector::get_instance()->has_param("d")
      &&
      vsx_argvector::get_instance()->has_param("f")
      &&
      vsx_argvector::get_instance()->has_param_with_value("s")
    )
      vsx_application_sdl_window_holder::get_instance()->window = SDL_CreateWindow(
        vsx_application_manager::get_instance()->get()->window_title_get().c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("s", 0, "x,", "1280") ),
        vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("s", 1, "x,", "720") ),
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_ALLOW_HIGHDPI
        | SDL_WINDOW_FULLSCREEN
        | SDL_WINDOW_SHOWN
      );

    // regular window, custom resolution, borderless optional
    if (
      !vsx_argvector::get_instance()->has_param("f")
    )
      vsx_application_sdl_window_holder::get_instance()->window = SDL_CreateWindow(
        vsx_application_manager::get_instance()->get()->window_title_get().c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("s", 0, "x,", "1280") ),
        vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("s", 1, "x,", "720") ),
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_ALLOW_HIGHDPI
        | SDL_WINDOW_SHOWN
        | SDL_WINDOW_BORDERLESS * (vsx_argvector::get_instance()->has_param("bl") ? 1 : 0)
      );




    SDL_StartTextInput();

    sdl_tools::checkSDLError(__LINE__);

    if (vsx_argvector::get_instance()->has_param("gl_debug"))
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    context = SDL_GL_CreateContext(vsx_application_sdl_window_holder::get_instance()->window);
    sdl_tools::checkSDLError(__LINE__);

    SDL_GL_MakeCurrent( vsx_application_sdl_window_holder::get_instance()->window, context );

    vsx_printf(L"    OpenGL Vendor: %hs\n", glGetString(GL_VENDOR));
    vsx_printf(L"    OpenGL Renderer: %hs\n", glGetString(GL_RENDERER));


    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
    SDL_GL_SetSwapInterval(1);

    glewInit();

    if (__GLEW_ARB_debug_output && vsx_argvector::get_instance()->has_param("gl_debug") )
    {
      vsx_printf(L"GLDEBUG: Enabling...\n");
      glDebugMessageCallbackARB( (GLDEBUGPROCARB)gl_debug_callback, NULL);
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

    update_viewport_size();

    vsx_application_manager::get_instance()->get()->init_graphics();

    if (vsx_argvector::get_instance()->has_param_with_value("p"))
    {
      int x_pos = vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("p", 0, "x,", "720") );
      int y_pos = vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_subvalue("p", 1, "x,", "300") );
      if (vsx_argvector::get_instance()->has_param_with_value("d"))
      {
        x_pos += display_bounds[chosen_display].x;
        y_pos += display_bounds[chosen_display].y;
      }

      SDL_SetWindowPosition( vsx_application_sdl_window_holder::get_instance()->window, x_pos, y_pos);
    }
  }

  SDL_GameController *gamecontroller[32] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
                                            0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
                                            0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
                                            0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
                                           };
  void setup_game_input()
  {
    if (SDL_NumJoysticks() == 0)
      return;

    int max_num_joysticks = SDL_NumJoysticks();

    if (max_num_joysticks > 32)
    {
      vsx_printf(L"SDL: Too many joysticks connected - %d, more than 32...\n", (int)SDL_NumJoysticks());
      max_num_joysticks = 32;
    }

    size_t current_controller = 0;
    for (int i = 0; i < max_num_joysticks; ++i)
    {
      if ( SDL_IsGameController(i) )
      {
        gamecontroller[current_controller] = SDL_GameControllerOpen(i);
        current_controller++;
      }
    }
  }


public:

  void run()
  {
    setup();
    setup_game_input();

    size_t frames = 0;

    if (!vsx_application_manager::get())
    {
      vsx_printf(L"Application manager not initialized with application. Exiting...");
      exit(1);
    }

    frame_timer.start();
    bool running = true;
    while( running )
    {
      update_viewport_size();


      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
        switch(event.type)
        {
          case SDL_JOYAXISMOTION:
            if (event.caxis.value == -32768)
              event.caxis.value = -32767;
          break;

          case SDL_CONTROLLERDEVICEADDED:
          case SDL_CONTROLLERDEVICEREMOVED:
            setup_game_input();


          case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
              case SDL_WINDOWEVENT_CLOSE:
                running = false;
              break;
              case SDL_WINDOWEVENT_MAXIMIZED:
              case SDL_WINDOWEVENT_RESIZED:
                update_viewport_size();
              break;

              default:
              break;

            }
          break;
        }
        event_handler.consume( event );
      }


      vsx_application_manager::get()->pre_draw();

      frames++;

      // Clear color buffer
      glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
      glClear( GL_COLOR_BUFFER_BIT );
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      vsx_application_manager::get()->draw();

      SDL_GL_SwapWindow(vsx_application_sdl_window_holder::get_instance()->window);

      if (vsx_application_control::get_instance()->is_preferences_path_creation_requested())
        if (!vsx_application_control::get_instance()->preferences_path.size())
        {
          char *base_path = SDL_GetPrefPath(
                vsx_application_manager::get()->organization_get().c_str(),
                vsx_application_manager::get()->application_name_get().c_str()
              );
          if (base_path) {
            vsx::filesystem_helper::create_directory(base_path);
            vsx_application_control::get_instance()->preferences_path = vsx_string<>(base_path);
          }
        }

      if (vsx_application_control::get_instance()->is_shutdown_requested())
        break;

      if (vsx_application_control::get_instance()->message_box_title.size())
      {
        sdl_tools::show_message_box( vsx_application_control::get_instance()->message_box_title, vsx_application_control::get_instance()->message_box_message );
        vsx_application_control::get_instance()->message_box_title = "";
        vsx_application_control::get_instance()->message_box_message = "";
      }

      if (vsx_application_control::get_instance()->window_title.size())
      {
        SDL_SetWindowTitle(vsx_application_sdl_window_holder::get_instance()->window, vsx_application_control::get_instance()->window_title.c_str());
        vsx_application_control::get_instance()->window_title = "";
      }

      if (vsx_application_control::get_instance()->is_show_console_window_requested())
      {
        #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
          ShowWindow( GetConsoleWindow(), SW_SHOW );
        #endif
        vsx_application_control::get_instance()->show_console_window_reset();
      }

      // time manager
      vsx::common::time::manager::get()->update((float)frame_timer.dtime());
    }

    // uninit with graphics
    vsx_application_manager::get()->uninit_graphics();

    // Close OpenGL window and terminate
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(vsx_application_sdl_window_holder::get_instance()->window);

    // uninit without graphics
    vsx_application_manager::get()->uninit_no_graphics();

    SDL_Quit();
  }
};
