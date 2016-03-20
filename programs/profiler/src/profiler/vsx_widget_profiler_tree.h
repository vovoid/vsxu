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

#ifndef VSX_WIDGET_SEQUENCE_TREE_H
#define VSX_WIDGET_SEQUENCE_TREE_H

#include <vsx_widget_window.h>
#include <widgets/vsx_widget_editor.h>

class vsx_widget_profiler_tree : public vsx_widget_editor
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

    profiler->load_profile( editor->selected_line );
  }
};

class vsx_widget_profiler_tree_window : public vsx_widget_window
{
  vsx_widget_profiler_tree* tree;

public:

  virtual void init()
  {
    vsx_widget_window::init();

    allow_resize_x = true;
    allow_resize_y = true;

    tree = (vsx_widget_profiler_tree*)add((vsx_widget*)(new vsx_widget_profiler_tree),"tree");
    tree->init();

    font_size = 0.02;
    tree->editor->set_font_size(0.012f);
    tree->editor->editing_enabled = false;
    tree->editor->selected_line_highlight = true;
    tree->editor->enable_syntax_highlighting = false;
    tree->editor->enable_line_action_buttons = true;

    title = "Profiler Data Files";

    tree->set_render_type(render_2d);
    tree->show();
  }

  virtual void i_draw()
  {
    vsx_widget_window::i_draw();

    tree->set_pos(vsx_vector3<>(size.x * 0.5,size.y * 0.5 - font_size * 0.5 + dragborder*0.5f));
  }

  void extra_init()
  {
    tree->extra_init();
  }

  void set_profiler( vsx_widget_profiler* p )
  {
    tree->set_profiler( p );
  }

  void set_string( vsx_string<>s )
  {
    tree->set_string( s );
  }


};



#endif // VSX_WIDGET_SEQUENCE_TREE_H
