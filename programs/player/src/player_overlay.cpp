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

#include <vsx_platform.h>
#include <string/vsx_string_helper.h>
#include <vsx_gl_global.h>
#include "player_overlay.h"

vsx_overlay::vsx_overlay() {
  help_id = 0;
  scroll_pos = 0.0f;
  title_timer = 0.0f;
  manager = 0;
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

void vsx_overlay::set_manager(vsx_manager_abs* new_manager)
{
  manager = new_manager;
}

void vsx_overlay::reinit() {
}
  
void vsx_overlay::render() {

  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_BLEND);
  ++frame_counter;
  ++delta_frame_counter;
  dt = time2.dtime();
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
    myf->print(vsx_vector3<>(-0.60f,-0.95f),"Press F1 for help   |    Welcome to Vovoid VSXu by jaw / cor / asterix / saidinesh5",0.04);
    intro_timer -= dt;
  }
  
  if (show_randomizer_timer > 0.0f)
  {
    myf->color.a = show_randomizer_timer;
    vsx_string<>message;
    if (manager->get_randomizer_status())
    {
      message = "Enabling randomizer...";
    } else
    {
      message = "Disabling randomizer...";
    }
    myf->print(vsx_vector3<>(-1.0f,-0.95f),message,0.04);
    
    show_randomizer_timer -= dt;
  }

  if (manager) {
    if (manager->visual_loading() != "") title_timer = 2.0f;
    myf->color.a = title_timer;
    vsx_string<>output;
    if (manager->get_meta_visual_name() != "") output += vsx_string<>(manager->get_meta_visual_name().c_str());
    if (manager->get_meta_visual_creator() != "") output += vsx_string<>(" by ")+manager->get_meta_visual_creator().c_str();
    if (manager->get_meta_visual_company() != "") output += vsx_string<>(" of ")+manager->get_meta_visual_company().c_str();
    if (output == "") 
    {
      output = vsx_string<>(manager->get_meta_visual_filename().c_str());
      int i = output.size()-1;
      while (output[i] != '/' && output[i] != '\\') 
      {
        --i;
      }
      output = vsx_string<>(manager->get_meta_visual_filename().substr(i+1, output.size()-i-5).c_str());
    }
    myf->print(vsx_vector3<>(-1.0f,0.96f),output,0.04);
  }
  //myf->print(vsx_vector(0.75f,0.96f),"Vovoid VSXu",0.04);
  myf->color.a = 1.0f;
  title_timer -= dt;

  if (fx_alpha > 0.0f) {
    float local_alpha = fx_alpha;
    if (local_alpha > 1.0f) local_alpha = 1.0f;
    glColor4f(0,0,0,0.5f*local_alpha);
    /*glBegin(GL_POLYGON);
    glVertex3f(1.0f,-1.0,0);
    glVertex3f(0.9f,-1.0,0);
      for (float i = 0; i <= 17; i+=0.1f) {
        glVertex3f(0.9f-(float)pow((double)i,(double)1.29)*0.01,-0.9+i/16.0f,0);
      }
      glVertex3f(1.0f,-0.9+17.0f/16.0f,0);
    glEnd();*/
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(1.0f,-1.0,0);
    glVertex3f(0.91f,-1.0,0);
      for (float i = 0; i <= 16.4; i+=0.1f) {
        glVertex3f(1.0f,-0.9+i/16.0f,0);
        glVertex3f(0.9f-(float)pow((double)i,(double)1.29)*0.01,-0.9+i/16.0f,0);
      }
      //glVertex3f(0.98f,-0.9+((*state_iter).fx_level)/16.0f,0);
    glEnd();


    //glColor4f(1.0f,1.0f,1.0f,1.0f*local_alpha);
    /*glBegin(GL_LINE_STRIP);
      for (float i = 0; i < 17; i+=0.1f) {
        glVertex3f(0.9f-(float)pow((double)i,(double)1.29)*0.01,-0.9+i/16.0f,0);
      }
    glEnd();*/

    glColor4f(1.0f,0.0f,0.0f,0.8f*local_alpha);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(0.98f,-0.9,0);
    glVertex3f(0.92f,-0.9,0);
      for (float i = 0; i <= manager->get_fx_level(); i+=0.1f) {
        glColor4f(i/16.0f,1.0f-i/16.0f,0,local_alpha);
        glVertex3f(0.98f,-0.9+i/16.0f,0);
        glVertex3f(0.92f-(float)pow((double)i,(double)1.29)*0.01,-0.9+i/16.0f,0);
      }
      //glVertex3f(0.98f,-0.9+((*state_iter).fx_level)/16.0f,0);
    glEnd();
    myf->color.r = myf->color.g = myf->color.b = 1.0f;
    myf->color.a = local_alpha;
    myf->print(vsx_vector3<>(0.915f,-0.98f),"FX LVL",0.035);
    myf->print(vsx_vector3<>(0.85f,0.1f),vsx_string_helper::f2s(manager->get_fx_level()),0.035);
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
    //GLint screenx = GLint(viewport[2]-viewport[0]);
    //G screeny = (viewport[3]-viewport[1]);
    glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
  } else
  {
    intro->draw();
  }
}

void vsx_overlay::set_help(int id) {
  if (help_id == id) help_id = 0;
  else help_id = id;
  scroll_pos = 0.0f;
}

void vsx_overlay::show_fx_graph()
{
  fx_alpha = 5.0f;
}

void vsx_overlay::show_randomizer_status()
{
  show_randomizer_timer = 5.0f;
}

void vsx_overlay::print_help()
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
      myf->print(vsx_vector3<>(-0.8,0.4f),"\
Doesn't VSX react to sound? If sound isn't set up properly in Windows\n\
some visuals might be totally black! Please check http://www.vsxu.com/documentation/faq\n\
and select \"VSXu is not reacting to sound\" for instructions!\n\
\n\
In brief, what you need to do is set the recording properties of the\n\
mixer to something like 'Stereo Mix' or similar. Some soundcards only allow\n\
one recording channel to be active at a time, others allow all simultaneously.\n\
\n\
Keyboard shortcuts:\n\
 F                     - display fps and fx level information\n\
 R                     - toggle randomization mode\n\
 Ctrl+R                - pick a random visual\n\
 arrow keys left/right - switch visuals\n\
 arrow keys up/down    - change sound reactivity/FX level (per visual)\n\
 pgup/pgdn             - increase/decrease speed (per visual)\n\
\n\
",0.05);
#endif
#if PLATFORM == PLATFORM_LINUX
      myf->print(vsx_vector3<>(-0.8,0.4f),"\
Doesn't VSX react to sound? If sound isn't set up properly\n\
some visuals might be totally black! Please check http://www.vsxu.com/documentation/faq\n\
and select \"VSXu is not reacting to sound\" for instructions!\n\
\n\
In GNU/Linux (Ubuntu) pulseaudio is normally used, you have to set the rec/routing\n\
properly using the helper program \"pavucontrol\" (you might need to\n\
install it).\n\
\n\
Keyboard shortcuts:\n\
F                     - display fps and fx level information\n\
R                     - toggle randomization mode\n\
Ctrl+R                - pick a random visual\n\
arrow keys left/right - switch visuals\n\
arrow keys up/down    - change sound reactivity/FX level (per visual)\n\
pgup/pgdn             - increase/decrease speed (per visual)\n\
\n\
",0.05);
#endif
      break; 
    case 2:
      if (manager) { 
        myf->print(vsx_vector3<>(-0.8,0.4),"\
Current visual  :\n\
Current FPS     :\n\
FX level        :\n\
Speed           :\n\
Run time        :\n\
Frames rendered :\n\
Modules in state:","ascii",0.06);
        if (manager)
        myf->print(
          vsx_vector3<>(-0.1,0.4),
          vsx_string<>(manager->get_meta_visual_filename().c_str())+"\n"+
          vsx_string_helper::f2s(delta_fps)+"\n"+
          vsx_string_helper::f2s(manager->get_fx_level(),3)+"\n"+
          vsx_string_helper::f2s(manager->get_speed(),3)+"\n"+
          vsx_string_helper::f2s(total_time,3)+"\n"+
          vsx_string_helper::i2s(frame_counter)+"\n"+
          vsx_string_helper::i2s(manager->get_engine_num_modules()),0.06
        );
      }
      break;
  };
}

  
  
