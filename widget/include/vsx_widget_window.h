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

#ifndef VSX_WIDGET_WINDOW_H_
#define VSX_WIDGET_WINDOW_H_

#include <vsx_widget.h>

class vsx_widget_window : public vsx_widget {
	vsx_widget* button_close;

public:

	vsx_widget_window();
  virtual void init();
  virtual void i_draw();

  virtual vsx_vector3<> get_inner_size()
  {
    return vsx_vector3<>( size.x - dragborder * 2, size.y - font_size - dragborder * 2.0 );
  }

  virtual vsx_vector3<> get_inner_pos()
  {
    return vsx_vector3<>(size.x * 0.5,size.y * 0.5 - font_size * 0.5 + dragborder*0.5f);
  }


};



#endif /*VSX_WIDGET_WINDOW_H_*/
