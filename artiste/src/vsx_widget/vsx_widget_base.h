/*******************************************************************************
                      _     _  __     ________  __         ___        _
__   _______   _____ (_) __| | \ \   / / ___\ \/ /   _    / _ \      / |
\ \ / / _ \ \ / / _ \| |/ _` |  \ \ / /\___ \\  / | | |  | | | |     | |
 \ V / (_) \ V / (_) | | (_| |   \ V /  ___) /  \ |_| |  | |_| |  _  | |
  \_/ \___/ \_/ \___/|_|\__,_|    \_/  |____/_/\_\__,_|   \___/  (_) |_|
*/

#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_BASE_H
#define VSX_WIDGET_BASE_H

#include <map>

#include "vsx_gl_global.h"
#include "vsx_mouse.h"
#include "gl_helper.h"
#include "vsx_command.h"
#include "vsxfst.h"
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_font.h"
#include "log/vsx_log_a.h"
#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#endif


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
#define VSX_WIDGET_TYPE_DESKTOP 100
#define VSX_WIDGET_TYPE_COMPONENT 101
#define VSX_WIDGET_TYPE_SERVER 102

// render types
#define VSX_WIDGET_RENDER_2D 0
#define VSX_WIDGET_RENDER_3D 1

// coordinate-system types
#define VSX_WIDGET_COORD_CENTER 1
#define VSX_WIDGET_COORD_CORNER 2


class vsx_widget_coords {
  static GLdouble modelMatrix[16];
  static GLdouble projMatrix[16];
  static GLint viewport[4];
public:
  vsx_vector world_local; // in local widget coordinates
  vsx_vector world_global; // untouched global coordinates
  vsx_vector screen_local; // in local widget coordinates
  vsx_vector screen_global; // untouched global coordinates
  void init(float x, float y);
};

// distance from center and corner for mouse event handlers
typedef struct {
  vsx_vector center;
  vsx_vector corner;
} vsx_widget_distance;

class vsx_widget {
  void calculate_mouse_distance(float x, float y, vsx_widget_coords &coord, vsx_widget_distance &distance);
public:
  // GLOBALS
  static bool global_delete;
// style
  static vsx_string skin_path;
  static vsx_color skin_color[20];
  static vsx_mouse mouse;
  static int static_id; // each widget object must have a unique id
  static float screen_x;
  static float screen_y;
  static float screen_aspect;
  static vsx_widget *root; // root widget
  static vsx_widget *a_focus; // application focus
  static vsx_widget *m_focus; // mouse focus (used while mouse is pressed on a widget)
  static vsx_widget *m_o_focus; // mouse over focus
  static vsx_widget *k_focus; // key focus
  static std::map<vsx_string, vsx_widget*> glist;
  static std::map<int, vsx_widget*> ilist;
  static vsx_widget* last_clicked;
  static float global_interpolation_speed;
  static float global_key_speed;
  static float global_framerate_limit;
  static std::map<vsx_string, vsx_string> configuration;
  static unsigned long help_timestamp;
  static bool mouse_down_l;
  static bool mouse_down_r;
  static float mouse_down_l_x; // TBD
  static float mouse_down_l_y;
  static vsx_widget_distance mouse_down_pos;  // in coordinates 2d/3d depending on render type
  float double_click_d[5]; // delta time value for determining double clicks
  static GLint viewport[4];
  static GLdouble modelMatrix[16];
  static GLdouble projMatrix[16];
// time
  // these have to be set from outside, by using #include "vsx_timer.h" and using the timer class
  static int frames;
  static double time; // elapsed time
  static double dtime; // time since last frame
  static bool ctrl, alt, shift;

  static bool performance_mode;

// LOCALS
  // information
  int id; // widget id (unique)
  vsx_string name; // internal name
  vsx_string title; // title, for public display
  vsx_string help_text; // help text for this widget
  unsigned int widget_type; // for finding out what type arbitrary widgets have so one can typecast correctly
  unsigned int render_type; // 2d or 3d drawing mode
  unsigned int coord_type; // centralized or corner-based coordinates?
  bool coord_related_parent; // coordinate relation with parent? or standalone?

  vsx_vector mouse_pos; // last translated mouse pos

  // widget relations to other widgets
  std::map<vsx_string, vsx_widget*>::const_iterator glist_iter;
  std::map<vsx_string, vsx_widget*> l_list;
  std::map<vsx_string, vsx_widget*>::const_iterator l_list_iter;
  std::list <vsx_widget*> children;
  std::list <vsx_widget*>::iterator children_iter;
  vsx_widget* parent;
  vsx_widget* cmd_parent;

  // my font, useful sometimes
  static vsx_font myf;
  float font_size;
// space
  // in 3-dimensional space widgets' pos should be at the center and size should be the radius of the circle
  // of course there exists exceptions from this, such as the vsx_widget_connector but this is a general rule of
  // thumb.
  // in 2-dimensional space the pos is the top left (screen-wise) coordinate and size is height/width of the widget.
  vsx_vector pos;
  vsx_vector target_pos;
  vsx_vector size;
  vsx_vector target_size;
  vsx_vector size_min, size_max;
  bool allow_resize_x;
  bool allow_resize_y;
  bool allow_move_x;
  bool allow_move_y;

  int resize_i;
  double dragborder;
  float interpolation_speed;
  bool interpolating_pos;
  bool interpolating_size;
//  bool shade;
  vsx_vector scaling_start;
  vsx_vector scaling_start_size;
  vsx_vector scaling_start_pos;

  bool support_interpolation;
  bool support_scaling;

  vsx_color color;
  // topmost, right now only one widget can be topmost (and its children)
  bool topmost;

  // 2d gui stuff
  float visible;
  virtual void show() {}
  virtual void hide() {}
  // popup menu support
  vsx_widget* menu;
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
  vsx_avector<vsx_string> log_cmd;
  virtual void log(const vsx_string& t) {
    if (widget_type >= 100)
    log_cmd.push_back(t);
  }
// internal command processing
  // backwards queue
  // this is what a child that issues a command runs on its parent. it starts a cycle that goes back and fills up
  // its parent with the command which in turn deals with it accordingly. For instance, a server does not run
  // the "parent" that is called to deal with commands that have been issued within this widget
  vsx_command_list command_q_b;
  virtual void vsx_command_process_b(vsx_command_s *t) {
    command_q_b.add(t);
  }
  // direct is when you don't want to run the parent command queue, just process the list
  virtual void vsx_command_queue_b(vsx_widget* source, bool direct = false, int iterations = 0) {
    // call parent
    vsx_command_s *t;
    while ( (t = source->command_q_b.pop()) )
    {
      // deal with the command
      // ... default is just send back
      log(t->str());
      LOG_A(vsx_string("message_b: ")+t->str());
      vsx_command_process_b(t);
    }
    // ok, we've processed the commands, now call our parent to go through our list
    if (
    			(parent != this)
    			&&
    			(source != this)
    	 )
    {
      cmd_parent->vsx_command_queue_b(this,direct,iterations+1);
    }
  }

  inline void backwards_message(const vsx_string &message) {
		command_q_b.add_raw(message);
  	cmd_parent->vsx_command_queue_b(this);
  }

  inline void message(const vsx_string &message) {
		command_q_b.add_raw(message);
  	this->vsx_command_queue_b(this);
  }


  // foward queue
  // does each-frame maintanence
  vsx_command_list command_q_f;
  virtual void vsx_command_process_f(); // implemented in .cpp
  void vsx_command_queue_f() {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    (*children_iter)->vsx_command_process_f();
  }
	//-------------------------------------------------------------

  // this is used to get a pointer by knowing the name of the widget - searches the global list
  vsx_widget *f(vsx_string t) {
    if (glist.find(t) != glist.end())
    return glist[t];
    else return 0;
  }
  // find by id
  vsx_widget *f(int t) {
    if (ilist.find(t) != ilist.end())
    return ilist[t];
    else return 0;
  }

  virtual void show_children();
  virtual void hide_children();


  // draw functions
  virtual void init_frame() {
    if (support_interpolation) {
      if (interpolating_size) {
        interpolate_size();
      }
      if (interpolating_pos) {
        interpolate_pos();
      }
    }
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    if ((*children_iter)->visible)
    (*children_iter)->init_frame();
  }
  void set_render_type(unsigned long new_render_type);
  virtual void set_border(float border);
  void set_font_size(float fsize);


  // special case draw, only used when children may implement this
  virtual void pre_draw() {}
  void pre_draw_children() {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) {
      (*children_iter)->pre_draw();
    }
  }


  virtual void i_draw() {}

  virtual void draw();// { if (render_type == VSX_WIDGET_RENDER_3D) { if (visible) i_draw(); } if (visible) draw_children_3d();}
  virtual void draw_2d();// { if (render_type == VSX_WIDGET_RENDER_2D) { if (visible) i_draw(); } if (visible) draw_children_2d();}

  void draw_children() {
    if (render_type == VSX_WIDGET_RENDER_3D) draw_children_3d();
    else draw_children_2d();
  }

  void draw_children_3d() {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) {
      (*children_iter)->draw();
    }
  }
  void draw_children_2d() {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) (*children_iter)->draw_2d();
  }

  virtual void interpolate_pos();
  virtual void interpolate_size();

  // moves the widget to the front (first in the draw list)
  void front(vsx_widget* t);
  virtual vsx_vector get_pos_p();
  virtual vsx_vector get_target_pos_p();

  bool inside_xy(vsx_widget_coords &coords, vsx_widget_distance &result);
  virtual int inside_xy_l(vsx_vector &test, vsx_vector &global);
  virtual vsx_widget *find_component(vsx_widget_coords coords, vsx_widget_distance &result_distance, int depth = 0);

  bool find_child_by_type(unsigned long t);
  bool find_child_by_name(vsx_string n);

  void delete_all_by_type(unsigned long t);

// EXTERNAL IN-DATA, MOUSE, KEYBOARD
  // this is bool so that external (super-global) processors can know wether or not to react on the keys..
  // this returning true means that they're allowed to process the keys, if not, it's exclusive to the widget
  // who has k_focus - like an edit box.
  virtual bool key_down(signed long key, bool n_alt, bool n_ctrl, bool n_shift) {
    this->alt = n_alt;
    this->ctrl = n_ctrl;
    this->shift = n_shift;
    if (k_focus) {
      return k_focus->event_key_down(key,n_alt,n_ctrl,n_shift);
    } else return true;
  }

  bool key_up(signed long key, bool alt, bool ctrl, bool shift) {
    this->ctrl = ctrl;
    if (k_focus) {
      return k_focus->event_key_up(key,alt,ctrl,shift);
    } else return true;
  }
  void set_key_modifiers(bool alt_, bool ctrl_, bool shift_);

  void mouse_down(float x, float y, int button);
  void mouse_up(float x, float y, int button);
  void mouse_move(float x, float y);
  void mouse_move_passive(float x, float y);
  void mouse_wheel(float y);

// INTERNAL EVENTS IN RESPONSE TO EXTERNAL INPUT, DEFAULTS (CAN BE OVERRIDDEN)
  virtual bool event_key_down(signed long key, bool alt = false, bool ctrl = false, bool shift = false) { return true; }
  virtual bool event_key_up(signed long key, bool alt = false, bool ctrl = false, bool shift = false) { return true; }
  // new events
  virtual void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  virtual void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  virtual void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button) {};
  virtual void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  virtual void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  virtual void event_mouse_wheel(float y);
  virtual void event_move_scale() {};

  //-----------------------------------------
  bool init_run;
  // basic init, mostly run by the constructor
  virtual void init() {init_run = true;}
  // if widget is related to a command, code that reads the command and sets the widget goes
  // in init_from_command
	virtual bool init_from_command(vsx_command_s* c) { return false; }
  virtual void init_children() {
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) (*children_iter)->init();
  }
  // called after just switching openGL context, reuploads all textures and rebuilds
  // call lists in fonts etc.
  virtual void stop();
  virtual void reinit();

  // move/scaling/resizing functions
  virtual void set_size(vsx_vector new_size) {
    size = target_size = new_size;
  }
  void set_pos(vsx_vector new_pos) {
    pos = target_pos = new_pos;
  }
  virtual void resize(vsx_widget_distance distance);
  virtual void resize_to(vsx_vector to_size);
  // move the widget, world coordinates
  virtual void move(double x, double y, double z = 0.0f);
  // move by vector - for convenience
  virtual void move(vsx_vector t);
  // move delta by vector
  virtual void move_d(vsx_vector t);
  //virtual bool scale_to(vsx_vector t);

  virtual void move_camera(vsx_vector world) {};

  // CONSTRUCTION/DESTRUCTION
  bool marked_for_deletion;
  //
  vsx_widget *add(vsx_widget *t,vsx_string name);

  // actual deletion event, don't use this unless you know what you are doing!
  virtual void _delete();
  virtual void mark_for_deletion();

  // ask the object to delete itself and all its children (nice)
  virtual void delete_();
  // deletion event
  virtual void on_delete();
  // event reacting on marking widget for deletion
  virtual void before_delete();

  vsx_widget();
  virtual ~vsx_widget();
};



class vsx_widget_connector_info {
public:
  vsx_widget* source;
  vsx_widget* dest;
  int order;
};
#endif
#endif
