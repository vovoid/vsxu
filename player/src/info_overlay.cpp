
#include "info_overlay.h"
#include <vsx_platform.h>

vsx_overlay::vsx_overlay() {
  help_id = 0;
  scroll_pos = 0.0f;
  title_timer = 5.0f;
  manager = 0;
  myf = new vsx_font(PLATFORM_SHARED_FILES);
  myf->init("font/font-ascii.png");
  myf->mode_2d = true;
  frame_counter = 0;
  delta_frame_counter = 0;
  delta_frame_time = 0.0f;
  delta_fps = 0;
  time2.start();
  total_time = 0.0f;
  intro_timer = 6.0f;
}

void vsx_overlay::set_manager(vsx_manager_abs* new_manager)
{
  manager = new_manager;
}

void vsx_overlay::reinit() {
  //delete myf;
  //myf = new vsx_font(own_path);
  //myf->init("_gfx/vsxu/font/font-ascii.png");
  myf->reinit_all_active();
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
      myf->print(vsx_vector(-0.55f,-0.95f),"Welcome to Vovoid VSX Ultra by jaw / cor / asterix - please press F1 for help to set up sound",0.04);
      intro_timer -= dt;
    }
    /*    glColor4f(0,0,0,0.4f);
    glBegin(GL_QUADS);                  // Draw A Quad
      glVertex3f(-1.0f, 1.0f, 0.0f);          // Top Left
      glVertex3f( 1.0f, 1.0f, 0.0f);          // Top Right
      glVertex3f( 1.0f,0.92f, 0.0f);          // Bottom Right
      glVertex3f(-1.0f,0.92f, 0.0f);          // Bottom Left
    glEnd();*/          
    if (manager) {
      myf->color.a = title_timer;
      vsx_string output;
      if (manager->get_meta_visual_name() != "") output += manager->get_meta_visual_name();
      if (manager->get_meta_visual_creator() != "") output += " by "+manager->get_meta_visual_creator();
      if (manager->get_meta_visual_company() != "") output += " of "+manager->get_meta_visual_company();
      if (output == "") {
        output = manager->get_meta_visual_filename();
        int i = output.size()-1;
        while (output[i] != '/' && output[i] != '\\') {
          --i;
        }
        output = manager->get_meta_visual_filename().substr(i+1, output.size()-i-5);
      }
      myf->print(vsx_vector(-1.0f,0.96f),output,0.04);
    }
    myf->print(vsx_vector(0.75f,0.96f),"Vovoid VSX Ultra",0.04);
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
      myf->print(vsx_vector(0.915f,-0.98f),"FX LVL",0.035);
      myf->print(vsx_vector(0.85f,0.1f),f2s(manager->get_fx_level()),0.035);        
    }
    myf->color.r = myf->color.g = myf->color.b = myf->color.a = 1.0f;
    fx_alpha -= dt*4;
    print_help();
  }
  
void vsx_overlay::set_help(int id) {
  if (help_id == id) help_id = 0;
  else help_id = id;
  scroll_pos = 0.0f;
}

void vsx_overlay::print_help() {

 vsx_string message = "\
VSX Ultra (c) Vovoid -:- libs by: Laurent de Soras, Mark J. Harris, The Freetype Project, Auran Development Ltd."
"-- for more vsxu goodies go to http://vsxu.com"
"-- visit our homepage at http://vovoid.com"
;


  /*if (gui_g_time < 15) {
    myf.print(vsx_vector__(-1,-1,0),"Press F1 for help - http://vsxu.com; http://doc.vsxu.com ","ascii",0.04);
  }*/
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
  switch(help_id) {
    case 1:
      scroll_pos -= dt*0.25f;//fmod(gui_d_time*0.25f,30.0f)
      myf->print(vsx_vector(-0.8,0.4f),"\
Doesn't VSX react to sound? If sound isn't set up properly in Windows\n\
some visuals might be totally black! Please check http://doc.vsxu.com\n\
and select \"VSXu Player\" for instructions!\n\
\n\
In brief, what you need to do is set the recording properties of the\n\
mixer to something like 'Stereo Mix' or similar. Some soundcards only allow\n\
one recording channel to be active at a time, others allow all simultaneously.\n\
\n\
In GNU/Linux (Ubuntu) you have to set the pulseaudio routing properly so that VSXu is recording.\n\
\n\
Keyboard shortcuts:\n\
 F                     - display fps and fx level information\n\
 ENTER                 - toggle fullscreen\n\
 R                     - toggle randomization mode\n\
 Ctrl+R                - pick a random visual\n\
 arrow keys left/right - switch visuals\n\
 arrow keys up/down    - change sound reactivity/FX level (per visual)\n\
 pgup/pgdn             - increase/decrease speed (per visual)\n\
\n\
",0.05);
      //myf.color = vsx_color(1.0f,1.0f,1.0f,1.0f);
      for (int i = 0; i < message.size(); ++i) {
        myf->color = vsx_color(sin((float)i*0.5f+total_time*3.5f)*0.4f+0.6f,cos((float)i*0.2f+total_time*0.72f)*0.4f+0.6f,sin((float)i*0.25f+total_time*2.01f)*0.4f+0.6f,1.0f);
        myf->print(vsx_vector(0.5+scroll_pos+0.035*(float)i,-0.85f+sin(total_time*2.5f+(float)i*0.2f)*sin(total_time*1.7f+(float)i*0.1f)*0.1f),message[i],0.07);
      }
      break; 
    case 2:
      if (manager) { 
        myf->print(vsx_vector(-0.8,0.4),"\
Current visual  :\n\
Current FPS     :\n\
FX level        :\n\
Speed           :\n\
Run time        :\n\
Frames rendered :\n\
Modules in state:","ascii",0.06);
        if (manager)
        myf->print(
          vsx_vector(-0.1,0.4),
          manager->get_meta_visual_filename()+"\n"+
          f2s(delta_fps)+"\n"+
          f2s(manager->get_fx_level(),3)+"\n"+
          f2s(manager->get_speed(),3)+"\n"+
          f2s(total_time,3)+"\n"+
          i2s(frame_counter)+"\n"+
          i2s(manager->get_engine_num_modules()),0.06
        );
      }
      break;
  };
}
  
  
