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

#include "vsx_gl_global.h"
#include <map>
#include <list>
#include <vector>
#include <command/vsx_command.h>
#include <string/vsx_string_helper.h>
#include <texture/vsx_texture.h>
#include "vsx_font.h"
#include <vsx_widget.h>
#include <widgets/vsx_widget_panel.h>
#include <widgets/vsx_widget_base_edit.h>
#include <widgets/vsx_widget_popup_menu.h>
#include <widgets/vsx_widget_button.h>
#include <stdlib.h>
#include <gl_helper.h>

vsx_widget_base_edit::vsx_widget_base_edit() {
  enable_syntax_highlighting = true;
  enable_line_action_buttons = false;
  title ="edit";
  widget_type = VSX_WIDGET_TYPE_EDITOR;
  render_type = render_2d;
  coord_type = VSX_WIDGET_COORD_CENTER;
  keywords["{"] = 3;
  keywords["}"] = 3;
  keywords["("] = 3;
  keywords[")"] = 3;
  keywords["<"] = 3;
  keywords[">"] = 3;
  keywords["."] = 3;
  keywords["["] = 3;
  keywords["]"] = 3;
  keywords[";"] = 3;
  keywords["-"] = 3;
  keywords["="] = 3;
  keywords["*"] = 3;
  keywords["/"] = 3;
  keywords[""]  = 3;
  keywords[";"] = 3;
  keywords["const"] = 4;
  keywords["varying"] = 4;
  keywords["uniform"] = 4;
  keywords["void"] = 4;
  keywords["float"] = 4;
  keywords["vec2"] = 4;
  keywords["vec3"] = 4;
  selected_line = -1;
  selected_line_highlight = false;
  editing_enabled = true;
  single_row = false;

  mirror_keystrokes_object = 0;
  mirror_mouse_move_object = 0;
  mirror_mouse_move_passive_object = 0;
  mirror_mouse_down_object = 0;
  mirror_mouse_up_object = 0;
  mirror_mouse_double_click_object = 0;
  filter_string_enabled = false;
  support_interpolation = false;

  font_size = 0.004f;
  caretx = 0;
  carety = 0;
  scroll_x = 0;
  scroll_y = 0;
  scrollbar_pos_x = 0;
  scrollbar_pos_y = 0;
  longest_line = 0;
  longest_y = 0;
  updates = 0;
  process_characters = true;
  num_hidden_lines = 0;

  font.syntax_colors[0] = vsx_widget_skin::get_instance()->get_color(14); // base color
  font.syntax_colors[1] = vsx_color<>(0.8f, 0.8f, 0.8f, 1.0f); // comment
  font.syntax_colors[2] = vsx_color<>(0, 1, 1, 1);
  font.syntax_colors[3] = vsx_color<>(153.0f/255.0f, 204.0f/255.0f, 1, 1);

  menu = add(new vsx_widget_popup_menu,".edit_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "clear", "clear","a"); //build a menu
  menu->commands.adds(VSX_COMMAND_MENU, "font_size>;very small", "font_size","0.002"); //build a menu
  menu->commands.adds(VSX_COMMAND_MENU, "font_size>;small", "font_size","0.004"); //build a menu
  menu->commands.adds(VSX_COMMAND_MENU, "font_size>;normal", "font_size","0.008"); //build a menu
  menu->commands.adds(VSX_COMMAND_MENU, "font_size>;large", "font_size","0.016"); //build a menu
  menu->commands.adds(VSX_COMMAND_MENU, "font_size>;larger", "font_size","0.022"); //build a menu
  menu->commands.adds(VSX_COMMAND_MENU, "font_size>;largest", "font_size","0.032"); //build a menu
  menu->size.x = 0.2f;
  menu->size.y = 0.5f;
  menu->init();

  lines[0] = "";
  lines_visible.push_back(0);

  calculate_scroll_size();
}

void vsx_widget_base_edit::command_process_back_queue(vsx_command_s *t) {
  updates++;
  if (t->cmd == "action")
  {
    // TODO: take into account hidden lines
    backwards_message("editor_action "+vsx_string_helper::i2s(id)+" "+lines[(size_t)scroll_y + vsx_string_helper::s2i(t->parts[1])]);
    return;
  }

  if (t->cmd == "font_size")
  {
    font_size = vsx_string_helper::s2f(t->parts[1]);
    return;
  }

  if (t->cmd == "clear") {
    k_focus = this;
    set_string("");
    return;
  }

  command_q_b.add(t);
}

void vsx_widget_base_edit::process_line(int n_line) {
  req(process_characters);

  int i = n_line;
  vsx_string<>b;

  char fill = (char)0x01;
  if (lines[i].size() >= 2)
    if (lines[i][0] == '/' && lines[i][1] == '/')
      fill = (char)0x02;

  for (size_t j = 0; j < lines[i].size(); ++j)
    b = b + fill;

  if (fill == (char)0x01)
    for (std::map<vsx_string<>,char>::iterator it = keywords.begin(); it != keywords.end(); ++it)
    {
      char f = (char)(*it).second;
      if (enable_syntax_highlighting)
        b = vsx_string_helper::str_replace_char_pad((*it).first, vsx_string<>(f), lines[i], b);
    }

  if (n_line > (int)lines_p.size()-1)
    lines_p.push_back(b);
  else
    lines_p[n_line] = b;
}

void vsx_widget_base_edit::process_lines() {
  req(process_characters);

  lines_p.clear();
  for (unsigned long i = 0; i < lines.size(); ++i)
  {
    vsx_string<>b;

    char fill = (char)0x01;
    if (lines[i].size() >= 2)
      if (lines[i][0] == '/' && lines[i][1] == '/')
        fill = (char)0x02;

    for (size_t j = 0; j < lines[i].size(); ++j)
      b = b + fill;

    if (fill == (char)0x01)
      for (std::map<vsx_string<>,char>::iterator it = keywords.begin(); it != keywords.end(); ++it)
      {
        char f = (char)(*it).second;
        b = vsx_string_helper::str_replace_char_pad((*it).first, vsx_string<>(f), lines[i], b);
      }
    lines_p.push_back(b);
  }
}

void vsx_widget_base_edit::calculate_scroll_size() {
  size_t t_longest_line = 0;
  foreach (lines, i)
    if (lines[i].size() > t_longest_line)
      t_longest_line = lines[i].size();

  t_longest_line += 3;
  if (t_longest_line > (size_t)longest_line) 
    longest_line = (float)t_longest_line;
  characters_width = (float)floor(target_size.x/(font_size*0.37));
  characters_height = (float)floor(target_size.y/(font_size));

    scroll_x_max = longest_line;
    scroll_x_size = 1-(float)(longest_line - characters_width)/longest_line;

  longest_y = 0;
  if (longest_y < (float)lines.size()+3)
    longest_y = (float)lines.size()+3-num_hidden_lines;

  if (scroll_x_size > 1) scroll_x_size = 1;

  scroll_y_max = longest_y;
  float ff = longest_y - characters_height;
  if (ff < 0) ff = 0;
  scroll_y_size = 1-(float)(ff)/longest_y;
  if (scroll_y_size > 1) scroll_y_size = 1;
}

void vsx_widget_base_edit::set_string(const vsx_string<>& str) {
  lines.clear();
  vsx_string<>deli = "\n";
  vsx_string<>f = vsx_string_helper::str_replace<char>("\r","",str);
  vsx_string_helper::explode(f, deli, lines);
  lines_visible.clear();
  for (unsigned long i = 0; i < lines.size(); i++) lines_visible.push_back(0);
  longest_line = 0;
  scrollbar_pos_x = 0;
  scrollbar_pos_y = 0;

  foreach (lines, i)
    if (lines[i].size() > longest_line)
      longest_line = (float)lines[i].size();

  // hide eventual action buttons
  for (size_t i = 0; i < action_buttons.size(); i++)
    action_buttons[i]->visible = 0.0f;

  process_lines();
  calculate_scroll_size();
}

void vsx_widget_base_edit::set_filter_string(vsx_string<>&filter_string)
{
  if (filter_string.size() == 0) {
    if (filter_string_enabled) lines_visible = lines_visible_stack;
    filter_string_enabled = false;
    return;
  }

  if (!filter_string_enabled) lines_visible_stack = lines_visible;

  for (unsigned long i = 0; i < lines.size(); ++i) {
    if (lines[i].find(filter_string) != -1) lines_visible[i] = 0;
    else lines_visible[i] = 1;
  }
  scroll_y = 0;
  scrollbar_pos_y = 0;
  filter_string_enabled = true;
}

void vsx_widget_base_edit::caret_goto_end()
{
  event_key_down(VSX_SCANCODE_END);
}

void vsx_widget_base_edit::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  vsx_widget::event_mouse_down(distance,coords,button);
  if (!lines.size())
    return;

  if (button == 0)
  {
    m_focus = this;
    caretx = (int)floor(distance.corner.x/(font_size*0.37));
    carety = (int)floor(((target_size.y-distance.corner.y)/font_size));
    if (carety < 0) carety = 0;
    if (carety > lines.size()-num_hidden_lines-1-scroll_y) carety = (int)floor(lines.size()-num_hidden_lines-1-scroll_y);

    if (lines_visible.size() == lines.size())
    {
      if (selected_line_highlight) {
        int clicked_line = 0;
        clicked_line = carety + (int)scroll_y;
        int real_line = 0;
        int num_visible_found = 0;
        while (num_visible_found < clicked_line && real_line < (int)(lines.size()-1))
        {
          if (lines_visible[real_line] == 0) num_visible_found++;
          real_line++;
        }
        while (lines_visible[real_line] != 0 && real_line < (int)(lines.size()-1)) real_line++;
        selected_line = real_line;
    }
    }
    if (caretx > lines[(int)(carety+scroll_y)].size()-scroll_x)
      event_key_down(VSX_SCANCODE_END);
  }
  if (mirror_mouse_down_object != 0)
    mirror_mouse_down_object->event_mouse_down(distance, coords, button);
}

void vsx_widget_base_edit::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  if (mirror_mouse_up_object)
    return (void)mirror_mouse_up_object->event_mouse_up(distance, coords,button);
  vsx_widget::event_mouse_up(distance, coords, button);
}

void vsx_widget_base_edit::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  if (mirror_mouse_move_object)
    return (void)mirror_mouse_move_object->event_mouse_move(distance, coords);
  vsx_widget::event_mouse_move(distance, coords);
}

void vsx_widget_base_edit::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  if (mirror_mouse_move_passive_object)
    return (void)mirror_mouse_move_passive_object->event_mouse_move_passive(distance, coords);
  vsx_widget::event_mouse_move_passive(distance, coords);
}

void vsx_widget_base_edit::event_text(wchar_t character_wide, char character)
{
  req(editing_enabled);

  if (allowed_chars.size())
    if (allowed_chars.find(character) == -1)
      return;

  if (lines.size() == 1 && !lines_visible.size())
    lines_visible.push_back(0);

  lines[carety + (int)scroll_y].insert(caretx+(int)scroll_x, character);
  updates++;
  ++caretx;
  if ((size_t)caretx > lines[carety+(int)scroll_y].size()-(int)scroll_x)
    caretx = (int)lines[carety+(int)scroll_y].size()-(int)scroll_x;

  int t_scroll_x = (int)scroll_x;

  if (caretx+(int)scroll_x > characters_width)
    ++scroll_x;

  caretx -= (int)scroll_x - t_scroll_x;
  process_line(carety+(int)scroll_y);
  if (mirror_keystrokes_object)
    mirror_keystrokes_object->event_text(character_wide, character);
}

void vsx_widget_base_edit::event_mouse_wheel(float y)
{
  if (single_row)
    return;

  float ff = longest_y - characters_height;
  scrollbar_pos_y -= y / ff;
  if (scrollbar_pos_y < 0)
    scrollbar_pos_y = 0;

  if (scrollbar_pos_y > 1.0f)
    scrollbar_pos_y = 1.0f;
}

int count_whitespaces(vsx_string<>& s)
{
  size_t i = 0;
  while (i < s.size() && s[i] == ' ')
    i++;
  return (int)i;
}

void vsx_widget_base_edit::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  if (mirror_mouse_double_click_object) {
    mirror_mouse_double_click_object->event_mouse_double_click(distance,coords,button);
  }

  if (button == 0)
  {
    // 0 0 a c
    // 1 0  +b
    // 2 0    +c
    // 3 0    +d
    // 4 0  +e
    // 5 0   +f
    // 6 1    +g
    // 7 1    +g
    // 8 0   +g
    // 9 0  +g
    // A 0 g

    if (filter_string_enabled) return;



    event_mouse_down(distance, coords,button);
    // 1. locate real line

    int real_line = selected_line;
    if (real_line < 0) return;
    /*int clicked_line = carety + (int)scroll_y;
    int real_line = 0;
    int num_visible_found = 0;

    while (num_visible_found < clicked_line) {
      if (lines_visible[real_line] == 0) num_visible_found++;
      real_line++;
    }
    while (lines_visible[real_line] != 0) real_line++;
    */
    //printf("real_line: %d\n", real_line);
    if (real_line >= (int)lines.size()-1) return;
    int real_white = count_whitespaces(lines[real_line]);
    if (real_white < count_whitespaces(lines[real_line+1]))
    {
      real_line++;
      int new_state = lines_visible[real_line]; // if next line is hidden (>1) set it to 0
      while (real_line < (int)lines.size() && count_whitespaces(lines[real_line]) > real_white)
      {
        //printf("real line: %d\n",real_line);
        if (!new_state) // next line is not hidden!
        {
          if (lines_visible[real_line] == 0) num_hidden_lines++;  // a previously unhidden line is now hidden
          lines_visible[real_line]++; // hide line

          //float ff = longest_y - characters_height;
          //scrollbar_pos_y += (1.0 / ff) * 0.10f;
          //if (scrollbar_pos_y > 1.0f) scrollbar_pos_y = 1.0f;
        } else
        {
          lines_visible[real_line]--;
          if (lines_visible[real_line] == 0) num_hidden_lines--; // a previously hidden line is now unhidden
          //float ff = longest_y - characters_height;
          //scrollbar_pos_y -= 1.0 / ff;
          //if (scrollbar_pos_y < 0.0f) scrollbar_pos_y = 0.0f;
        }
        real_line++;
      }
    }
    calculate_scroll_size();
    // 2. check next line if it's more indented
    // 3. if it's not - do nothing, we're at lowest level
    // 4. else hide lines until end or less indented
  }
}

void vsx_widget_base_edit::fold_all()
{
  int prev_white = 0;
  int val = 0;
  int white_frac = 0;
  for (unsigned long i = 0; i < lines_visible.size(); i++)
  {
    int white = count_whitespaces(lines[i]);
    if (white_frac == 0 && white > prev_white)
      white_frac = white;

    if (white_frac == 0)
      val = 0;
    else
      val = white / white_frac;

    lines_visible[i] = val;
  }
  calculate_scroll_size();
}

void vsx_widget_base_edit::i_draw()
{
  if (!lines.size())
    return;

  allow_move_y = allow_move_x = false;
  //base_draw();
  scroll_x = round(scrollbar_pos_x*(longest_line-characters_width));
  if (scroll_x < 0) scroll_x = 0;
  float ff = longest_y - characters_height;
  if (ff < 0) ff = 0;
  scroll_y = round(scrollbar_pos_y*(ff));
  vsx_vector3<> pos = get_pos_p();
  pos.x -= target_size.x*0.5f;
  pos.y -= target_size.y*0.5f;

  if (render_type == render_2d)
    pos.z = 0.0f;

  vsx_widget_skin::get_instance()->set_color_gl(18);

  draw_box(pos, target_size.x, target_size.y);


  pos.y += target_size.y-font_size;

  render_caret();

  calculate_scroll_size();

  bool run = true;

  int num_visible_found = 0;
  int real_line = 0;
  while (num_visible_found < scroll_y && real_line < (int)(lines.size()-1)) {
    if (lines_visible[real_line] == 0)
      num_visible_found++;
    real_line++;
  }
  req(lines_visible.size());

  while (lines_visible[real_line] != 0 && real_line < (int)(lines.size()-1))
    real_line++;

  int curline = real_line;
  vsx_vector3<> pp = pos;
  //std::vector <vsx_string<> >::iterator it = lines.begin();
  font.syntax_colors[0] = vsx_widget_skin::get_instance()->get_color(14);
  int cur_render_line = 0;
  if (selected_line_highlight)
    font.color = vsx_widget_skin::get_instance()->get_color(14);

  if (draw_line_numbers)
    font.print(pp + vsx_vector3<>(0.0f, font_size, 0.0f),
             vsx_string_helper::i2s(carety + (int)scroll_y + 1),
             font_size);

  if (scroll_y < lines.size())
  if (curline < (int)lines.size())
  while (run)
  {
    if (lines_visible[curline] == 0)
    {
      // save current size for speed in a variable
      long cursize = (long)lines[curline].size();
      if (selected_line_highlight && curline == selected_line)
      {
        vsx_widget_skin::get_instance()->set_color_gl(15);
        font.syntax_colors[0] = vsx_widget_skin::get_instance()->get_color(16); // base color
        draw_box(pp,target_size.x,font_size);
      }

      if (cursize-(long)scroll_x >= (long)characters_width)
        font.print(pp,
                   lines[curline].substr((int)scroll_x,(int)characters_width),
                   font_size,
                   lines_p[curline].substr((int)scroll_x,(int)characters_width));
      else
        if (scroll_x < cursize)
          font.print(
               pp,
               lines[curline].substr( (int)scroll_x, (int)(lines[curline].size()-floor(scroll_x)) ),
               font_size,
               lines_p[curline].substr((int)scroll_x,(int)(lines[curline].size()-scroll_x))
             );



      if (enable_line_action_buttons)
      {
        if (cur_render_line+1 > (int)action_buttons.size())
        {
          vsx_widget* new_action_button = add(new vsx_widget_button,"ab_"+vsx_string_helper::i2s(cur_render_line));
          new_action_button->init();
          new_action_button->set_size( vsx_vector3<>(0.005f, font_size) );
          new_action_button->coord_related_parent = false;
          new_action_button->render_type = this->render_type;
          new_action_button->title = "x";
          new_action_button->commands.adds(4,"","action",vsx_string_helper::i2s(cur_render_line));
          action_buttons.push_back(new_action_button);
        }
        action_buttons[cur_render_line]->set_pos(pp + vsx_vector3<>(target_size.x-0.0025f,font_size*0.5f));
        action_buttons[cur_render_line]->visible = 1.0f;
      }

      pp.y -= font_size;

      if (selected_line_highlight && curline == selected_line)
      {
        font.syntax_colors[0] = vsx_widget_skin::get_instance()->get_color(14);
      }
      cur_render_line++;
    }
    ++curline;
    if (cur_render_line > (long)characters_height-1 || (long)curline > (long)lines.size()-1)
      run = false;
  }

  font.color = vsx_color<>(1,1,1,1);
}

void vsx_widget_base_edit::render_caret()
{
  req(k_focus == this);
  req(editing_enabled);

  vsx_vector3<> pos = get_pos_p();
  pos.x -= target_size.x*0.5f;

  if (render_type == render_2d)
  {
    pos.y -= target_size.y * 0.5f;
    pos.y -= font_size*(float)(carety);
  }

  if (render_type == render_3d)
  {
    pos.y += target_size.y*0.5f;
    pos.y -= font_size*(float)(carety + 1);
  }

  pos.x += (float)caretx*font_size*0.37f;

  float tt = (float)((int)(vsx_widget_time::get_instance()->get_time()*3000) % 1000)*0.001f;

  if (selected_line_highlight)
    font.color = vsx_widget_skin::get_instance()->get_color(17);

  font.color.a = 1 - tt;
  font.print(pos, "_", font_size);
  font.color.a = 1;
}


bool vsx_widget_base_edit::event_key_down(uint16_t key) {
  if (!editing_enabled) return true;
  size_t it = 0;
  std::vector<int>::iterator itlv = lines_visible.begin();

  std::vector <vsx_string<> >::iterator itp = lines_p.begin();
  int c2 = 0;
  scroll_x = floor(scroll_x);
  vsx_string<>tempstring;
  vsx_string<>tempstring2;
  switch(key)
  {
    // arrow left
    case VSX_SCANCODE_LEFT:
      --caretx;
      if (caretx < 0) {
        if (fabsf(scroll_x) > 0.0f) {
          ++caretx;
          --scroll_x;
          //fix_pos();
        } else
        if (carety) {
          --carety;
        event_key_down(VSX_SCANCODE_END);
        } else caretx = 0;
      }
      break;

    // arrow right
    case VSX_SCANCODE_RIGHT:
      ++caretx;
      if ((size_t)caretx > lines[carety+(int)scroll_y].size()-(int)scroll_x) {
        event_key_down(VSX_SCANCODE_DOWN);
        event_key_down(VSX_SCANCODE_HOME);
      }
      if (caretx > characters_width-3) {
        --caretx;
        ++scroll_x;
      }
      break;

    // arrow up
    case VSX_SCANCODE_UP:
      if (!single_row) {
        --carety;
        if (carety < 0) {
          carety = 0;
          if (fabsf(scroll_y) > 0.0f) {
            --scroll_y;
          }
        }
        if ((size_t)caretx > lines[carety+(int)scroll_y].size()-(int)scroll_x)
        event_key_down(VSX_SCANCODE_END);
        }
      break;

    // page up
    case VSX_SCANCODE_PAGEUP:
      if (!single_row) {
        for (int zz = 0; zz < characters_height*0.95; ++zz) {
        event_key_down(VSX_SCANCODE_UP);
        }
      }
      break;

    // arrow down
    case VSX_SCANCODE_DOWN:
      if (!single_row) {
        ++carety;
        if (carety > lines.size()-1-scroll_y) carety = (int)((float)lines.size()-1.0f-scroll_y);
        if (carety > characters_height-2) {
          ++scroll_y;
          --carety;
        }
        if ((size_t)caretx > lines[carety+(int)scroll_y].size()-(int)scroll_x)
         caretx = (int)lines[carety+(int)scroll_y].size()-(int)scroll_x;
      }
      break;

    // page down
    case VSX_SCANCODE_PAGEDOWN:
      if (!single_row) {
        for (int zz = 0; zz < characters_height*0.95; ++zz) {
        event_key_down(VSX_SCANCODE_DOWN);
        }
      }
      break;
    // home
    case VSX_SCANCODE_HOME:
      scroll_x = 0;
      caretx = 0;
      break;

    // end
    case VSX_SCANCODE_END:
      caretx = (int)lines[carety+(int)scroll_y].size()-(int)scroll_x;
      if (caretx > characters_width-3) {
        scroll_x += caretx - characters_width+3;
        caretx = (int)characters_width-3;
      }

      if (caretx < 0) {
        scroll_x += caretx-5;
        if (scroll_x < 0) scroll_x = 0;
        caretx = (int)lines[carety+(int)scroll_y].size()-(int)scroll_x;
      }
    break;

    // backspace
    case VSX_SCANCODE_BACKSPACE:
      if (caretx+(int)scroll_x) {
        lines[carety+(int)scroll_y].erase(caretx-1+(int)scroll_x,1);
        --caretx;
        if (caretx < 0) {--scroll_x; ++caretx;}
        process_line(carety+(int)scroll_y);
      } else {
        if ((int)(scroll_y + carety))
        {
          while (c2 < carety+scroll_y)
          {
            ++c2;
            ++it;
            ++itp;
            ++itlv;
          }
          tempstring = lines[carety+(int)scroll_y];
          lines.remove_index(it);
          lines_p.erase(itp);
          lines_visible.erase(itlv);
        event_key_down(VSX_SCANCODE_UP);
        event_key_down(VSX_SCANCODE_END);
          lines[carety+(int)scroll_y] += tempstring;
          lines_p[carety+(int)scroll_y] += tempstring;
          process_line(carety+(int)scroll_y);
          process_line(carety+(int)scroll_y+1);
        }
      }
      if (mirror_keystrokes_object) mirror_keystrokes_object->event_key_down(key);
    break;

    // delete
    case VSX_SCANCODE_DELETE:
      event_key_down(VSX_SCANCODE_RIGHT);
      event_key_down(VSX_SCANCODE_BACKSPACE);
      process_line(carety+(int)scroll_y);
      if (mirror_keystrokes_object) mirror_keystrokes_object->event_key_down(key);
    break;

    // enter / return
    case VSX_SCANCODE_RETURN:
    case VSX_SCANCODE_RETURN2:
      if (single_row) {
        vsx_string<>d;
        if (command_prefix.size()) d = command_prefix+" ";
        command_q_b.add_raw(d+lines[0]);
        parent->vsx_command_queue_b(this);
      } else {
        if ((size_t)caretx+(size_t)scroll_x > lines[carety+(int)scroll_y].size()) event_key_down(-35);
        while (c2 < carety+(int)scroll_y) { ++c2; ++it; ++itp; ++itlv; }
        ++it;
        ++itp;
        ++itlv;
        tempstring = lines[carety+(int)scroll_y].
          substr(
            caretx + (int)scroll_x,   
            (int)lines[carety+(int)scroll_y].size() - (caretx + (int)scroll_x)
          );
        tempstring2 = lines[carety+(int)scroll_y].substr(0,caretx+(int)scroll_x);
        lines[carety+(int)scroll_y] = tempstring2;
        lines.insert(it,tempstring);
        lines_visible.insert(itlv,0);

        tempstring = lines_p[carety+(int)scroll_y].substr(
            caretx + (int)scroll_x,
            (int)lines_p[carety+(int)scroll_y].size() - (caretx+(int)scroll_x)
          );
        tempstring2 = lines_p[carety+(int)scroll_y].substr(0,caretx+(int)scroll_x);
        lines_p[carety+(int)scroll_y] = tempstring2;
        lines_p.insert(itp,tempstring);

        event_key_down(VSX_SCANCODE_DOWN);
        event_key_down(VSX_SCANCODE_HOME);
        process_line(carety-1+(int)scroll_y);
        process_line(carety+(int)scroll_y);
      }
      if (mirror_keystrokes_object) mirror_keystrokes_object->event_key_down(key);
    break;

    // esc
    case VSX_SCANCODE_ESCAPE:
      if (single_row) {
        command_q_b.add_raw("cancel");
        parent->vsx_command_queue_b(this);
      } else
      a_focus = k_focus = parent;
    break;
  }
  calculate_scroll_size();
  if (longest_line-characters_width <= 0) {
    scrollbar_pos_x = 0;
  } else {
    scrollbar_pos_x = (float)scroll_x/(longest_line-characters_width);
  }
  if (longest_y-characters_height <= 0) {
    scrollbar_pos_y = 0;
  } else {
    scrollbar_pos_y = (float)scroll_y/(longest_y-characters_height);
  }
  //printf("scroll_x: %f scroll_y: %f\n caretx: %d  carety: %d\n",scroll_x,scroll_y,caretx,carety);
  return false;
}

vsx_string<> vsx_widget_base_edit::get_string()
{
  vsx_string<> nl("\n");
  return vsx_string_helper::implode(lines,nl);
}

vsx_string<>vsx_widget_base_edit::get_line(unsigned long line)
{
  if (line < lines.size()) {
    return lines[line];
  }
  return "";
}


