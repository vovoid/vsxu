#pragma once

#include <SDL2/SDL.h>
#include "input_keys.h"
#include "application.h"

class sdl_event_handler
{
  bool keys[512];
  int64_t keys_counter[512];

public:

  sdl_event_handler()
  {
    for (size_t i = 0; i < 512; i++)
    {
      keys[i] = false;
      keys_counter[i] = 0;
    }
  }

  void handle_key_down(SDL_Event& event)
  {
    keys[event.key.keysym.scancode] = true;
    app_key_down(event.key.keysym.scancode);
  }

  void handle_key_up(SDL_Event& event)
  {
    keys[event.key.keysym.scancode] = false;
    app_key_up(event.key.keysym.scancode);
  }

  bool get_key(int16_t key)
  {
    return keys[key];
  }

  void consume(SDL_Event& event)
  {
    if (event.type == SDL_KEYDOWN)
      handle_key_down(event);

    if (event.type == SDL_KEYUP)
      handle_key_up(event);
  }

  static sdl_event_handler* get_instance()
  {
    static sdl_event_handler seh;
    return &seh;
  }
};
