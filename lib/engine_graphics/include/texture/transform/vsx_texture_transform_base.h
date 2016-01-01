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

#pragma once

#include <vsx_gl_state.h>

class vsx_texture_transform_base {
protected:
  int valid;
  vsx_texture_transform_base* previous_transform;
public:

  vsx_texture_transform_base(vsx_texture_transform_base* prev_trans) :
    previous_transform(prev_trans)
  {}

  void set_previous_transform(vsx_texture_transform_base* prev_trans)
  {
    previous_transform = prev_trans;
  }

  virtual inline int is_transform()
  {
    return valid;
  }

  virtual void transform()
  {
    if(previous_transform)
      previous_transform->transform();
  }

  virtual ~vsx_texture_transform_base()
  {}
};
