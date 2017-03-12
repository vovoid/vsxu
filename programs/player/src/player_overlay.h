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

#pragma once

#include "vsx_font.h"
#include <audiovisual/vsx_state_manager.h>
#include <time/vsx_timer.h>
#include <filesystem/vsx_filesystem.h>
#include <vsx_logo_intro.h>
#include <perf/vsx_perf.h>


class vsx_overlay {
	vsx_timer time2;
	vsx_font* myf;
	unsigned long frame_counter;
	unsigned long delta_frame_counter;
	float delta_frame_time;
	float delta_fps;
	float total_time;
	float title_timer;	
  float show_randomizer_timer;
	float scroll_pos;
	float dt;
	float intro_timer;
  int help_id;
  float fx_alpha;
  bool first;
  vsx_logo_intro* intro;
  vsx_perf perf;

public:

  vsx_string<> current_visual_name()
  {
    vsx_string<> result = vsx_string<>(vsx::engine::audiovisual::state_manager::get()->get_meta_visual_name().c_str());
    if (vsx::engine::audiovisual::state_manager::get()->get_meta_visual_author() != "")
      result += vsx_string<>(" by ") + vsx::engine::audiovisual::state_manager::get()->get_meta_visual_author().c_str();
    return result;
  }
	
  vsx_overlay()
  {
    help_id = 0;
    scroll_pos = 0.0f;
    title_timer = 0.0f;
    myf = new vsx_font(PLATFORM_SHARED_FILES);
    myf->load(vsx_string<>("font/font-ascii.png"), vsx::filesystem::get_instance());
    frame_counter = 0;
    delta_frame_counter = 0;
    delta_frame_time = 0.0f;
    delta_fps = 0;
    time2.start();
    total_time = 0.0f;
    intro_timer = 6.0f;
    fx_alpha = 0.0f;
    show_randomizer_timer = 0.0f;
    intro = new vsx_logo_intro;
    intro->set_destroy_textures(false);
  }

  void render()
  {

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    ++frame_counter;
    ++delta_frame_counter;
    dt = (float)time2.dtime();
    delta_frame_time+= dt;
    total_time += dt;
    if (delta_frame_counter == 100) {
      delta_fps = 100.0f/delta_frame_time;
      delta_frame_counter = 0;
      delta_frame_time = 0.0f;
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (intro_timer > 0.0f) {
      myf->color.a = intro_timer;
      myf->print(vsx_vector3<>(-0.40f,-0.95f),"Welcome to VSXu!   --   Press F1 for help",0.04);
      intro_timer -= dt;
    }

    if (show_randomizer_timer > 0.0f)
    {
      myf->color.a = show_randomizer_timer;
      vsx_string<>message;
      if (vsx::engine::audiovisual::state_manager::get()->get_randomizer_status())
      {
        message = "Enabling randomizer...";
      } else
      {
        message = "Disabling randomizer...";
      }
      myf->print(vsx_vector3<>(-1.0f,-0.95f),message,0.04);

      show_randomizer_timer -= dt;
    }

    if (vsx::engine::audiovisual::state_manager::get()->state_loading() != "")
      title_timer = 2.0f;

    myf->color.a = title_timer;
    vsx_string<>output;
    if (vsx::engine::audiovisual::state_manager::get()->get_meta_upcoming_visual_name() != "")
      output += vsx_string<>(vsx::engine::audiovisual::state_manager::get()->get_meta_upcoming_visual_name().c_str());

    if (vsx::engine::audiovisual::state_manager::get()->get_meta_upcoming_visual_author() != "")
      output += vsx_string<>(" by ") + vsx::engine::audiovisual::state_manager::get()->get_meta_upcoming_visual_author().c_str();

    if (output == "")
      output = current_visual_name();

    myf->print(vsx_vector3<>(-0.99f,0.94f),output,0.04);
    myf->color.a = 1.0f;
    title_timer -= dt;

    if (fx_alpha > 0.0f)
    {
      float local_alpha = fx_alpha;
      if (local_alpha > 1.0f) local_alpha = 1.0f;
      glColor4f(0,0,0,0.5f*local_alpha);
      glBegin(GL_TRIANGLE_STRIP);
      glVertex3f(1.0f,-1.0,0);
      glVertex3f(0.91f,-1.0,0);
        for (float i = 0; i <= 16.4; i+=0.1f) {
          glVertex3f(1.0f,-0.9+i/16.0f,0);
          glVertex3f(0.9f-(float)pow((double)i,(double)1.29)*0.01,-0.9+i/16.0f,0);
        }
      glEnd();

      glColor4f(1.0f,0.0f,0.0f,0.8f*local_alpha);
      glBegin(GL_TRIANGLE_STRIP);
      glVertex3f(0.98f,-0.9,0);
      glVertex3f(0.92f,-0.9,0);
        for (float i = 0; i <= vsx::engine::audiovisual::state_manager::get()->fx_level_get(); i+=0.1f) {
          glColor4f(i/16.0f,1.0f-i/16.0f,0,local_alpha);
          glVertex3f(0.98f,-0.9+i/16.0f,0);
          glVertex3f(0.92f-(float)pow((double)i,(double)1.29)*0.01,-0.9+i/16.0f,0);
        }
      glEnd();
      myf->color.r = myf->color.g = myf->color.b = 1.0f;
      myf->color.a = local_alpha;
      myf->print(vsx_vector3<>(0.915f,-0.98f),"FX LVL",0.035);
      myf->print(vsx_vector3<>(0.85f,0.1f),vsx_string_helper::f2s(vsx::engine::audiovisual::state_manager::get()->fx_level_get()),0.035);
    }

    myf->color.r = myf->color.g = myf->color.b = myf->color.a = 1.0f;
    fx_alpha -= dt*4;
    print_help();
    if (title_timer > -5.0f && total_time > 4.0f)
    {
      // small intro
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      glViewport(viewport[2]-(GLint)(viewport[2]*0.17f),viewport[3]-(GLint)(viewport[3]*0.17f),viewport[2]/5,viewport[3]/5);
      bool draw_always = false;
      if (title_timer>1.8f) draw_always = true;
      intro->draw(draw_always,false,false);
      glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
    } else
    {
      intro->draw();
    }
  }

  void set_help(int id)
  {
    if (help_id == id) help_id = 0;
    else help_id = id;
    scroll_pos = 0.0f;
  }

  void print_help()
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    if (help_id) {
      glColor4f(0,0,0,0.5);
      glBegin(GL_QUADS);
        glVertex3f(-1,-1,0);
        glVertex3f(1,-1,0);
        glVertex3f(1,1,0);
        glVertex3f(-1,1,0);
      glEnd();
    }
    switch(help_id)
    {
      case 1:
        scroll_pos -= dt*0.25f;
        #if PLATFORM == PLATFORM_WINDOWS
        myf->print(vsx_vector3<>(-0.8,0.4f),
          "Doesn't VSX react to sound? If sound isn't set up properly in Windows\n"
          "some visuals might be totally black! Please check http://www.vsxu.com/documentation/faq\n"
          "and select \"VSXu is not reacting to sound\" for instructions!\n"
          "\n"
          "In brief, what you need to do is set the recording properties of the\n"
          "mixer to something like 'Stereo Mix' or similar. Some soundcards only allow\n"
          "one recording channel to be active at a time, others allow all simultaneously.\n"
          "\n"
          "Keyboard shortcuts:\n"
          " F                     - display fps and fx level information\n"
          " R                     - toggle randomization mode\n"
          " Ctrl+R                - pick a random visual\n"
          " arrow keys left/right - switch visuals\n"
          " arrow keys up/down    - change sound reactivity/FX level (per visual)\n"
          " pgup/pgdn             - increase/decrease speed (per visual)\n"
        ,0.05);
        #endif
        #if PLATFORM == PLATFORM_LINUX
        myf->print(vsx_vector3<>(-0.8,0.4f),
          "Doesn't VSX react to sound? If sound isn't set up properly\n"
          "some visuals might be totally black! Please check http://www.vsxu.com/documentation/faq\n"
          "and select \"VSXu is not reacting to sound\" for instructions!\n"
          "\n"
          "In GNU/Linux (Ubuntu) pulseaudio is normally used, you have to set the rec/routing\n"
          "properly using the helper program \"pavucontrol\" (you might need to\n"
          "install it).\n"
          "\n"
          "Keyboard shortcuts:\n"
          " F                     - display fps and fx level information\n"
          " R                     - toggle randomization mode\n"
          " Ctrl+R                - pick a random visual\n"
          " arrow keys left/right - switch visuals\n"
          " arrow keys up/down    - change sound reactivity/FX level (per visual)\n"
          " pgup/pgdn             - increase/decrease speed (per visual)\n"
          ,0.05);
        #endif
        break;
      case 2:
        myf->print(vsx_vector3<>(-0.8,0.4),
          "Current visual  :\n"
          "Current FPS     :\n"
          "FX level        :\n"
          "Speed           :\n"
          "Run time        :\n"
          "Frames rendered :\n"
          "Modules in state:\n"
          "Loading in background:\n"
          "RAM usage (total):\n"
          ,
          "ascii",
          0.06
        );
        myf->print(
          vsx_vector3<>(-0.1,0.4),
          vsx_string<>(current_visual_name().c_str())+"\n"+
          vsx_string_helper::f2s(delta_fps)+"\n"+
          vsx_string_helper::f2s(vsx::engine::audiovisual::state_manager::get()->fx_level_get(),3)+"\n"+
          vsx_string_helper::f2s(vsx::engine::audiovisual::state_manager::get()->speed_get(),3)+"\n"+
          vsx_string_helper::f2s(total_time,3)+"\n"+
          vsx_string_helper::i2s(frame_counter) + "\n" +
          vsx_string_helper::i2s( vsx::engine::audiovisual::state_manager::get()->get_meta_modules_in_engine() ) + "\n" +
          vsx::engine::audiovisual::state_manager::get()->get_meta_upcoming_visual_name() + "\n" +
          vsx_string_helper::i2s( perf.memory_currently_used() ) + " MB\n"
          ,
          0.06
        );
      break;
    };
  }

  void show_fx_graph()
  {
    fx_alpha = 5.0f;
  }

  void show_randomizer_status()
  {
    show_randomizer_timer = 5.0f;
  }

};
