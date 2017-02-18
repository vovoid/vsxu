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
#include <gl_helper.h>
#include <map>
#include <list>
#include <vector>
#include <math.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include <filesystem/vsx_filesystem.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <texture/vsx_texture.h>


// local includes
#include "log/vsx_log_a.h"
#include "vsx_widget.h"
#include "vsx_widget_window.h"
#include "server/vsx_widget_server.h"
#include "server/vsx_widget_comp.h"
#include "vsx_widget_module_chooser.h"
#include "helpers/vsx_widget_assistant.h"
#include "helpers/vsx_widget_object_inspector.h"
#include "artiste_desktop.h"

// widget
#include <dialogs/dialog_query_string.h>


vsx_command_list module_chooser_colors(true);
std::map<vsx_string<>,vsx_color<> > mc_colors;
std::map<vsx_string<>,vsx_color<> > mc_r_colors;



//----------------------------------------------------------------------------------------------




//----------------------------------------------------------------------------------------------------------------------

int vsx_widget_ultra_chooser::inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global)
{
  VSX_UNUSED(test);
  VSX_UNUSED(global);

  if (visible >= 0.0f)
  {
    return coord_type;
  }
  else
  {
    return 0;
  }
}

void vsx_widget_ultra_chooser::init() {

}


void vsx_widget_ultra_chooser::center_on_item(vsx_string<>target) {
  vsx_widget_hyperbolic_tree* t = treedraw->findNode(target);
  if (t) {
    treedraw->translateToOrigin(t);
    treedraw->endTranslation();
    endpoint = clickpoint;
  }
}

void vsx_widget_ultra_chooser::cancel_drop() {
  visible = 1;
  drag_module = false;
  drag_dropped = false;
  allow_move = false;
  ((vsxu_assistant*)((vsx_artiste_desktop*)root)->assistant)->temp_show();
}

bool vsx_widget_ultra_chooser::event_key_down(uint16_t key)
{
  if (key == VSX_SCANCODE_ESCAPE) {
    if (drag_dropped) {
      cancel_drop();
    } else
    hide();
  } else
  // home
  if (key == VSX_SCANCODE_A)
  {
    HTCoord h;
    endpoint = h;
    h = treedraw->draw_root->getCoordinates();
    h.x+=0.001;
    h.y+=0.001;
    clickpoint = startpoint = h;
  }
  return true;
}

void vsx_widget_ultra_chooser::command_process_back_queue(vsx_command_s *t) {
  if (t->cmd == "cancel" || t->cmd == "component_create_name_cancel") {
    visible = 1;
    m_focus = this;
    k_focus = this;
    a_focus = this;
    drag_dropped = false;
    ((vsxu_assistant*)((vsx_artiste_desktop*)root)->assistant)->temp_show();
    parent->front(this);
  } else
  if (t->cmd == "component_create_name") {
    visible = 1;
    m_focus = this;
    k_focus = this;
    a_focus = this;
    drag_dropped = false;
    ((vsxu_assistant*)((vsx_artiste_desktop*)root)->assistant)->temp_show();
    // check wich type it is
    if (treedraw->selected->node->node->module_info->component_class == "macro") {
      // everything else will be contained in this macro, so modify the macro_name var
      vsx_string<>local_macro_name = t->cmd_data;
      // first create the macro
      command_q_b.add_raw("macro_create "+treedraw->selected->node->node->module_info->identifier+" "+macro_name+local_macro_name+" "+vsx_string_helper::f2s(drop_pos.x)+" "+vsx_string_helper::f2s(drop_pos.y));
      // in here, send all the commands contained in the macro to the server..
      // this is terrible yes, but it will hopefully work :)
      parent->vsx_command_queue_b(this);

    } else
    {
      // syntax:
      //  component_create math_logic;oscillator_dlux macro1.my_oscillator 0.013 0.204
      command_q_b.add_raw("component_create "+treedraw->selected->node->node->module_info->identifier+" "+macro_name+t->cmd_data+" "+vsx_string_helper::f2s(drop_pos.x)+" "+vsx_string_helper::f2s(drop_pos.y));
      parent->vsx_command_queue_b(this);
      ((vsxu_assistant*)((vsx_artiste_desktop*)root)->assistant)->temp_show();
    }
  }
}

bool doubleclix;

void vsx_widget_ultra_chooser::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);
  VSX_UNUSED(button);

  LOG_A("begin doubleclix\n");
  if (!treedraw) return;
  allow_move = false;
  HTCoord test_coord;
  test_coord.x = distance.corner.x;
  test_coord.y = distance.corner.y;
  vsx_widget_hyperbolic_tree* test = treedraw->draw_root->findNode__(test_coord);
    if (mode == 0) {
      startpoint.projectionStoE(0.5f*size.x, 0.5f*size.y,treedraw->getSOrigin(),treedraw->getSMax());
      clickpoint = startpoint;
    }
    if (test) {
      LOG_A("test");
      if (mode == 0) {
        HTCoord zn = test->getCoordinates();
        zn.x+=0.001;
        zn.y+=0.001;



        HTCoord zf;
        endpoint = zf;
        clickpoint = startpoint = zn;
        interpolation_speed = 3;
      }

      // act on impulse!
      if (mode == 1)
      if (test->node->node->isLeaf()) {
        LOG_A("command_q_b.add_raw("+command+" "+test->node->node->module_info->identifier+");");
        command_q_b.add_raw(command+" " + vsx_string_helper::base64_encode(test->node->node->module_info->identifier), VSX_COMMAND_GARBAGE_COLLECT);
        LOG_A("command_receiver->vsx_command_queue_b(this);")
        command_receiver->vsx_command_queue_b(this);
        LOG_A("((vsx_window_object_inspector*)object_inspector)->unload();")
        ((vsx_window_object_inspector*)object_inspector)->unload();
        LOG_A("hide();")
        hide();
        return;
      }

    }
    else {
      if (mode == 0) {
        startpoint.projectionStoE(distance.corner.x, distance.corner.y,treedraw->getSOrigin(),treedraw->getSMax());
        clickpoint = startpoint;
        endpoint.x = 0;
        endpoint.y = 0;
      }
    }
    doubleclix = true;
    LOG_A("end doubleclix\n");
}

void vsx_widget_ultra_chooser::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);

  HTCoord test_coord;
  test_coord.x = distance.corner.x;
  test_coord.y = distance.corner.y;
  vsx_widget_hyperbolic_tree* test = treedraw->draw_root->findNode__(test_coord);
  draw_tooltip = 0;
  tooltip_text = "";
  if (test) {
    if (test->node->node->isLeaf()) {
      if (test->node->node->module_info->description != "") {
        tooltip_text = test->node->node->module_info->description;
        if (tooltip_text.size() > 1) {
          tooltip_text = test->node->node->module_info->identifier + "\n---------------\n" + tooltip_text;
          draw_tooltip = 1;
          tooltip_pos = distance.corner;
          tooltip_pos.y-=0.01;
        }
      }
    }
  }
}

void vsx_widget_ultra_chooser::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);

  if (drag_module) { cancel_drop(); return; }
  HTCoord test_coord;
  test_coord.x = distance.corner.x;
  test_coord.y = distance.corner.y;
  vsx_widget_hyperbolic_tree* test = treedraw->draw_root->findNode__(test_coord);

  k_focus = this;
  if (!drag_dropped)
  if (button == 2) {
    hide();
    return;
  }
  if (drag_dropped && button == 2) {
    cancel_drop();
  }
  drag_dropped = false;
  if (!treedraw) return;
  if (animating) {
    treedraw->endTranslation();
    animating = false;
  }
  a_focus = this;
  m_focus = this;
  moved = false;

  if (((dialog_query_string*)name_dialog)->visible) {
    ((dialog_query_string*)name_dialog)->visible = 0;
    visible = 1;
  }
  if (!test) treedraw->selected = 0;
  if (!moved && test) {
    if (test->node->node->isLeaf())
    {
      drag_pos = distance.corner;
      drag_module = true;
      treedraw->selected = test;
      if (server) {
        help_text = ((vsx_widget_server*)server)->build_comp_helptext(test->node->node->module_info->identifier);
        ++help_timestamp;
      }
      if (test->node->node->module_info->component_class == "state") {
        drag_module = false;
      } else
      if (test->node->node->module_info->component_class == "resource") {
        drag_module = false;
        ((vsx_window_object_inspector*)object_inspector)->load_file(
          "resources/" +
          vsx_string_helper::str_replace<char>(
            "\\ ",
            " ",
            vsx_string_helper::str_replace<char>(
              ";",
              "/",
              test->node->node->module_info->identifier
            )
          )
        );
      }
    }
    // create a component here
  }
  if (!doubleclix)
  if (!drag_module) {
    interpolation_speed = 6;
    treedraw->endTranslation();
    startpoint.projectionStoE(distance.corner.x, distance.corner.y,treedraw->getSOrigin(),treedraw->getSMax());
    endpoint = clickpoint = startpoint;
  }
}

void vsx_widget_ultra_chooser::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  if (hide_) return;
  if (!treedraw) return;
  if (!allow_move) return;
  if (drag_module) {

    draw_tooltip = 0;
        vsx_widget_distance l_distance;
        vsx_widget* tt = root->find_component(coords,l_distance);
        if (tt)
    if (drag_pos.distance(distance.corner) > 0.0015 || drag_dropped) {
      drag_dropped = true;
      visible = 0.15;
      drag_pos = distance.corner;

      ((vsxu_assistant*)((vsx_artiste_desktop*)root)->assistant)->temp_hide();
    }
    return;
  }
  moved = true;

  v_anim_endpoint.x = distance.corner.x*2;
  v_anim_endpoint.y = distance.corner.y*2;
  endpoint.projectionStoE(distance.corner.x, distance.corner.y,treedraw->getSOrigin(),treedraw->getSMax());
}


void vsx_widget_ultra_chooser::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(button);

  if (!treedraw) return;
  {
    allow_move = true;
    drag_module = false;
    if (drag_dropped) {
      drag_module = false;
      visible = 0;
      if (mode == 0) {
        vsx_widget_distance l_distance;
        vsx_widget* tt = root->find_component(coords,l_distance);
        if (tt) {
          bool macro = false;
          if (tt->widget_type == VSX_WIDGET_TYPE_COMPONENT) if (((vsx_widget_component*)tt)->component_type == "macro") macro = true;
          if (macro) macro_name = tt->name+"."; else macro_name = "";
          if (tt->widget_type == VSX_WIDGET_TYPE_SERVER || macro) {
            drop_pos = l_distance.center;
            if (treedraw->selected) {
              if (server) {
                drag_dropped = false;
                if (vsx_input_keyboard.pressed_ctrl())
                ((dialog_query_string*)name_dialog)->show(((vsx_widget_server*)server)->get_unique_name(treedraw->selected->getName()));
                else
                {
                  command_q_b.add_raw("component_create_name "+((vsx_widget_server*)server)->get_unique_name(treedraw->selected->getName()));
                  vsx_command_queue_b(this);
                  visible = 1;
                  drag_dropped = false;
                  return;
                }
              }
              visible = 0.3;
            } else {
              visible = 1;
              drag_dropped = false;
            }
          }
        }
      }
    }
  }
}

void vsx_widget_ultra_chooser::draw_2d()
{
  float dtime = vsx_widget_time::get_instance()->get_dtime();
  vsx_vector3<> a(0.5f*size.x,0.5f*size.y);
  if (animating) {
    anim_x += dtime*2;
    double lx = anim_endpoint.x - anim_startpoint.x;
    double ly = anim_endpoint.y - anim_startpoint.y;
    if (anim_x < 0.5) {
      anim_curpoint.x = lx*2*anim_x*anim_x;
      anim_curpoint.y = ly*2*anim_x*anim_x;
      treedraw->translate(&anim_curpoint, &anim_startpoint);
    } else {
      if (anim_x < 1) {
        double a = 0.5 - fabs(anim_x-0.5);
        anim_curpoint.x = lx-lx*2*(a*a);
        anim_curpoint.y = ly-ly*2*(a*a);
        treedraw->translate(&anim_curpoint, &anim_startpoint);
        treedraw->endTranslation();
      } else {
        treedraw->endTranslation();
        animating = false;
        anim_x = 0;
      }
    }
  }
  if (show_)
  if (visible != 1) {
    visible += dtime*6;
    if (visible > 1.0f) {
      visible = 1;
      show_ = false;
      a_focus = this;
      k_focus = this;
    }
  }
  if (hide_)
  if (visible != 0) {
    visible -= dtime*6;
    if (visible < 0.0f) {
      visible = 0;
      hide_ = false;
      a_focus = parent;
      k_focus = parent;
    }
  }
  if (visible != 0)
  {
    // interpolation magic
    float tt = dtime*interpolation_speed * vsx_widget_global_interpolation::get_instance()->get();
    if (tt > 1) tt = 1;

    if (clickpoint.x != endpoint.x && clickpoint.y != endpoint.y)
    {
      doubleclix = false;
      clickpoint.x = clickpoint.x*(1-tt)+endpoint.x*tt;
      clickpoint.y = clickpoint.y*(1-tt)+endpoint.y*tt;

      if (endpoint.isValid()) {
        treedraw->translate(&startpoint,&clickpoint);
        treedraw->endTranslation();
        startpoint = clickpoint;
      }
    }

    if (!performance_mode)
    {
    float nu = 20;
    glBegin(GL_TRIANGLE_FAN);
      glColor4f(0 , 0.5 , 0.6 , visible);
      glVertex2f(a.x,a.y);

      glColor4f(0,0,0,0);

      float n = 2*PI/nu;
      for (float i = 0; i <= nu; i++) {
        glVertex3f(size.x*cos(n*i)/2+0.5f,size.y*sin(n*i)/2+0.5f,0);
      }
    glEnd();
    }

    if (treedraw) {
      treedraw->draw_root->color.a = visible;
      glLineWidth(1.0f);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      treedraw->refreshScreenCoordinates();
      treedraw->drawBranches();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      if (drag_dropped) {
        if (visible != 1) {
          glColor4f(1,1,1,0.5);
          float dragbox = 0.01;
          glBegin(GL_QUADS);
            glVertex3f(drag_pos.x - dragbox ,drag_pos.y - dragbox, 0);
            glVertex3f(drag_pos.x + dragbox ,drag_pos.y - dragbox, 0);
            glVertex3f(drag_pos.x + dragbox ,drag_pos.y + dragbox, 0);
            glVertex3f(drag_pos.x - dragbox ,drag_pos.y + dragbox, 0);
          glEnd();
        }
      }

      if (first) {
        first = false;
        startpoint.projectionStoE(0.0, 0.0, treedraw->getSOrigin(), treedraw->getSMax());
        endpoint.projectionStoE(0.01, 0.01,treedraw->getSOrigin(),treedraw->getSMax());
        treedraw->translate(&startpoint,&endpoint);
        treedraw->endTranslation();
      }
    }
    glColor4f(1,1,1,visible);
    font.color.a = visible*0.3;
    if (message != "")
    font.print_center(a, message,0.035);
    if (drag_dropped) {
      font.color.a = (sin(PI + vsx_widget_time::get_instance()->get_time() *13)+1)*0.4f+0.2f;
      font.print_center(vsx_vector3<>(a.x,a.y+0.16), "* Dropping "+treedraw->selected->node->node->module_info->identifier+" *",0.025);
      font.print_center(vsx_vector3<>(a.x,a.y+0.13), "Hold the mouse button, move/drag the module where you want to place it.",0.025);
      font.print_center(vsx_vector3<>(a.x,a.y+0.1), "Then release the left mouse button to drop it.",0.025);
      font.print_center(vsx_vector3<>(a.x,a.y-0.15), "Press right mouse button or ESC to cancel drop.",0.035);
    }
    if (draw_tooltip) {
      font.color.a = 0.0f;
      vsx_vector3<> sz = font.get_size(tooltip_text, 0.025);
      glColor4f(0.0f,0.0f,0.0f,0.6f*visible);
      draw_box(vsx_vector3<>(tooltip_pos.x,tooltip_pos.y+0.025*1.05), sz.x, -sz.y);
      font.color.a = 1.0f*visible;
      font.print(tooltip_pos, tooltip_text,0.022);
    }
  }
  draw_children_2d();
}

void vsx_widget_ultra_chooser::show() {
  if (!object_inspector) {
    object_inspector = find("object_inspector");
  }
  a_focus = this;
  k_focus = this;
  show_ = true;
  hide_ = false;
}

void vsx_widget_ultra_chooser::hide() {
  hide_ = true;
  show_ = false;
}

vsx_widget_ultra_chooser::vsx_widget_ultra_chooser()
{
  server = 0;
  object_inspector = 0;
  widget_type = VSX_WIDGET_TYPE_ULTRA_CHOOSER;
  animating = false;
  show_ = false;
  hide_ = false;
  first = true;
  drag_dropped = false;
  draw_tooltip = false;
  drag_module = false;
  module_tree = new HTNode;
  interpolation_speed = 7;
  coord_type = VSX_WIDGET_COORD_CORNER;
  coord_related_parent = false;
  mode = 0; // normal component chooser
  mymodel = 0;
  treedraw = 0;
  treedraw = 0;
  size.x = 1.0f;
  size.y = 1.0f;
  visible = 0;
  topmost = true;
  name_dialog = add(new dialog_query_string("name of component","Choose a unique name for your component"),"component_create_name");
  init_children();
  set_render_type(render_2d);

  if (!module_chooser_colors.count())
    module_chooser_colors.load_from_file( vsx_widget_skin::get_instance()->skin_path_get() +"module_chooser.conf",true,4);

  if (!module_chooser_colors.count()) {
    module_chooser_colors.add_raw("ccolor default 0.2,0.2,0.6,1");
    module_chooser_colors.add_raw("ccolor default_ 0.2,0.2,0.6,1");
    module_chooser_colors.add_raw("ccolor resource 0.8,0.4,0.4,0.6");
    module_chooser_colors.add_raw("ccolor resource_ 0.5,0.5,0.5,1");
    module_chooser_colors.add_raw("ccolor macro 0.3,0.8,0.8,0.3");
    module_chooser_colors.add_raw("ccolor macro_ 0.5,0.9,0.9,1");
    module_chooser_colors.add_raw("ccolor render 1,0.4,0.3,0.5");
    module_chooser_colors.add_raw("ccolor render_ 1,0.4,0.3,1");
    module_chooser_colors.add_raw("ccolor texture 0,0.4,0.8,0.8");
    module_chooser_colors.add_raw("ccolor texture_ 0.5,0.5,1,1");
    module_chooser_colors.add_raw("ccolor texture_surface 0.7,0,1,0.8");
    module_chooser_colors.add_raw("ccolor texture_surface_ 0.9,0.2,1,1");
    module_chooser_colors.add_raw("ccolor bitmap 1,0,0.5,0.5");
    module_chooser_colors.add_raw("ccolor bitmap_ 1,0,0.5,1");
    module_chooser_colors.add_raw("ccolor parameters 0.8,0.5,0.6,0.3");
    module_chooser_colors.add_raw("ccolor parameters_ 0.8,0.5,0.6,1");
    module_chooser_colors.add_raw("ccolor mesh 0.6,0.7,0.7,1");
    module_chooser_colors.add_raw("ccolor mesh_ 0.6,0.7,0.7,0.5");
    module_chooser_colors.add_raw("rcolor jpg 0,1,0,1");
    module_chooser_colors.add_raw("rcolor png 1,0,1,1");
  }
  module_chooser_colors.reset();

  vsx_command_s* mc = 0;
  while ( (mc = module_chooser_colors.get()) ) {
    if (mc->cmd == "ccolor") {
      vsx_nw_vector<vsx_string<> > parts;
      vsx_string<>deli = ",";
      vsx_string_helper::explode(mc->parts[2],deli, parts);
      vsx_color<> p;
      p.r = vsx_string_helper::s2f(parts[0]);
      p.g = vsx_string_helper::s2f(parts[1]);
      p.b = vsx_string_helper::s2f(parts[2]);
      p.a = vsx_string_helper::s2f(parts[3]);
      mc_colors[mc->parts[1]] = p;
    } else
    if (mc->cmd == "rcolor") {
      vsx_nw_vector<vsx_string<> > parts;
      vsx_string<>deli = ",";
      vsx_string_helper::explode(mc->parts[2],deli, parts);
      vsx_color<> p;
      p.r = vsx_string_helper::s2f(parts[0]);
      p.g = vsx_string_helper::s2f(parts[1]);
      p.b = vsx_string_helper::s2f(parts[2]);
      p.a = vsx_string_helper::s2f(parts[3]);
      mc_r_colors[mc->parts[1]] = p;
    }
  }
  help_text = "This is the browser:\n\
- right-click to close\n\
- click+drag or double-click to create\n\
  or choose an item\n\
- some items are previewable:\n\
  left-click them once to see a preview\n\
- press [home] to return to the middle\n\
- left-double-click to travel to\n\
  a new destination";
}

void vsx_widget_ultra_chooser::on_delete() {
  mc_colors.clear();
  mc_r_colors.clear();
  delete mymodel;
  delete module_tree;
}

void vsx_widget_ultra_chooser::build_tree() {
  if (mymodel && treedraw)
  {
    delete mymodel;
    treedraw->_delete();
  }

  allow_move = true;
  mymodel = new HTModel(module_tree);

  treedraw = (vsx_widget_hyperbolic_tree*)add(new vsx_widget_hyperbolic_tree(0,0,mymodel),"treedraw");
  treedraw->size.x = size.x;
  treedraw->size.y = size.y;
  treedraw->pos = vsx_vector3<>(0.5f*size.x, 0.5f*size.y);
  treedraw->changeProjType(0);
}

void vsx_widget_ultra_chooser::reinit()
{
}

