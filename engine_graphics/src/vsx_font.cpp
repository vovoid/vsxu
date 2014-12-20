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
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_font.h"
#include "debug/vsx_error.h"

#ifndef VSX_FONT_NO_FT
#include "ftgl/FTGLPolygonFont.h"
#include "ftgl/FTGLBitmapFont.h"
#include "ftgl/FTGLOutlineFont.h"
#include "ftgl/FTGLTextureFont.h"
#endif


std::map<vsx_string<>, vsx_font_info*> vsx_font::glist;

vsx_font_info* vsx_font::load(vsx_string<>font, vsxf* filesystem)
{
  my_font_info = glist[font] = new vsx_font_info;
  my_font_info->name = font;

  if (font[0] != '-')
  {
    my_font_info->type = 0;
    my_font_info->texture = new vsx_texture();
    vsx_string<>ss = base_path+font;
    my_font_info->texture->load_png( base_path+font, true, filesystem );
    ch = 16.0f/255.0f;
    cw = 10.0f/255.0f;
  }
  else
  {
    #ifndef VSX_FONT_NO_FT
    font_info->type = 1;
    font_info->ftfont = new FTGLPolygonFont((base_path+"resources\\fonts\\font"+font+".ttf").c_str());
    font_info->ftfont->FaceSize(10);
    font_info->ftfont->CharMap(ft_encoding_unicode);
    font_info->ftfont_outline = new FTGLOutlineFont((base_path+"resources\\fonts\\font"+font+".ttf").c_str());
    font_info->ftfont_outline->FaceSize(10);
    font_info->ftfont_outline->CharMap(ft_encoding_unicode);
    #endif
  }

  return my_font_info;
}  

void vsx_font::unload()
{
  if (!my_font_info)
    return;

  if (my_font_info->texture)
    delete my_font_info->texture;

  glist.erase( my_font_info->name );
  delete my_font_info;
  my_font_info = 0x0;
}

void vsx_font::reinit(vsx_font_info* f_info,vsx_string<>font)
{
  if (f_info->type == 0)
  {
    vsxf filesystem;
    f_info->texture->load_png(base_path+font,true, &filesystem);
    return;
  }

  if (f_info->type == 1)
  {
    #ifndef VSX_FONT_NO_FT
    delete f_info->ftfont;
    delete f_info->ftfont_outline;
    f_info->ftfont = new FTGLPolygonFont((base_path+"resources/fonts/font"+font+".ttf").c_str());
    f_info->ftfont->FaceSize(10);
    f_info->ftfont->CharMap(ft_encoding_unicode);
    f_info->ftfont_outline = new FTGLOutlineFont((base_path+"resources/fonts/font"+font+".ttf").c_str());
    f_info->ftfont_outline->FaceSize(10);
    f_info->ftfont_outline->CharMap(ft_encoding_unicode);
    #endif
  }
}  

void vsx_font::reinit_all_active()
{
  for (std::map<vsx_string<>, vsx_font_info*>::iterator it = glist.begin(); it != glist.end(); ++it)
  {
    reinit((*it).second,(*it).first);
  }
}  

  vsx_vector3<> vsx_font::print(vsx_vector3<> p, const vsx_string<>& str, const vsx_string<>& font, float size = 1, vsx_string<>colors) {
    if (!my_font_info) {
      if (glist.find(font) != glist.end()) {
        my_font_info = glist[font];
      } else {
        vsxf filesystem;
        my_font_info = load(font, &filesystem);
      }
    }
    return print(p,str,size,colors);
  }

  vsx_vector3<> vsx_font::print(vsx_vector3<> p, const vsx_string<>& str, const float size = 1, vsx_string<>colors)
  {
    #ifndef VSXU_OPENGL_ES
      if (!my_font_info)
        VSX_ERROR_RETURN_V("no my_font_info", vsx_vector3<>());

      if (str == "")
        return p;

      bool use_colors = false;

      if (colors.size())
        use_colors = true;

      char current_color = 1;

      if (my_font_info->type == 0)
      {
        size_s = 0.37f*size;

        dx = 0;
        dy = 0;

        glPushMatrix();
        glTranslatef(p.x,p.y,p.z);

          if (!(*(my_font_info->texture)).bind())
          {
            vsx_printf(L"font could not bind texture!\n");

            return vsx_vector3<>();
          }

          colc = (char*)colors.c_str();

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

          ddy = size*1.05f;
          if (mode_2d)
          {
            ddy = -size*1.05f;
          }


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
              dy -= ddy;
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
          (*(my_font_info->texture))._bind();
        glPopMatrix();
        ep.x = dx+p.x;  // this might need to be fixed
        ep.y = dy+p.y;
        return ep;
      }
      else
      {
        #ifndef VSX_FONT_NO_FT
        double size_s = 0.5*size*0.1;
        glEnable(GL_BLEND);
        glPushMatrix();
          double dx = p.x;
          double dy = p.y;
        if (background) {
          glColor4f(background_color.r,background_color.g,background_color.b,background_color.a);
          vsx_vector ps = get_size(str,size);
          ps.x *= 1.05;
            glBegin(GL_QUADS);
                glVertex3f(dx,dy,dz);
                glVertex3f(dx,dy+size*1.05,dz);
                glVertex3f(dx+ps.x,dy+size*1.05,dz);
                glVertex3f(dx+ps.x,dy,dz);
            glEnd();
        }
        glColor4f(color.r,color.g,color.b,color.a);
        glTranslatef(p.x,p.y+size*0.2,p.z);
        if (outline_transparency > 0.3)
        glColor4f(color.r,color.g,color.b,color.a);
        else
        glColor4f(color.r,color.g,color.b,(1-outline_transparency)*color.a);

        glScalef(size*0.8*0.1,size*0.1,0.018);
        my_font_info->ftfont->Render(str.c_str());
        glColor4f(color.r,color.g,color.b,outline_transparency*color.a);
        glPopMatrix();
        #endif
      }
    #endif // VSXU_OPENGL_ES
    return vsx_vector3<>();
  }

vsx_vector3<> vsx_font::get_size(const vsx_string<>& str, float size = 1)
{
  #ifndef VSX_FONT_NO_FT
    if (!my_font_info) {
      return vsx_vector(0);
    }
    if (font[0] == '-') {
      float x1, y1, z1, x2, y2, z2;
      my_font_info->ftfont->BBox( str.c_str(), x1, y1, z1, x2, y2, z2);
      vsx_vector pp;
      pp.x = (x2*size-x1*size)*0.8*0.1;
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
  float x1, y1, z1, x2, y2, z2;
  if (my_font_info)
  {
    my_font_info->ftfont->BBox( str.c_str(), x1, y1, z1, x2, y2, z2);
  }
  p.x -= (x2*size-x1*size)*0.5*0.08;
  #endif
  return print(p,str,size);
}

vsx_vector3<> vsx_font::print_right(vsx_vector3<> p, const vsx_string<>& str, float size = 1)
{
  p.x -= (align*0.37f*size*(float)str.size());
  #ifndef VSX_FONT_NO_FT
    float x1, y1, z1, x2, y2, z2;
    if (my_font_info) {
      my_font_info->ftfont->BBox( str.c_str(), x1, y1, z1, x2, y2, z2);
    }
    //vsx_vector pp = p;
    p.x -= (x2*size-x1*size)*0.08;
  #endif
  return print(p,str,size);
}
  


