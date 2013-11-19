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


#ifndef VSX_TEXTURE_INFO_H
#define VSX_TEXTURE_INFO_H

#ifdef _WIN32
#include <windows.h>
#endif
#include "vsx_gl_global.h"

//--------------------------------------------------

class vsx_transform_obj {
protected:
  int valid;
	vsx_transform_obj* previous_transform;
public:
	vsx_transform_obj(vsx_transform_obj* prev_trans) : previous_transform(prev_trans) {}
	void set_previous_transform(vsx_transform_obj* prev_trans) {
		previous_transform = prev_trans;
	}

  virtual inline int is_transform()
  {
    return valid;
  }

	virtual void transform() {
		if(previous_transform) previous_transform->transform();
	};
	void operator()() {transform();}
  virtual ~vsx_transform_obj() {}
};

//--------------------------------------------------

class vsx_transform_neutral : public vsx_transform_obj
{
public:
  vsx_transform_neutral(vsx_transform_obj* prev_trans = 0)
    :
      vsx_transform_obj(prev_trans)

  {
    valid = 0;
  }
};

//--------------------------------------------------

class vsx_transform_translate : public vsx_transform_obj
{
	float x, y, z;
public:

  vsx_transform_translate(float x1, float y1, float z1, vsx_transform_obj* prev_trans = 0)
    :
      vsx_transform_obj(prev_trans),
      x(x1),
      y(y1),
      z(z1)
  {
    valid = 1;
  }

  void transform()
  {
		vsx_transform_obj::transform();
		glTranslatef(x, y, z);
	}

  void update(float nx = 0, float ny = 0, float nz = 0)
  {
		x = nx; y = ny; z = nz;
	}
};

//--------------------------------------------------

class vsx_transform_scale : public vsx_transform_obj
{
	float x, y, z;
public:
	vsx_transform_scale(float x1, float y1, float z1, vsx_transform_obj* prev_trans = 0)
    :
      vsx_transform_obj(prev_trans),
      x(x1),
      y(y1),
      z(z1)
  {
    valid = 1;
  }

  void transform()
  {
		vsx_transform_obj::transform();
		glScalef(x, y, z);
	}

  void update(float nx = 1, float ny = 1, float nz = 1)
  {
		x = nx; y = ny; z = nz;
	}
};

//--------------------------------------------------

class vsx_transform_rotate : public vsx_transform_obj{
	float angle, x, y, z;
public:
	vsx_transform_rotate(float angle1, float x1, float y1, float z1, vsx_transform_obj* prev_trans = 0)
    :
      vsx_transform_obj(prev_trans),
      angle(angle1),
      x(x1),
      y(y1),
      z(z1)
  {
    valid = 1;
  }

  void transform()
  {
		vsx_transform_obj::transform();
		glRotatef(angle, x, y, z);
	}

	void update(float nangle = 0, float nx = 1, float ny = 0, float nz = 0) {
		angle = nangle; x = nx; y = ny; z = nz;
	}
};



//--------------------------------------------------

#define VSX_TEXTURE_INFO_TYPE_OTHER 0
#define VSX_TEXTURE_INFO_TYPE_PNG 1
#define VSX_TEXTURE_INFO_TYPE_JPG 2

class vsx_texture_info
{
public:

  // size of the texture in pixels
  float size_x;
  float size_y;

  int type;
	GLuint ogl_id;
	GLuint ogl_type;

  vsx_texture_info() :
    size_x(0.0f),
    size_y(0.0f),
    type(VSX_TEXTURE_INFO_TYPE_OTHER),
    ogl_id(0),
    ogl_type(0)
  {}
};

class vsx_texture_glist_holder
{
public:
  // glist reference counter
  size_t references;
  vsx_texture_info texture_info;
  vsx_texture_glist_holder() :
    references(0)
  {
  }
};


#endif
