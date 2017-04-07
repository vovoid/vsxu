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

#ifndef VSX_WIDGET_BASE_H
#define VSX_WIDGET_BASE_H

// engine
#include <vsx_gl_global.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <color/vsx_color.h>
#include <command/vsx_command_list.h>

// engine_graphics
#include <vsx_font.h>

// profiler
#include <profiler/vsx_profiler_manager.h>

// stl
#include <map>
#include <list>

// application
#include <vsx_application_input_state_manager.h>
#include <vsx_application_control.h>

// widget
#include "vsx_widget_dllimport.h"
#include "vsx_widget_coords.h"
#include "vsx_widget_distance.h"
#include "vsx_widget_camera.h"
#include "vsx_widget_time.h"
#include "vsx_widget_global_interpolation.h"
#include "vsx_widget_skin.h"


// different widget types
#define VSX_WIDGET_TYPE_WIDGET 0
#define VSX_WIDGET_TYPE_2D_MENU_POPUP 1
#define VSX_WIDGET_TYPE_3D_LABEL 2
#define VSX_WIDGET_TYPE_ANCHOR 3
#define VSX_WIDGET_TYPE_CONNECTOR 4
#define VSX_WIDGET_TYPE_SEQUENCE_CHANNEL 5
#define VSX_WIDGET_TYPE_CONTROLLER 6
#define VSX_WIDGET_TYPE_ULTRA_CHOOSER 7
#define VSX_WIDGET_TYPE_EDITOR 8
#define VSX_WIDGET_TYPE_WINDOW 9

// these two defines should be moved into the artiste project:
#define VSX_WIDGET_TYPE_COMPONENT 1001
#define VSX_WIDGET_TYPE_SERVER 1002

enum vsx_widget_render_type
{
  render_2d = 1,
  render_3d = 2
};



class WIDGET_DLLIMPORT vsx_widget
{
  void calculate_mouse_distance(float x, float y, vsx_widget_coords &coord, vsx_widget_distance &distance);
public:
  // GLOBALS
  WIDGET_DLLIMPORT static bool global_delete;
  // style
  WIDGET_DLLIMPORT static float screen_x;
  WIDGET_DLLIMPORT static float screen_y;
  WIDGET_DLLIMPORT static float screen_aspect;
  WIDGET_DLLIMPORT static vsx_widget *root; // root widget

  WIDGET_DLLIMPORT static vsx_widget *a_focus; // application focus
  WIDGET_DLLIMPORT static vsx_widget *m_focus; // mouse focus (used while mouse is pressed on a widget)
  WIDGET_DLLIMPORT static vsx_widget *m_o_focus; // mouse over focus
  WIDGET_DLLIMPORT static vsx_widget *k_focus; // keyboard focus

  // camera
  WIDGET_DLLIMPORT static vsx_widget_camera camera;


  // global list of widgets, by name
  WIDGET_DLLIMPORT static std::map<vsx_string<>, vsx_widget*> global_widget_list;
  WIDGET_DLLIMPORT static int static_widget_id_accumulator; // each widget object must have a unique id

  // global list of widgets, by id
  WIDGET_DLLIMPORT static std::map<int, vsx_widget*> global_index_list;



  WIDGET_DLLIMPORT static vsx_widget* last_clicked;
  WIDGET_DLLIMPORT static float global_framerate_limit;
  WIDGET_DLLIMPORT static std::map<vsx_string<>, vsx_string<> > configuration;
  WIDGET_DLLIMPORT static unsigned long help_timestamp;
  WIDGET_DLLIMPORT static bool mouse_down_l;
  WIDGET_DLLIMPORT static bool mouse_down_r;
  WIDGET_DLLIMPORT static vsx_widget_distance mouse_down_pos;  // in coordinates 2d/3d depending on render type
  float double_click_d[5]; // delta time value for determining double clicks
  WIDGET_DLLIMPORT static int viewport[4];
  WIDGET_DLLIMPORT static double modelMatrix[16];
  WIDGET_DLLIMPORT static double projMatrix[16];

  // time
  // these have to be set from outside, by using #include <time/vsx_timer.h> and using the timer class
  WIDGET_DLLIMPORT static int frames;

  //
  WIDGET_DLLIMPORT static bool performance_mode;

  // profiler
  VSXP_CLASS_DECLARE

// LOCALS
  // information
  int id; // widget id (unique)
  vsx_string<>name; // internal name
  vsx_string<>title; // title, for public display
  vsx_string<>help_text; // help text for this widget
  unsigned int widget_type; // for finding out what type arbitrary widgets have so one can typecast correctly
  vsx_widget_render_type render_type; // 2d or 3d drawing mode
  unsigned int coord_type; // centralized or corner-based coordinates?
  bool coord_related_parent; // coordinate relation with parent? or standalone?

  vsx_vector3<> mouse_pos; // last translated mouse pos

  // widget relations to other widgets

  // local by-name lookup list
  std::map<vsx_string<>, vsx_widget*> l_list;

  std::list <vsx_widget*> children;
  std::list <vsx_widget*>::iterator children_iter;

  vsx_widget* parent;


  // font
  WIDGET_DLLIMPORT static vsx_font font;
  float font_size;

  // space
  // in 3-dimensional space widgets' pos should be at the center and size should be the radius of the circle
  // of course there exists exceptions from this, such as the vsx_widget_connector but this is a general rule of
  // thumb.
  // in 2-dimensional space the pos is the top left (screen-wise) coordinate and size is height/width of the widget.
  vsx_vector3<> pos;
  vsx_vector3<> target_pos;
  vsx_vector3<> size;
  vsx_vector3<> target_size;
  vsx_vector3<> size_min, size_max;
  bool allow_resize_x;
  bool allow_resize_y;
  bool allow_move_x;
  bool allow_move_y;

  int resize_i;
  double dragborder;
  float interpolation_speed;
  bool interpolating_pos;
  bool interpolating_size;
  vsx_vector2f scaling_start;
  vsx_vector2f scaling_start_size;
  vsx_vector2f scaling_start_pos;

  bool support_interpolation;
  bool support_scaling;

  vsx_color<> color;
  // topmost, right now only one widget can be topmost (and its children)
  bool topmost;

  // 2d gui stuff
  float visible = 1.0f;
  virtual void show() { visible = 1.0f; }
  virtual void hide() { visible = 0.0f; }
  // popup menu support
  vsx_widget* menu = 0x0;
  bool menu_temp_disable; // temporary disable showing the menu

  // available list of commands for a widget
  vsx_command_list commands;
  //
  bool enabled;
  bool constrained_x;
  bool constrained_y;
  bool constrained_z;
  bool region_exclusive;

  // logging
  vsx_nw_vector< vsx_string<> > log_cmd;
  virtual void log(const vsx_string<>& t)
  {
    if (widget_type >= 100)
    log_cmd.push_back(t);
  }





// internal command processing
  // backwards queue
  // this is what a child that issues a command runs on its parent. it starts a cycle that goes back and fills up
  // its parent with the command which in turn deals with it accordingly. For instance, a server does not run
  // the "parent" that is called to deal with commands that have been issued within this widget

  vsx_widget* cmd_parent;
  vsx_command_list command_q_b;
  virtual void command_process_back_queue(vsx_command_s *t)
  {
    command_q_b.add(t);
  }
  // direct is when you don't want to run the parent command queue, just process the list
  virtual void vsx_command_queue_b(vsx_widget* source, bool direct = false, int iterations = 0)
  {
    // call parent
    vsx_command_s *t;
    while ( (t = source->command_q_b.pop()) )
    {
      // deal with the command
      // ... default is just send back
      log(t->str());
      command_process_back_queue(t);
    }
    // ok, we've processed the commands, now call our parent to go through our list
    if (
    			(parent != this)
    			&&
    			(source != this)
    	 )
    {
      if (cmd_parent)
      {
        cmd_parent->vsx_command_queue_b(this,direct,iterations+1);
      }
    }
  }

  inline void backwards_message(const vsx_string<>&message)
  {
    command_q_b.add_raw(message);
    if (cmd_parent)
    {
      cmd_parent->vsx_command_queue_b(this);
    }
  }

  inline void message(const vsx_string<>&message)
  {
    command_q_b.add_raw(message);
  	this->vsx_command_queue_b(this);
  }







  // foward queue
  // does each-frame maintanence
  vsx_command_list command_q_f;
  virtual void vsx_command_process_f(); // implemented in .cpp
  void vsx_command_queue_f()
  {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    (*children_iter)->vsx_command_process_f();
  }
	//-------------------------------------------------------------


  // find operations
  bool find_child_by_type(unsigned long t);
  bool find_child_by_name(vsx_string<>n);

  // this is used to get a pointer by knowing the name of the widget - searches the global list
  vsx_widget *find(vsx_string<>t)
  {
    if (global_widget_list.find(t) != global_widget_list.end())
    return global_widget_list[t];
    else return 0;
  }


  // find by id
  vsx_widget *find(int t)
  {
    if (global_index_list.find(t) != global_index_list.end())
    return global_index_list[t];
    else return 0;
  }




  virtual void show_children();
  virtual void hide_children();






  // draw functions
  virtual void init_frame()
  {
    if (support_interpolation)
    {
      if (interpolating_size)
      {
        interpolate_size();
      }
      if (interpolating_pos)
      {
        interpolate_pos();
      }
    }
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    {
      if ((*children_iter)->visible > 0.0f)
      (*children_iter)->init_frame();
    }
  }


  void set_render_type(vsx_widget_render_type new_render_type);
  virtual void set_border(float border);
  void set_font_size(float fsize);


  // special case draw, only used when children may implement this
  virtual void pre_draw()
  {
  }

  void pre_draw_children()
  {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    {
      (*children_iter)->pre_draw();
    }
  }


  virtual void i_draw() {}

  virtual void draw();
  virtual void draw_2d();

  void draw_children()
  {
    if (render_type == render_3d)
      draw_children_3d();
    else
      draw_children_2d();
  }

  void draw_children_3d()
  {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    {
      (*children_iter)->draw();
    }
  }

  void draw_children_2d()
  {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    {
      (*children_iter)->draw_2d();
    }
  }

  virtual void interpolate_pos();
  virtual void interpolate_size();

  // moves the widget to the front (first in the draw list)
  void front(vsx_widget* t);
  virtual vsx_vector3<> get_pos_p();
  virtual vsx_vector3<> get_target_pos_p();

  bool inside_xy(vsx_widget_coords &coords, vsx_widget_distance &result);
  virtual int inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global);
  virtual vsx_widget *find_component(vsx_widget_coords coords, vsx_widget_distance &result_distance, int depth = 0);


  void delete_all_by_type(unsigned long t);

  // INPUT
  // this is bool so that external (super-global) processors can know wether or not to react on the keys..
  // this returning true means that they're allowed to process the keys, if not, it's exclusive to the widget
  // who has k_focus - like an edit box.
  virtual bool input_key_down(uint16_t key)
  {
    reqrv(k_focus, true);
    return k_focus->event_key_down(key);
  }

  bool input_key_up(uint16_t key)
  {
    reqrv(k_focus, true);
    return k_focus->event_key_up(key);
  }

  void input_text(wchar_t character_wide, char character) {
    req(k_focus);
    k_focus->event_text(character_wide, character);
  }

  void input_mouse_down(float x, float y, int button);
  void input_mouse_up(float x, float y, int button);
  void input_mouse_move(float x, float y);
  void input_mouse_move_passive(float x, float y);
  void input_mouse_wheel(float y);

  // INTERNAL EVENTS IN RESPONSE TO EXTERNAL INPUT, DEFAULTS (CAN BE OVERRIDDEN)
  virtual bool event_key_down(uint16_t key)
  {
    VSX_UNUSED(key);
    return true;
  }

  virtual bool event_key_up(uint16_t key)
  {
    VSX_UNUSED(key);
    return true;
  }

  virtual void event_text(wchar_t character_wide, char character)
  {
    VSX_UNUSED(character_wide);
    VSX_UNUSED(character_wide);
  }

  virtual void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  virtual void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  virtual void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);
    VSX_UNUSED(button);
  }
  virtual void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  virtual void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  virtual void event_mouse_wheel(float y);
  virtual void event_move_scale() {}

  //-----------------------------------------
  bool init_run;
  // basic init, mostly run by the constructor
  virtual void init() {init_run = true;}
  // if widget is related to a command, code that reads the command and sets the widget goes
  // in init_from_command
  virtual bool init_from_command(vsx_command_s* c)
  {
    VSX_UNUSED(c);
    return false;
  }

  virtual void init_children()
  {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    {
      if ( (*children_iter)->init_run)
        continue;

      (*children_iter)->init();
    }
  }

  // called after just switching openGL context, reuploads all textures and rebuilds
  // call lists in fonts etc.
  virtual void stop();
  virtual void reinit();

  // move/scaling/resizing functions
  virtual void set_size(vsx_vector3<> new_size)
  {
    size = target_size = new_size;
  }

  // returns size relative to parent
  virtual vsx_vector3<> get_inner_size()
  {
    return size;
  }

  // returns position relative to parent
  virtual vsx_vector3<> get_inner_pos()
  {
    return pos;
  }

  void set_pos(vsx_vector3<> new_pos)
  {
    pos = target_pos = new_pos;
  }

  virtual void resize(vsx_widget_distance distance);
  virtual void resize_to(vsx_vector3<> to_size);
  // move the widget, world coordinates
  virtual void move(double x, double y, double z = 0.0f);

  // move by vector - for convenience
  virtual void move(vsx_vector3<> t);

  // move delta by vector
  virtual void move_d(vsx_vector3<> t);

  virtual void move_camera(vsx_vector3<> world)
  {
    VSX_UNUSED(world);
  }

  // CONSTRUCTION/DESTRUCTION
  bool marked_for_deletion;
  //
  vsx_widget *add(vsx_widget *t,vsx_string<>name);

protected:
  // actual deletion event, don't use this unless you know what you are doing!
  virtual void delete_();
  virtual void mark_for_deletion();

  // event reacting on marking widget for deletion
  virtual void before_delete();

  // deletion event
  virtual void on_delete();
public:
  // ask the object to delete itself and all its children (nice)
  virtual void _delete();


  vsx_widget();
  virtual ~vsx_widget();
};


#endif
