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


#include <vsx_gl_global.h>
#include <gl_helper.h>

void draw_box(const vsx_vector<> &pos, const float &width, const float &height)
{
  const GLfloat squareVertices[] =
  {
    pos.x, pos.y,
    pos.x+width,  pos.y,
    pos.x,  pos.y+height,
    pos.x+width,   pos.y+height,
  };

  glDisableClientState(GL_COLOR_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, squareVertices);
  glEnableClientState(GL_VERTEX_ARRAY);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glDisableClientState(GL_VERTEX_ARRAY);
  
}
void draw_box_c(const vsx_vector<> &pos, const float &width, const float &height)
{
  const GLfloat squareVertices[] = {
    pos.x-width, pos.y-width,
    pos.x+width,  pos.y-width,
    pos.x-width,  pos.y+height,
    pos.x+width,   pos.y+height,
  };
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, squareVertices);
  glEnableClientState(GL_VERTEX_ARRAY);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableClientState(GL_VERTEX_ARRAY);

}
void draw_box_tex_c(const vsx_vector<> &pos, const float &width, const float &height)
{

  const GLshort squareTexcoords[] =
  {
    0, 0,
    1, 0,
    0, 1,
    1, 1
  };

  const GLfloat squareVertices[] =
  {
    pos.x-width, pos.y-width,
    pos.x+width,  pos.y-width,
    pos.x-width,  pos.y+height,
    pos.x+width,   pos.y+height
  };
  
  glVertexPointer(2, GL_FLOAT, 0, squareVertices);
  glTexCoordPointer(2, GL_SHORT, 0, squareTexcoords);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  
}

void draw_box_tex(const vsx_vector<> &pos, const float &width, const float &height)
{
  const GLshort squareTexcoords[] =
  {
    0, 0,
    1, 0,
    0, 1,
    1, 1
  };

  const GLfloat squareVertices[] =
  {
    pos.x, pos.y,
    pos.x+width,  pos.y,
    pos.x,  pos.y+height,
    pos.x+width,   pos.y+height
  };
  
  glVertexPointer(2, GL_FLOAT, 0, squareVertices);
  glTexCoordPointer(2, GL_SHORT, 0, squareTexcoords);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

}

void draw_box_gradient(const vsx_vector<> &pos, float width, float height, const vsx_color<> &a, const vsx_color<> &b, const vsx_color<> &c, const vsx_color<> &d)
{
	glBegin(GL_QUADS);
    glColor4f(
      a.r,
      a.g,
      a.b,
      a.a
    );
      glVertex2f(pos.x, pos.y+height);
    glColor4f(
      b.r,
      b.g,
      b.b,
      b.a
    );
      glVertex2f(pos.x, pos.y);
    glColor4f(
      c.r,
      c.g,
      c.b,
      c.a
    );
      glVertex2f(pos.x+width, pos.y);
    glColor4f(
      d.r,
      d.g,
      d.b,
      d.a
    );
 			glVertex2f(pos.x+width, pos.y+height);
	glEnd();
}

void draw_box_border(const vsx_vector<> &pos, const vsx_vector<> &size, const float &dragborder)
{
  float pysy = pos.y+size.y;
  float pxsx = pos.x+size.x;

  const GLfloat squareVertices[] =
  {
    pos.x                    , pysy,
    pos.x+dragborder         , pysy,
    pos.x+dragborder         , pos.y,
    pos.x                    , pos.y,

    pxsx                     , pysy,
    pxsx-dragborder          , pysy,
    pxsx-dragborder          , pos.y,
    pxsx                     , pos.y,

    pos.x                    , pysy-dragborder,
    pxsx                     , pysy-dragborder,
    pxsx                     , pysy,
    pos.x                    , pysy,

    pos.x                    , pos.y+dragborder,
    pxsx                     , pos.y+dragborder,
    pxsx                     , pos.y,
    pos.x                    , pos.y,

  };

  glVertexPointer(2, GL_FLOAT, 0, squareVertices);

  glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_QUADS, 0, 16);
  glDisableClientState(GL_VERTEX_ARRAY);

}
 
