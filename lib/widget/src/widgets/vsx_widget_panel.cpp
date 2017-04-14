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
#include <texture/vsx_texture.h>
#include "vsx_font.h"
#include <vsx_widget.h>
#include <widgets/vsx_widget_panel.h>
#include <gl_helper.h>

void vsx_widget_panel::calc_size() {
  if (size_from_parent)
    return;

  vsx_vector3<> psize = parent->get_inner_size();

  size = target_size = psize;

//  size.x = target_size.x = psize;
//  size.y = target_size.y = parent->size.y-dragborder*2;
}



vsx_vector3<> vsx_widget_panel::calc_pos() {
  vsx_vector3<> p = get_pos_p();
  if (pos_from_parent) {
    p.x += target_pos.x;
    p.y += target_pos.y;
  }
  p.x -= target_size.x*0.5f;
  p.y -= target_size.y*0.5f;
  if (render_type == render_3d) {
    p.z = pos.z;
  } else {
    p.z = 0.0f;
  }
  return p;
}

/*vsx_vector3<> vsx_widget_panel::calc_pos()
{
  vsx_vector3<> p = get_pos_p();

  if (pos_from_parent) {
    return pos - target_size * 0.5;
//    p.x += target_pos.x;
//    p.y += target_pos.y;
      p.x -= target_size.x*0.5;
      p.y -= target_size.y*0.5;
  } else
  p = parent->get_inner_pos();


  if (render_type == render_3d) {
    p.z = pos.z;
  } else {
    p.z = 0.0f;
  }
  return p;
}
*/
int vsx_widget_panel::inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global)
{
  VSX_UNUSED(test);
  VSX_UNUSED(global);
  //return vsx_widget::inside_xy_l(test,global);
  return 0;
}

void vsx_widget_panel::base_draw() {
  calc_size();
  vsx_vector3<> p = calc_pos();
  //vsx_color b(0,0,0,0), w(0,0,0,1), gr(0,1,0,1), r(1,0,0,1);
  vsx_color<> b(0,0,0,0);
  vsx_color<> w(0,0,0,1);
  //draw_box_gradient(p, dragborder, target_size.y, skin_color[0], skin_color[1], skin_color[1], skin_color[0]);

  draw_box_gradient(p, (float)dragborder, target_size.y, vsx_widget_skin::get_instance()->get_color(0), vsx_widget_skin::get_instance()->get_color(1), vsx_widget_skin::get_instance()->get_color(1), vsx_widget_skin::get_instance()->get_color(0));

  //  draw_box_gradient(p, target_size.x, dragborder, r, b, b, r);
  draw_box_gradient(p, target_size.x, (float)dragborder, w, b, b, w);
  p.y += size.y - (float)dragborder;
  draw_box_gradient(p, target_size.x, (float)dragborder, vsx_widget_skin::get_instance()->get_color(0), vsx_widget_skin::get_instance()->get_color(0), vsx_widget_skin::get_instance()->get_color(1), vsx_widget_skin::get_instance()->get_color(1));

  //  draw_box_gradient(p, target_size.x, dragborder, skin_color[0], skin_color[0], skin_color[1], gr);
  p.x += size.x - (float)dragborder;

  //p.y +=
  //draw_box_gradient(p, dragborder, -(target_size.y-dragborder), gr, skin_color[0], skin_color[1], skin_color[1]);
}

vsx_widget_panel::vsx_widget_panel() {
  size_min.x = 0;
  size_min.y = 0;
  size_from_parent = false;
  pos_from_parent = false;
  allow_move_y = allow_move_x = false;
}

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************

void vsx_widget_split_panel::set_border(float border) {
  splitter_size = border*0.5f;
  vsx_widget::set_border(border);
}

vsx_widget_split_panel::vsx_widget_split_panel() {
  splitter_size = (float)dragborder*0.5f;
  split_pos = 0.2f;
  one = (vsx_widget_panel*)add(new vsx_widget_panel,"1");
  one->size_from_parent = true;
  two = (vsx_widget_panel*)add(new vsx_widget_panel,"2");
  two->size_from_parent = true;
  orientation = VSX_WIDGET_SPLIT_PANEL_VERT;
}

void vsx_widget_split_panel::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
}

void vsx_widget_split_panel::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) 
{
  VSX_UNUSED(coords);
  split_pos = CLAMP(((distance.center.y+(size.y)*0.5f)/(size.y)), 0.05f, 0.95f);
}


int vsx_widget_split_panel::inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global)
{
  if (coord_type == VSX_WIDGET_COORD_CENTER) {
    if (
      //(test.y > global.y-size.y*0.5f) && (test.y < global.y+size.y*0.5f) &&
      (test.x > global.x-size.x*0.5f) && 
      (test.x < global.x+size.x*0.5f) &&
      (test.y > global.y-size.y*0.5f+split_pos*sy) && 
      (test.y < global.y-size.y*0.5f+split_pos*sy+splitter_size)
    ) {
      return 1;
    }
  }/* else
  if (coord_type == VSX_WIDGET_COORD_CORNER) {
    if (test.x > global.x && 
        test.x < global.x+size.x && 
        test.y < global.y && 
        test.y > global.y-size.y) 
    {
      return 2;
    }
  }*/
  return 0;
}


void vsx_widget_split_panel::i_draw()
{
  calc_size();
  vsx_vector3<> p = calc_pos();
  if (render_type == render_2d)
  p.z = 0.0f;
  if (orientation == VSX_WIDGET_SPLIT_PANEL_VERT) {
    sy = size.y-splitter_size;
    float sx = size.x;

    if (fabsf(two->size_min.y) > 0.0f) {
      if (sy*(1-split_pos) < two->size_min.y) {
        split_pos = (sy-two->size_min.y)/sy;
      }
    }
    if (fabsf(two->size_max.y) > 0.0f) {
      if (sy*(1-split_pos) > two->size_max.y) {
        split_pos = (sy-two->size_max.y)/sy;
      }
    }

    if (fabsf(one->size_min.y) > 0.0f) {
      if (sy*(split_pos) < one->size_min.y) {
        split_pos = (one->size_min.y)/sy;
      }
    }
    if (fabsf(one->size_max.y) > 0.0f) {
      if (sy*(split_pos) > two->size_max.y) {
        split_pos = (one->size_max.y)/sy;
      }
    }
      
    two->target_size.x = sx;
    two->target_size.y = sy*(1-split_pos);
    two->size = two->target_size;


    one->target_size.x = sx;
    one->target_size.y = sy*split_pos;
    one->size = one->target_size;
      
    one->target_pos.x = 0;
    one->target_pos.y = -target_size.y*0.5f + 0.5f*split_pos*sy;
    one->target_pos.z = pos.z;
    one->pos = one->target_pos;

    two->target_pos.x = 0;
    two->target_pos.y = target_size.y*0.5f - 0.5f*(1.0f-split_pos)*sy;
    two->target_pos.z = pos.z;
    two->pos = two->target_pos;

    glColor4f(1,1,1,1);
    if (!FLOAT_EQUALS(splitter_size, 0) )
    draw_box(p+vsx_vector3<>(0,split_pos*sy),sx,splitter_size);
  }
}
