/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2003-2014
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

#ifndef VSX_WIDGET_DIALOG_MESSAGEBOX
#define VSX_WIDGET_DIALOG_MESSAGEBOX

#include "vsx_widget_window.h"

#include "widgets/vsx_widget_button.h"
#include "widgets/vsx_widget_label.h"


class dialog_messagebox : public vsx_widget_window {
public:
  bool allow_resize_y;

  void command_process_back_queue(vsx_command_s *t) {
    if (t->cmd == "ok") {
      k_focus = parent;
      a_focus = parent;
      _delete();
    }
    visible = 0;
  }
  void init() {
    if (!init_run) vsx_widget_window::init();
  }

  virtual void init_children() {
  }

  void show(vsx_string<>value)
  {
    VSX_UNUSED(value);

    show();
  }


  virtual bool event_key_down(uint16_t key)
  {
    VSX_UNUSED(key);
    k_focus = parent;
    a_focus = parent;
    _delete();
    return true;
  }

  void show() {
    visible = 1;
    pos.x = 0.5-size.x/2;
    pos.y = 0.5-size.y/2;
  }

  dialog_messagebox(vsx_string<>title_,vsx_string<>hint)
  {
    init_run = false;
    init();
    topmost = true;
    init_run = true;
    vsx_widget *button1 = add(new vsx_widget_button,"");
    std::vector<vsx_widget*> labels;
    vsx_nw_vector< vsx_string<> > lines;
    vsx_string<> deli = "|";
    vsx_string_helper::explode(hint,deli,lines);
    for (unsigned long i = 0; i < lines.size(); i++) {
      vsx_widget* b = add(new vsx_widget_label,"");
      labels.push_back(b);
    }
    vsx_widget::init_children();

    size_t max_len = 0;
    for (unsigned long i = 0; i < lines.size(); ++i)
      if (lines[i].size() > max_len)
        max_len = lines[i].size();

    set_size(vsx_vector3<>((float)max_len * 0.0094 , 0.10+(float)lines.size() * 0.017));

    float y = size.y*0.5+0.02;
    unsigned long i;
    for (i = 0; i < lines.size(); ++i) {
      labels[i]->set_pos(vsx_vector3<>(size.x*0.5, y));
      labels[i]->target_size.y = labels[i]->size.y = 0.018;
      labels[i]->title = lines[i];
      y -= 0.017;
    }
    y -= 0.017;

    button1->title = "ok";
    button1->set_pos(vsx_vector3<>(size.x*0.5,y));//(size.x-button1->size.x*0.5)*0.5,y));
    button1->commands.adds(4,"ok","ok","");

    title = title_;
    allow_resize_x = false;
    allow_resize_y = false;
    visible = 1;
    set_pos(vsx_vector3<>(0.5-size.x/2,0.5-size.y/2));
    k_focus = this;

  }
};

#endif
