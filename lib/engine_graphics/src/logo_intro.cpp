/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


//#include <string>
//#ifdef FOO
#include <map>
#include "vsx_gl_global.h"
#include <texture/vsx_texture.h>
#include <time/vsx_timer.h>
#include <vsx_logo_intro.h>

// logo animation settings
float startanim = 0;
//float animlen = 12;
float animlen = 8;
float fade = 1;

float inalpha = 1;//(logo_time-startanim)/(fade*0.5);
float alpha;

bool finished = false;

void vsx_logo_intro::draw(bool always,bool draw_background,bool draw_black_overlay) {
  if (logo_time > animlen)
  {
    if (finished)
    {
      return;
    }
    else
    {
      if (destroy_textures)
      {
        luna.reset(nullptr);
        luna_bkg.reset(nullptr);
        finished = true;
      }
    }
  }

  float dtime = (float)timer.dtime();
  if (always) {
    dtime = 0;
    logo_time = 0.0f;
  }

  logo_rot1 += dtime*0.01f;
  logo_rot2 -= dtime*0.014f;
  logo_rot3 += dtime*0.005f;
  logo_time += dtime*2.0f;

  if (inalpha > 1) inalpha = 1;
  alpha = 1-(logo_time-((animlen-fade)/(fade<1?1:fade)));

  float b_alpha = 1-(logo_time-((animlen-fade)/(fade<1?1:fade)));
  if (alpha > 1) alpha = 1;
  if (alpha < 0) alpha = 0;

  glEnable(GL_BLEND);

  glDepthMask(GL_TRUE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  float screenx = (float)(viewport[2]-viewport[0]);
  float screeny = (float)(viewport[3]-viewport[1]);
  gluPerspective(45, screenx/screeny, 0.001, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0,0,20,0,0,0.0,0.0,1.0,0.0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  vsx_vector3<> luna_size;
  luna_size.x = 15;
  luna_size.y = 15;
  logo_pos.x = 0;
  logo_pos.y = 0;
  logo_pos.z = 1.1f;
  logo_size.x = 20;
  logo_size.y = 20;
  if (logo_time < animlen) {
    glDisable(GL_DEPTH_TEST);

    if (draw_black_overlay)
    {
      glColor4f(0,0,0,b_alpha);
      glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(logo_pos.x-logo_size.x*1.5f/2,logo_pos.y-logo_size.y/2,logo_pos.z);
        glTexCoord2f(0, 1);
        glVertex3f(logo_pos.x-logo_size.x*1.5f/2,logo_pos.y+logo_size.y/2,logo_pos.z);
        glTexCoord2f(1, 1);
        glVertex3f(logo_pos.x+logo_size.x*1.5f/2,logo_pos.y+logo_size.y/2,logo_pos.z);
        glTexCoord2f(1, 0);
        glVertex3f(logo_pos.x+logo_size.x*1.5f/2,logo_pos.y-logo_size.y/2,logo_pos.z);
      glEnd();
    }
    float alphab = alpha * 0.4f;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    if (draw_background)
    {
      luna_bkg->bind();
        glPushMatrix();
        glColor4f(44.0f / 255.0f, 57.0f / 255.0f, 83.0f/ 255.0f, alphab);
        glRotatef(logo_rot1*360,0,0,1);
        glBegin(GL_QUADS);
          glTexCoord2f(0, 0);
          glVertex3f(logo_pos.x-logo_size.x/2,logo_pos.y-logo_size.y/2,logo_pos.z);
          glTexCoord2f(0, 1);
          glVertex3f(logo_pos.x-logo_size.x/2,logo_pos.y+logo_size.y/2,logo_pos.z);
          glTexCoord2f(1, 1);
          glVertex3f(logo_pos.x+logo_size.x/2,logo_pos.y+logo_size.y/2,logo_pos.z);
          glTexCoord2f(1, 0);
          glVertex3f(logo_pos.x+logo_size.x/2,logo_pos.y-logo_size.y/2,logo_pos.z);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        glColor4f(42.0f / 255.0f, 98.0f / 255.0f, 104.0f/ 255.0f, alphab);
        glRotatef(360*logo_rot2,0,0,alphab);
        glBegin(GL_QUADS);
          glTexCoord2f(0, 0);
          glVertex3f(logo_pos.x-logo_size.x/2,logo_pos.y-logo_size.y/2,logo_pos.z);
          glTexCoord2f(0, 1);
          glVertex3f(logo_pos.x-logo_size.x/2,logo_pos.y+logo_size.y/2,logo_pos.z);
          glTexCoord2f(1, 1);
          glVertex3f(logo_pos.x+logo_size.x/2,logo_pos.y+logo_size.y/2,logo_pos.z);
          glTexCoord2f(1, 0);
          glVertex3f(logo_pos.x+logo_size.x/2,logo_pos.y-logo_size.y/2,logo_pos.z);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        glColor4f(42.0f / 255.0f, 58.0f / 255.0f, 104.0f/ 255.0f, alphab);
        glRotatef(360*logo_rot3,0,0,1);
        glBegin(GL_QUADS);
          glTexCoord2f(0, 0);
          glVertex3f(logo_pos.x-logo_size.x/2,logo_pos.y-logo_size.y/2,logo_pos.z);
          glTexCoord2f(0, 1);
          glVertex3f(logo_pos.x-logo_size.x/2,logo_pos.y+logo_size.y/2,logo_pos.z);
          glTexCoord2f(1, 1);
          glVertex3f(logo_pos.x+logo_size.x/2,logo_pos.y+logo_size.y/2,logo_pos.z);
          glTexCoord2f(1, 0);
          glVertex3f(logo_pos.x+logo_size.x/2,logo_pos.y-logo_size.y/2,logo_pos.z);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        logo_size.x = 22;
        logo_size.y = 22;

        glColor4f(60.0f / 255.0f, 67.0f / 255.0f, 100.0f/ 255.0f, alphab);
        glRotatef(360.0f * logo_rot3 * 0.5f, 0, 0, 1);
        glBegin(GL_QUADS);
          glTexCoord2f(0, 0);
          glVertex3f(logo_pos.x-logo_size.x/2,logo_pos.y-logo_size.y/2,logo_pos.z);
          glTexCoord2f(0, 1);
          glVertex3f(logo_pos.x-logo_size.x/2,logo_pos.y+logo_size.y/2,logo_pos.z);
          glTexCoord2f(1, 1);
          glVertex3f(logo_pos.x+logo_size.x/2,logo_pos.y+logo_size.y/2,logo_pos.z);
          glTexCoord2f(1, 0);
          glVertex3f(logo_pos.x+logo_size.x/2,logo_pos.y-logo_size.y/2,logo_pos.z);
        glEnd();
        glPopMatrix();
      luna_bkg->_bind();
      glColor4f(1, 1, 1, alpha*0.8f);
    } else
    {
      glColor4f(1, 1, 1, alpha*1.0f);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    luna->bind();
      glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f(logo_pos.x-luna_size.x/2,logo_pos.y-luna_size.y/2,logo_pos.z);
        glTexCoord2f(0, 0);
        glVertex3f(logo_pos.x-luna_size.x/2,logo_pos.y+luna_size.y/2,logo_pos.z);
        glTexCoord2f(1, 0);
        glVertex3f(logo_pos.x+luna_size.x/2,logo_pos.y+luna_size.y/2,logo_pos.z);
        glTexCoord2f(1, 1);
        glVertex3f(logo_pos.x+luna_size.x/2,logo_pos.y-luna_size.y/2,logo_pos.z);
      glEnd();
    luna->_bind();
    glEnable(GL_DEPTH_TEST);
  }
}

vsx_logo_intro::vsx_logo_intro() {
  logo_time = 0;
  logo_rot1 = 0;
  logo_rot2 = 0.13f;
  logo_rot3 = 0.3f;
  destroy_textures = true;

  luna = vsx_texture_loader::load( PLATFORM_SHARED_FILES+"gfx"+DIRECTORY_SEPARATOR+"vsxu_logo.dds", vsx::filesystem::get_instance(), false, vsx_bitmap::flip_vertical_hint, vsx_texture_gl::linear_interpolate_hint );
  luna_bkg = vsx_texture_loader::load( PLATFORM_SHARED_FILES+"gfx"+DIRECTORY_SEPARATOR+"vsxu_logo_bkg.dds",vsx::filesystem::get_instance(), false, vsx_bitmap::flip_vertical_hint, vsx_texture_gl::linear_interpolate_hint );
  timer.start();
}

void vsx_logo_intro::reinit() {
  //luna->load_png_thread("_gfx/vsxu_logo.png",false);
  //luna_bkg->load_png_thread("_gfx/vsxu_logo_bkg.png",false);
}
//#endif
