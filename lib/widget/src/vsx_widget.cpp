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

#include <vsx_gl_global.h>
#include "vsx_widget.h"

bool vsx_widget::global_delete = false;


int vsx_widget::static_widget_id_accumulator = 0;
std::map<vsx_string<>, vsx_widget*> vsx_widget::global_widget_list;
std::map<int, vsx_widget*> vsx_widget::global_index_list;


vsx_widget *vsx_widget::root;

vsx_widget_camera vsx_widget::camera;

vsx_widget *vsx_widget::m_focus = 0;
vsx_widget *vsx_widget::m_o_focus = 0;
vsx_widget *vsx_widget::k_focus = 0;
vsx_widget *vsx_widget::a_focus = 0;

bool vsx_widget::mouse_down_l;
bool vsx_widget::mouse_down_r;

int vsx_widget::frames;

float vsx_widget::screen_x;
float vsx_widget::screen_y;
float vsx_widget::screen_aspect;

unsigned long vsx_widget::help_timestamp = 0;
vsx_widget_distance vsx_widget::mouse_down_pos;

int vsx_widget::viewport[4];
double vsx_widget::modelMatrix[16];
double vsx_widget::projMatrix[16];

std::map<vsx_string<>, vsx_string<> > vsx_widget::configuration;

float vsx_widget::global_framerate_limit = -1;
vsx_widget* vsx_widget::last_clicked;
vsx_font vsx_widget::font;
bool vsx_widget::performance_mode = false;


vsx_widget::vsx_widget()
  :
    commands(false),
    command_q_b(false),
    command_q_f(false)
{
  widget_type = VSX_WIDGET_TYPE_WIDGET;
  interpolation_speed = 10.0f;
  interpolating_size = false;
  interpolating_pos = false;
  render_type = render_3d;
  coord_type = VSX_WIDGET_COORD_CENTER;
  coord_related_parent = true;
  init_run = false;
  id = static_widget_id_accumulator;
  global_index_list[id] = this;
  ++static_widget_id_accumulator;
  parent = this;
  double_click_d[0] =
  double_click_d[1] =
  double_click_d[2] =
  double_click_d[3] =
  double_click_d[4] = vsx_widget_time::get_instance()->get_time();
  mouse_down_l = false;
  mouse_down_r = false;
  enabled = 1; // is this object to receive events?
  constrained_x = true;
  constrained_y = true;
  region_exclusive = false;
  size.x = 20; // x size
  size.y = 20; // y size
  size.z = 0; // z size
  support_interpolation = false;
  topmost = false;
  size_min.x = 0.1f;
  size_min.y = 0.1f;
  marked_for_deletion = false;
  resize_i = 0;
  allow_move_x = true;
  allow_move_y = true;
  allow_resize_x = false;
  allow_resize_y = false;
  dragborder = 0.0025;
  support_scaling = false;
  font_size = 0.025f;
  menu_temp_disable = false;

  // init profiler
  VSXP_CLASS_CONSTRUCTOR
}


vsx_widget::~vsx_widget()
{
  global_index_list.erase(id);
  if (global_widget_list.find(name) != global_widget_list.end())
  {
    global_widget_list.erase(name);
  }

  if (this == root)
    global_delete = true;

  if (global_delete)
  {
    for (std::list <vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) {
      (*it)->on_delete();
      delete *it;
    }
  }
  if (this == root)
  {
    global_widget_list.clear();
    global_index_list.clear();
    global_delete = false;
    font.unload();
  }

}

//******************************************************************************
// CONSTRUCTOR END
//******************************************************************************



vsx_vector3<> vsx_widget::get_pos_p() {
  if (parent != this && coord_related_parent) {
    vsx_vector3<> tt = pos;
    if (parent->render_type == render_type )
    tt = tt + parent->get_pos_p();
    return tt;
  }
  return pos;
}

vsx_vector3<> vsx_widget::get_target_pos_p() {
  if (parent != this && coord_related_parent) {
    vsx_vector3<> tt = target_pos;
    if (parent->render_type == render_type)
    tt = tt + parent->get_target_pos_p();
    return tt;
  }
  return target_pos;
}


bool vsx_widget::find_child_by_type(unsigned long t)
{
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    if ((*children_iter)->widget_type==t) return true;
  return false;
}

bool vsx_widget::find_child_by_name(vsx_string<>n)
{
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    if ((*children_iter)->name==n) return true;
  return false;
}



//------------------------------------------------------------------------------
// MOUSE MOVE PASSIVE
//------------------------------------------------------------------------------

int vsx_widget::inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global)
{
   if (coord_type == VSX_WIDGET_COORD_CENTER)
   {
    if (
      (test.x > global.x-target_size.x*0.5f) && (test.x < global.x+target_size.x*0.5f) &&
      (test.y > global.y-target_size.y*0.5f) && (test.y < global.y+target_size.y*0.5f)
    )
    {
      return coord_type;
    }
    return 0;
  }
  if (coord_type == VSX_WIDGET_COORD_CORNER)
  {
    if (test.x > global.x &&
        test.x < global.x+target_size.x &&
        test.y > global.y &&
        test.y < global.y+target_size.y)
    {
      return coord_type;
    }
    return 0;
  }
  return 0;
}

bool vsx_widget::inside_xy(vsx_widget_coords &coords, vsx_widget_distance &result)
{
  vsx_vector3<>* test = 0x0;
  vsx_vector3<>* global = 0x0;

  if (!coord_related_parent)
  {
    coords.world_local = target_pos;
    coords.screen_local = target_pos;
  }

  if (render_type == render_2d)
  {
    test = &(coords.screen_global);
    global = &(coords.screen_local);
  }
  else
  if (render_type == render_3d) {
    test = &(coords.world_global);
    global = &(coords.world_local);
  }

  if (!global)
    return false;

  if (!test)
    return false;

  switch (inside_xy_l(*test,*global))
  {
    case VSX_WIDGET_COORD_CENTER:
      result.center.x = (*test).x-((*global).x);
      result.center.y = (*test).y-(*global).y;
      result.corner.x = result.center.x + target_size.x*0.5f;
      result.corner.y = result.center.y + target_size.y*0.5f;
      if (result.corner.y > 0.0f &&
      result.corner.x > 0.0f &&
      result.corner.x < target_size.x && result.corner.y < target_size.y)
      return true;
      else
      return false;
    case VSX_WIDGET_COORD_CORNER:
      result.corner.x = -((*global).x-(*test).x);
      result.corner.y = -((*global).y-(*test).y);
      result.center.x = result.corner.x - target_size.x*0.5f;
      result.center.y = result.corner.y - target_size.y*0.5f;
      return true;
    default:
      return false;
  }
}



vsx_widget* vsx_widget::find_component(vsx_widget_coords coords, vsx_widget_distance &result_distance, int depth)
{
  if (depth == 0)
  {
    coords.world_local.x = coords.world_local.y = 0.0f;
    coords.screen_local.x = coords.screen_local.y = 0.0f;
  }

  vsx_widget *tt;
  reqrv(enabled, 0);
  reqrv(visible > 0.0f, 0);


  if (coord_related_parent)
  {
    if (render_type == render_3d)
    {
      coords.world_local.x += target_pos.x;
      coords.world_local.y += target_pos.y;
    }
    else
    {
      coords.screen_local.x += target_pos.x;
      coords.screen_local.y += target_pos.y;
    }
  }


  if (inside_xy(coords,result_distance))
  {
    if (region_exclusive)
    {
      return this;
    }

    if (!children.size())
      return this;

    // scan children
    children.reverse();
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    {
      vsx_widget_distance dist = result_distance;
      tt = (*children_iter)->find_component(coords,dist,1);
      if (tt)
      {
        children.reverse();
        result_distance = dist;
        return tt;
      }
    }
    children.reverse();
    return this;
  }


  if (!children.size())
    return 0;

  children.reverse();

  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
  {
    vsx_widget_distance dist = result_distance;
    tt = (*children_iter)->find_component(coords,dist,1);
    if (tt)
    {
      result_distance = dist;
      children.reverse();
      return tt;
    }
  }
  children.reverse();
  return 0;
} // find_component



void vsx_widget::input_mouse_wheel(float y)
{
	if (m_o_focus)
    m_o_focus->event_mouse_wheel(y);
}

void vsx_widget::event_mouse_wheel(float y)
{
  VSX_UNUSED(y);
}

void vsx_widget::input_mouse_move_passive(float x, float y)
{
  vsx_mouse_control.set_cursor(MOUSE_CURSOR_ARROW);
  vsx_widget_coords coord;
  coord.init(x,y);
  vsx_widget_distance result;
  vsx_widget *t = find_component(coord,result);
  if (t != 0)
  {
    t->event_mouse_move_passive(result,coord);
    m_o_focus = t;
  }
}


void vsx_widget::input_mouse_down(float x, float y, int button)
{
  vsx_mouse_control.set_cursor(MOUSE_CURSOR_ARROW);
  vsx_mouse_control.show_cursor();

  vsx_widget_coords coord;
  coord.init(x,y);
  vsx_widget_distance result;
  vsx_widget *widget_obj_at_cursor = find_component(coord,result);
  if (!widget_obj_at_cursor)
    return;

  // set up coordinates for
  mouse_down_pos = result;
  k_focus = widget_obj_at_cursor;
  m_focus = widget_obj_at_cursor;
  widget_obj_at_cursor->event_mouse_down(result,coord,button);

  double itime = (vsx_widget_time::get_instance()->get_time());

  if (
      itime
      -
      widget_obj_at_cursor->double_click_d[button]
      < 0.3
      &&
      (last_clicked == widget_obj_at_cursor))
  {
    widget_obj_at_cursor->event_mouse_double_click(result,coord,button);
    widget_obj_at_cursor->double_click_d[button] = 0;
    last_clicked = 0;
  }
  else
  {
    widget_obj_at_cursor->double_click_d[button] = vsx_widget_time::get_instance()->get_time();
    last_clicked = widget_obj_at_cursor;
  }
}

void vsx_widget::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);

  if (this != root)
  parent->front(this);

  if (button == 0)
  {
    a_focus = this;
    mouse_down_l = true;
    if (support_scaling)
    if (vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_ctrl()) {
      scaling_start = distance.center;
      scaling_start_size = size;
      scaling_start_pos = target_pos;
    }
  }

  if (button == 2)
  {
    mouse_down_r = true;
  }
}


void vsx_widget::input_mouse_move(float x, float y)
{
  vsx_mouse_control.set_cursor(MOUSE_CURSOR_ARROW);
  if (m_focus)
  {
    vsx_widget_coords coord;
    vsx_widget_distance distance;
    m_focus->calculate_mouse_distance(x,y,coord,distance);
    m_focus->event_mouse_move(distance,coord);
  }
}

void vsx_widget::input_mouse_up(float x, float y, int button)
{
  vsx_mouse_control.set_cursor(MOUSE_CURSOR_ARROW);
  if (m_focus)
  {
    vsx_widget_coords coord;
    vsx_widget_distance distance;
    m_focus->calculate_mouse_distance(x,y,coord,distance);

    if (button == 0)
      m_focus->mouse_down_l = false;

    if (button == 2)
      m_focus->mouse_down_r = false;

    m_focus->event_mouse_up(distance,coord,button);
    m_focus = 0;
  }
}




void vsx_widget::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);
  req(visible > 0.0f);

  resize_i = 0;
  float db = (float)(dragborder*1.5);
  if (allow_resize_y) {
    // /|
    // ||
    if ((distance.corner.y < size.y) && (distance.corner.y > size.y-db)) resize_i += 1;
    // ||
    // \|
    if (distance.corner.y < db) resize_i += 4;
  }
  if (allow_resize_x) {
    // |          ---->|
    if ((distance.corner.x < target_size.x) && (distance.corner.x > target_size.x-db)) resize_i += 2;
    // |<----          |
    if (distance.corner.x < db) resize_i += 8;
  }

  if ((resize_i == 2) || (resize_i == 8))
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_WE);

  if ((resize_i == 1) || (resize_i == 4))
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NS);

  if (resize_i == 6)
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NWSE);

  if (resize_i == 9)
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NWSE);

  if (resize_i == 3)
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NESW);

  if (resize_i == 12)
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NESW);
}

void vsx_widget::calculate_mouse_distance(float x, float y, vsx_widget_coords &coord, vsx_widget_distance &distance)
{
  coord.init(x,y);
  vsx_vector3<> world;
  if (render_type == render_2d)
  {
    world = coord.screen_global;
  }
  if (render_type == render_3d)
  {
    world = coord.world_global;
  }

  world = world - get_target_pos_p();

  if (coord_type == VSX_WIDGET_COORD_CENTER)
  {
    distance.center = world;
    distance.corner.x = target_size.x*0.5f - world.x;
    distance.corner.y = target_size.y*0.5f - world.y;
    return;
  }

  distance.corner = world;
  distance.center.x = world.x - target_size.x*0.5f;
  distance.center.y = world.y - target_size.y*0.5f;
}

void vsx_widget::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  req(button == 2);
  req(menu);

  if (menu_temp_disable)
  {
    menu_temp_disable = false;
    return;
  }

  if (menu->render_type == vsx_widget_render_type::render_3d)
    menu->pos = menu->target_pos = coords.world_global;
  else
    menu->pos = menu->target_pos = coords.screen_global;
  menu->show();
}


void vsx_widget::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);

  if (!mouse_down_l)
    return;

  if
  (
    support_scaling
    &&
    vsx_input_keyboard.pressed_alt()
    &&
    !vsx_input_keyboard.pressed_ctrl()
  )
  {
    target_size.x += (distance.center.x-mouse_down_pos.center.x)*2.0f;

    if (target_size.x < this->size_min.x)
      target_size.x = this->size_min.x;

    interpolating_size = true;
    mouse_down_pos = distance;
  }

  if (resize_i)
  {
    resize(distance);
  }
  else
  {
    move_d(vsx_vector3<>(distance.center.x-mouse_down_pos.center.x,distance.center.y-mouse_down_pos.center.y));
  }
}

void vsx_widget::resize(vsx_widget_distance distance)
{
  req(visible > 0.0f);

  if ((resize_i == 2) || (resize_i == 8))
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_WE);

  if ((resize_i == 1) || (resize_i == 4))
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NS);

  if (resize_i == 6)
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NWSE);

  if (resize_i == 9)
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NWSE);

  if (resize_i == 3)
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NESW);

  if (resize_i == 12)
    vsx_mouse_control.set_cursor(MOUSE_CURSOR_NESW);

  float dx = distance.center.x-mouse_down_pos.center.x;
  float dy = mouse_down_pos.center.y - distance.center.y;

  // BOTTOM
  if (resize_i & 1)
  {
    if (coord_type == VSX_WIDGET_COORD_CORNER) {
      target_size.y -= dy;
      if (target_size.y < size_min.y)
      target_size.y = size_min.y;
      else
      mouse_down_pos.center.y -= dy*0.5f;
    } else {
      target_size.y -= dy*2.0f;
      if (target_size.y < size_min.y)
      target_size.y = size_min.y;
      else
      mouse_down_pos.center.y = distance.center.y;
    }
  }


  // TOP
  if (resize_i & 4)
  {
    if (coord_type == VSX_WIDGET_COORD_CORNER)
    {
      target_size.y += dy;
      if (!(target_size.y < size_min.y)) {
        target_pos.y -= dy;
      mouse_down_pos.center.y -= dy*0.5f;
      } else target_size.y = size_min.y;

    } else {
      float dy = -(distance.center.y-mouse_down_pos.center.y);
      target_size.y += dy*2.0f;
      mouse_down_pos.center.y = distance.center.y;

    }
  }


  // LEFT
  if (resize_i & 8)
  {
    if (coord_type == VSX_WIDGET_COORD_CORNER)
    {
      target_size.x -= dx;
      if (target_size.x < size_min.x)
      {
        target_size.x = size_min.x;
      }
      else
      {
        target_pos.x += dx;
        mouse_down_pos.center.x += dx*0.5f;
      }
    }
    else
    {
      target_size.x -= dx*2.0f;
      mouse_down_pos.center.x = distance.center.x;
    }
  }



  // RIGHT
  if (resize_i & 2)
  {
    if (coord_type == VSX_WIDGET_COORD_CENTER)
    {
      mouse_down_pos.center.x += dx;
      target_size.x += dx*2.0f;
    }
    else
    {
      mouse_down_pos.center.x += dx*0.5f;
      target_size.x += dx;
    }


    if (target_size.x < size_min.x)
    {
      target_size.x = size_min.x;
    }
  }
  resize_to(target_size);
}



void vsx_widget::front(vsx_widget* t)
{
  children_iter = children.end();
  children_iter--;
  if
  (
    !(*children_iter)->topmost
    ||
    t->topmost
    ||
    t->render_type == render_2d
  )
  {
    children.remove(t);
    children.push_back(t);
  }
  else
  {
    if (t != *children_iter)
    {
      children.remove(t);
      children_iter = children.end();
      --children_iter;
      if (children_iter != children.begin())
      while ((*(children_iter))->topmost && children_iter != children.begin()) {
      --children_iter;
      };
      if (!((*children_iter))->topmost)
      ++children_iter;
      children.insert(children_iter,1,(vsx_widget *)t);
    }
  }
  if (parent != this)
    parent->front(this);
}

void vsx_widget::before_delete()
{
}

void vsx_widget::on_delete()
{
}


void vsx_widget::move(double x, double y, double z)
{
	event_move_scale();

  if (constrained_x)
  {
    if (x > parent->size.x/2-size.x/2)
      x = parent->size.x/2-size.x/2;

    if (x < -(parent->size.x/2-size.x/2))
      x = -(parent->size.x/2-size.x/2);
  }

  if (constrained_y)
  {
    if (y > parent->size.y/2-size.y/2)
      y = parent->size.y/2-size.y/2;

    if (y < -(parent->size.y/2-size.y/2))
      y = -(parent->size.y/2-size.y/2);
  }

  if (allow_move_x)
    target_pos.x = (float)x;

  if (allow_move_y)
    target_pos.y = (float)y;

  target_pos.z = (float)z;

  if (support_interpolation)
  {
    interpolating_pos = true;
  }
  else
  {
    pos = target_pos;
  }
}

void vsx_widget::move(vsx_vector3<> t)
{
  move(t.x,t.y,t.z);
}

void vsx_widget::move_d(vsx_vector3<> t)
{
  if (support_interpolation)
  {
    move(
      target_pos.x + t.x,
      target_pos.y + t.y,
      target_pos.z + t.z
    );
    return;
  }

  move(
    pos.x+t.x,
    pos.y+t.y,
    pos.z+t.z
  );
}


void vsx_widget::show_children()
{
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
  {
    (*it)->visible = 1;
    (*it)->show_children();
  }
}

void vsx_widget::hide_children()
{
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
  {
    (*it)->visible = 0;
    (*it)->hide_children();
  }
}


vsx_widget* vsx_widget::add(vsx_widget *widget_pointer, vsx_string<>name)
{
  // set the name of the widget internally
  widget_pointer->name = name;

  // setup the parent relationship
  widget_pointer->parent = this;

  // setup the command parent relationship
  widget_pointer->cmd_parent = this;

  // if this is intended to be topmost
  if (widget_pointer->topmost)
  {
    children.push_back(widget_pointer);
  }
  else
  {
    // if needed, iterate backwards to not override topmost widgets
    if (children.size())
    {
      children_iter = children.end();
      --children_iter;
      if (children_iter != children.begin())
      {
        while ((*(children_iter))->topmost && children_iter != children.begin())
        {
          --children_iter;
        }
      }
      // now that we found our pos, insert the widget here
      children.insert(children_iter,1,widget_pointer);
    }
    else
    {
      // nothing special, just push it on the stack
      children.push_back(widget_pointer);
    }
  }
  // add the pointer to the global list
  global_widget_list[name] = widget_pointer;

  // add the pointer to the local by-name lookup list
  l_list[name] = widget_pointer;

  // finally return the widget pointer
  return widget_pointer;
}

void vsx_widget::vsx_command_process_f()
{
  if (this != root)
  {
    vsx_command_queue_f();
    return;
  }

  vsx_command_s *c = 0;

  while ( (c = command_q_f.pop()) )
  {
    if (c->cmd == "delete")
    {
      vsx_widget *t = find( vsx_string_helper::s2i(c->cmd_data) );

      if (t)
      {
        #ifdef VSX_DEBUG
        printf("deleting component %d , %s\n",t->id,t->name.c_str());
        #endif
        t->delete_();


        #ifdef VSX_DEBUG
        printf("before on_delete\n",t->id);
        #endif
        t->on_delete();


        #ifdef VSX_DEBUG
        printf("removing from children\n",t->id);
        #endif
        t->parent->children.remove(t);
        t->parent->l_list.erase(t->name);

        if (t->parent->marked_for_deletion)
        {
          m_focus = k_focus = a_focus = root;
        } else
        {
          m_focus = t->parent;
          k_focus = t->parent;
          a_focus = t->parent;
        }

        #ifdef VSX_DEBUG
        printf("actual deletion\n",t->id);
        #endif
        delete t;
        #ifdef VSX_DEBUG
        printf("after actual deletion\n",t->id);
        #endif
      }
    }
  }
  vsx_command_queue_f();
  #ifdef VSX_DEBUG
  printf("end ofqf %s\n",name.c_str());
  #endif
}


void vsx_widget::delete_()
{
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
  {
    (*it)->marked_for_deletion = true;
    (*it)->before_delete();
    (*it)->delete_();
    (*it)->on_delete();
    delete *it;
  }
}

void vsx_widget::_delete()
{
  if (marked_for_deletion)
    return;

  // mark us for deletion
  mark_for_deletion();

  // call deletion handlers
  before_delete();

  // request our deletion
  root->command_q_f.add("delete",id, true);
}

void vsx_widget::mark_for_deletion()
{
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
  {
    (*it)->mark_for_deletion();
  }
  marked_for_deletion = true;
}


void vsx_widget::delete_all_by_type(unsigned long t)
{
  for (std::map<int, vsx_widget*>::iterator it = global_index_list.begin();  it != global_index_list.end(); ++it)
  {
    if ((*it).second->widget_type == t)
    {
      (*it).second->_delete();
    }
  }
}



void vsx_widget::set_render_type(vsx_widget_render_type new_render_type)
{
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
  {
    (*children_iter)->set_render_type(new_render_type);
  }
  render_type = new_render_type;
}

void vsx_widget::set_border(float border)
{
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) {
    (*children_iter)->set_border(border);
  }
  dragborder = border;
}

void vsx_widget::set_font_size(float fsize)
{
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
  {
    (*children_iter)->set_font_size(fsize);
  }
  font_size = fsize;
}



void vsx_widget::stop()
{
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    (*children_iter)->stop();
}

void vsx_widget::reinit()
{
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    (*children_iter)->reinit();
}


void vsx_widget::interpolate_pos()
{
  float tt =
      (float)vsx_widget_time::get_instance()->get_dtime() * interpolation_speed *
      vsx_widget_global_interpolation::get_instance()->get();
  if (tt > 1)
  {
    tt = 1;
    interpolating_pos = false;
  }

  pos.x = pos.x*(1-tt)+target_pos.x*tt;
  pos.y = pos.y*(1-tt)+target_pos.y*tt;

  if (
    ((int)round(pos.x*2000.0f) == (int)round(target_pos.x*2000.0f)) &&
    ((int)round(pos.y*2000.0f) == (int)round(target_pos.y*2000.0f))
  )
    interpolating_pos = false;
}

void vsx_widget::interpolate_size()
{
  float tt = (float)vsx_widget_time::get_instance()->get_dtime() * interpolation_speed
      * vsx_widget_global_interpolation::get_instance()->get();
  if (tt > 1)
  {
    tt = 1;
    interpolating_size = false;
  }
  size.x = size.x*(1-tt)+target_size.x*tt;
  size.y = size.y*(1-tt)+target_size.y*tt;
  if (
    ((int)round(size.x*2000) == (int)round(target_size.x*2000)) &&
    ((int)round(size.y*2000) == (int)round(target_size.y*2000))
  ) interpolating_size = false;
}



void vsx_widget::resize_to(vsx_vector3<> to_size)
{
  if (target_size.x < size_min.x)
    to_size.x = size_min.x;

  if (target_size.y < size_min.y)
    to_size.y = size_min.y;

	event_move_scale();

  if (support_interpolation)
  {
    interpolating_size = true;
    interpolating_pos = true;
    target_size = to_size;
  }
  else
  {
    size = target_size = to_size;
    pos = target_pos;
  }
}

void vsx_widget::draw()
{
  req(visible > 0.0f);

  if (render_type == render_3d)
  {
    i_draw();
  }
  draw_children_3d();
}

void vsx_widget::draw_2d()
{
	#ifdef VSX_DEBUG
	printf("draw 2d: %s\n",name.c_str());
	#endif
  req(visible > 0.0f);

  if (render_type == render_2d)
  {
    i_draw();
  }
  draw_children_2d();

  #ifdef VSX_DEBUG
	printf("end_draw 2d: %s\n",name.c_str());
	#endif
}


