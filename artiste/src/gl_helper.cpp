/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
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

#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "gl_helper.h"

void draw_box(vsx_vector pos, float width, float height) {
  const GLfloat squareVertices[] = {
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
  
  /*
	glBegin(GL_QUADS);
			glVertex2f(pos.x, pos.y+height);
			glVertex2f(pos.x, pos.y);
			glVertex2f(pos.x+width, pos.y);
 			glVertex2f(pos.x+width, pos.y+height);
	glEnd();*/
}
void draw_box_c(vsx_vector pos, float width, float height) {
  const GLfloat squareVertices[] = {
    pos.x-width, pos.y-width,
    pos.x+width,  pos.y-width,
    pos.x-width,  pos.y+height,
    pos.x+width,   pos.y+height,
  };
  glDisableClientState(GL_COLOR_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, squareVertices);
  glEnableClientState(GL_VERTEX_ARRAY);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableClientState(GL_VERTEX_ARRAY);

  /*
  glBegin(GL_QUADS);
			glVertex2f(pos.x-width, pos.y+height);
			glVertex2f(pos.x-width, pos.y-height);
			glVertex2f(pos.x+width, pos.y-height);
 			glVertex2f(pos.x+width, pos.y+height);
	glEnd();*/
}
void draw_box_tex_c(vsx_vector pos, float width, float height) {

  const GLshort squareTexcoords[] = {
    0, 0,
    1, 0,
    0, 1,
    1, 1
  };

  const GLfloat squareVertices[] = {
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
  
  /*
  glBegin(GL_QUADS);
  	glTexCoord2f(0, 1);
  	glVertex2f(pos.x-width, pos.y+height);
  	glTexCoord2f(0, 0);
		glVertex2f(pos.x-width, pos.y-height);
  	glTexCoord2f(1, 0);
		glVertex2f(pos.x+width, pos.y-height);
  	glTexCoord2f(1,1);
		glVertex2f(pos.x+width, pos.y+height);
	glEnd();*/
}

void draw_box_tex(vsx_vector pos, float width, float height) {
  {
  const GLshort squareTexcoords[] = {
    0, 0,
    1, 0,
    0, 1,
    1, 1
  };

  const GLfloat squareVertices[] = {
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

  /*
  glBegin(GL_QUADS);
  	glTexCoord2f(0, 1);
  	glVertex2f(pos.x, pos.y+height);
  	glTexCoord2f(0, 0);
		glVertex2f(pos.x, pos.y);
  	glTexCoord2f(1, 0);
		glVertex2f(pos.x+width, pos.y);
  	glTexCoord2f(1,1);
		glVertex2f(pos.x+width, pos.y+height);
	glEnd();*/
}
/*
void draw_box_texf(float pos_x, float pos_y, float pos_z, float width, float height) {

  const GLshort squareTexcoords[] = {
    0, 0,
    1, 0,
    0, 1,
    1, 1
  };

  const GLfloat squareVertices[] = {
    pos_x-width,   pos_y-width,
    pos_x+width,   pos_y-width,
    pos_x-width,   pos_y+height,
    pos_x+width,   pos_y+height,
  };

  glVertexPointer(2, GL_FLOAT, 0, squareVertices);
  glTexCoordPointer(2, GL_SHORT, 0, squareTexcoords);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
*/
void draw_box_gradient(vsx_vector pos, float width, float height, vsx_color a, vsx_color b, vsx_color c, vsx_color d) {
	glBegin(GL_QUADS);
    a.gl_color();
			glVertex2f(pos.x, pos.y+height);
		b.gl_color();
			glVertex2f(pos.x, pos.y);
		c.gl_color();
			glVertex2f(pos.x+width, pos.y);
		d.gl_color();
 			glVertex2f(pos.x+width, pos.y+height);
	glEnd();
}

void draw_box_border(vsx_vector pos, vsx_vector size, float dragborder) {
  float pysy = pos.y+size.y;
  float pxsx = pos.x+size.x;
  const GLfloat squareVertices[] = {
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

  /*
  
  glBegin(GL_QUADS);
  	glVertex2f(pos.x,pos.y+size.y);
  	glVertex2f(pos.x+dragborder, pos.y+size.y);
  	glVertex2f(pos.x+dragborder, pos.y);
  	glVertex2f(pos.x,pos.y);
  
  	glVertex2f(pos.x+size.x,pos.y+size.y);
  	glVertex2f(pos.x+size.x-dragborder, pos.y+size.y);
  	glVertex2f(pos.x+size.x-dragborder, pos.y);
  	glVertex2f(pos.x+size.x,pos.y);
  
  	glVertex2f(pos.x,           pos.y+size.y-dragborder);
  	glVertex2f(pos.x+size.x,    pos.y+size.y-dragborder);
  	glVertex2f(pos.x+size.x,    pos.y+size.y);
  	glVertex2f(pos.x,           pos.y+size.y);
  
  	glVertex2f(pos.x,          pos.y+dragborder);
  	glVertex2f(pos.x+size.x,   pos.y+dragborder);
  	glVertex2f(pos.x+size.x,   pos.y);
  	glVertex2f(pos.x,          pos.y);
  glEnd();*/
}
 
vsx_vector vsx_vec_viewport() {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  return vsx_vector(viewport[2],viewport[3],0);  
}
