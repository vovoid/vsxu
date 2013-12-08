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

#include "vsxfst.h"
#include "vsx_gl_global.h"
#include <vsx_vector_aux.h>
#include <gl_helper.h>
#include <iomanip>
#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsx_mouse.h"
#include "vsx_param.h"
#include "vsx_module.h"
// local includes
#include "vsx_widget_base.h"
#include "vsx_widget_sequence.h"
#include "vsx_widget_seq_chan.h"
#include "vsx_widget_timeline.h"
#include "vsx_widget_popup_menu.h"
#include "vsx_quaternion.h"
#include "vsx_bezier_calc.h"
#include "controllers/vsx_widget_controller_base.h"
#include "controllers/vsx_widget_controller_seq.h"
#include "dialogs/vsx_widget_window_statics.h"


vsx_color vsx_widget_seq_channel::col_temp_1;
vsx_color vsx_widget_seq_channel::col_temp_2;
vsx_texture* vsx_widget_seq_channel::mtex_blob = 0;

// WARNING: OVER BOOLEAN ASPHYXIATION IN THIS FILE!!!
// PROCEED WITH CAUTION!
// (better get your standard issue blanket and O² mask first)

void vsx_widget_seq_channel::init()
{
  index_hit = -1; // none hit

  widget_type = VSX_WIDGET_TYPE_SEQUENCE_CHANNEL;

  hidden_by_sequencer = true;
  display_exclusive = 0;
  is_controller = false;
  graph_color = vsx_color(1.0f, 1.0f, 1.0f);
  graph_oob_color = vsx_color(1, 0, 0, 1);
  y_start = -2;
  y_end = 2;
  if (channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_PARAMETER)
  {
    command_q_b.add_raw("pseq_p inject_get " + channel_name + " " + param_name);
    parent->vsx_command_queue_b(this);
  }
  else
  if (channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER)
  {
    command_q_b.add_raw("mseq_channel inject_get " + channel_name);
    parent->vsx_command_queue_b(this);
  }
  title = "channel";
  target_pos = pos;
  menu = add(new vsx_widget_popup_menu, ".comp_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "close", "menu_close", "");
  menu->commands.adds(VSX_COMMAND_MENU, "delete", "menu_remove", "");
  menu->size.x = 0.1;
  menu->init();

  visible = 0;

  menu_interpolation = add(new vsx_widget_popup_menu, ".interp_menu");
  menu_interpolation->size.x = size.x * 0.4;
  menu_interpolation->commands.adds(VSX_COMMAND_MENU, "enter value with keyboard",
      "menu_interp_keyboard", "0");
  menu_interpolation->commands.adds(VSX_COMMAND_MENU, "----------",
      "-", "0");
  menu_interpolation->commands.adds(VSX_COMMAND_MENU, "no interpolation",
      "menu_interp", "0");
  menu_interpolation->commands.adds(VSX_COMMAND_MENU, "linear interpolation",
      "menu_interp", "1");
  menu_interpolation->commands.adds(VSX_COMMAND_MENU, "cosine interpolation",
      "menu_interp", "2");
  menu_interpolation->commands.adds(VSX_COMMAND_MENU, "bezier interpolation",
      "menu_interp", "4");
  menu_interpolation->init();

  value_dialog = add(new dialog_query_string("set value","set value"),"menu_interp_keyboard_value");
  ((dialog_query_string*)value_dialog)->init();
}

// sends a full inject dump to the parent in the widget chain
void vsx_widget_seq_channel::send_parent_dump()
{
  std::vector<vsx_string> parts;

  for (std::vector<vsx_widget_param_sequence_item>::iterator it = items.begin(); it
      != items.end(); ++it)
  {
    // delay;interp;value
    parts.push_back(
      f2s((*it).get_total_length()) + ";" +
      i2s((*it).get_interpolation()) + ";" +
      base64_encode((*it).get_value_interpolation())
    );
  }

  if (parts.size())
  {
    vsx_string deli = "|";
    command_q_b.add_raw("update " + name + " " + implode(parts, deli));
    parent->vsx_command_queue_b(this);
  }
}

void vsx_widget_seq_channel::event_mouse_down(vsx_widget_distance distance,
    vsx_widget_coords coords, int button)
{
  bool run_event_mouse_down = true;
  mouse_clicked_id = -1;
  float time_iterator = 0;
  int item_iterator = 0;
  int item_action_id = -1;
  index_hit = -1; // we don't know which index has been hit
  extra_hit = -1;

  // first position the iterator and time just outside the viewing area so we don't
  // do unnecesarry heavy calculations
  if (view_time_start < 0 && view_time_end > 0)
  {
    // start at 0
    time_iterator = 0;
    item_iterator = 0;
  }
  else
  {
    item_iterator = 0;
    while (item_iterator < (int) items.size() && time_iterator
        < view_time_start)
    {
      time_iterator += items[item_iterator].get_total_length();
      ++item_iterator;
    }
    if (item_iterator)
    {
      --item_iterator;
      time_iterator -= items[item_iterator].get_total_length();
    }
  }

  // now line_iterator and t_cur are next to outside our iteration phase
  float o_dlx = -1000;
  float dlx = 0.0f, dly = 0.0f;
  float i_distance = 0.0f;

  if (channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER)
  {
    run_event_mouse_down = true;
    clicked_time_pos = pos_to_time(distance.center.x);
    while (item_iterator < (int) items.size() && time_iterator < view_time_end)
    {
      float local_time = clicked_time_pos - time_iterator;
      float total_length = items[item_iterator].get_total_length();
      float item_length = items[item_iterator].get_length();

      if (local_time > 0.0f && local_time < total_length)
      {
        mouse_clicked_id = item_iterator;
        if (alt ) index_hit = 1;
        if (ctrl) index_hit = 0;
        extra_hit = (local_time > item_length) ? 0 : 1;
        if (shift)
        {
          // delete this one!
          backwards_message("mseq_channel row remove " + channel_name + " " + i2s(mouse_clicked_id));
        }
        return;
      }
      time_iterator += items[item_iterator].get_total_length();
      ++item_iterator;
    }
  }

  if (channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_PARAMETER)
  {
    while (item_iterator < (int) items.size() && time_iterator <= view_time_end)
    {
      vsx_avector<vsx_string> parts;
      vsx_string deli = ",";
      vsx_string value = items[item_iterator].get_value();
      explode(value, deli, parts);
      // go through the parameter tuple to check if a value box has been hit
      for (int i = 0; i < index_count; ++i)
      {
        float vv = s2f(parts[i]);
        totalysize = y_end - y_start;
        dlx = (time_iterator - view_time_start) / totalsize * size.x - size.x
            * 0.5f;
        dly = (vv - y_start) / totalysize * size.y - size.y / 2;

        // check position of NEW point
        if (shift || (shift && ctrl && !alt) || (shift && alt && !ctrl))
        {
          if (button == 0)
          {
            if (o_dlx == -1000)
            {
              // test the beginning first
              if (distance.center.x > 0 && distance.center.x < dlx)
              {
                i_distance = (totalsize * ((distance.center.x - size.x * 0.5f)
                    / (size.x)));
                item_action_id = item_iterator - 1;
              }
            }
            else if (distance.center.x > o_dlx && distance.center.x < dlx)
            {
              i_distance = (totalsize * ((distance.center.x - o_dlx) / (size.x)));
              item_action_id = item_iterator - 1;
            }
            o_dlx = dlx;
          }
        }

        // check if we've hit a point
        if (!ctrl || (shift && button == 2))
          if (distance.center.x > dlx - 0.001 && distance.center.x < dlx + 0.001
              && distance.center.y > dly - 0.001 && distance.center.y < dly
              + 0.001)
          {
            clicked_item_x_diff = dlx - distance.center.x;
            m_pos = pos_to_time(distance.center.x);
            if (display_exclusive == i + 1 || !display_exclusive)
            {
              index_hit = i;
              mouse_clicked_id = item_iterator;
            }
          }
      }
      // if nothing found, check if any of the handles of a bezier has been hit
      if (mouse_clicked_id == -1)
      {
        if (items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
        { // 4 = bezier
          dlx = (time_iterator + items[item_iterator].get_handle1().x
              * items[item_iterator].get_total_length() - view_time_start) / totalsize
              * size.x - size.x * 0.5f;
          dly = (s2f(items[item_iterator].get_value()) + items[item_iterator].get_handle1().y
              - y_start) / totalysize * size.y - size.y * 0.5f;
          if (distance.center.x > dlx - 0.001f && distance.center.x < dlx
              + 0.001f && distance.center.y > dly - 0.001f && distance.center.y
              < dly + 0.001f)
          {
            if (display_exclusive == 1 || !display_exclusive)
            {
              extra_hit = 1;
              mouse_clicked_id = item_iterator;
            }
          }
          else if (item_iterator < (int) items.size() - 1)
          {
            dlx = (time_iterator + items[item_iterator].get_handle2().x
                * items[item_iterator].get_total_length() - view_time_start)
                / totalsize * size.x - size.x / 2;
            dly = (s2f(items[item_iterator + 1].get_value())
                + items[item_iterator].get_handle2().y - y_start) / totalysize * size.y
                - size.y / 2;
            if (distance.center.x > dlx - 0.001 && distance.center.x < dlx
                + 0.001 && distance.center.y > dly - 0.001 && distance.center.y
                < dly + 0.001)
            {
              if (display_exclusive == 1 || !display_exclusive)
              {
                extra_hit = 2;
                mouse_clicked_id = item_iterator;
              }
            }
          }
        }
      }
      time_iterator += items[item_iterator].get_total_length();
      ++item_iterator;
    }
    // add new point if nothing clicked up there
    if ( item_action_id == -1 && (shift || ctrl) && button == 0)
    {
      // dlx has been increased up until the final point,
      // now check if the distance is further to the right - outside of the range of points
      // if it is, means we should make a new point at the end
      if (distance.center.x > dlx)
      {
        i_distance = (totalsize * ((distance.center.x - dlx) / (size.x)));
        item_action_id = item_iterator - 1;
      }
    }
    if (item_action_id != -1)
    {
      // code for adding a new keyframe/point, relies on above code
      size_t interpolation_type = VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_LINEAR;
      if (shift && ctrl && !alt)
        interpolation_type = VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_COSINE;
      else if (shift && !ctrl && !alt)
        interpolation_type = VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_LINEAR;
      else if (!shift && ctrl && !alt)
        interpolation_type = VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_NONE;

      double_click_d[0] = 0.0f;

      vsx_string val;
      switch (param_type)
      {
      case VSX_MODULE_PARAM_ID_FLOAT:
      {
        val = base64_encode(f2s((distance.center.y + size.y / 2) / size.y
            * totalysize + y_start));
      }
        break;
      case VSX_MODULE_PARAM_ID_QUATERNION:
      {
        val = items[item_action_id].get_value();
      }
        break;
      }

      if (is_controller)
      {
        command_q_b.add_raw("pseq_r_ok insert z z " + val + " " + f2s(i_distance)
            + " " + i2s(interpolation_type) + " " + i2s(item_action_id));
        vsx_command_queue_b(this);
        // send a full inject dump to the parent in the widget chain
        send_parent_dump();
      }
      else
      {
        // 0=pseq_r 1=insert 2=[module] 3=[param] 4=[value] 5=[local_time_distance] 6=[interpolation_type] 7=[item_action_id]
        server_message("pseq_r insert", val + " " + f2s(i_distance) + " " +
                       i2s( interpolation_type) + " " + i2s(item_action_id));
      }
    }
    // code for removing a keyframe
    if (shift && button == 2)
    {
      if (mouse_clicked_id > 0 && items.size() > 2)
      {
        run_event_mouse_down = false;
        if (!is_controller)
        {
          server_message("pseq_r remove", i2s(mouse_clicked_id));
          mouse_clicked_id = -1;
        }
        else
        {
          command_q_b.add_raw("pseq_r_ok remove z z " + i2s(mouse_clicked_id));
          vsx_command_queue_b(this);
          send_parent_dump();
          mouse_clicked_id = -1;
        }
      }
      return;
    }

    // should we bring up the menu?
    if (mouse_clicked_id != -1 && !ctrl && !alt && !shift && button == 2)
    {
      front(menu_interpolation);
      if (parent != root)
        parent->front(this);
      menu_interpolation->visible = 2;
      menu_interpolation->set_pos(coords.screen_global);
      run_event_mouse_down = false;
      menu_temp_disable = true;
    } else
    if (mouse_clicked_id == -1)
    {
      if (alt)
      {
        // set time
        if (owner)
        ((vsx_widget_timeline*)( (static_cast<vsx_widget_sequence_editor*>(owner))->timeline))->move_time(distance.center);
      }
    }
  }

  if (run_event_mouse_down)
    vsx_widget::event_mouse_down(distance, coords, button);
}

void vsx_widget_seq_channel::event_mouse_up(vsx_widget_distance distance,
    vsx_widget_coords coords, int button)
{
  if (!shift && !ctrl && !alt)
  vsx_widget::event_mouse_up(distance, coords, button);
}

void vsx_widget_seq_channel::event_mouse_double_click(
    vsx_widget_distance distance, vsx_widget_coords coords, int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  VSX_UNUSED(button);

  if (channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER)
  {
    if (mouse_clicked_id != -1)
    {
      {
        if (extra_hit == 1)
        {
          vsx_widget** time_controller = &(items[mouse_clicked_id].get_master_channel_time_sequence());
          if (!*time_controller)
          {
            *time_controller = add((vsx_widget*)new vsx_widget_controller_sequence,name+"seq_edit");
            ((vsx_widget_controller_sequence*)*time_controller)->set_size_controlled_from_outside(1);
            ((vsx_widget_controller_sequence*)*time_controller)->set_command_suffix(vsx_string(" ")+i2s(mouse_clicked_id));
            ((vsx_widget_controller_sequence*)*time_controller)->init();
            ((vsx_widget_controller_sequence*)*time_controller)->set_span(0.0f, 1.0f);
            ((vsx_widget_controller_sequence*)*time_controller)->set_view_time(-0.000000001f, 1.0f);
            ((vsx_widget_controller_sequence*)*time_controller)->set_draw_base(0);
            ((vsx_widget_controller_sequence*)*time_controller)->set_parent_removal(1);
            (*time_controller)->coord_related_parent = false;
          } else
          {
            if (!(*time_controller)->visible)
            {
              (*time_controller)->visible = 1.0f;
            } else
              (*time_controller)->visible = 0.0f;
          }
        }
      }
      return;
    } else
    {
      command_q_b.add_raw("remove_chan " + channel_name + " " + param_name);
      parent->vsx_command_queue_b(this);
    }
  } else
  {
    command_q_b.add_raw("remove_chan " + channel_name + " " + param_name);
    parent->vsx_command_queue_b(this);
  }
}

void vsx_widget_seq_channel::event_mouse_wheel(float y)
{
  if (ctrl)
    y *= 10.0f;
  float dt = (y_end - y_start) * 0.5;
  y_start += y * dt * 0.05;
  if (alt)
    y = -y;
  y_end += y * dt * 0.05;
}

void vsx_widget_seq_channel::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  float value_pos = (distance.center.y / size.y + 0.5f) * (y_end - y_start) + y_start;
  float time_pos = (distance.center.x / size.x + 0.5f) * (view_time_end - view_time_start) + view_time_start;

  passive_time = "  "+f2s(time_pos);
  passive_time[0] = 26;
  passive_value = "  "+f2s(value_pos);
  passive_value[0] = 24;
  passive_mouse_pos = coords.world_global;
}

void vsx_widget_seq_channel::event_mouse_move(vsx_widget_distance distance,
    vsx_widget_coords coords)
{
  event_mouse_move_passive(distance, coords);
  if (channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER)
  {
    if (index_hit != -1 && mouse_clicked_id != -1)
    {
      if (0 == index_hit) // move the block
      {
        // this involves altering the time of the previous
        if (mouse_clicked_id > 0) // we can't move the first one!
        {
          // ____COMMAND STRUCTURE______________________
          // 0=mseq_channel
          // 1=row
          // 2=update
          // 3=[channel_name]
          // 4=[item_action_id]
          // 5=[local_time_distance if -1, don't set it]
          // 6=[length - if -1 don't set it]
          vsx_widget_param_sequence_item* previous_item = &items[mouse_clicked_id-1];
          vsx_widget_param_sequence_item* clicked_item = &items[mouse_clicked_id];
          float new_time = pos_to_time(distance.center.x);
          float prev_total_length = previous_item->get_total_length();

          float total_time_this_and_next = previous_item->get_total_length() + clicked_item->get_total_length();

          previous_item->increase_total_length( new_time - clicked_time_pos);

          if (previous_item->get_total_length() < previous_item->get_length())
          {
            previous_item->set_total_length( previous_item->get_length() );

          }
          items[mouse_clicked_id].increase_total_length( prev_total_length - previous_item->get_total_length() );

          if (clicked_item->get_total_length() < clicked_item->get_length())
            clicked_item->set_total_length( clicked_item->get_length() );

          float tlength_test = clicked_item->get_total_length();

          if (mouse_clicked_id != (int)items.size()-1 && ( (previous_item->get_total_length() + tlength_test) > (total_time_this_and_next+0.000001f)))
          {
            previous_item->set_total_length( total_time_this_and_next - clicked_item->get_length() );
          }

          clicked_time_pos = new_time;
            backwards_message(
                  "mseq_channel " // 0
                  "row " // 1
                  "update "+ // 2
                  channel_name+" "+ // 3
                  i2s(mouse_clicked_id-1)+" "+ // 4
                  f2s(previous_item->get_total_length())+" "+ // 5
                  "-1.0" // 6
            );
            backwards_message(
                  "mseq_channel " // 0
                  "row " // 1
                  "update "+ // 2
                  channel_name+" "+ // 3
                  i2s(mouse_clicked_id)+" "+ // 4
                  f2s(clicked_item->get_total_length())+" "+ // 5
                  "-1.0" // 6
            );
        }
      } // if (0 == index_hit)
      else // we assume index_hit is 1 which means scaling of internal length, fix this code if more cases arise
      {
        if (mouse_clicked_id > 0) // we can't scale the first one!
        {

          // in here the internal length of the block is changed
          // ____COMMAND STRUCTURE______________________
          // 0=mseq_channel
          // 1=row
          // 2=update
          // 3=[channel_name]
          // 4=[item_action_id]
          // 5=[local_time_distance if -1, don't set it]
          // 6=[length - if -1 don't set it]
          vsx_widget_param_sequence_item* action_item = &items[mouse_clicked_id];
          float new_time = pos_to_time(distance.center.x);
          action_item->increase_length( new_time - clicked_time_pos );

          clicked_time_pos = new_time;
            // pointless to send a message if the data is identical hey..
            backwards_message(
                  "mseq_channel " // 0
                  "row " // 1
                  "update "+ // 2
                  channel_name+" "+ // 3
                  i2s(mouse_clicked_id)+" "+ // 4
                  "-1.0 "+ // 5
                  f2s(action_item->get_length()) // 6
          );
        }
      }
    }
    return;
  }

  // extra hit = bezier spline handles
  if (extra_hit != -1)
  {
    // this applies only to bezier splines, not actual value points
    float t_delay = 0.0f;
    for (int i = 0; i < mouse_clicked_id; ++i)
      t_delay += items[i].get_total_length();
    float f = (1 - (-distance.center.x + size.x / 2) / size.x) * totalsize
        + view_time_start - t_delay;

    float y = ((distance.center.y + size.y / 2) / size.y) * totalysize + y_start;
    m_pos = f;
    if (extra_hit == 1)
    {
      items[mouse_clicked_id].set_handle1( vsx_vector(f / items[mouse_clicked_id].get_total_length(), y	- s2f(items[mouse_clicked_id].get_value()) ) );

      // move the previous bezier handle
      if (mouse_clicked_id > 0 && ctrl && !shift && !alt)
      {
        float cur_time_x = items[mouse_clicked_id].get_handle1().x * items[mouse_clicked_id].get_total_length();

        items[mouse_clicked_id-1].set_handle2(
          vsx_vector(
            1.0 - cur_time_x / items[mouse_clicked_id-1].get_total_length(),
            -items[mouse_clicked_id].get_handle1().y
          )
        );
      }

    }

    if (extra_hit == 2)
    {
      items[mouse_clicked_id].set_handle2( vsx_vector(f / items[mouse_clicked_id].get_total_length(), y - s2f(items[mouse_clicked_id + 1].get_value()) ) );

      // move the next bezier handle
      if (mouse_clicked_id + 1 < items.size() && ctrl && !shift && !alt)
      {
        float cur_time_x = (1.0 - items[mouse_clicked_id].get_handle2().x ) * items[mouse_clicked_id].get_total_length();

        items[mouse_clicked_id+1].set_handle1(
          vsx_vector(
            cur_time_x / items[mouse_clicked_id+1].get_total_length(),
            -items[mouse_clicked_id].get_handle2().y
          )
        );
      }

    }

    if (!is_controller)
      server_message("pseq_r update",
        base64_encode( items[mouse_clicked_id].get_value_interpolation() ) + " " +
                     f2s(items[mouse_clicked_id].get_total_length()) + " " +
                     i2s(items[mouse_clicked_id].get_interpolation()) + " " +
        i2s(mouse_clicked_id)
      );
    else
      send_parent_dump();

  }
  else
  if (mouse_clicked_id != -1)
  {
    // mouse is pressed on a regular value-point
    //totalsize*size.x-size.x/2+pos.x
    float f = 0.0f;
    float d = 0.0f;

    float cur_time_line_pos_x = time_to_pos(cur_time);
    float cur_hover_pos = distance.center.x;

    // code for snapping to timeline
    if (shift && !is_controller
        &&
        cur_time_line_pos_x + 0.001f > cur_hover_pos
        &&
        cur_time_line_pos_x - 0.001f < cur_hover_pos
    )
    {
        distance.center.x = cur_time_line_pos_x - clicked_item_x_diff;

        f = (1 - (-distance.center.x + size.x / 2) / size.x) * totalsize
          + view_time_start;
        d = m_pos - f;
        m_pos = f;
    } else
    {
      f = (1 - (-distance.center.x + size.x / 2) / size.x) * totalsize
          + view_time_start;
      d = m_pos - f;
      m_pos = f;
    }
    // delta time calculations for updating the engine
    if (
        (
          items[mouse_clicked_id - 1].get_total_length() - d > 0
          &&
          items[mouse_clicked_id].get_total_length() + d > 0
        )
        ||
        mouse_clicked_id == 0
        ||
        mouse_clicked_id == (int) items.size() - 1
      )
    {
      if (mouse_clicked_id != 0 && !alt)
      {
        // update the previous one
        if (is_controller)
        {
          float accum_time = 0.0f;
          for (int ii = 0; ii < mouse_clicked_id-1; ii++)
          {
            accum_time += items[ii].get_total_length();
          }
          if (accum_time + items[mouse_clicked_id - 1].get_total_length() - d > 1.0f)
          {
            d += accum_time + items[mouse_clicked_id - 1].get_total_length() - d -1.0f;

          }
        }

        // regular sequencer
        items[mouse_clicked_id - 1].increase_total_length( -d );
        if (mouse_clicked_id != (int) items.size() - 1)
        {
          items[mouse_clicked_id].increase_total_length( d );
        }
        else
        {
          items[mouse_clicked_id].set_total_length( 1 );
        }

        if (!is_controller)
        {

          server_message("pseq_r update",
            base64_encode( items[mouse_clicked_id - 1].get_value_interpolation() ) + " " +
              f2s( items[mouse_clicked_id - 1].get_total_length()) + " " +
              i2s( items[mouse_clicked_id - 1].get_interpolation()) + " " +
              i2s( mouse_clicked_id - 1)
          );
        }
        else
          send_parent_dump();
      }
      else if (alt)
      {
        items[mouse_clicked_id].increase_total_length( d );
      }
      if (!alt)
      {
        float y = ((distance.center.y + size.y / 2) / size.y) * totalysize
            + y_start;

        if (is_controller)
        {
          if (y > 1.0f) y = 1.0f;
          if (y < 0.0f) y = 0.0f;
        }

        // prepare existing value with updated value
        vsx_avector<vsx_string> parts;
        vsx_string deli = ",";
        vsx_string value = items[mouse_clicked_id].get_value();
        explode(value, deli, parts);
        parts[index_hit] = f2s(y);

        if (param_type == VSX_MODULE_PARAM_ID_QUATERNION)
        {
          vsx_string t = implode(parts, deli);
          vsx_quaternion q;
          q.from_string(t);
          q.normalize();
          parts[0] = f2s(q.x);
          parts[1] = f2s(q.y);
          parts[2] = f2s(q.z);
          parts[3] = f2s(q.w);
        }

        items[mouse_clicked_id].set_value( implode(parts, deli) );
      }

      if (!is_controller)
        server_message("pseq_r update",
          base64_encode( items[mouse_clicked_id].get_value_interpolation()) + " " +
                       f2s( items[mouse_clicked_id].get_total_length()) + " " +
                       i2s( items[mouse_clicked_id].get_interpolation()) + " " +
          i2s(mouse_clicked_id)
        );
      else
        send_parent_dump();
    }
  }
  else
  if (alt)
  {
    // set time
    if (owner)
    {
      ((vsx_widget_timeline*)(owner->timeline))->move_time(distance.center);
      if (shift && !is_controller)
      {
        // look through all items
        bool run = true;
        float time_start = 0.0f;
        float pos_clicked = distance.center.x;

        for (size_t i = 0; i < items.size()-1 && run; i++)
        {
          time_start += items[i].get_total_length();
          float pos_iterated = time_to_pos(time_start);

          if
            (
                pos_iterated + 0.001f > pos_clicked
                &&
                pos_iterated - 0.001f < pos_clicked
            )
          {
            distance.center.x = pos_iterated;
            ((vsx_widget_timeline*)(owner->timeline))->move_time(distance.center);
          }
        }
      }

    }
  }
}

void vsx_widget_seq_channel::i_remove_line(long i_td)
{
  std::vector<vsx_widget_param_sequence_item>::iterator it = items.begin();
  if (i_td)
    for (long i = 0; i < (long) i_td; ++i)
    {
      ++it;
    }
  if (i_td < (int) items.size() - 1)
  {
    items[i_td - 1].increase_total_length( items[i_td].get_total_length() );
  }
  items.erase(it);
}

void vsx_widget_seq_channel::remove_master_channel_items_with_name(vsx_string name)
{
  int run = 1;
  size_t i = 0;
  while (run)
  {
    if (items[i].get_pool_name() == name)
    {
      i_remove_line(i);
      i = 0;
    } else
    {
      i++;
    }
    if (i == items.size()) run = 0;
  }
}

void vsx_widget_seq_channel::command_process_back_queue(vsx_command_s *t)
{

  // keyboard entry of value
  if (t->cmd == "menu_interp_keyboard")
  {
    if (mouse_clicked_id != -1)
    {
      vsx_string val = items[mouse_clicked_id].get_value();
      value_dialog->set_render_type(VSX_WIDGET_RENDER_2D);
      ((dialog_query_string*)value_dialog)->show(val);
    }
    return;
  }



  // keyboard entry of value
  if (t->cmd == "menu_interp_keyboard_value")
  {
    if (mouse_clicked_id != -1)
    {
      items[mouse_clicked_id].set_value( t->cmd_data );
      if (!is_controller)
        server_message("pseq_r update",
          base64_encode(
            items[mouse_clicked_id].get_value_interpolation()) + " " +
                       f2s( items[mouse_clicked_id].get_total_length()) + " " +
                       i2s( items[mouse_clicked_id].get_interpolation()) + " " +
            i2s( mouse_clicked_id));
      else
        send_parent_dump();
    }
    return;
  }



  if (t->cmd == "menu_interp")
  {
    if (mouse_clicked_id != -1)
    {
      size_t interpolation_type = vsx_string_aux::s2i(t->cmd_data);

      vsx_widget_param_sequence_item *next_value_p = 0x0;

      if ( (size_t)mouse_clicked_id + 1 < items.size() )
      {
        next_value_p = &items[mouse_clicked_id + 1];
      }

      items[mouse_clicked_id].set_interpolation( interpolation_type, next_value_p );


      if (is_controller)
      {
        send_parent_dump();
        return;
      }

      server_message("pseq_r update",
        base64_encode( items[mouse_clicked_id].get_value_interpolation()) + " " +
                     f2s( items[mouse_clicked_id].get_total_length()) + " " +
                     i2s( items[mouse_clicked_id].get_interpolation()) + " " +
        i2s(mouse_clicked_id)
      );
    }
    return;
  }



  if (t->cmd == "menu_remove")
  {
    if (!is_controller)
    {
      backwards_message("pseq_p remove " + channel_name + " " + param_name);
    }
    return;
  }


  if (t->cmd == "menu_close")
  {
    if (!is_controller)
    {
      command_q_b.add_raw("remove_chan " + channel_name + " " + param_name);
    }
    else
    {
      command_q_b.add_raw("menu_close foo");
      parent->vsx_command_queue_b(this);
    }
    return;
  }



  if (t->cmd == "pseq_p_ok")
  {
    if (t->parts[1] == "inject_get")
    {
      vsx_string deli = "|";
      std::list<vsx_string> pl;
      explode(t->parts[4], deli, pl);
      if (t->parts.size() > 5)
      {
        param_type = vsx_string_aux::s2i(t->parts[5]);
      }
      else
        param_type = VSX_MODULE_PARAM_ID_FLOAT;
      switch (param_type)
      {
      case VSX_MODULE_PARAM_ID_FLOAT:
        index_count = 1;
        break;
      case VSX_MODULE_PARAM_ID_QUATERNION:
        index_count = 4;
        break;
      }

      for (std::list<vsx_string>::iterator it = pl.begin(); it != pl.end(); ++it)
      {
        std::vector<vsx_string> pld;
        vsx_string pdeli = ";";
        explode((*it), pdeli, pld);
        vsx_widget_param_sequence_item pa;
        if (pld.size() < 2)
          continue;
        pa.set_interpolation( vsx_string_aux::s2i(pld[1]) );
        if (pa.get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
        {
          std::vector<vsx_string> pld_l;
          vsx_string pdeli_l = ":";
          vsx_string vtemp = base64_decode(pld[2]);
          explode(vtemp, pdeli_l, pld_l);
          pa.set_value( pld_l[0] );
          pa.set_handle1( vsx_vector_aux::from_string( pld_l[1] ) );
          pa.set_handle2( vsx_vector_aux::from_string( pld_l[2] ) );
        }
        else
        {
          pa.set_value( base64_decode(pld[2]) );
        }
        pa.set_total_length( s2f(pld[0]) );
        items.push_back(pa);
      }
    }
    return;
  }




  if (t->cmd == "pseq_r_ok")
  {
    if (t->parts[1] == "insert")
    {
      long action_item = vsx_string_aux::s2i(t->parts[7]);
      std::vector<vsx_widget_param_sequence_item>::iterator it = items.begin();
      if (action_item)
        for (long i = 0; i < (long) action_item; ++i)
        {
          ++it;
        }
      ++it;
      vsx_widget_param_sequence_item pa;
      pa.set_value( base64_decode(t->parts[4]) ); //?
      pa.set_total_length( items[action_item].get_total_length() - s2f(t->parts[5]) );
      items[action_item].set_total_length( s2f(t->parts[5]) );
      pa.set_interpolation( vsx_string_aux::s2i( t->parts[6] ) );
      if (pa.get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
      {
        std::vector<vsx_string> pld_l;
        vsx_string pdeli_l = ":";
        vsx_string vtemp = base64_decode(t->parts[4]);
        explode(vtemp, pdeli_l, pld_l);
        pa.set_value( pld_l[0] );
        pa.set_handle1( vsx_vector_aux::from_string(pld_l[1]) );
        pa.set_handle2( vsx_vector_aux::from_string(pld_l[2]) );
      }
      else
      {
        pa.set_value( base64_decode(t->parts[4]) );
      }

      items.insert(it, pa);
    }
    else if (t->parts[1] == "remove")
    {
      long i_td = vsx_string_aux::s2i(t->parts[4]);
      i_remove_line(i_td);
    }
    return;
  }



  // handle messages from time sequence
  if (channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER)
  {
    if (t->cmd == "mseq_channel_ok")
    {
      if (t->parts[1] == "row")
      {
        // 0=mseq_channel 1=row 2=remove 3=[channel_name] 4=[item_action_id]
        if (t->parts[2] == "remove")
        {
          long i_td = vsx_string_aux::s2i(t->parts[4]);
          std::vector<vsx_widget_param_sequence_item>::iterator it = items.begin();
          if (i_td)
            for (long i = 0; i < (long) i_td; ++i)
            {
              ++it;
            }
          if (i_td < (int) items.size() - 1)
          {
            items[i_td - 1].increase_total_length( items[i_td].get_total_length() );
          }
          items.erase(it);
        } else
        // 0=mseq_channel_ok 1=row 2=insert 3=[channel_name] 4=[after_this_id] 5=[local_time_distance] 6=[length] 7=[pool_name]
        if (t->parts[2] == "insert")
        {
          long after_this_id = vsx_string_aux::s2i(t->parts[4]);
          if (after_this_id == (long)items.size()-1)
          {
            items[items.size()-1].set_total_length( s2f(t->parts[5]) );
            vsx_widget_param_sequence_item pa;
            pa.set_total_length( s2f(t->parts[6]) );
            pa.set_length( s2f(t->parts[6]) );
            pa.set_pool_name( t->parts[7] );
            items.push_back(pa);
          } else
          {
            std::vector<vsx_widget_param_sequence_item>::iterator it = items.begin();
            long i;
            if (after_this_id)
            {
              for (i = 0; i < (long) after_this_id; ++i)
              {
                ++it;
              }
            }
            i++;
            ++it;

            vsx_widget_param_sequence_item pa;
            pa.set_length( s2f(t->parts[6]) );
            pa.set_total_length( items[after_this_id].get_total_length() - s2f(t->parts[5]) );
            pa.set_pool_name( t->parts[7] );
            items[after_this_id].set_total_length( s2f(t->parts[5]) );
            items.insert(it, pa);
          }
        } else
        // 0=mseq_channel_ok 1=row 2=time_sequence 3=[name] 4=[item_id] 5=[get]/[set] 6(optional)=[sequence_dump]
        if (t->parts[2] == "time_sequence")
        {
          // send pg64_ok to the sequencer in question
          long item_id = vsx_string_aux::s2i(t->parts[4]);
          if (items[item_id].get_master_channel_time_sequence())
          {
            // 0=pg64_ok 1=param_name 2=system_id 3=sequencer_data
            command_q_b.add_raw("pg_ok - - "+t->parts[6]);
            items[item_id].get_master_channel_time_sequence()->vsx_command_queue_b(this);

          }
        }
      } else

      //0=mseq_channel_ok 1=inject_get 2=[channel_name] 3=[dump_data]
      if (t->parts[1] == "inject_get")
      {
        vsx_string deli = "|";
        std::list<vsx_string> pl;
        explode(t->parts[3], deli, pl);
        for (std::list<vsx_string>::iterator it = pl.begin(); it != pl.end(); ++it)
        {
          std::vector<vsx_string> pld;
          vsx_string pdeli = ";";
          explode((*it), pdeli, pld);
          vsx_widget_param_sequence_item pa;
          pa.set_type( VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER );
          pa.set_total_length( s2f(pld[0]) );
          pa.set_length( s2f(pld[1]) );
          if (pld.size() > 2)
          pa.set_pool_name( base64_decode(pld[2]) );
          items.push_back(pa);
        }
      }
    }
    else
    // [0=controller_sequence_close] [1=global_widget_id] [2=local_item_id]
    if (t->cmd == "controller_sequence_close")
    {
      int item_id = vsx_string_aux::s2i(t->parts[2]);
      items[item_id].get_master_channel_time_sequence()->_delete();
      items[item_id].set_master_channel_time_sequence( 0x0 );
    }
    else
    // 0=param_set 1=[data] 2=[id]
    if (t->cmd == "param_set")
    {
      // 0=mseq_channel_ok 1=row 2=time_sequence 3=[name] 4=[item_id] 5=[get]/[set] 6(optional)=[sequence_dump]
      backwards_message("mseq_channel row time_sequence "+channel_name+" "+t->parts[2]+" set "+t->parts[1]);
    }
    else
    // [0=pg64] [1=param_name] [2=global_widget_id] [3=local_item_id]
    if (t->cmd == "pg64")
    {
      // message from a time sequencer to initialize its value
      // 0=mseq_channel_ok 1=row 2=time_sequence 3=[name] 4=[item_id] 5=[get]/[set] 6(optional)=[sequence_dump]
      backwards_message("mseq_channel row time_sequence "+channel_name+" "+t->parts[3]+" get");
    }
  }
}

void vsx_widget_seq_channel::i_draw()
{
  if (!visible || hidden_by_sequencer)
    return;

  if (owner)
  {
    view_time_start = owner->tstart;
    view_time_end = owner->tend;
    cur_time = owner->curtime;
  }
  parentpos = parent->get_pos_p();

  // position + size calculations
  vsx_vector self_pos = parentpos + pos;
  float sizex_div_2 = size.x * 0.5f;
  float sizey_div_2 = size.y * 0.5f;

  totalsize = (view_time_end - view_time_start);
  glPushMatrix();
  glTranslatef(0.0,0.0,pos.z);
  glBegin(GL_QUADS);

  if (view_time_start < 0)
  {
    glColor4f(0.15f, 0.1f, 0.1f, 0.1f);
    ff = size.x * (fabs(view_time_start) / totalsize);

    glVertex2f(self_pos.x - sizex_div_2, self_pos.y + sizey_div_2);
    glVertex2f(self_pos.x - sizex_div_2 + ff, self_pos.y + sizey_div_2);
    glVertex2f(self_pos.x - sizex_div_2 + ff, self_pos.y	- sizey_div_2);
    glVertex2f(self_pos.x - sizex_div_2, self_pos.y - sizey_div_2);

    glColor4f(0.1f, 0.2f, 0.1f, 0.1f);
    glVertex2f(self_pos.x - sizex_div_2 + ff, self_pos.y + sizey_div_2);
    glVertex2f(self_pos.x + sizex_div_2, self_pos.y + sizey_div_2);
    glVertex2f(self_pos.x + sizex_div_2, self_pos.y - sizey_div_2);
    glVertex2f(self_pos.x - sizex_div_2 + ff, self_pos.y - sizey_div_2);
  }
  else
  {
    glColor4f(0.1f, 0.2f, 0.1f, 0.1f);
    glVertex2f(self_pos.x - sizex_div_2, self_pos.y + sizey_div_2);
    glVertex2f(self_pos.x + sizex_div_2, self_pos.y + sizey_div_2);
    glVertex2f(self_pos.x + sizex_div_2, self_pos.y - sizey_div_2);
    glVertex2f(self_pos.x - sizex_div_2, self_pos.y - sizey_div_2);
  }

  glEnd();

  levelstart = 0;
  glLineWidth(1);
  for (int i = (int) view_time_start; i < (int) (view_time_end) + 1; ++i)
  {
    glColor4f(0.3, 0.3, 0.3,0.5f);
    float x = (float) (i - view_time_start) / totalsize * size.x;
    if (x > 0)
    {
      x += parentpos.x + pos.x - size.x * 0.5f + levelstart * size.x;
      glBegin(GL_LINE_STRIP);
      glVertex2f(x, self_pos.y + size.y * 0.497f);
      glVertex2f(x, self_pos.y - size.y * 0.497f);
      glEnd();
    }
  }

  //vertical time marks
  {
    glColor4f(1.0f, 0.3f, 0.3f, 0.5f);
    float f = ((cur_time - view_time_start) / (view_time_end - view_time_start))
        * size.x;
    glBegin(GL_LINES);

    if (!is_controller)
    {
      glVertex2f(self_pos.x - sizex_div_2 + f, self_pos.y + size.y * 0.497f);
      glVertex2f(self_pos.x - sizex_div_2 + f, self_pos.y - size.y * 0.497f);
    }

    glColor4f(0.3f, 0.3f, 0.3f,0.5f);
    if (y_end - y_start > 1)
    {
      if (y_start < 0 && y_end > 0)
      {
        // we have the zero line to draw..
        f = self_pos.y - sizey_div_2 + fabs(y_start) / (y_end - y_start)
            * size.y;
        glColor4f(0.3f, 0.5f, 0.5f,0.5f);

        // zero line
        if (!is_controller && !(channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER))
        {
          glVertex2f(self_pos.x - sizex_div_2, f);
          glVertex2f(self_pos.x + sizex_div_2, f);
        }
      }
    }
    glEnd();
  }

  float w = 0.0f;
  float inc = round((y_end - y_start) / 10);
  if (inc == 0)
    inc = 1;

  if (channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER)
  {
    if (items.size())
    {
      float time_iterator = 0;
      int item_iterator = 0;
      font.color.a = 0.4f;
      while
      (
        item_iterator < (int) items.size()
        &&
        time_iterator < view_time_start
      )
      {
        time_iterator += items[item_iterator].get_total_length();
        ++item_iterator;
      }
      --item_iterator;
      if (item_iterator < 0)
        item_iterator = 0;
      time_iterator -= items[item_iterator].get_total_length();
      if (time_iterator < 0.0f) time_iterator = 0.0f;
      // now we can iterate
      float size_calc = (size.x / ( view_time_end - view_time_start));
      while (
            item_iterator < (int) items.size()
            &&
            time_iterator < view_time_end
      )
      {
        vsx_vector corner_pos =
          vsx_vector(
            self_pos.x + time_to_pos(
                  time_iterator
            ),
            self_pos.y - sizey_div_2
        )
        ;
        float total_length_calc = items[item_iterator].get_total_length()   // area length
          *
          size_calc
        ;

        glColor4f(0.2f,0.3f,0.4f,0.3f);
        draw_box
        (
          corner_pos,
          total_length_calc,
          size.y
        );
        glColor4f(0.3f,0.4f,0.5f,0.3f);
        draw_box
        (
          corner_pos,
          items[item_iterator].get_length()   // block length, not total length
          *
          size_calc,
          size.y
        );

        if (items[item_iterator].get_master_channel_time_sequence())
        {
          // position the time sequence controller kthx
          // it'll get drawn later..
          float sc_x = items[item_iterator].get_length()   // block length, not total length
              *
              size_calc;
          items[item_iterator].get_master_channel_time_sequence()->set_size(
            vsx_vector(
              sc_x
              ,
              size.y
            )
          );
          items[item_iterator].get_master_channel_time_sequence()->set_pos(corner_pos+vsx_vector(sc_x * 0.5f, size.y * 0.5f));
        }
        glColor4f(0.4f,0.5f,0.7f,0.9f);
        glBegin(GL_LINE_STRIP);
          glVertex2f(corner_pos.x, corner_pos.y);
          glVertex2f(corner_pos.x, corner_pos.y+size.y);
          glVertex2f(corner_pos.x+total_length_calc, corner_pos.y+size.y);
          glVertex2f(corner_pos.x+total_length_calc, corner_pos.y);
          glVertex2f(corner_pos.x, corner_pos.y);
        glEnd();

        font.print(
          corner_pos
          +
          vsx_vector(
            items[item_iterator].get_length()   // block length, not total length
            *
            size_calc*0.3f
          ),
          items[item_iterator].get_pool_name(), 0.007
        );

        // TODO: if present, position the controller here.

        time_iterator += items[item_iterator].get_total_length();
        ++item_iterator;
      } // while
      font.color.a = 1.0f;
    }
  }

  if (channel_type == VSX_WIDGET_SEQ_CHANNEL_TYPE_PARAMETER)
  {
    if (y_start < 0)
      w = y_start + fabs(fmod(y_start, inc));
    else
      w = y_start - fabs(fmod(y_start, inc)) + 1;

    if (w - 0.5 > y_start)
      draw_h_line(w - 0.5, 1, 1, 1, 0.4);
    while (w < y_end)
    {
      if (w != 0)
      {
        draw_h_line(w, 0.6, 0.6, 1, 0.7);
      }
      if (inc == 1)
        if (w + 0.5 < y_end)
        {
          draw_h_line(w + 0.5f, 1, 1, 1, 0.4);
        }
      font.color.a = 0.1f;
      font.print_center(vsx_vector(self_pos.x - size.x * 0.3,
          self_pos.y - size.y / 2 + (w - y_start) / (y_end - y_start)
              * size.y), f2s(w), font_size * 0.2f);
      w += inc;
    } //while
    glColor3f(1, 1, 1);
    if (items.size() >= 2)
    {
      float time_iterator = 0;
      int item_iterator = 0;
      float sv = 0.0f;
      if (view_time_start < 0 && view_time_end > 0)
      {
        // start at 0
        time_iterator = 0;
        item_iterator = 0;
      }
      else
      {
        while (item_iterator < (int) items.size() && time_iterator
            < view_time_start)
        {
          time_iterator += items[item_iterator].get_total_length();
          ++item_iterator;
        }
        --item_iterator;
        if (item_iterator < 0)
          item_iterator = 0;
        time_iterator -= items[item_iterator].get_total_length();
        sv = s2f(items[item_iterator].get_value());
      }

      // start pos assigned. now draw from here and to the end
      glLineWidth(3.0f);
      switch (param_type)
      {
      case VSX_MODULE_PARAM_ID_FLOAT:
      {
        vsx_color l_color = vsx_color(1.0f, 1.0f, 1.0f, 1.0f);
        while (item_iterator < (int) items.size() && time_iterator
            <= view_time_end)
        {
          sv = s2f(items[item_iterator].get_value() );
          if (item_iterator < (int) items.size() - 1)
          {
            if (items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
            {
              // BEZIER (x⁴ INTERPOLATION)
              vsx_bezier_calc calc;
              vsx_color lb_color = vsx_color(0.2f, 1.0f, 0.8f, 1.0f);
              float ev = s2f(items[item_iterator + 1].get_value() );
              draw_line(
                time_iterator,
                sv,
                time_iterator
                +
                items[item_iterator].get_handle1().x * items[item_iterator].get_total_length(),
                sv + items[item_iterator].get_handle1().y,
                lb_color
              );


              draw_line(
                    time_iterator + items[item_iterator].get_total_length(),
                ev,
                time_iterator + items[item_iterator].get_handle2().x
                    * items[item_iterator].get_total_length(),
                    ev + items[item_iterator].get_handle2().y,
                    lb_color
              );
              float delay = items[item_iterator].get_total_length();

              draw_chan_box(time_iterator + items[item_iterator].get_handle1().x * delay,
                  sv + items[item_iterator].get_handle1().y, 0.0007f);
              draw_chan_box(time_iterator + items[item_iterator].get_handle2().x * delay,
                  ev + items[item_iterator].get_handle2().y, 0.0007f);


              calc.x0 = 0.0f;
              calc.y0 = sv;
              calc.x1 = items[item_iterator].get_handle1().x;

              calc.y1 = sv + items[item_iterator].get_handle1().y;
              calc.x2 = items[item_iterator].get_handle2().x;
              calc.y2 = ev + items[item_iterator].get_handle2().y;
              calc.x3 = 1.0f;
              calc.y3 = ev;
              calc.init();
              for (float i = 0.0f; i < 1.0f; i += 0.05f)
              {
                float yp = calc.y_from_t(i + 0.05f);
                float xp = calc.x_from_t(i);
                float xp1 = calc.x_from_t(i + 0.05f);
                draw_line(time_iterator + xp * delay, sv, time_iterator + (xp1)
                    * delay, yp, l_color);
                sv = yp;
              }
            }
            else if (items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_COSINE)
            {
              float ev = s2f(items[item_iterator + 1].get_value());
              if (time_iterator < view_time_start)
              {
                //float part = ((tstart-t_cur)/(lines[td].delay));
                for (float i = 0; i < 10; ++i)
                {
                  float ft = PI / 10 * i;
                  float f = (1 - cos(ft)) * 0.5;
                  float ee = sv * (1 - f) + ev * f;
                  draw_line(time_iterator + items[item_iterator].get_total_length()
                      / 10 * (i), sv, time_iterator
                            + items[item_iterator].get_total_length() / 10 * (i + 1), ee,
                      l_color);
                  sv = ee;
                }
              }
              else if (items[item_iterator].get_total_length() + time_iterator
                  > view_time_end)
              {
                for (float i = 0; i < 10; ++i)
                {
                  float ft = PI / 10 * i;
                  float f = (1 - cos(ft)) * 0.5;
                  float ee = sv * (1 - f) + ev * f;
                  draw_line(time_iterator + items[item_iterator].get_total_length()
                      / 10 * (i), sv, time_iterator
                      + items[item_iterator].get_total_length() / 10 * (i + 1), ee,
                      l_color);
                  sv = ee;
                }
              }
              else
              {
                for (float i = 0; i < 10; ++i)
                {
                  float ft = PI / 10 * i;
                  float f = (1 - cos(ft)) * 0.5;
                  float ee = sv * (1 - f) + ev * f;
                  draw_line(time_iterator + items[item_iterator].get_total_length()
                      / 10 * (i), sv, time_iterator
                      + items[item_iterator].get_total_length() / 10 * (i + 1), ee,
                      l_color);
                  sv = ee;
                }
              }
            }
            else if (items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_LINEAR)
            {
              float ev = s2f(items[item_iterator + 1].get_value());
              if (time_iterator < view_time_start)
              {
                draw_line(time_iterator, sv, time_iterator
                  + items[item_iterator].get_total_length(), ev, l_color);
              }
              else if (items[item_iterator].get_total_length() + time_iterator
                  > view_time_end)
              {
                draw_line(time_iterator, sv, time_iterator
                  + items[item_iterator].get_total_length(), ev, l_color);
              }
              else
              {
                draw_line(time_iterator, sv, time_iterator
                  + items[item_iterator].get_total_length(), ev, l_color);
              }
            }
            else
            if
            (
              items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_NONE
              ||
              items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_RESERVED
            )
            {
              float ev = s2f(items[item_iterator + 1].get_value());

              draw_line(time_iterator, sv, time_iterator
                        + items[item_iterator].get_total_length(), sv, l_color);
              draw_line(time_iterator + items[item_iterator].get_total_length(), sv,
                        time_iterator + items[item_iterator].get_total_length(), ev,
                        l_color);
            }
          }
          if (mouse_clicked_id == item_iterator && k_focus == this)
          {
            draw_selection_box(time_iterator, s2f(items[item_iterator].get_value() ) );
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          }
          draw_chan_box(time_iterator, s2f(items[item_iterator].get_value()));
          time_iterator += items[item_iterator].get_total_length();
          ++item_iterator;
        } // while
        draw_line(time_iterator - items[item_iterator - 1].get_total_length(), sv,
            view_time_end, sv, l_color);
      }
        break; // FLOAT
      } // switch
      glLineWidth(2);
    }
    if (m_o_focus == this)
    {
      //TODO: work here
      font.print(passive_mouse_pos+vsx_vector(0.001,0.002), passive_value, 0.003);
      font.print(passive_mouse_pos+vsx_vector(-0.015,-0.002), passive_time, 0.003);
    }
  } // if channel type is parameter
  if (!is_controller)
  {
    font.color.a = 0.1f;
    font.print(
          vsx_vector(
                parentpos.x + pos.x - size.x * 0.5,
                parentpos.y + pos.y - size.y / 2
          ),
          channel_name +
          (
            channel_type == 0 ? (":" + param_name):""
          ) + "   " + param_name + "   " + param_name + "   " +
            param_name + "   " + param_name + "   " + param_name + "   " + param_name
          ,
          size.y * 0.5f
        );
    if (display_exclusive)
      font.print(vsx_vector(parentpos.x + pos.x - size.x * 0.5, parentpos.y
          + pos.y + size.y / 2 - 0.01f),
          "only editing " + i2s(display_exclusive), 0.007);
  }
  if (a_focus == this)
  {
    glColor4f(1, 1, 1, 1);
  }
  else
    glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
  draw_box_border(vsx_vector(parentpos.x + pos.x - size.x * 0.5, parentpos.y
      + pos.y - size.y * 0.5f), vsx_vector(size.x, size.y), 0.0001);
  glPopMatrix();
}

void vsx_widget_seq_channel::draw_h_line(float y0 = 0, float r, float g,
    float b, float a)
{
  glColor4f(r, g, b, a);
  glBegin(GL_LINE_STRIP);
    dly = (y0 - y_start) / totalysize * size.y + parentpos.y + pos.y - size.y / 2;
    glVertex2f(parentpos.x + pos.x - size.x / 2, dly);
    glVertex2f(parentpos.x + pos.x + size.x / 2, dly);
  glEnd();
}

void vsx_widget_seq_channel::draw_red_line(float t0, float t1, float y0)
{
  glBegin(GL_LINE_STRIP);
    totalysize = y_end - y_start;
    dlx = (t0 - view_time_start) / totalsize * size.x + parentpos.x + pos.x
        - size.x / 2;
    dly = (y0 - y_start) / totalysize * size.y + parentpos.y + pos.y - size.y / 2;
    glColor4f(col_temp_1.r, col_temp_1.g, col_temp_1.b, col_temp_1.a);
    glVertex2f(dlx, dly);
    dlx = (t1 - view_time_start) / totalsize * size.x + parentpos.x + pos.x
        - size.x / 2;

    glColor4f(col_temp_2.r, col_temp_2.g, col_temp_2.b, col_temp_2.a);
    glVertex2f(dlx, dly);
  glEnd();

}

void vsx_widget_seq_channel::draw_line(float t0, float y0, float t1, float y1,
    vsx_color& l_color)
{
  if (t0 < view_time_start)
  {
    if (t1 < view_time_start)
      return;
    else
    {
      float dt = fabs(t1 - t0);
      float a = view_time_start - t0;
      float dd = (1 - (a / dt));
      t0 = view_time_start;
      y0 = y1 - (y1 - y0) * dd;
    }
  }
  if (t1 > view_time_end)
  {
    if (t0 > view_time_end)
    {
      return;
    }
    else
    {
      float dt = fabs(t1 - t0);
      float a = view_time_end - t0;
      float dd = a / dt;
      t1 = view_time_end;

      y1 = y0 + (y1 - y0) * dd;
    }
  }

  ///--------
  ///--------
  if (y0 < y_start)
  {
    if (y1 < y_start)
    {
      col_temp_1.alpha_blend(&l_color, &graph_oob_color, (y_start - y0)
          / (totalysize * 0.5));
      col_temp_2.alpha_blend(&l_color, &graph_oob_color, (y_start - y1)
          / (totalysize * 0.5));
      draw_red_line(t0, t1, y_start);
      return;
    }
    else
    {
      float dy = fabs(y1 - y0);
      float a = y_start - y0;
      float dd = 1 - a / dy;
      col_temp_1.alpha_blend(&l_color, &graph_oob_color, a / (totalysize * 0.5));
      col_temp_2 = graph_color;

      y0 = y_start;
      float nt0 = t1 - (t1 - t0) * dd;

      draw_red_line(t0, nt0, y_start);
      t0 = nt0;
    }
  }
  else

  if (y1 < y_start)
  {
    float dy = fabs(y0 - y1);
    float a = y_start - y1;
    float dd = 1 - a / dy;
    col_temp_1 = graph_color;
    col_temp_2.alpha_blend(&l_color, &graph_oob_color, a / (totalysize * 0.5));

    y1 = y_start;
    float nt1 = t0 + dd * (t1 - t0);
    draw_red_line(nt1, t1, y_start);
    t1 = nt1;
  }
  else if (y0 > y_end)
  {
    if (y1 > y_end)
    {
      col_temp_1.alpha_blend(&l_color, &graph_oob_color, (y0 - y_end)
          / (totalysize * 0.5));
      col_temp_2.alpha_blend(&l_color, &graph_oob_color, (y1 - y_end)
          / (totalysize * 0.5));

      draw_red_line(t0, t1, y_end);
      return;
    }
    else
    {
      float dy = fabs(y0 - y1);
      float a = y0 - y_end;
      float dd = (a / dy);
      col_temp_1.alpha_blend(&l_color, &graph_oob_color, a / (totalysize * 0.5));
      col_temp_2 = graph_color;

      float nt0 = t0 + dd * (t1 - t0);
      draw_red_line(t0, nt0, y_end);
      t0 = nt0;
      y0 = y_end;
    }
  }
  else if (y1 > y_end)
  { //OK
    float dy = fabs(y1 - y0);
    float a = y1 - y_end;
    float dd = 1 - a / dy;
    col_temp_2.alpha_blend(&l_color, &graph_oob_color, a / (totalysize * 0.5));
    col_temp_1 = graph_color;

    y1 = y_end;
    float nt1 = t0 + (t1 - t0) * dd;
    draw_red_line(nt1, t1, y_end);
    t1 = nt1;
  }

  glColor4f(
    l_color.r,
    l_color.g,
    l_color.b,
    l_color.a
  );

  glBegin(GL_LINE_STRIP);
  totalysize = y_end - y_start;
  dlx = (t0 - view_time_start) / totalsize * size.x + parentpos.x + pos.x
      - size.x / 2;
  dly = (y0 - y_start) / totalysize * size.y + parentpos.y + pos.y - size.y / 2;

  glVertex2f(dlx, dly);
  dlx = (t1 - view_time_start) / totalsize * size.x + parentpos.x + pos.x
      - size.x / 2;
  dly = (y1 - y_start) / totalysize * size.y + parentpos.y + pos.y - size.y / 2;
  glVertex2f(dlx, dly);
  glEnd();
}

void vsx_widget_seq_channel::draw_chan_box(float t0, float y0, float c_size)
{
  float cs05 = c_size*0.5f;
  if (t0+cs05 < view_time_start || t0-c_size > view_time_end)
  {
    return;
  }
  if (y0-cs05 > y_end)
  {
    y0 = y_end;
    c_size *= 0.5;
    glColor3f(1, 0.6, 0.6);
  }
  else if (y0+cs05 < y_start)
  {
    y0 = y_start;
    c_size *= 0.5;
    glColor3f(1, 0.6, 0.6);
  }

  glBegin(GL_TRIANGLE_FAN);
  totalysize = y_end - y_start;
  dlx = (t0 - view_time_start) / totalsize * size.x + parentpos.x + pos.x
      - size.x / 2;
  dly = (y0 - y_start) / totalysize * size.y + parentpos.y + pos.y - size.y / 2;
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glVertex2f(dlx, dly);
  glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
  glVertex2f(dlx - c_size, dly - c_size);
  glVertex2f(dlx + c_size, dly - c_size);
  glVertex2f(dlx + c_size, dly + c_size);
  glVertex2f(dlx - c_size, dly + c_size);
  glVertex2f(dlx - c_size, dly - c_size);
  glEnd();
  font.print(vsx_vector(dlx + 0.002, dly), f2s(y0), c_size * 2.14f);
}

void vsx_widget_seq_channel::draw_selection_box(float t0, float y0)
{
  float c_size = 0.0025f;
  if (t0 < view_time_start || t0 > view_time_end)
  {
    return;
  }
  if (y0 > y_end)
  {
    y0 = y_end;
    c_size *= 0.5;
  }
  else if (y0 < y_start)
  {
    y0 = y_start;
    c_size *= 0.5;
  }
  glColor4f(1.0f, 0.0f, 0.0f, 1.0f - fmod(time * 4.5, 1.0f));
  glBegin(GL_TRIANGLE_FAN);
  totalysize = y_end - y_start;
  dlx = (t0 - view_time_start) / totalsize * size.x + parentpos.x + pos.x
      - size.x / 2;
  dly = (y0 - y_start) / totalysize * size.y + parentpos.y + pos.y - size.y / 2;
  glVertex2f(dlx, dly);
  glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
  glVertex2f(dlx - c_size, dly - c_size);
  glVertex2f(dlx + c_size, dly - c_size);
  glVertex2f(dlx + c_size, dly + c_size);
  glVertex2f(dlx - c_size, dly + c_size);
  glVertex2f(dlx - c_size, dly - c_size);
  glEnd();
  glColor4f(1.0f, 0.0f, 0.0f, 1.0f - fmod(time * 4.5, 1.0f));
  glBegin(GL_LINES);
  glVertex2f(dlx, size.y * 0.5 + parentpos.y + pos.y);
  glVertex2f(dlx, -size.y * 0.5 + parentpos.y + pos.y);
  glEnd();
}

bool vsx_widget_seq_channel::event_key_down(signed long key, bool alt,
    bool ctrl, bool shift)
{
  VSX_UNUSED(shift);

  if (is_controller) return true;
  switch (key)
  {
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
    toggle_exclusive(key - 48);
    break;
  case 'a':
  {
    if (ctrl)
      backwards_message("play");
    else
      backwards_message("stop");
  }
    break;
  case 'E':
  case 'e':
  {
    float dt = (y_end - y_start) * 0.03;
    y_start += dt;
    y_end += dt;
    return false;
  }
    break;
  case 'd':
  {
    float dt = (y_end - y_start) * 0.03;
    y_start -= dt;
    y_end -= dt;
    return false;
  }
    break;
  case 's':
  {
    if (ctrl)
    {
      return ((vsx_widget_timeline*) owner->timeline)->event_key_down(key,
          false, false, false);
    }
    else
    {

    }
  }
    break;
  case 'f':
  {
    if (ctrl)
    {
      if (owner)
        return ((vsx_widget_timeline*) owner->timeline)->event_key_down(key,
            false, false, false);
    }
  }
    break;
  case 'r':
  {
    if (ctrl)
    {
      if (owner)
        ((vsx_widget_timeline*) owner->timeline)->event_key_down(key, false,
            false, false);
    }
    if (alt)
    {
      float dt = (y_end - y_start) * 0.5;
      y_start = y_start + dt - dt * 0.97;
      y_end = y_end - dt + dt * 0.97;
    }
    return false;
  }
    break;
  case 'w':
  {
    if (ctrl)
    {
      if (owner)
        ((vsx_widget_timeline*) owner->timeline)->event_key_down(key, false,
            false, false);
    }
    if (alt)
    {
      float dt = (y_end - y_start) * 0.5;
      y_start = y_start + dt - dt * 1.03;
      y_end = y_end - dt + dt * 1.03;
    }
    return false;
  }
    break;
  }
  return true;
}

void vsx_widget_seq_channel::drop_master_channel(vsx_widget_distance distance,
    vsx_widget_coords coords, vsx_string name)
{
  VSX_UNUSED(distance);
  float time_iterator = 0;
  unsigned long item_iterator = 0;
  vsx_vector drop_pos_local = coords.world_global - get_pos_p();
  float time_pos = (drop_pos_local.x / size.x + 0.5f) * (view_time_end
      - view_time_start) + view_time_start;
  while (item_iterator < items.size() && time_iterator < time_pos)
  {
    time_iterator += items[item_iterator].get_total_length();
    ++item_iterator;
  }
  if (item_iterator && time_iterator > time_pos)
  {
    item_iterator--;
    time_iterator -= items[item_iterator].get_total_length();
  }
  float time_diff = time_pos - time_iterator;

  if (item_iterator >= items.size()) {
    item_iterator = items.size()-1;
    time_diff += items[item_iterator].get_total_length();
  }

  // 0=mseq_channel 1=row 2=insert 3=[channel_name] 4=[item_action_id] 5=[local_time_distance] 6=[length]
  command_q_b.add_raw("mseq_channel row insert " + channel_name + " " + i2s(item_iterator) + " " + f2s(
      time_diff) + " 0.1 " +name);
  parent->vsx_command_queue_b(this);
}

void vsx_widget_seq_channel::set_view_time(float start, float end)
{
  view_time_start = start;
  view_time_end = end;
}
