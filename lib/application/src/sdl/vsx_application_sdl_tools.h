#ifndef SDL_TOOLS_H
#define SDL_TOOLS_H

#include <string/vsx_string.h>
#include <SDL.h>


class sdl_tools
{
public:

  /* A simple function that prints a message, the error code returned by SDL,
   * and quits the application */
  static void sdldie(const char *msg)
  {
    vsx_printf(L"%hs: %hs\n", msg, SDL_GetError());
    SDL_Quit();
    exit(1);
  }


  static void checkSDLError(int line = -1)
  {
  #ifndef NDEBUG
          const char *error = SDL_GetError();
          if (*error != '\0')
          {
                  vsx_printf(L"SDL Error: %hs\n", error);
                  if (line != -1)
                          vsx_printf(L" + line: %i\n", line);
                  SDL_ClearError();
          }
  #endif
  }



  static void show_message_box(vsx_string<> title, vsx_string<> msg)
  {
    const SDL_MessageBoxButtonData buttons[] = {
        { /* .flags, .buttonid, .text */        0, 0, "ok" }
        //{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "yes" },
        //{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "cancel" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
        { /* .colors (.r, .g, .b) */
            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
            { 0,   0,   0 },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
            {   255, 255,   255 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
            { 255, 255,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
            {   0,   0, 255 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
            { 255,   0, 255 }
        }
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        title.c_str(),
        msg.c_str(),
        SDL_arraysize(buttons), /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        vsx_printf(L"error displaying message box");
    }

  }

};

#endif
