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


#ifndef VSX_GL_HELPER_H
#define VSX_GL_HELPER_H


#include <vsx_platform.h>
#include <math/vector/vsx_vector2.h>
#include <math/vector/vsx_vector3.h>
#include <color/vsx_color.h>
#include <engine_graphics_dllimport.h>




VSX_ENGINE_GRAPHICS_DLLIMPORT void draw_box(const vsx_vector2f &pos, const float &width, const float &height);
VSX_ENGINE_GRAPHICS_DLLIMPORT void draw_box_tex_upside_down(const vsx_vector3<> &pos, const float &width, const float &height);
VSX_ENGINE_GRAPHICS_DLLIMPORT void draw_box_c(const vsx_vector3<> &pos, const float &width, const float &height);
VSX_ENGINE_GRAPHICS_DLLIMPORT void draw_box_tex(const vsx_vector3<> &pos, const float &width, const float &height);
VSX_ENGINE_GRAPHICS_DLLIMPORT void draw_box_tex_c(const vsx_vector3<> &pos, const float &width, const float &height);


#define draw_box_texf(pos_x, pos_y, pos_z, width, height) {\
\
  const GLshort squareTexcoords[] = {\
    0, 0,\
    1, 0,\
    0, 1,\
    1, 1\
  };\
\
  const GLfloat squareVertices[] = {\
    pos_x-width,   pos_y-width,\
    pos_x+width,   pos_y-width,\
    pos_x-width,   pos_y+height,\
    pos_x+width,   pos_y+height,\
  };\
\
  glVertexPointer(2, GL_FLOAT, 0, squareVertices);\
  glTexCoordPointer(2, GL_SHORT, 0, squareTexcoords);\
  glEnableClientState(GL_VERTEX_ARRAY);\
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);\
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);\
  glDisableClientState(GL_VERTEX_ARRAY);\
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);\
}

#define draw_box_texf_correct(pos_x, pos_y, pos_z, width, height) {\
\
  const GLshort squareTexcoords[] = {\
    0, 1,\
    1, 1,\
    0, 0,\
    1, 0\
  };\
\
  const GLfloat squareVertices[] = {\
    pos_x-width,   pos_y-width,\
    pos_x+width,   pos_y-width,\
    pos_x-width,   pos_y+height,\
    pos_x+width,   pos_y+height,\
  };\
\
  glVertexPointer(2, GL_FLOAT, 0, squareVertices);\
  glTexCoordPointer(2, GL_SHORT, 0, squareTexcoords);\
  glEnableClientState(GL_VERTEX_ARRAY);\
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);\
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);\
  glDisableClientState(GL_VERTEX_ARRAY);\
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);\
}




VSX_ENGINE_GRAPHICS_DLLIMPORT void draw_box_gradient(const vsx_vector3<> &pos, float width, float height, const vsx_color<> &a, const vsx_color<> &b, const vsx_color<> &c, const vsx_color<> &d);
VSX_ENGINE_GRAPHICS_DLLIMPORT void draw_box_border(const vsx_vector3<> &pos, const vsx_vector3<> &size, const float &dragborder);


#endif
