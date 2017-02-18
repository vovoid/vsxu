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


#include "vsx_gl_global.h"
#include <map>
#include <list>
#include <filesystem/vsx_filesystem.h>
#include "vsx_font.h"
#include "debug/vsx_error.h"

#ifndef VSX_FONT_NO_FT
#include <FTGL/ftgl.h>
#endif


void vsx_font::load(vsx_string<>font, vsx::filesystem* filesystem)
{
  if (my_font_info.name.size())
    return;

  my_font_info.name = font;

  if (font[0] != '-')
  {
    my_font_info.type = 0;
    vsx_string<> ss = base_path+font;
    my_font_info.texture = vsx_texture_loader::load( base_path+font, filesystem, true, vsx_bitmap::flip_vertical_hint, vsx_texture_gl::generate_mipmaps_hint | vsx_texture_gl::linear_interpolate_hint );
    ch = 16.0f/255.0f;
    cw = 10.0f/255.0f;
  }
  else
  {
    #ifndef VSX_FONT_NO_FT
    my_font_info.type = 1;

    FTGLPolygonFont* pfont = new FTGLPolygonFont( (base_path+"resources\\fonts\\font"+font+".ttf").c_str() );
    pfont->FaceSize(10);
    pfont->CharMap(ft_encoding_unicode);
    my_font_info.ftfont = (void*)pfont;

    FTGLOutlineFont* pfont_outline = new FTGLOutlineFont( (base_path+"resources\\fonts\\font"+font+".ttf").c_str() );
    pfont_outline->FaceSize(10);
    pfont_outline->CharMap(ft_encoding_unicode);
    my_font_info.ftfont_outline = (void*)pfont_outline;
    #endif
  }
}

void vsx_font::unload()
{
  if(my_font_info.type == 0)
  {
    req(my_font_info.texture);
    my_font_info.texture.reset(nullptr);
  }

}

void vsx_font::reinit(vsx_font_info* f_info, vsx_string<>font, vsx::filesystem* filesystem)
{
  if (f_info->type == 0)
  {
    f_info->texture = vsx_texture_loader::load(base_path+font, filesystem, true, vsx_bitmap::flip_vertical_hint, vsx_texture_gl::generate_mipmaps_hint | vsx_texture_gl::linear_interpolate_hint );
    return;
  }

  if (f_info->type == 1)
  {
    #ifndef VSX_FONT_NO_FT
    delete (FTGLPolygonFont*)f_info->ftfont;
    delete (FTGLOutlineFont*)f_info->ftfont_outline;
    FTGLPolygonFont* pfont = new FTGLPolygonFont( (base_path+"resources\\fonts\\font"+font+".ttf").c_str() );
    pfont->FaceSize(10);
    pfont->CharMap(ft_encoding_unicode);
    my_font_info.ftfont = (void*)pfont;

    FTGLOutlineFont* pfont_outline = new FTGLOutlineFont( (base_path+"resources\\fonts\\font"+font+".ttf").c_str() );
    pfont_outline->FaceSize(10);
    pfont_outline->CharMap(ft_encoding_unicode);
    my_font_info.ftfont_outline = (void*)pfont_outline;
    #endif
  }
}  


  vsx_vector3<> vsx_font::print(vsx_vector3<> p, const vsx_string<>& str, const vsx_string<>& font, float size = 1, vsx_string<>colors) {
    load(font, vsx::filesystem::get_instance());
    return print(p,str,size,colors);
  }

  vsx_vector3<> vsx_font::print(vsx_vector3<> p, const vsx_string<>& str, const float size = 1, vsx_string<>colors)
  {
    req_error_v(my_font_info.name.size(), "font not loaded", p)
    reqrv(str.size(), p);

    bool use_colors = false;

    if (colors.size())
      use_colors = true;

    char current_color = 1;

    if (my_font_info.type == 0)
    {
      size_s = 0.37f*size;

      dx = 0;
      dy = 0;


      reqrv(my_font_info.texture->bind(), vsx_vector3<>());
      colc = (char*)colors.c_str();
      glPushMatrix();
      glTranslatef(p.x,p.y,p.z);

        if (use_colors)
        {
          glColor4f(
            syntax_colors[*colc-1].r,
            syntax_colors[*colc-1].g,
            syntax_colors[*colc-1].b,
            syntax_colors[*colc-1].a
          );
        }
        else
        glColor4f(color.r,color.g,color.b,color.a);

        glBegin(GL_QUADS);

        float ddy = -size*1.05f;

        ddx = align*size_s;
        for (stc = (char*)str.c_str(); *stc; ++stc)
        {
          if (use_colors)
          {
            if (*colc != current_color)
            {
              current_color = *colc;

              glColor4f(
                syntax_colors[*colc-1].r,
                syntax_colors[*colc-1].g,
                syntax_colors[*colc-1].b,
                syntax_colors[*colc-1].a
              );
            }
            ++colc;
          }

          if (*stc == 0x0A)
          {
            dy += ddy;
            dx = 0;
          }
          else
          {
            sx = (float)(*stc % 16) * cw;
            sy = (float)(*stc / 16) * ch;
            ex = sx+cw;
            ey = -(sy+ch)+1.0f;
            sy = -sy+0.995f;

              glTexCoord2f(sx, ey);  glVertex2f(dx       ,  dy);
              glTexCoord2f(sx, sy);  glVertex2f(dx       ,  dy+size);
              glTexCoord2f(ex, sy);  glVertex2f(dx+size_s,  dy+size);
              glTexCoord2f(ex, ey);  glVertex2f(dx+size_s,  dy);
            dx += ddx;
          }
        }
        glEnd();
      glPopMatrix();
      my_font_info.texture->_bind();
      ep.x = dx+p.x;  // this might need to be fixed
      ep.y = dy+p.y;
      return ep;
    }
    else
    {
      #ifndef VSX_FONT_NO_FT
      glEnable(GL_BLEND);
      glPushMatrix();
      glColor4f(color.r,color.g,color.b,color.a);
      glTranslatef(p.x,p.y+size*0.2f,p.z);
      if (outline_transparency > 0.3f)
      glColor4f(color.r,color.g,color.b,color.a);
      else
      glColor4f(color.r,color.g,color.b,(1-outline_transparency)*color.a);

      glScalef(size * 0.8f * 0.1f, size*0.1f, 0.018f);
      ((FTGLPolygonFont*)my_font_info.ftfont)->Render(str.c_str());
      glColor4f(color.r,color.g,color.b,outline_transparency*color.a);
      glPopMatrix();
      #endif
    }
    return vsx_vector3<>();
  }

vsx_vector3<> vsx_font::get_size(const vsx_string<>& str, float size = 1)
{
  #ifndef VSX_FONT_NO_FT
    if (my_font_info.type == 1)
    {
      float x1, y1, z1, x2, y2, z2;
      ((FTGLPolygonFont*)my_font_info.ftfont)->BBox( str.c_str(), x1, y1, z1, x2, y2, z2);
      vsx_vector3<> pp;
      pp.x = (x2*size-x1*size)*0.8f*0.1f;
      return pp;
    }    
  #endif

    int lines = 0;
    int max_char = 0;
    int cur_pos = 0;
    for (size_t i = 0; i < str.size(); ++i) {
      if (lines == 0) lines = 1;
      ++cur_pos;
      if (str[i] == 0x0A) {
        cur_pos = 0;
        ++lines;
      } else
      if (cur_pos > max_char) max_char = cur_pos;
    }
    return vsx_vector3<>(((float)max_char)*0.37f*size,size*1.05f*(float)lines);
  }  


vsx_vector3<> vsx_font::print_center(vsx_vector3<> p, const vsx_string<>& str, float size = 1)
{
  p.x -= (align*0.37f*size*(float)str.size())*0.5f;
  #ifndef VSX_FONT_NO_FT
  float x1 = 0.0f;
  float y1 = 0.0f;
  float z1 = 0.0f; 
  float x2 = 0.0f;
  float y2 = 0.0f;
  float z2 = 0.0f;
  if (my_font_info.type == 1)
    ((FTGLPolygonFont*)my_font_info.ftfont)->BBox( str.c_str(), x1, y1, z1, x2, y2, z2);
  p.x -= (x2*size-x1*size)*0.5f*0.08f;
  #endif
  return print(p,str,size);
}

vsx_vector3<> vsx_font::print_right(vsx_vector3<> p, const vsx_string<>& str, float size = 1)
{
  p.x -= (align*0.37f*size*(float)str.size());
  #ifndef VSX_FONT_NO_FT
  float x1 = 0.0f;
  float y1 = 0.0f;
  float z1 = 0.0f;
  float x2 = 0.0f;
  float y2 = 0.0f;
  float z2 = 0.0f;
  if (my_font_info.type == 1)
      ((FTGLPolygonFont*)my_font_info.ftfont)->BBox( str.c_str(), x1, y1, z1, x2, y2, z2);
    p.x -= (x2*size-x1*size)*0.08f;
  #endif
  return print(p,str,size);
}
  


