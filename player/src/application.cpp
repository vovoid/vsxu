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

#define PLATFORM_SHARED_FILES_STL
#include <string>
#include <stdlib.h>
// vsxu includes
#include <vsx_version.h>
#include <vsx_platform.h>
#include <vsx_manager.h>

// local includes
#include "application.h"

// info_overlay is the text on top of the graphics
// you might want to remove this.
#include "vsx_overlay.h"

#include <GL/glfw.h>

vsx_manager_abs* manager;



vsx_overlay* overlay = 0;
bool first = true;

/* uncomment for manual sound injection
float sound_wave_test[513];
float sound_freq_test[513];
*/

void (*app_set_fullscreen)(int,bool) = 0;
bool (*app_get_fullscreen)(int) = 0;

void app_init(int id)
{
  VSX_UNUSED(id);
}
void app_pre_draw() {}

bool app_draw(int id)
{
  VSX_UNUSED(id);
  if (first)
  {
    first = false;
    // create a new manager
    manager = manager_factory();
    manager->set_option_preload_all(option_preload_all);

    // init manager with the shared path and sound input type.
    // manual sound injection: manager->init( path.c_str() , "media_player");
    std::string path = PLATFORM_SHARED_FILES_STLSTRING;
    manager->init( path.c_str(), "");
    // create a new text overlay
    overlay = new vsx_overlay;
    overlay->set_manager(manager);
    // create a new intro (Luna logo) object
    if (disable_randomizer) manager->set_randomizer(false);
  }

  /*
  uncomment for manual sound injection
  for (unsigned long i = 0; i < 512; i++)
  {
    sound_wave_test[i] = (float)(rand()%65535-32768)*(1.0f/32768.0f);
  }
  for (unsigned long i = 0; i < 512; i++)
  {
    sound_freq_test[i] = (float)(rand()%65535)*(1.0f/65535.0f);
  }
  manager->set_sound_freq(&sound_freq_test[0]);
  manager->set_sound_wave(&sound_wave_test[0]);
  */

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (manager) manager->render();
  if (overlay && !no_overlay) overlay->render();
  return true;
}

void app_key_down(long key)
{
  switch (key)
  {
    case GLFW_KEY_ESC:
      if (manager) manager_destroy(manager);
      exit(0);
    case GLFW_KEY_PAGEUP:
      manager->inc_speed();
      break;
    case GLFW_KEY_PAGEDOWN:
      manager->dec_speed();
      break;
    case GLFW_KEY_UP:
      manager->inc_fx_level(); overlay->show_fx_graph();
      break;
    case GLFW_KEY_DOWN:
      manager->dec_fx_level(); overlay->show_fx_graph();
      break;
    case GLFW_KEY_LEFT:
      manager->prev_visual();
      break;
    case GLFW_KEY_RIGHT:
      manager->next_visual();
      break;
    case GLFW_KEY_F1:
      overlay->set_help(1);
      break;
    // "F" key:
    case 'f':
    case 'F':
      overlay->set_help(2);
      break;
    // "R" key:
    case 'r':
    case 'R':
      if (app_ctrl)
      {
        manager->pick_random_visual();
      }
      else
      {
        manager->toggle_randomizer();
        overlay->show_randomizer_status();
      }
      break;
  }
}

void app_key_up(long key)
{
  VSX_UNUSED(key);
}
void app_mouse_move_passive(int x, int y)
{
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_mouse_move(int x, int y)
{
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_mouse_down(unsigned long button,int x,int y)
{
  VSX_UNUSED(button);
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_mouse_up(unsigned long button,int x,int y)
{
  VSX_UNUSED(button);
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_mousewheel(float diff,int x,int y)
{
  VSX_UNUSED(diff);
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_char(long key)
{
  VSX_UNUSED(key);
}

