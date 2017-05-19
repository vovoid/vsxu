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

#include <filesystem/vsx_filesystem.h>
#include "vsx_gl_global.h"
#include <math/vector/vsx_vector3_helper.h>
#include <gl_helper.h>
#include <iomanip>
#include <map>
#include <list>
#include <vector>
#include <math.h>
#include <math/quaternion/vsx_quaternion_helper.h>
#include <texture/vsx_texture.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include "vsx_param.h"
#include <module/vsx_module.h>
// local includes
#include "vsx_widget.h"
#include "vsx_widget_sequence_editor.h"
#include "vsx_widget_sequence_channel.h"
#include "vsx_widget_timeline.h"
#include "widgets/vsx_widget_popup_menu.h"
#include <math/quaternion/vsx_quaternion.h>
#include <math/vsx_bezier_calc.h>
#include "controllers/vsx_widget_controller_base.h"
#include "controllers/vsx_widget_controller_seq.h"

// widget
#include <dialogs/dialog_query_string.h>

vsx_color<> vsx_widget_sequence_channel::col_temp_1;
vsx_color<> vsx_widget_sequence_channel::col_temp_2;
vsx_texture<>* vsx_widget_sequence_channel::mtex_blob = 0;

// WARNING: OVER BOOLEAN ASPHYXIATION IN THIS FILE!!!
// PROCEED WITH CAUTION!
// (better get your standard issue blanket and O² mask first)

void vsx_widget_sequence_channel::init()
{
  index_hit = -1; // none hit

  widget_type = VSX_WIDGET_TYPE_SEQUENCE_CHANNEL;

  hidden_by_sequencer = true;
  display_exclusive = 0;
  is_controller = false;
  graph_color = vsx_color<>(1.0f, 1.0f, 1.0f);
  graph_oob_color = vsx_color<>(1, 0, 0, 1);
  y_start = -0.1;
  y_end = 1.1;
  if (channel_type == vsx_widget_sequence_channel_TYPE_PARAMETER)
  {
    command_q_b.add_raw("pseq_p inject_get " + channel_name + " " + param_name);
    parent->vsx_command_queue_b(this);
  }
  else
  if (channel_type == vsx_widget_sequence_channel_TYPE_MASTER)
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
  dynamic_cast<vsx_widget_popup_menu*>(menu)->row_size_by_font_size( 0.006f );

  visible = 0;

  menu_interpolation = add(new vsx_widget_popup_menu, ".interp_menu");
  menu_interpolation->size.x = size.x * 0.4;
  menu_interpolation->commands.adds(VSX_COMMAND_MENU, "enter value manually",
      "menu_interp_keyboard", "0");
  menu_interpolation->commands.adds(VSX_COMMAND_MENU, "enter time manually",
      "menu_time_keyboard", "0");
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
  dynamic_cast<vsx_widget_popup_menu*>(menu_interpolation)->row_size_by_font_size( 0.006f );

  manual_value_input_dialog = add(new dialog_query_string("set value","set value"),"menu_interp_keyboard_value");
  ((dialog_query_string*)manual_value_input_dialog)->init();
  manual_value_input_dialog->set_render_type(render_2d);


  manual_time_input_dialog = add(new dialog_query_string("set time position","set time position"),"menu_time_keyboard_value");
  ((dialog_query_string*)manual_time_input_dialog)->init();
  manual_time_input_dialog->set_render_type(render_2d);
}

// sends a full inject dump to the parent in the widget chain
void vsx_widget_sequence_channel::send_parent_dump()
{
  vsx_nw_vector <vsx_string<> > parts;

  for (std::vector<vsx_widget_param_sequence_item>::iterator it = items.begin(); it
      != items.end(); ++it)
  {
    // delay;interp;value
    parts.push_back(
      vsx_string_helper::f2s((*it).get_total_length()) + ";" +
      vsx_string_helper::i2s((*it).get_interpolation()) + ";" +
      vsx_string_helper::base64_encode((*it).get_value_by_interpolation())
    );
  }

  if (parts.size())
  {
    vsx_string<>deli = "|";
    command_q_b.add_raw("update " + name + " " + vsx_string_helper::implode(parts, deli));
    parent->vsx_command_queue_b(this);
  }
}

void vsx_widget_sequence_channel::event_mouse_down(vsx_widget_distance distance,
    vsx_widget_coords coords, int button)
{
  bool run_event_mouse_down = true;
  can_move = true;
  mouse_clicked_id = -1;
  float time_iterator = 0;
  float time_iterator_old = 0;
  int item_iterator = 0;
  int item_action_id = -1;
  index_hit = -1; // we don't know which index has been hit
  extra_hit = -1;

  if (button == 0)
    mouse_down_l = true;

  if (button == 2)
    mouse_down_r = true;

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

  if (channel_type == vsx_widget_sequence_channel_TYPE_MASTER)
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
        if (vsx_input_keyboard.pressed_alt() ) index_hit = 1;
        if (vsx_input_keyboard.pressed_ctrl()) index_hit = 0;
        extra_hit = (local_time > item_length) ? 0 : 1;
        if (vsx_input_keyboard.pressed_shift())
        {
          // delete this one!
          backwards_message("mseq_channel row remove " + channel_name + " " + vsx_string_helper::i2s(mouse_clicked_id));
        }
        return;
      }
      time_iterator += items[item_iterator].get_total_length();
      ++item_iterator;
    }
  }

  if (channel_type == vsx_widget_sequence_channel_TYPE_PARAMETER)
  {
    while (item_iterator < (int) items.size() && time_iterator <= view_time_end)
    {
      vsx_nw_vector< vsx_string<> > parts;
      vsx_string<>deli = ",";
      vsx_string<>value = items[item_iterator].get_value();
      vsx_string_helper::explode(value, deli, parts);
      // go through the parameter tuple to check if a value box has been hit
      for (int i = 0; i < index_count; ++i)
      {
        float vv = items[item_iterator].convert_to_float(parts[i]);
        totalysize = y_end - y_start;

        // compute the local time ending
        dlx = (time_iterator - view_time_start) / totalsize * size.x - size.x * 0.5f;
        dly = (vv - y_start) / totalysize * size.y - size.y / 2;

        // check position of NEW point
        if (
          button == 0
          &&
          vsx_input_keyboard.pressed_shift()
        )
        {
          // if cur_time is within the current
          if (o_dlx == -1000)
          {
            // test the beginning first
            if (distance.center.x > 0 && distance.center.x < dlx)
            {
              i_distance = (totalsize * ((distance.center.x - size.x * 0.5f)
                  / (size.x)));
              item_action_id = item_iterator - 1;
            }
            if (vsx_input_keyboard.pressed_alt())
            {
              if (time_iterator_old < cur_time && time_iterator > cur_time )
              {
                i_distance = cur_time - time_iterator_old;
                can_move = false;
              }
            }

          }
          else if (distance.center.x > o_dlx && distance.center.x < dlx)
          {
            i_distance = (totalsize * ((distance.center.x - o_dlx) / (size.x)));
            item_action_id = item_iterator - 1;
            if (vsx_input_keyboard.pressed_alt())
              if (time_iterator_old < cur_time && time_iterator > cur_time )
              {
                i_distance = cur_time - time_iterator_old;
                can_move = false;
              }
          }
          o_dlx = dlx;
        }

        // check if we've hit a point
        if (
            (!vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_shift())
            ||
            (vsx_input_keyboard.pressed_ctrl() && vsx_input_keyboard.pressed_alt() && vsx_input_keyboard.pressed_shift())
            ||
            (vsx_input_keyboard.pressed_ctrl() && vsx_input_keyboard.pressed_alt())
            ||
            (vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift())
            ||
            (vsx_input_keyboard.pressed_shift() && button == 2)
        )
        {
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
      }
      // if nothing found, check if any of the handles of a bezier has been hit
      if (mouse_clicked_id == -1)
      {
        if (items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
        { // 4 = bezier
          dlx = (time_iterator + items[item_iterator].get_handle1().x
              * items[item_iterator].get_total_length() - view_time_start) / totalsize
              * size.x - size.x * 0.5f;
          dly = (vsx_string_helper::s2f(items[item_iterator].get_value()) + items[item_iterator].get_handle1().y
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
            dly = (vsx_string_helper::s2f(items[item_iterator + 1].get_value())
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
      time_iterator_old = time_iterator;
      time_iterator += items[item_iterator].get_total_length();
      ++item_iterator;
    }



    // add new point if nothing clicked up there
    if ( item_action_id == -1 && (vsx_input_keyboard.pressed_shift()) && button == 0)
    {
      // dlx has been increased up until the final point,
      // now check if the distance is further to the right - outside of the range of points
      // if it is, means we should make a new point at the end
      if (distance.center.x > dlx)
      {
        i_distance = (totalsize * ((distance.center.x - dlx) / (size.x)));
        if (vsx_input_keyboard.pressed_alt())
        {
          i_distance = cur_time - time_iterator_old;
          can_move = false;
        }
        item_action_id = item_iterator - 1;
      }
    }

    if
    (
      item_action_id != -1
      &&
      button == 0
      &&
      vsx_input_keyboard.pressed_shift()
    )
    {
      // code for adding a new keyframe/point, relies on above code
      size_t interpolation_type = VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_LINEAR;

      double_click_d[0] = 0.0f;

      vsx_string<>val;
      switch (param_type)
      {
        case VSX_MODULE_PARAM_ID_FLOAT:
        case VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE:
          val = vsx_string_helper::f2s(
            (distance.center.y + size.y / 2) / size.y * totalysize + y_start
          );
          if (vsx_input_keyboard.pressed_ctrl())
            val = items[item_action_id].get_value();
          val = vsx_string_helper::base64_encode( val );
        break;
        case VSX_MODULE_PARAM_ID_STRING:
        case VSX_MODULE_PARAM_ID_STRING_SEQUENCE:
          val = vsx_string_helper::base64_encode("_");
          break;
        case VSX_MODULE_PARAM_ID_QUATERNION:
          val = items[item_action_id].get_value();
        break;
      }

      if (is_controller)
      {
        command_q_b.add_raw(
              "pseq_r_ok insert z z " +
              val + " " +
              vsx_string_helper::f2s(i_distance) + " " +
              vsx_string_helper::i2s(interpolation_type) + " " +
              vsx_string_helper::i2s(item_action_id)
              );
        vsx_command_queue_b(this);
        // send a full inject dump to the parent in the widget chain
        send_parent_dump();
      }
      else
      {
        // 0=pseq_r 1=insert 2=[module] 3=[param] 4=[value] 5=[local_time_distance] 6=[interpolation_type] 7=[item_action_id]
        server_message(
              "pseq_r insert",
              val + " " +
              vsx_string_helper::f2s(i_distance) + " " +
              vsx_string_helper::i2s(interpolation_type) + " " +
              vsx_string_helper::i2s(item_action_id)
        );
      }
    }

    // code for removing a keyframe
    if (vsx_input_keyboard.pressed_shift() && button == 2)
    {
      if (mouse_clicked_id > 0 && items.size() > 2)
      {
        run_event_mouse_down = false;
        if (!is_controller)
        {
          server_message("pseq_r remove", vsx_string_helper::i2s(mouse_clicked_id));
          mouse_clicked_id = -1;
        }
        else
        {
          command_q_b.add_raw("pseq_r_ok remove z z " + vsx_string_helper::i2s(mouse_clicked_id));
          vsx_command_queue_b(this);
          send_parent_dump();
          mouse_clicked_id = -1;
        }
      }
    }

    // should we bring up the menu?
    if (mouse_clicked_id != -1 && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_shift() && button == 2)
    {
      front(menu_interpolation);
      if (parent != root)
        parent->front(this);
      menu_interpolation->visible = 2;
      menu_interpolation->set_render_type(vsx_widget_render_type::render_3d);

      vsx_vector3f menu_target_pos = get_pos_p();
      menu_target_pos.x += distance.center.x;
      menu_target_pos.y += distance.center.y;
      menu_interpolation->set_pos(menu_target_pos);
      run_event_mouse_down = false;
      menu_temp_disable = true;
    }

    if (mouse_clicked_id == -1 && vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift())
    {
        // set time
        if (owner)
        ((vsx_widget_timeline*)( (static_cast<vsx_widget_sequence_editor*>(owner))->timeline))->move_time(distance.center);
    }
  }

  if (run_event_mouse_down)
    vsx_widget::event_mouse_down(distance, coords, button);
}

void vsx_widget_sequence_channel::event_mouse_up(vsx_widget_distance distance,
    vsx_widget_coords coords, int button)
{
  if (button == 0)
    mouse_down_l = false;

  if (button == 2)
    mouse_down_r = false;

  if (!vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_alt())
  vsx_widget::event_mouse_up(distance, coords, button);
}



void vsx_widget_sequence_channel::event_mouse_double_click(
    vsx_widget_distance distance, vsx_widget_coords coords, int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  VSX_UNUSED(button);

  if (channel_type == vsx_widget_sequence_channel_TYPE_MASTER)
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
            ((vsx_widget_controller_sequence*)*time_controller)->set_command_suffix(vsx_string<>(" ")+vsx_string_helper::i2s(mouse_clicked_id));
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
      return;
    }
  }

  if (vsx_input_keyboard.pressed_ctrl())
  {
    command_q_b.add_raw("remove_chan " + channel_name + " " + param_name);
    parent->vsx_command_queue_b(this);
  }
}

void vsx_widget_sequence_channel::event_mouse_wheel(float y)
{
  if (vsx_input_keyboard.pressed_ctrl())
    y *= 10.0f;

  float dt = (y_end - y_start) * 0.5;

  if (!vsx_input_keyboard.pressed_alt())
  {
    y_start += y * dt * 0.05;
    y_end += y * dt * 0.05;
  }

  if (vsx_input_keyboard.pressed_alt())
  {
    float center_y = (hover_value_pos - y_start) / (y_end - y_start);
    y_start += y * dt * 0.05 * (center_y);
    y_end -= y * dt * 0.05 * (1 - center_y);
  }
}

void vsx_widget_sequence_channel::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  hover_value_pos = (distance.center.y / size.y + 0.5f) * (y_end - y_start) + y_start;
  hover_time_pos = (distance.center.x / size.x + 0.5f) * (view_time_end - view_time_start) + view_time_start;

  passive_time = "  "+vsx_string_helper::f2s(hover_time_pos);
  passive_time[0] = 26;
  passive_value = "  "+vsx_string_helper::f2s(hover_value_pos);
  passive_value[0] = 24;
  passive_mouse_pos = coords.world_global;
}

void vsx_widget_sequence_channel::event_mouse_move(vsx_widget_distance distance,
    vsx_widget_coords coords)
{
  event_mouse_move_passive(distance, coords);

  if (mouse_down_r)
    return;

  // handling of Master channels
  if (channel_type == vsx_widget_sequence_channel_TYPE_MASTER)
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
                  vsx_string_helper::i2s(mouse_clicked_id-1)+" "+ // 4
                  vsx_string_helper::f2s(previous_item->get_total_length())+" "+ // 5
                  "-1.0" // 6
            );
            backwards_message(
                  "mseq_channel " // 0
                  "row " // 1
                  "update "+ // 2
                  channel_name+" "+ // 3
                  vsx_string_helper::i2s(mouse_clicked_id)+" "+ // 4
                  vsx_string_helper::f2s(clicked_item->get_total_length())+" "+ // 5
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
                  vsx_string_helper::i2s(mouse_clicked_id)+" "+ // 4
                  "-1.0 "+ // 5
                  vsx_string_helper::f2s(action_item->get_length()) // 6
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

    int other_item_id_to_update = -1;

    float t_delay = 0.0f;
    for (int i = 0; i < mouse_clicked_id; ++i)
      t_delay += items[i].get_total_length();
    float f = (1 - (-distance.center.x + size.x / 2) / size.x) * totalsize
        + view_time_start - t_delay;

    float y = ((distance.center.y + size.y / 2) / size.y) * totalysize + y_start;
    m_pos = f;
    // first bezier handle
    if (extra_hit == 1)
    {
      float y_val = y	- vsx_string_helper::s2f(items[mouse_clicked_id].get_value());
      if (vsx_input_keyboard.pressed_alt())
        y_val = 0;
      items[mouse_clicked_id].set_handle1(
        vsx_vector3<>(
          f / items[mouse_clicked_id].get_total_length(),
          y_val
        )
      );

      // move the previous bezier handle
      if ( vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift())
      {
        vsx_widget_param_sequence_item* other_item = 0x0;
        float cur_time_x = items[mouse_clicked_id].get_handle1().x * items[mouse_clicked_id].get_total_length();

        if ( mouse_clicked_id > 0 )
        {
          other_item_id_to_update = mouse_clicked_id-1;
          other_item = &items[other_item_id_to_update];
        }

        if ( mouse_clicked_id == 0 )
        {
          other_item_id_to_update = items.size()-2;
          other_item = &items[other_item_id_to_update];
        }

        if (other_item)
        {
          other_item->set_handle2(
            vsx_vector3<>(
              1.0 - cur_time_x / other_item->get_total_length(),
              -items[mouse_clicked_id].get_handle1().y
            )
          );
        }
      }
    }

    // second bezier handle
    if (extra_hit == 2)
    {
      float y_val = y	- vsx_string_helper::s2f(items[mouse_clicked_id + 1].get_value());
      if (vsx_input_keyboard.pressed_alt())
        y_val = 0;
      items[mouse_clicked_id].set_handle2(
        vsx_vector3<>(
          f / items[mouse_clicked_id].get_total_length(),
          y_val
        )
      );

      // move the next bezier handle
      if (vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift())
      {
        vsx_widget_param_sequence_item* other_item = 0x0;
        float cur_time_x = (1.0 - items[mouse_clicked_id].get_handle2().x ) * items[mouse_clicked_id].get_total_length();


        if ( (size_t)(mouse_clicked_id + 2) >= items.size() )
        {
          other_item_id_to_update = 0;
          other_item = &items[other_item_id_to_update];
        }

        if ( (size_t)(mouse_clicked_id + 2) < items.size() )
        {
          other_item_id_to_update = mouse_clicked_id+1;
          other_item = &items[other_item_id_to_update];
        }

        if (other_item)
        {
          other_item->set_handle1(
            vsx_vector3<>(
              cur_time_x / other_item->get_total_length(),
              -items[mouse_clicked_id].get_handle2().y
            )
          );
        }
      }
    }

    if (!is_controller)
    {
      if (other_item_id_to_update != -1)
        server_message("pseq_r update",
          vsx_string_helper::base64_encode( items[other_item_id_to_update].get_value_by_interpolation() ) + " " +
                       vsx_string_helper::f2s(items[other_item_id_to_update].get_total_length()) + " " +
                       vsx_string_helper::i2s(items[other_item_id_to_update].get_interpolation()) + " " +
          vsx_string_helper::i2s(other_item_id_to_update)
        );

      server_message("pseq_r update",
        vsx_string_helper::base64_encode( items[mouse_clicked_id].get_value_by_interpolation() ) + " " +
                     vsx_string_helper::f2s(items[mouse_clicked_id].get_total_length()) + " " +
                     vsx_string_helper::i2s(items[mouse_clicked_id].get_interpolation()) + " " +
        vsx_string_helper::i2s(mouse_clicked_id)
      );
    }
    else
      send_parent_dump();
    return;
  }







  // Regular Point clicked
  if (mouse_clicked_id != -1)
  {
    int other_item_id_to_update = -1;

    // mouse is pressed on a regular value-point
    float absolute_clicked_time = 0.0f;
    float delta_time_movement = 0.0f;

    float cur_time_line_pos_x = time_to_pos(cur_time);
    float cur_hover_pos = distance.center.x;

    // code for snapping to timeline
    if (vsx_input_keyboard.pressed_shift() && !is_controller
        &&
        cur_time_line_pos_x + 0.001f > cur_hover_pos
        &&
        cur_time_line_pos_x - 0.001f < cur_hover_pos
    )
    {
        distance.center.x = cur_time_line_pos_x - clicked_item_x_diff;

        absolute_clicked_time = (1 - (-distance.center.x + size.x / 2) / size.x) * totalsize
          + view_time_start;
        delta_time_movement = m_pos - absolute_clicked_time;
        m_pos = absolute_clicked_time;
    } else
    {
      absolute_clicked_time = (1 - (-distance.center.x + size.x / 2) / size.x) * totalsize
          + view_time_start;
      delta_time_movement = m_pos - absolute_clicked_time;
      m_pos = absolute_clicked_time;
    }

    // delta time calculations for updating the engine
    if (
        (
          items[mouse_clicked_id - 1].get_total_length() - delta_time_movement > 0
          &&
          items[mouse_clicked_id].get_total_length() + delta_time_movement > 0
        )
        ||
        mouse_clicked_id == 0
        ||
        mouse_clicked_id == (int) items.size() - 1
      )
    {

      if (mouse_clicked_id != 0 && (!vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_ctrl() ))
      {
        // update the previous one
        if (is_controller)
        {
          float accum_time = 0.0f;

          for (int ii = 0; ii < mouse_clicked_id-1; ii++)
            accum_time += items[ii].get_total_length();

          // make sure time doesn't extend beyond 1.0
          if (accum_time + items[mouse_clicked_id - 1].get_total_length() - delta_time_movement > 1.0f)
            delta_time_movement += accum_time + items[mouse_clicked_id - 1].get_total_length() - delta_time_movement -1.0f;

        }

        // regular sequencer
        items[mouse_clicked_id - 1].increase_total_length( -delta_time_movement );

        // special handling of last element
        if (mouse_clicked_id != (int) items.size() - 1)
          items[mouse_clicked_id].increase_total_length( delta_time_movement );
        else
          items[mouse_clicked_id].set_total_length( 1 );

        if (is_controller)
        {
          float accum_time = items[0].get_total_length();
          for (int ii = 1; ii < mouse_clicked_id-1; ii++)
            accum_time += items[ii].get_total_length();
          items[ items.size() -1 ].set_total_length( 1.0 - accum_time );
        }


        if (!is_controller)
        {
          server_message("pseq_r update",
            vsx_string_helper::base64_encode( items[mouse_clicked_id - 1].get_value_by_interpolation() ) + " " +
              vsx_string_helper::f2s( items[mouse_clicked_id - 1].get_total_length()) + " " +
              vsx_string_helper::i2s( items[mouse_clicked_id - 1].get_interpolation()) + " " +
              vsx_string_helper::i2s( mouse_clicked_id - 1)
          );
        }
        else
          send_parent_dump();
      }
      else if (vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_ctrl())
      {
        items[mouse_clicked_id].increase_total_length( delta_time_movement );
        if (!vsx_input_keyboard.pressed(VSX_SCANCODE_HOME))
          if (mouse_clicked_id > 0)
          {
            items[mouse_clicked_id-1].increase_total_length( -delta_time_movement );
            other_item_id_to_update = mouse_clicked_id - 1;
          }
      }

      if ( (!vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_ctrl()) || (vsx_input_keyboard.pressed_alt() && vsx_input_keyboard.pressed_ctrl()))
      {
        float y = ((distance.center.y + size.y / 2) / size.y) * totalysize
            + y_start;

        if (is_controller)
          y = CLAMP(y, 0.0, 1.0);

        // prepare existing value with updated value
        if (param_type == VSX_MODULE_PARAM_ID_QUATERNION)
        {
          vsx_nw_vector< vsx_string<> > parts;
          vsx_string<>deli = ",";
          vsx_string<>value = items[mouse_clicked_id].get_value();
          vsx_string_helper::explode(value, deli, parts);
          parts[index_hit] = vsx_string_helper::f2s(y);

          vsx_string<>t = vsx_string_helper::implode(parts, deli);
          vsx_quaternion<> q;
          q = vsx_quaternion_helper::from_string<float>(t);
          q.normalize();
          parts[0] = vsx_string_helper::f2s(q.x);
          parts[1] = vsx_string_helper::f2s(q.y);
          parts[2] = vsx_string_helper::f2s(q.z);
          parts[3] = vsx_string_helper::f2s(q.w);

          items[mouse_clicked_id].set_value( vsx_string_helper::implode(parts, deli) );
        }

        if (param_type == VSX_MODULE_PARAM_ID_FLOAT || param_type == VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE)
          items[mouse_clicked_id].set_value(vsx_string_helper::f2s(y));

        if (vsx_input_keyboard.pressed_ctrl() && vsx_input_keyboard.pressed_alt() && vsx_input_keyboard.pressed_shift())
        {
          if (mouse_clicked_id == 0)
          {
            // update the last point as well
            other_item_id_to_update = items.size()-1;
            items[other_item_id_to_update].set_value(vsx_string_helper::f2s(y));
          }
          if ((size_t)mouse_clicked_id == items.size()-1)
          {
            // update the last point as well
            other_item_id_to_update = 0;
            items[other_item_id_to_update].set_value(vsx_string_helper::f2s(y));
          }
        }
      }

      if (!is_controller)
      {
        if (other_item_id_to_update != -1)
          server_message("pseq_r update",
            vsx_string_helper::base64_encode( items[other_item_id_to_update].get_value_by_interpolation() ) + " " +
                         vsx_string_helper::f2s(items[other_item_id_to_update].get_total_length()) + " " +
                         vsx_string_helper::i2s(items[other_item_id_to_update].get_interpolation()) + " " +
            vsx_string_helper::i2s(other_item_id_to_update)
          );

        server_message("pseq_r update",
          vsx_string_helper::base64_encode( items[mouse_clicked_id].get_value_by_interpolation()) + " " +
                       vsx_string_helper::f2s( items[mouse_clicked_id].get_total_length()) + " " +
                       vsx_string_helper::i2s( items[mouse_clicked_id].get_interpolation()) + " " +
          vsx_string_helper::i2s(mouse_clicked_id)
        );
      }

      if (is_controller)
        send_parent_dump();
    }
    return;
  }






  // No point clicked, maybe move timeline
  if (vsx_input_keyboard.pressed_alt() && owner && can_move)
  {
    ((vsx_widget_timeline*)(owner->timeline))->move_time(distance.center);

    // snap to point
    if (vsx_input_keyboard.pressed_shift() && !is_controller)
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

void vsx_widget_sequence_channel::create_keyframe(float time, vsx_string<> value, size_t interpolation_type)
{
  server_message(
        "pseq_r insert_absolute",
        vsx_string_helper::base64_encode( value )  + " " + // 4
        vsx_string_helper::f2s(time) + " " + // 5
        vsx_string_helper::i2s(interpolation_type) + " " // 6
  );
}

void vsx_widget_sequence_channel::get_keyframe_value(float time_start, float time_end, vsx_nw_vector< vsx_string<> >& values, vsx_ma_vector<float>& time_offsets, vsx_ma_vector<size_t>& interpolation_types)
{
  float accum_time = 0.0f;
  for (size_t i = 0; i < items.size(); i++)
  {
    if
    (
      accum_time > time_start
      &&
      accum_time < time_end
    )
    {
      values.push_back( items[i].get_value_by_interpolation() );
      time_offsets.push_back( accum_time - time_start );
      interpolation_types.push_back( items[i].get_interpolation() );
    }

    accum_time += items[i].get_total_length();
  }
}

void vsx_widget_sequence_channel::clear_selection(float time_start, float time_end)
{
  float accum_time = 0.0f;
  size_t deleted = 0;
  for (size_t i = 0; i < items.size() - 1; i++)
  {
    if
    (
      i > 0
      &&
      accum_time > time_start
      &&
      accum_time < time_end
    )
    {
      server_message("pseq_r remove", vsx_string_helper::i2s(i - deleted));
      deleted++;
    }
    accum_time += items[i].get_total_length();
  }
}

void vsx_widget_sequence_channel::i_remove_line(long i_td)
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

void vsx_widget_sequence_channel::remove_master_channel_items_with_name(vsx_string<>name)
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
    if (i == items.size())
      run = 0;
  }
}

void vsx_widget_sequence_channel::command_process_back_queue(vsx_command_s *t)
{

  // keyboard entry of value
  if (t->cmd == "menu_interp_keyboard")
  {
    if (mouse_clicked_id == -1)
      return;

    vsx_string<>val = items[mouse_clicked_id].get_value();
    ((dialog_query_string*)manual_value_input_dialog)->show(val);
    manual_value_input_dialog->set_render_type(render_2d);
    return;
  }



  // keyboard entry of value
  if (t->cmd == "menu_interp_keyboard_value")
  {
    if (mouse_clicked_id == -1)
      return;

    items[mouse_clicked_id].set_value( t->cmd_data );
    if (is_controller)
    {
      send_parent_dump();
      return;
    }

    server_message("pseq_r update",
      vsx_string_helper::base64_encode(items[mouse_clicked_id].get_value_by_interpolation()) + " " +
      vsx_string_helper::f2s( items[mouse_clicked_id].get_total_length()) + " " +
      vsx_string_helper::i2s( items[mouse_clicked_id].get_interpolation()) + " " +
      vsx_string_helper::i2s( mouse_clicked_id)
    );
    return;
  }


  // keyboard entry of time
  if (t->cmd == "menu_time_keyboard")
  {
    if (mouse_clicked_id < 1)
      return;

    // convert local value time to global time
    float accumulated_time = 0.0f;
    for (size_t i = 0; i < (size_t)mouse_clicked_id; i++)
    {
      accumulated_time += items[i].get_total_length();
    }

    vsx_string<>val = vsx_string_helper::f2s(accumulated_time);
    ((dialog_query_string*)manual_time_input_dialog)->show(val);
    manual_time_input_dialog->set_render_type(render_2d);

    return;
  }


  // keyboard entry of time
  if (t->cmd == "menu_time_keyboard_value")
  {
    if (mouse_clicked_id == -1)
      return;

    // find the minimum possible time
    float min_time_possible = 0.0f;
    for (int i = 0; i < mouse_clicked_id-1; i++)
    {
      min_time_possible += items[i].get_total_length();
    }

    float proposed_new_time = vsx_string_helper::s2f( t->cmd_data );
    if (proposed_new_time < min_time_possible)
      // TODO: display error here
      return;

    float new_delta_time = (proposed_new_time - min_time_possible) - items[mouse_clicked_id-1].get_total_length();

    // modify the previous time
    items[mouse_clicked_id-1].increase_total_length( new_delta_time );
    // modify after time
    items[mouse_clicked_id].increase_total_length( -new_delta_time );

    if (is_controller)
    {
      send_parent_dump();
      return;
    }

    size_t updated_id = mouse_clicked_id -1;

    server_message("pseq_r update",
      vsx_string_helper::base64_encode(items[updated_id].get_value_by_interpolation()) + " " +
      vsx_string_helper::f2s( items[updated_id].get_total_length()) + " " +
      vsx_string_helper::i2s( items[updated_id].get_interpolation()) + " " +
      vsx_string_helper::i2s( updated_id)
    );
    server_message("pseq_r update",
      vsx_string_helper::base64_encode(items[mouse_clicked_id].get_value_by_interpolation()) + " " +
      vsx_string_helper::f2s( items[mouse_clicked_id].get_total_length()) + " " +
      vsx_string_helper::i2s( items[mouse_clicked_id].get_interpolation()) + " " +
      vsx_string_helper::i2s( mouse_clicked_id)
    );
    return;
  }






  if (t->cmd == "menu_interp")
  {
    if (mouse_clicked_id != -1)
    {
      size_t interpolation_type = vsx_string_helper::s2i(t->cmd_data);

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
        vsx_string_helper::base64_encode( items[mouse_clicked_id].get_value_by_interpolation()) + " " +
                     vsx_string_helper::f2s( items[mouse_clicked_id].get_total_length()) + " " +
                     vsx_string_helper::i2s( items[mouse_clicked_id].get_interpolation()) + " " +
        vsx_string_helper::i2s(mouse_clicked_id)
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
      vsx_string<>deli = "|";

      if (!param_type)
        param_type = VSX_MODULE_PARAM_ID_FLOAT;

      if (t->parts.size() > 5)
        param_type = vsx_string_helper::s2i(t->parts[5]);

      switch (param_type)
      {
        case VSX_MODULE_PARAM_ID_FLOAT:
        case VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE:
        case VSX_MODULE_PARAM_ID_STRING_SEQUENCE:
        case VSX_MODULE_PARAM_ID_STRING:
          index_count = 1;
          break;
        case VSX_MODULE_PARAM_ID_QUATERNION:
          index_count = 4;
          break;
      }

      vsx_nw_vector<vsx_string<> > pl;
      vsx_string_helper::explode(t->parts[4], deli, pl);
      foreach (pl, i)
      {
        vsx_nw_vector<vsx_string<> > pld;
        vsx_string<>pdeli = ";";
        vsx_string_helper::explode(pl[i], pdeli, pld);
        vsx_widget_param_sequence_item pa;
        if (pld.size() < 2)
          continue;
        pa.set_interpolation( vsx_string_helper::s2i(pld[1]) );
        if (pa.get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
        {
          vsx_nw_vector<vsx_string<> > pld_l;
          vsx_string<>pdeli_l = ":";
          vsx_string<>vtemp = vsx_string_helper::base64_decode(pld[2]);
          vsx_string_helper::explode(vtemp, pdeli_l, pld_l);
          pa.set_value( pld_l[0] );
          if (pld_l.size() == 1)
            VSX_ERROR_CONTINUE("Error, no handle information for value of interpolation type bezier");

          pa.set_handle1( vsx_vector3_helper::from_string<float>( pld_l[1] ) );
          pa.set_handle2( vsx_vector3_helper::from_string<float>( pld_l[2] ) );
        }
        else
        {
          if (pld.size() > 2)
            pa.set_value( vsx_string_helper::base64_decode(pld[2]) );
        }
        pa.set_total_length( vsx_string_helper::s2f(pld[0]) );
        items.push_back(pa);
      }
    }
    return;
  }




  if (t->cmd == "pseq_r_ok")
  {
    if (t->parts[1] == "insert")
    {
      long action_item = vsx_string_helper::s2i(t->parts[7]);
      std::vector<vsx_widget_param_sequence_item>::iterator it = items.begin();
      if (action_item)
        for (long i = 0; i < (long) action_item; ++i)
        {
          ++it;
        }
      ++it;
      vsx_widget_param_sequence_item pa;

      pa.set_value( vsx_string_helper::base64_decode(t->parts[4]) ); //?
      pa.set_total_length( items[action_item].get_total_length() - vsx_string_helper::s2f(t->parts[5]) );
      items[action_item].set_total_length( vsx_string_helper::s2f(t->parts[5]) );
      pa.set_interpolation( vsx_string_helper::s2i( t->parts[6] ) );

      if (pa.get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
        set_value_bezier( vsx_string_helper::base64_decode(t->parts[4]), pa );

      items.insert(it, pa);
      return;
    }

    if (t->parts[1] == "insert_absolute")
    {
      vsx_string<> value = vsx_string_helper::base64_decode(t->parts[4]);
      float time = vsx_string_helper::s2f(t->parts[5]);
      size_t interpolation = vsx_string_helper::s2i(t->parts[6]);
      insert_absolute( time, value, interpolation );
      return;
    }

    if (t->parts[1] == "remove")
    {
      long i_td = vsx_string_helper::s2i(t->parts[4]);
      i_remove_line(i_td);
      return;
    }
  }



  // handle messages from time sequence
  if (channel_type == vsx_widget_sequence_channel_TYPE_MASTER)
  {
    if (t->cmd == "mseq_channel_ok")
    {
      if (t->parts[1] == "row")
      {
        // 0=mseq_channel 1=row 2=remove 3=[channel_name] 4=[item_action_id]
        if (t->parts[2] == "remove")
        {
          long i_td = vsx_string_helper::s2i(t->parts[4]);
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
          long after_this_id = vsx_string_helper::s2i(t->parts[4]);
          if (after_this_id == (long)items.size()-1)
          {
            items[items.size()-1].set_total_length( vsx_string_helper::s2f(t->parts[5]) );
            vsx_widget_param_sequence_item pa;
            pa.set_total_length( vsx_string_helper::s2f(t->parts[6]) );
            pa.set_length( vsx_string_helper::s2f(t->parts[6]) );
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
            pa.set_length( vsx_string_helper::s2f(t->parts[6]) );
            pa.set_total_length( items[after_this_id].get_total_length() - vsx_string_helper::s2f(t->parts[5]) );
            pa.set_pool_name( t->parts[7] );
            items[after_this_id].set_total_length( vsx_string_helper::s2f(t->parts[5]) );
            items.insert(it, pa);
          }
        } else
        // 0=mseq_channel_ok 1=row 2=time_sequence 3=[name] 4=[item_id] 5=[get]/[set] 6(optional)=[sequence_dump]
        if (t->parts[2] == "time_sequence")
        {
          // send pg64_ok to the sequencer in question
          long item_id = vsx_string_helper::s2i(t->parts[4]);
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
        vsx_string<>deli = "|";
        vsx_nw_vector< vsx_string<> > pl;
        vsx_string_helper::explode(t->parts[3], deli, pl);
        foreach(pl, i)
        {
          vsx_nw_vector<vsx_string<> > pld;
          vsx_string<>pdeli = ";";
          vsx_string_helper::explode(pl[i], pdeli, pld);
          vsx_widget_param_sequence_item pa;
          pa.set_type( vsx_widget_sequence_channel_TYPE_MASTER );
          pa.set_total_length( vsx_string_helper::s2f(pld[0]) );
          pa.set_length( vsx_string_helper::s2f(pld[1]) );
          if (pld.size() > 2)
          pa.set_pool_name( vsx_string_helper::base64_decode(pld[2]) );
          items.push_back(pa);
        }
      }
    }
    else
    // [0=controller_sequence_close] [1=global_widget_id] [2=local_item_id]
    if (t->cmd == "controller_sequence_close")
    {
      int item_id = vsx_string_helper::s2i(t->parts[2]);
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

void vsx_widget_sequence_channel::i_draw()
{
  if (!visible || hidden_by_sequencer)
    return;

  if (owner)
  {
    view_time_start = owner->time_left_border;
    view_time_end = owner->time_right_border;
    cur_time = owner->time;
  }
  parentpos = parent->get_pos_p();

  // position + size calculations
  vsx_vector3<> self_pos = parentpos + pos;
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
        if (!is_controller && !(channel_type == vsx_widget_sequence_channel_TYPE_MASTER))
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

  if (channel_type == vsx_widget_sequence_channel_TYPE_MASTER)
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
        vsx_vector3<> corner_pos =
          vsx_vector3<>(
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
            vsx_vector3<>(
              sc_x
              ,
              size.y
            )
          );
          items[item_iterator].get_master_channel_time_sequence()->set_pos(corner_pos+vsx_vector3<>(sc_x * 0.5f, size.y * 0.5f));
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
          vsx_vector3<>(
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

  if (channel_type == vsx_widget_sequence_channel_TYPE_PARAMETER)
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
        draw_h_line(w, 0.6, 0.6, 1, 0.7);

      if (inc == 1)
        if (w + 0.5 < y_end)
          draw_h_line(w + 0.5f, 1, 1, 1, 0.4);

      font.color.a = 0.1f;
      font.print_center(vsx_vector3<>(self_pos.x - size.x * 0.3,
          self_pos.y - size.y / 2 + (w - y_start) / (y_end - y_start)
              * size.y), vsx_string_helper::f2s(w), font_size * 0.2f);
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
        sv = vsx_string_helper::s2f(items[item_iterator].get_value());
      }

      // start pos assigned. now draw from here and to the end
      glLineWidth(3.0f);
      switch (param_type)
      {
        case VSX_MODULE_PARAM_ID_FLOAT:
        case VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE:
        case VSX_MODULE_PARAM_ID_STRING_SEQUENCE:
        case VSX_MODULE_PARAM_ID_STRING:
        {
          vsx_color<> line_color = vsx_color<>(1.0f, 1.0f, 1.0f, 1.0f);
          while (item_iterator < (int) items.size() && time_iterator
              <= view_time_end)
          {
            sv = vsx_string_helper::s2f(items[item_iterator].get_value() );
            if (item_iterator < (int) items.size() - 1)
            {
              if (items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
              {
                line_color = vsx_color<>(0.2f, 1.0f, 0.8f, 1.0f);

                // BEZIER (x⁴ INTERPOLATION)
                vsx_bezier_calc<float> calc;
                vsx_color<> handle_color = vsx_color<>(0.2f, 1.0f, 0.8f, 1.0f);
                float ev = vsx_string_helper::s2f(items[item_iterator + 1].get_value() );
                draw_line(
                  time_iterator,
                  sv,
                  time_iterator
                  +
                  items[item_iterator].get_handle1().x * items[item_iterator].get_total_length(),
                  sv + items[item_iterator].get_handle1().y,
                  handle_color
                );


                draw_line(
                      time_iterator + items[item_iterator].get_total_length(),
                  ev,
                  time_iterator + items[item_iterator].get_handle2().x
                      * items[item_iterator].get_total_length(),
                      ev + items[item_iterator].get_handle2().y,
                      handle_color
                );
                float delay = items[item_iterator].get_total_length();

                draw_chan_box(
                  time_iterator + items[item_iterator].get_handle1().x * delay,
                  sv + items[item_iterator].get_handle1().y,
                  "",
                  vsx_color<>(0.2f, 1.0f, 0.8f,1),
                  SEQ_CHAN_BOX_SIZE05
                );
                draw_chan_box(
                  time_iterator + items[item_iterator].get_handle2().x * delay,
                  ev + items[item_iterator].get_handle2().y,
                  "",
                  vsx_color<>(0.2f, 1.0f, 0.8f,1),
                  SEQ_CHAN_BOX_SIZE05
                );


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
                      * delay, yp, line_color);
                  sv = yp;
                }
              }
              else if (items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_COSINE)
              {
                line_color = vsx_color<>(0.5f, 1.0f, 0.5f, 1.0f);

                float ev = vsx_string_helper::s2f(items[item_iterator + 1].get_value());
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
                        line_color);
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
                        line_color);
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
                        line_color);
                    sv = ee;
                  }
                }
              }
              else if (items[item_iterator].get_interpolation() == VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_LINEAR)
              {
                line_color = vsx_color<>(1.0f, 0.5f, 0.5f, 1.0f);

                float ev = vsx_string_helper::s2f(items[item_iterator + 1].get_value());
                if (time_iterator < view_time_start)
                {
                  draw_line(time_iterator, sv, time_iterator
                    + items[item_iterator].get_total_length(), ev, line_color);
                }
                else if (items[item_iterator].get_total_length() + time_iterator
                    > view_time_end)
                {
                  draw_line(time_iterator, sv, time_iterator
                    + items[item_iterator].get_total_length(), ev, line_color);
                }
                else
                {
                  draw_line(time_iterator, sv, time_iterator
                    + items[item_iterator].get_total_length(), ev, line_color);
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
                vsx_color<> l_color = vsx_color<>(1.0f, 1.0f, 1.0f, 1.0f);

                float ev = vsx_string_helper::s2f(items[item_iterator + 1].get_value());

                draw_line(time_iterator, sv, time_iterator
                          + items[item_iterator].get_total_length(), sv, l_color);
                draw_line(time_iterator + items[item_iterator].get_total_length(), sv,
                          time_iterator + items[item_iterator].get_total_length(), ev,
                          l_color);
              }
            }
            if (mouse_clicked_id == item_iterator && k_focus == this)
            {
              draw_selection_box(time_iterator, vsx_string_helper::s2f(items[item_iterator].get_value() ) );
              glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            }

            if (param_type == VSX_MODULE_PARAM_ID_FLOAT || param_type == VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE)
              draw_chan_box(
                time_iterator,
                vsx_string_helper::s2f(items[item_iterator].get_value()),
                items[item_iterator].get_value(),
                vsx_color<>(1,1,1,1)
              );

            if (param_type == VSX_MODULE_PARAM_ID_STRING_SEQUENCE || param_type == VSX_MODULE_PARAM_ID_STRING)
              draw_chan_box(
                time_iterator,
                0.0f,
                items[item_iterator].get_value(),
                vsx_color<>(1,1,1,1)
              );

            time_iterator += items[item_iterator].get_total_length();
            ++item_iterator;
          } // while
          draw_line(time_iterator - items[item_iterator - 1].get_total_length(), sv,
              view_time_end, sv, line_color);
        }
        break; // FLOAT
      } // switch
      glLineWidth(2);
    }
    if (m_o_focus == this)
    {
      font.print(passive_mouse_pos+vsx_vector3<>(0.001,0.002), passive_value, 0.003);
      font.print(passive_mouse_pos+vsx_vector3<>(-0.015,-0.002), passive_time, 0.003);
    }



    // draw time-selection
    if (owner && owner->time_selection_active)
    {
      float y_mid = parentpos.y + pos.y;
      float y_size = size.y;
      float y_top = y_mid+y_size*0.5;
      float y_bottom = y_mid-y_size*0.5;
      float left_selection_factor = ((owner->time_selection_left - owner->time_left_border) /
                                   (owner->time_right_border - owner->time_left_border))
                                  * size.x
          + parentpos.x+pos.x-size.x * 0.5f;
      float right_selection_factor = ((owner->time_selection_right - owner->time_left_border) /
                                   (owner->time_right_border - owner->time_left_border))
                                  * size.x
          + parentpos.x+pos.x-size.x * 0.5f;
      glColor4f(0.7, 0.7, 1.0, 0.2);
      glBegin(GL_QUADS);
        glVertex2f( left_selection_factor, y_top );
        glVertex2f( right_selection_factor, y_top );
        glVertex2f( right_selection_factor, y_bottom);
        glVertex2f( left_selection_factor, y_bottom);
      glEnd();
    }



  } // if channel type is parameter
  if (!is_controller)
  {
    font.color.a = 0.1f;
    font.print(
          vsx_vector3<>(
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
      font.print(vsx_vector3<>(parentpos.x + pos.x - size.x * 0.5, parentpos.y
          + pos.y + size.y / 2 - 0.01f),
          "only editing " + vsx_string_helper::i2s(display_exclusive), 0.007);
  }
  if (a_focus == this)
  {
    glColor4f(1, 1, 1, 1);
  }
  else
    glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
  draw_box_border(vsx_vector3<>(parentpos.x + pos.x - size.x * 0.5, parentpos.y
      + pos.y - size.y * 0.5f), vsx_vector3<>(size.x, size.y), 0.0001);
  glPopMatrix();
}

void vsx_widget_sequence_channel::draw_h_line(float y0 = 0, float r, float g,
    float b, float a)
{
  glColor4f(r, g, b, a);
  glBegin(GL_LINE_STRIP);
    dly = (y0 - y_start) / totalysize * size.y + parentpos.y + pos.y - size.y / 2;
    glVertex2f(parentpos.x + pos.x - size.x / 2, dly);
    glVertex2f(parentpos.x + pos.x + size.x / 2, dly);
  glEnd();
}

void vsx_widget_sequence_channel::draw_red_line(float t0, float t1, float y0)
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

void vsx_widget_sequence_channel::draw_line(float t0, float y0, float t1, float y1,
    vsx_color<>& l_color)
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

void vsx_widget_sequence_channel::draw_chan_box(float t0, float y0, vsx_string<> display_value, vsx_color<> box_color, float c_size)
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
    glColor4f(box_color.r, box_color.g, box_color.b, 1.0f);
    glVertex2f(dlx, dly);
    glColor4f(box_color.r, box_color.g, box_color.b, 0.0f);
    glVertex2f(dlx - c_size, dly - c_size);
    glVertex2f(dlx + c_size, dly - c_size);
    glVertex2f(dlx + c_size, dly + c_size);
    glVertex2f(dlx - c_size, dly + c_size);
    glVertex2f(dlx - c_size, dly - c_size);
  glEnd();

  glColor4f(1,1,1,1);

  font.color = vsx_color<>(1,1,1,0.5);
  font.print(vsx_vector3<>(dlx + 0.002, dly), display_value, c_size * 2.14f);
}

void vsx_widget_sequence_channel::draw_selection_box(float t0, float y0)
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
  glColor4f(1.0f, 0.0f, 0.0f, 1.0f - fmod(vsx_widget_time::get_instance()->get_time() * 4.5, 1.0f));
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
  glColor4f(1.0f, 0.0f, 0.0f, 1.0f - fmod(vsx_widget_time::get_instance()->get_time() * 4.5, 1.0f));
  glBegin(GL_LINES);
  glVertex2f(dlx, size.y * 0.5 + parentpos.y + pos.y);
  glVertex2f(dlx, -size.y * 0.5 + parentpos.y + pos.y);
  glEnd();
}

bool vsx_widget_sequence_channel::event_key_down(uint16_t key)
{
  if (is_controller)
    return true;

  switch (key)
  {
    case VSX_SCANCODE_1:
      toggle_exclusive(0);
      break;
    case VSX_SCANCODE_2:
      toggle_exclusive(1);
      break;
    case VSX_SCANCODE_3:
      toggle_exclusive(2);
      break;
    case VSX_SCANCODE_4:
      toggle_exclusive(3);
      break;
    case VSX_SCANCODE_5:
      toggle_exclusive(4);
      break;

    case VSX_SCANCODE_A:
    {
      if (vsx_input_keyboard.pressed_ctrl())
        backwards_message("but_play");
      else
        backwards_message("stop");
    }
    break;
    case VSX_SCANCODE_E:

      if (vsx_input_keyboard.pressed_alt())
      {
        float dt = (y_end - y_start) * 0.03;
        y_start += dt;
        y_end += dt;
        return false;
      }
    break;
    case VSX_SCANCODE_D:

      if (vsx_input_keyboard.pressed_alt())
      {
        float dt = (y_end - y_start) * 0.03;
        y_start -= dt;
        y_end -= dt;
        return false;
      }
    break;
    case VSX_SCANCODE_S:
      {
        if (vsx_input_keyboard.pressed_ctrl())
        {
          return ((vsx_widget_timeline*) owner->timeline)->event_key_down(key);
        }
        else
        {

        }
      }
    break;
    case VSX_SCANCODE_F:
      {
        if (vsx_input_keyboard.pressed_ctrl())
        {
          if (owner)
            return ((vsx_widget_timeline*) owner->timeline)->event_key_down(key);
        }
      }
    break;
    case VSX_SCANCODE_R:
      {
        if (vsx_input_keyboard.pressed_ctrl())
        {
          if (owner)
          {
            ((vsx_widget_timeline*) owner->timeline)->event_key_down(key);
            return false;
          }
        }
        if (vsx_input_keyboard.pressed_alt())
        {
          float dt = (y_end - y_start) * 0.5;
          y_start = y_start + dt - dt * 0.97;
          y_end = y_end - dt + dt * 0.97;
          return false;
        }
      }
    break;
    case VSX_SCANCODE_W:
      {
        if (vsx_input_keyboard.pressed_ctrl())
        {
          if (owner)
          {
            ((vsx_widget_timeline*) owner->timeline)->event_key_down(key);
            return false;
          }
        }
        if (vsx_input_keyboard.pressed_alt())
        {
          float dt = (y_end - y_start) * 0.5;
          y_start = y_start + dt - dt * 1.03;
          y_end = y_end - dt + dt * 1.03;
          return false;
        }
      }
    break;

    case VSX_SCANCODE_T:
      if (vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift())
        // set bezier interpolation type
        set_bezier_time_aligned_handles();

      if (vsx_input_keyboard.pressed_ctrl() && vsx_input_keyboard.pressed_shift())
        owner->set_bezier_time_aligned_handles();
      break;

    case VSX_SCANCODE_C:
      if (vsx_input_keyboard.pressed_ctrl())
      {
        owner->action_copy();
        return false;
      }
      break;

    case VSX_SCANCODE_V:
      if (vsx_input_keyboard.pressed_ctrl())
      {
        owner->action_paste();
        return false;
      }
      break;

    case VSX_SCANCODE_X:
      if (vsx_input_keyboard.pressed_ctrl())
      {
        owner->action_cut();
        return false;
      }
      break;

  }
  return true;
}

void vsx_widget_sequence_channel::drop_master_channel(vsx_widget_distance distance,
    vsx_widget_coords coords, vsx_string<>name)
{
  VSX_UNUSED(distance);
  float time_iterator = 0;
  unsigned long item_iterator = 0;
  vsx_vector3<> drop_pos_local = coords.world_global - get_pos_p();
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
  command_q_b.add_raw("mseq_channel row insert " + channel_name + " " + vsx_string_helper::i2s(item_iterator) + " " + vsx_string_helper::f2s(
      time_diff) + " 0.1 " +name);
  parent->vsx_command_queue_b(this);
}

void vsx_widget_sequence_channel::set_view_time(float start, float end)
{
  view_time_start = start;
  view_time_end = end;
}


void vsx_widget_sequence_channel::set_bezier_time_aligned_handles()
{
  float accum_time = 0.0f;
  for (size_t i = 0; i < items.size()-1; i++)
  {
    if (cur_time > accum_time
        &&
        cur_time < accum_time + items[i].get_total_length()
        )
    {
      items[i].set_interpolation( VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER, &items[i+1] );
      items[i].set_bezier_handles_factor( (cur_time - accum_time) / items[i].get_total_length() );
      server_message("pseq_r update",
        vsx_string_helper::base64_encode( items[i].get_value_by_interpolation()) + " " +
                     vsx_string_helper::f2s( items[i].get_total_length()) + " " +
                     vsx_string_helper::i2s( items[i].get_interpolation()) + " " +
        vsx_string_helper::i2s(i)
                     );
    }
    accum_time += items[i].get_total_length();
  }
}

void vsx_widget_sequence_channel::align_bezier_time_handles()
{
  float accum_time = 0.0f;
  for (size_t i = 0; i < items.size()-1; i++)
  {
    if (cur_time > accum_time
        &&
        cur_time < accum_time + items[i].get_total_length()
        )
    {
      if (items[i].get_interpolation() != VSX_WIDGET_PARAM_SEQUENCE_INTERPOLATION_BEZIER)
        return;

      items[i].set_bezier_handles_factor( (cur_time - accum_time) / items[i].get_total_length() );
      server_message("pseq_r update",
        vsx_string_helper::base64_encode( items[i].get_value_by_interpolation()) + " " +
                     vsx_string_helper::f2s( items[i].get_total_length()) + " " +
                     vsx_string_helper::i2s( items[i].get_interpolation()) + " " +
        vsx_string_helper::i2s(i)
                     );
    }
    accum_time += items[i].get_total_length();
  }
}
