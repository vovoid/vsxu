#include <vsx_platform.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

class vsx_application_sdl_window_holder
{
public:

  SDL_Window *window = 0x0;

  static vsx_application_sdl_window_holder* get_instance()
  {
    static vsx_application_sdl_window_holder h;
    return &h;
  }
};
