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
#include <math.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include "vsx_font.h"
#include <filesystem/vsx_filesystem.h>
#include "vsx_widget_anchor.h"
#include "vsx_widget_comp.h"
#include <math/vsx_bezier_calc.h>
#include <texture/vsx_texture.h>

#include "widgets/vsx_widget_popup_menu.h"
#include "vsx_widget_connector_bezier.h"
#include <gl_helper.h>

bool vsx_widget_connector_bezier::receiving_focus;
float vsx_widget_connector_bezier::dim_alpha = 1.0f;

void vsx_widget_connector_bezier::command_process_back_queue(vsx_command_s *t) {
  if (t->cmd == "disconnect") {
    // syntax:
    //   param_unalias [-1/1] [component] [param_name]
    if (alias_conn) 
    {
      vsx_string<>bb;
      if (((vsx_widget_anchor*)parent)->io == -1) bb = "-1"; else bb = "1";
      if (((vsx_widget_anchor*)parent)->io == -1)
      command_q_b.add_raw("param_unalias "+bb+" "+((vsx_widget_component*)((vsx_widget_anchor*)destination)->component)->name+" "+destination->name);
      else
      command_q_b.add_raw("param_unalias "+bb+" "+((vsx_widget_component*)((vsx_widget_anchor*)parent)->component)->name+" "+parent->name);
    } else
    {
      command_q_b.add_raw(
        "param_disconnect "+
        ((vsx_widget_anchor*)parent)->component->name+" "+
        parent->name+" "+
        ((vsx_widget_anchor*)destination)->component->name+" "+
        ((vsx_widget_anchor*)destination)->name
      );
    }
    parent->vsx_command_queue_b(this);
  }
}

int vsx_widget_connector_bezier::inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global)
{
  vsx_vector3<> world = test-global;
  if (!visible) return false;
  if (((vsx_widget_component*)((vsx_widget_anchor*)parent)->component)->ethereal_all) return false;
  if (!receiving_focus) return false;
  if (((vsx_widget_anchor*)parent)->get_drag_status()) return false;
  if (destination)
  if (!((vsx_widget_anchor*)parent)->component->visible || !((vsx_widget_anchor*)destination)->component->visible) return false;
  if (open && order != -1) {
    float s = parent->size.x * 0.2f;
    pv.y = world.y - (parent->size.y/2 - ((float)order)*s*2.2);
    pv.x = world.x + parent->size.y*0.7;
    if ( fabs(pv.x) < s && fabs(pv.y) < s)
    {
      return coord_type;
    }
  }
  #define sensitivity 0.005f

  //if (alt || !ctrl) return 0;
  //if (alt && ctrl) return 0;
  // NOTE! sx/sy is lower right!

  float tsx = sx;
  float tex = ex;
  if (ex > sx)
  {
    tex = sx;
    tsx = ex;
  }

  if (test.x > tsx+sensitivity || test.x < tex-sensitivity)
  {
    #if VSX_DEBUG
      printf("outside sx/ex\n");
    #endif
    return 0;
  }
  float tsy = sy;
  float tey = ey;
  if (ey < sy)
  {
    tey = sy;
    tsy = ey;
  }
    
  
  if (test.y < tsy-sensitivity || test.y > tey+sensitivity) 
  {
    #if VSX_DEBUG
      printf("outside sy/ey\n");
    #endif
    return 0;
  }
  // calculate t position from y

  vsx_bezier_calc<float> bez_calc;
  bez_calc.x0 = sx;
  bez_calc.y0 = sy;

  bez_calc.x1 = sx+(ex-sx)*0.5f;
  bez_calc.y1 = sy;

  bez_calc.x2 = ex-(ex-sx)*0.5f;
  bez_calc.y2 = ey;

  bez_calc.x3 = ex;
  bez_calc.y3 = ey;

  int a = 0, b = 0, c = 0, d = 0;

  if (test.y < tsy+parent->size.y*0.5f) a=1;//printf("A\n");
  if (test.x > tsx-parent->size.x*0.5f) b=1;//printf("B\n");//return 0;
  if (test.x < tex+parent->size.x*0.5f) c=1;//printf("C\n");//return 0;
  //printf("%f  %f\n",test.y,ey);
  if (test.y > tey-parent->size.y*0.5f) d=1;//printf("D\n");
  //printf("a:%d b:%d c:%d d:%d\n",a,b,c,d);
  if (a && b) return 0;
  if (a && c) return 0;
  if (c && d) return 0;
  if (b && d) return 0;
  //printf("ex-sx: %f\n",ex-sx);
  if (fabs(ex-sx) < 0.005)
  {
    if (b && c) return coord_type;
  } else
  if (fabs(ex-sx) > 0.02)
  {
    bez_calc.init();
    float t = bez_calc.t_from_x(test.x);
    //printf("global: %f,%f    t: %f\n", test.x, test.y, t);
    float y = bez_calc.y_from_t(t);
    if ( fabs(test.y - y) < sensitivity )
    {
      #if VSX_DEBUG
      printf("hit on bezier!\n");
      #endif
      return coord_type;
    }
  } else
  {
    // newton raphson fails on too narrow lines; treat this one as straight line
    //  *
    //   '
    //dy  '
    // dx  *
    float dy = ey-sy;
    float dx = ex-sx;
    //printf("test.x %f  test.y %f\n",test.x-sx,test.y-sy);
    float dxdy = dy/dx;
    float dty = (test.y-sy);
    float xtest = dty * dxdy;
    //printf("calc: %f\n",xtest);
    if ( (test.x > xtest-sensitivity) || test.x < xtest+sensitivity)
      return coord_type;
    //
  }
  return 0;
}
void vsx_widget_connector_bezier::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);


  if (button == 0)
  {
    mouse_down_l = 1;
    m_focus = this;
    a_focus = this;
  }
}

void vsx_widget_connector_bezier::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);
  move_d(vsx_vector3<>(0.0f,(distance.center.y-mouse_down_pos.center.y)));
}

void vsx_widget_connector_bezier::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  if (open)
  {
    command_q_b.add_raw("connections_order_int 1");
    parent->vsx_command_queue_b(this);
    set_pos(vsx_vector3<>(0.0f));
  }
  vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_connector_bezier::draw() 
{
  float alpha = 1.0f;
  if (marked_for_deletion) return;

  if ( ((vsx_widget_component*) ((vsx_widget_anchor*)parent)->component)->deleting )
  {
    return;
  }
  if ( destination && ( (vsx_widget_component*) ((vsx_widget_anchor*)destination)->component)->deleting)
  {
    return;
  }


  if (destination) {
    if (!((vsx_widget_anchor*)destination)->component->visible) return;
      if (
        ((vsx_widget_component*)((vsx_widget_anchor*)parent)->component)->open
        && ((vsx_widget_anchor*)parent)->io == -1
        ) {
        alpha = 0.1f;
      }
  } //else return;
  pv = parent->get_pos_p();
  if (!parent->visible)
  {
    open = false;
    ((vsx_widget_anchor*)parent)->conn_open = false;
  }

  if (((vsx_widget_anchor*)parent)->io == -1)
  if (open && order != -1) {
    float s = parent->size.x * 0.2;

    pv.x = pos.x + pv.x - parent->size.y*0.7;
    pv.y = pos.y + pv.y + parent->size.y/2 - ((float)order)*s*2.2;
    pv.z = pos.z;
    if (a_focus == this)
    glColor4f(1,0.5,0.5,alpha);
      else
    glColor4f(0.8,0.8,0.8,alpha);
  }

  if (!destination) {
    //printf("a1:%s:%d\n",name.c_str(),destination);
    ex = pv.x+size.x;
    ey = pv.y+size.y;
  } else {
    vsx_vector3<> dv = destination->get_pos_p();
    ex = dv.x;
    ey = dv.y;
  }

  // set color (if not already done)
  if (!color_initialized)
  {
    vsx_string<>p_type = ((vsx_widget_anchor*)parent)->p_type;
    if (p_type == "complex")
    {
      color = vsx_color<>(216.0f/255.0f,76.0f/255.0f,202.0f/255.0f,1.0f);
    } else
    if (p_type == "float" || p_type == "float_array")
    {
      color = vsx_color<>(20.0f/255.0f,121.0f/255.0f,72.0f/255.0f,1.0f);
    } else
    if (p_type == "float3" || p_type == "float3_array")
    {
      color = vsx_color<>(64.0f/255.0f,190.0f/255.0f,78.0f/255.0f,0.8f);
    } else
    if (p_type == "float4")
    {
      color = vsx_color<>(142.0f/255.0f,49.0f/255.0f,168.0f/255.0f,1.0f);
    } else
    if (p_type == "texture")
    {
      color = vsx_color<>(15.0f/255.0f,99.0f/255.0f,206.0f/255.0f,1.0f);
    } else
    if (p_type == "render")
    {
      color = vsx_color<>(236.0f/255.0f,70.0f/255.0f,70.0f/255.0f,1.0f);
    } else
    if (p_type == "mesh")
    {
      color = vsx_color<>(0.0f/255.0f,255.0f/255.0f,255.0f/255.0f,1.0f);
    }
    else
    if (p_type == "bitmap")
    {
      color = vsx_color<>(255.0f/255.0f,0.0f/255.0f,255.0f/255.0f,1.0f);
    } else
    if (p_type == "particlesystem")
    {
      color = vsx_color<>(160.0f/255.0f,38.0f/255.0f,190.0f/255.0f,1.0f);
    } else
    if (p_type == "quaternion")
    {
      color = vsx_color<>(249.0f/255.0f,167.0f/255.0f,86.0f/255.0f,1.0f);
    }
    color_initialized = true;
  }


  glEnable(GL_LINE_SMOOTH);


  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  mtex_blob->bind();
    draw_box_texf(pv.x,pv.y,0,0.004f,0.004f);
    draw_box_texf((float)ex,(float)ey,0,0.004f,0.004f);
  mtex_blob->_bind();

  sx = pv.x;
  sy = pv.y;
  
  
  
  if (old_sx != sx || old_sy != sy || old_ex != ex || old_ey != ey)
  {
    // recalc
    vsx_bezier_calc<float> bez_calc;
    bez_calc.x0 = sx;
    bez_calc.y0 = sy;
    
    bez_calc.x1 = sx+(ex-sx)*0.5f;
    bez_calc.y1 = sy;
    
    bez_calc.x2 = ex-(ex-sx)*0.5f;
    bez_calc.y2 = ey;
    
    bez_calc.x3 = ex;
    bez_calc.y3 = ey;
    bez_calc.init();
    
    size_t i = 0;
    for (float t = 0.0f; t <= 1.05f; t += 0.05f)
    {
      cached_spline[i].x = bez_calc.x_from_t(t);
      cached_spline[i].y = bez_calc.y_from_t(t);
      i++;
    }
    old_sx = sx;
    old_sy = sy;
    old_ex = ex;
    old_ey = ey;
  }
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
  glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
      for (size_t i = 0; i < 21; i++)
      {
        glVertex3f(cached_spline[i].x,cached_spline[i].y,pos.z);
      }
    glEnd();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  float lw = 2.0f;

  if (m_focus == this || a_focus == this || k_focus == this)
  {
    lw = 1.0f+fmod(vsx_widget_time::get_instance()->get_time()*20.0f,5.0f);
    glLineWidth(lw);
  }
  else
  {
    glLineWidth(1.0f);
  }

  
  glLineWidth(lw);
  if (k_focus == this)
  {
    glColor4f(1,0.5,0.5,alpha);
  }
  else
  {
    if (parent == a_focus || destination == a_focus)
    {
      glColor4f(1,1,1,alpha);
    }
    else
    {
      glColor4f(1,1,1,alpha*(dim_alpha+dim_my_alpha));
    }
  }

  glColor4f(
    color.r,
    color.g,
    color.b,
    color.a
  );

  glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < 21; i++)
    {
      glVertex3f(cached_spline[i].x,cached_spline[i].y,pos.z);
    }
  glEnd();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLineWidth(lw * 0.5f);
  glBegin(GL_LINE_STRIP);
  for (size_t i = 0; i < 21; i++)
  {
    glColor4f(1.0,1.0,1.0,fabs(0.8f*sin(((float)i/21.0f) * PI + vsx_widget_time::get_instance()->get_time() * 2.5f)));
    glVertex3f(cached_spline[i].x,cached_spline[i].y,pos.z);
  }
  glEnd();

  glDisable(GL_LINE_SMOOTH);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  real_pos = pv;
  draw_children();
}

void vsx_widget_connector_bezier::before_delete() 
{
  marked_for_deletion = true;
  ((vsx_widget_anchor*)parent)->connections.remove(this);
  if (destination) {
    ((vsx_widget_anchor*)destination)->connectors.remove(this);
    // see if the destination is an alias that needs to go as well
    vsx_widget_anchor* dest = (vsx_widget_anchor*)destination;
    if (dest)
    if (dest->io != 1)
    if (dest->alias && dest->io == ((vsx_widget_anchor*)parent)->io) {
      dest->_delete();
      ((vsx_widget_component*)dest->component)->macro_fix_anchors();
    }
    destination = 0;
  }
}

void vsx_widget_connector_bezier::on_delete() 
{
  mtex_blob.reset(nullptr);

  if (!global_delete)
    ((vsx_widget_anchor*)parent)->update_connection_order();
}

void vsx_widget_connector_bezier::init() 
{
  order = 0;
  dim_my_alpha = 0.0f;
  open = false;
  if (init_run) return;
  widget_type = VSX_WIDGET_TYPE_CONNECTOR;
  title = "connector";
  coord_type = VSX_WIDGET_COORD_CORNER;
  transparent = false;
  menu = add(new vsx_widget_popup_menu,".connector_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "disconnect", "disconnect","");
  menu->size.x = 0.3;
  menu->size.y = 0.5;
  init_children();

  mtex_blob = vsx_texture_loader::load(
    vsx_widget_skin::get_instance()->skin_path_get() + "interface_extras/connection_blob.png",
    vsx::filesystem::get_instance(),
    true, // threaded
    vsx_bitmap::flip_vertical_hint,
    vsx_texture_gl::linear_interpolate_hint
  );

  mtex_blob->upload_gl();

  constrained_x = false;
  constrained_y = false;
  constrained_z = false;

  if (((vsx_widget_component*)((vsx_widget_anchor*)parent)->component)->component_type == "macro") macro_child = true;
  init_run = true;
  receiving_focus = true;
}


bool vsx_widget_connector_bezier::event_key_down(uint16_t key)
{
  if (key == VSX_SCANCODE_DELETE) {
    command_q_b.add_raw("disconnect");
    vsx_command_queue_b(this);
  }
  return true;
}



vsx_widget_connector_bezier::vsx_widget_connector_bezier()
{
  mtex_blob = 0x0;
  alias_conn = false;
  visible = 1;
  destination = 0;
  receiving_focus = true;
  support_interpolation = true;
  old_sx = old_sy = old_ex = old_ey = 0.0f;
  color_initialized = false;
}


