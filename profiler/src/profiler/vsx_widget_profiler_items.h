/**
* Project: VSXu Profiler - Data collection and data visualizer
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2014
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

#ifndef VSX_WIDGET_PROFILER_ITEMS_H
#define VSX_WIDGET_PROFILER_ITEMS_H

#include <vsx_widget_window.h>
#include <widgets/vsx_widget_base_edit.h>

class vsx_widget_profiler_items : public vsx_widget_editor
{
  vsx_widget_profiler* profiler;

public:

  void set_profiler( vsx_widget_profiler* p )
  {
    profiler = p;
  }

  void extra_init()
  {
    editor->enable_syntax_highlighting = false;
    editor->mirror_mouse_double_click_object = this;
  }

  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);
    VSX_UNUSED(button);

    vsx_string<>deli = " ";
    vsx_string<>cur = editor->get_line(editor->selected_line);
    cur.trim_lf();
    vsx_nw_vector< vsx_string<> > parts;
    vsx_string_helper::explode(cur, deli, parts, 2);

    if (parts[0] == "thread")
      profiler->load_thread( vsx_string_helper::s2i( parts[1] ) );

    if (parts[0] == "plot")
      profiler->load_plot( vsx_string_helper::s2i( parts[1] ) );
  }
};

class vsx_widget_profiler_items_window : public vsx_widget_window
{
  vsx_widget_profiler_items* items;

public:

  virtual void init()
  {
    vsx_widget_window::init();

    allow_resize_x = true;
    allow_resize_y = true;

    items = (vsx_widget_profiler_items*)add((vsx_widget*)(new vsx_widget_profiler_items),"tree");
    items->init();

    font_size = 0.02;
    items->editor->set_font_size(0.012f);
    items->editor->editing_enabled = false;
    items->editor->selected_line_highlight = true;
    items->editor->enable_syntax_highlighting = false;
    items->editor->enable_line_action_buttons = true;

    title ="Profile Items";

    items->set_render_type(render_2d);
    items->show();
  }

  virtual void i_draw()
  {
    vsx_widget_window::i_draw();

    items->set_pos(vsx_vector3<>(size.x * 0.5,size.y * 0.5 - font_size * 0.5 + dragborder*0.5f));
  }

  void extra_init()
  {
    items->extra_init();
  }

  void set_profiler( vsx_widget_profiler* p )
  {
    items->set_profiler( p );
  }

  void set_string( vsx_string<>s )
  {
    items->set_string( s );
  }


};



#endif // VSX_WIDGET_SEQUENCE_TREE_H
