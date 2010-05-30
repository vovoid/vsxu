/*******************************************************************************
                      _     _  __     ________  __         ___        _
__   _______   _____ (_) __| | \ \   / / ___\ \/ /   _    / _ \      / |
\ \ / / _ \ \ / / _ \| |/ _` |  \ \ / /\___ \\  / | | |  | | | |     | |
 \ V / (_) \ V / (_) | | (_| |   \ V /  ___) /  \ |_| |  | |_| |  _  | |
  \_/ \___/ \_/ \___/|_|\__,_|    \_/  |____/_/\_\__,_|   \___/  (_) |_|
*/
#ifndef VSX_NO_CLIENT
//#include <string>
#include <map>
#include <list>
//#include <iostream>
//#include <fstream>
#include "vsx_math_3d.h"
#include "vsx_gl_global.h"
#include "vsx_texture_info.h"
#include "vsx_string.h"
#include "vsx_texture.h"
#include "vsx_font.h"
#include "vsx_command.h"

#include "vsx_widget_base.h"

GLdouble vsx_widget_coords::modelMatrix[16];
GLdouble vsx_widget_coords::projMatrix[16];
GLint vsx_widget_coords::viewport[4];

void vsx_widget_coords::init(float x, float y) {
  GLfloat wx=x,wy,wz;
  glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
  glGetIntegerv(GL_VIEWPORT, viewport);
  wy=viewport[3]-y;
  //printf("x: %f, y: %f\n",x,y);
  wx = x;
  glReadPixels((int)x,(int)y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
  //printf("wz: %f\n",wz);
  //wz = 0.9999;
  GLdouble xx,yy,zz;
  gluUnProject(wx,wy,wz,modelMatrix,projMatrix,viewport,&xx,&yy,&zz);
  //printf("zz: %f\n",zz);
  //printf("xx: %f   yy: %f\n",xx,yy);
  // world
  world_global.x = xx;
  world_global.y = yy;
  world_global.z = 0.0f;
  world_local.x = world_local.y = 0.0f;
  //world_local.z = 0.0f;
  // screen
  screen_global.x = x;
  screen_global.y = y;
  screen_global.z = 0.0f;
  screen_global = screen_global / vsx_vec_viewport();
  //printf("screen x: %f\n",screen_global.x);
  screen_global.y = 1-screen_global.y;
  //printf("world_global.x = %f\n",world_global.x);
  //printf("world_global.y = %f\n",world_global.y);
  //screen_local.x = screen_local.y = screen_local.z = 0.0f;
}


//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************


bool vsx_widget::global_delete = false;
int vsx_widget::static_id = 0;
std::map<vsx_string, vsx_widget*> vsx_widget::glist;
std::map<int, vsx_widget*> vsx_widget::ilist;
vsx_mouse vsx_widget::mouse;
vsx_widget *vsx_widget::root;
vsx_widget *vsx_widget::m_focus = 0;
vsx_widget *vsx_widget::m_o_focus = 0;
vsx_widget *vsx_widget::k_focus = 0;
vsx_widget *vsx_widget::a_focus = 0;
double vsx_widget::time;
double vsx_widget::dtime;
int vsx_widget::frames;
bool vsx_widget::ctrl;
bool vsx_widget::alt;
bool vsx_widget::shift;
float vsx_widget::screen_x;
float vsx_widget::screen_y;
float vsx_widget::screen_aspect;
float vsx_widget::mouse_down_l_x;
float vsx_widget::mouse_down_l_y;
bool vsx_widget::mouse_down_l;
bool vsx_widget::mouse_down_r;
unsigned long vsx_widget::help_timestamp = 0;
//float vsx_widget::double_click_d[5]; // delta time value for determining double clicks
vsx_widget_distance vsx_widget::mouse_down_pos;
GLint vsx_widget::viewport[4];
GLdouble vsx_widget::modelMatrix[16];
GLdouble vsx_widget::projMatrix[16];
std::map<vsx_string, vsx_string> vsx_widget::configuration;
float vsx_widget::global_interpolation_speed = 10.0f;
float vsx_widget::global_key_speed = 3.0f;
float vsx_widget::global_framerate_limit = -1;
vsx_widget* vsx_widget::last_clicked;
vsx_font vsx_widget::myf;
bool vsx_widget::performance_mode = false;

// skin support
vsx_string vsx_widget::skin_path;
vsx_color vsx_widget::skin_color[20];


vsx_widget::vsx_widget() {
  widget_type = VSX_WIDGET_TYPE_WIDGET;
  interpolation_speed = 10.0f;
  interpolating_size = false;
  interpolating_pos = false;
  render_type = VSX_WIDGET_RENDER_3D;
  coord_type = VSX_WIDGET_COORD_CENTER;
  coord_related_parent = true;
  menu = 0;
  init_run = false;
  id = static_id;
  ilist[id] = this;
  ++static_id;
  visible = 1;
  parent = this;
  double_click_d[0] = time;
  double_click_d[1] = time;
  double_click_d[2] = time;
  double_click_d[3] = time;
  double_click_d[4] = time;
  mouse_down_l = false;
  mouse_down_r = false;
  mouse_down_l_x = 0;
  mouse_down_l_y = 0;
  enabled = 1; // is this object to receive events?
  constrained_x = true;
  constrained_y = true;
  region_exclusive = false;
  size.x = 20; // x size
  size.y = 20; // y size
  size.z = 0; // z size
  support_interpolation = false;
  topmost = false;
  size_min.x = 0.1;
  size_min.y = 0.1;
  marked_for_deletion = false;
  resize_i = 0;
  allow_move_x = true;
  allow_move_y = true;
  allow_resize_x = false;
  allow_resize_y = false;
  dragborder = 0.0025;
  support_scaling = false;
  font_size = 0.025;
  menu_temp_disable = false;
}


vsx_widget::~vsx_widget() {
  ilist.erase(id);
  if (glist.find(name) != glist.end())
  {
    glist.erase(name);
  }

  if (this == root) {
    global_delete = true;
//    stop();
  }
  if (global_delete) {
    //printf("deleting %s ... ",name.c_str());
    for (std::list <vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) {
      (*it)->on_delete();
      delete *it;
    }
  }
  if (this == root) {
    glist.clear();
    ilist.clear();
    global_delete = false;
  }

}

//******************************************************************************
// CONSTRUCTOR END
//******************************************************************************



vsx_vector vsx_widget::get_pos_p() {
  if (parent != this && coord_related_parent) {
    vsx_vector tt = pos;
    if (parent->render_type == render_type )
    tt = tt + parent->get_pos_p();
    //if (render_type == VSX_WIDGET_RENDER_2D) tt.z = 0.0f;
    return tt;
  }
  return pos;
}

vsx_vector vsx_widget::get_target_pos_p() {
  if (parent != this && coord_related_parent) {
    vsx_vector tt = target_pos;
    if (parent->render_type == render_type)
    tt = tt + parent->get_target_pos_p();
    //if (render_type == VSX_WIDGET_RENDER_2D) tt.z = 0.0f;
    return tt;
  }
  return target_pos;
}


/*vsx_vector vsx_widget::translate_xy(double x, double y) {
  GLfloat wx=x,wy,wz;
  glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
  glGetIntegerv(GL_VIEWPORT, viewport);
  wy=viewport[3]-y;
  wx = x;
  glReadPixels((int)x,(int)y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
  //printf("wz: %f\n",wz);
  //wz = 0.9999;
  GLdouble xx,yy,zz;
  gluUnProject(wx,wy,wz,modelMatrix,projMatrix,viewport,&xx,&yy,&zz);
  vsx_vector foo;
  foo.x = xx;
  foo.y = yy;
  foo.z = 1.1;
  return foo;
}*/

/*void vsx_widget::translate_coords(vsx_vector& world, vsx_vector& screen) {
  screen.x -= pos.x;
  screen.y -= pos.y;
  if (coord_type == VSX_WIDGET_COORD_CENTER) {
    screen.x += size.x*0.5f;
    screen.y += size.y*0.5f;
  }
  world.x -= pos.x;
  world.y -= pos.y;
  if (coord_type == VSX_WIDGET_COORD_CENTER) {
    world.x += size.x*0.5f;
    world.y += size.y*0.5f;
  }
}*/

/*bool vsx_widget::inside_xyd(vsx_vector world, vsx_vector screen) {
  if (!visible) return false;
  if (coord_type == VSX_WIDGET_COORD_CENTER) {
    if (
      (world.x > pos.x-size.x/2) && (world.x < pos.x+size.x/2) &&
      (world.y > pos.y-size.y/2) && (world.y < pos.y+(size.y/2))
    ) return true;
    return false;
  }
  if (coord_type == VSX_WIDGET_COORD_CORNER) {
    if (world.x > 0.0f &&
        world.x < size.x &&
        world.y < 0 &&
        world.y > -size.y)
      return true;
    return false;
  }
}*/


/*bool vsx_widget::inside_xy(vsx_vector world, vsx_vector screen) {
  if (render_type == VSX_WIDGET_RENDER_3D) return inside_xyd(world,screen);
  if (render_type == VSX_WIDGET_RENDER_2D) {
    screen.x -= pos.x;
    screen.y -= pos.y;
//    screen.x -= pos.x;
//    screen.y -= pos.y;
    if (coord_type == VSX_WIDGET_COORD_CENTER) {
//      screen.x += size.x*0.5f;
//      screen.y += size.y*0.5f;
    }
    return inside_xyd(screen,screen);
  }
}*/


/*
vsx_widget* vsx_widget::find_component(vsx_vector world, vsx_vector screen, bool translated) {
  //vsx_vector u;
  vsx_vector uu;
  if (!translated) {
    uu = translate_xy(world.x,world.y);
    screen = screen / vsx_vec_viewport();
    screen.y = 1-screen.y;
  } else {
    uu = world;
  }
  if (render_type == VSX_WIDGET_RENDER_3D) {
    uu.x -= pos.x;
    uu.y -= pos.y;
  }
//  if (render_type == VSX_WIDGET_RENDER_2D) {
//    screen.x -= pos.x;
//    screen.y -= pos.y;
//  }
  vsx_widget *tt = 0;
  if (a_focus && this == root)
  if (a_focus != this)
  if (tt = (a_focus)->find_component(uu, screen, true)) {
    return tt;
  }
  if (!children.size()) return 0;

  children.reverse();
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
  if (((*children_iter)->enabled) && ((*children_iter)->visible))// is this to receive events?
    if ((*children_iter)->inside_xy(uu,screen))
    {
      // this is for the case when there's children owned by the component *iter and we don't want them to
      // receive clicks that are within the region of *iter
      if ((*children_iter)->region_exclusive) {
        children.reverse();
        return *children_iter;
      }
      if (tt = (*children_iter)->find_component(uu, screen, true))
      {
        children.reverse();
        return tt;
      } else {
        children.reverse();
        return *children_iter;
      }
    } else {
      if (tt = (*children_iter)->find_component(uu, screen, true))
      {
        children.reverse();
        return tt;
      }
    }
  children.reverse();
  return 0;
} // find_component
 */
bool vsx_widget::find_child_by_type(unsigned long t)
{
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    if ((*children_iter)->widget_type==t) return true;
  return false;
}

bool vsx_widget::find_child_by_name(vsx_string n)
{
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
    if ((*children_iter)->name==n) return true;
  return false;
}



//------------------------------------------------------------------------------
// MOUSE MOVE PASSIVE
//------------------------------------------------------------------------------

int vsx_widget::inside_xy_l(vsx_vector &test, vsx_vector &global) {
//  printf("test: %f %f\n",test.y,global.y);
   if (coord_type == VSX_WIDGET_COORD_CENTER) {
    if (
      (test.x > global.x-target_size.x*0.5f) && (test.x < global.x+target_size.x*0.5f) &&
      (test.y > global.y-target_size.y*0.5f) && (test.y < global.y+target_size.y*0.5f)
    ) {
      //
      return coord_type;
    }
  } else
  if (coord_type == VSX_WIDGET_COORD_CORNER) {
    if (test.x > global.x &&
        test.x < global.x+target_size.x &&
        test.y > global.y &&
        test.y < global.y+target_size.y)
    {
      return coord_type;
    }
  }
  return 0;
}

bool vsx_widget::inside_xy(vsx_widget_coords &coords, vsx_widget_distance &result) {
  vsx_vector* test;
  vsx_vector* global;

  if (!coord_related_parent) {
    coords.world_local = target_pos;
    coords.screen_local = target_pos;
  }

  if (render_type == VSX_WIDGET_RENDER_2D) {
    test = &(coords.screen_global);
    global = &(coords.screen_local);
    //printf("screen.local.y %f\n",coords.screen_local.y);
  }
  else
  if (render_type == VSX_WIDGET_RENDER_3D) {
    test = &(coords.world_global);
    global = &(coords.world_local);
  }
  //printf("%s test.x: %f  test.y: %f  %f::%f\n",name.c_str(),test->x, test->y, global->x,global->y);
  //int hit = inside_xy_l(*test,*global);
  switch (inside_xy_l(*test,*global)) {
    case VSX_WIDGET_COORD_CENTER:
      result.center.x = (*test).x-((*global).x);
      result.center.y = (*test).y-(*global).y;
      result.corner.x = result.center.x + target_size.x*0.5f;
      result.corner.y = result.center.y + target_size.y*0.5f;
      //result.corner.x = -((*global).x-(*test).x-target_size.x*0.5f);
      //result.corner.y = -((*global).y-(*test).y-target_size.y*0.5f);
      //printf("hit center! %s\n",name.c_str());
      if (result.corner.x > 0.0f &&
      result.corner.y > 0.0f &&
      result.corner.x < target_size.x && result.corner.y < target_size.y)
      return true;
      else
      return false;
    case VSX_WIDGET_COORD_CORNER:
      //result.center.x = ((*global).x+(*test).x-target_size.x*0.5f);
      //result.center.y = (-(*global).y+(*test).y-target_size.y*0.5f);
      result.corner.x = -((*global).x-(*test).x);
      result.corner.y = -((*global).y-(*test).y);
      result.center.x = result.corner.x - target_size.x*0.5f;
      result.center.y = result.corner.y - target_size.y*0.5f;
      //printf("hit corner! %s\n",name.c_str());
      return true;
    default:
      return false;
  }
}



vsx_widget* vsx_widget::find_component(vsx_widget_coords coords, vsx_widget_distance &result_distance, int depth) {
  //vsx_vector u;
  if (depth == 0) {
    coords.world_local.x = coords.world_local.y = 0.0f;
    coords.screen_local.x = coords.screen_local.y = 0.0f;
  }
  vsx_widget *tt;
  if (!enabled) return 0;
  if (!visible) return 0;


  if (coord_related_parent)
  {
    if (render_type == VSX_WIDGET_RENDER_3D) {
      coords.world_local.x += target_pos.x;
      coords.world_local.y += target_pos.y;
    } else {
      coords.screen_local.x += target_pos.x;
      coords.screen_local.y += target_pos.y;
    }
  }
  if (inside_xy(coords,result_distance)) {
    if (region_exclusive) {
      return this;
    }
    if (!children.size()) return this;
    // scan children
    children.reverse();
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) {
      vsx_widget_distance dist = result_distance;
      tt = (*children_iter)->find_component(coords,dist,1);
      if (tt) {
        children.reverse();
        result_distance = dist;
        return tt;
      }
    }
    children.reverse();
    return this;
  } else {
    if (!children.size()) return 0;
    children.reverse();
    for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) {
      //printf("child: %s\n",(*children_iter)->name.c_str());
      vsx_widget_distance dist = result_distance;
      tt = (*children_iter)->find_component(coords,dist,1);
      if (tt) {
        result_distance = dist;
        children.reverse();
        return tt;
      }
    }
    children.reverse();
    return 0;
  }
} // find_component

void vsx_widget::mouse_wheel(float y) {
	if (m_o_focus)
	m_o_focus->event_mouse_wheel(y);
}

void vsx_widget::event_mouse_wheel(float y) {
}

void vsx_widget::mouse_move_passive(float x, float y) {
	//if (a_focus) printf("afocus: %s\n",a_focus->name.c_str());
  mouse.set_cursor(MOUSE_CURSOR_ARROW);
  vsx_widget_coords coord;
  coord.init(x,y);
  vsx_widget_distance result;
  vsx_widget *t = find_component(coord,result);
  if (t != 0) {
    t->event_mouse_move_passive(result,coord);
    m_o_focus = t;
    //printf("widget found: %s\n",t->name.c_str());
    /*printf("result center-coord: %f ,  %f\n",result.center.x, result.center.y);
    printf("result corner-coord: %f ,  %f\n",result.corner.x, result.corner.y);
    printf("2d/3d: %d     coord_t: %d\n-------------------\n",t->render_type, t->coord_type);*/
  }
	mouse.set_cursor_implement();
}

void vsx_widget::set_key_modifiers(bool alt_, bool ctrl_, bool shift_) {
	alt = alt_;
	ctrl = ctrl_;
	shift = shift_;
  /*int s = glutGetModifiers();
  alt = (bool)(GLUT_ACTIVE_ALT & s);
  ctrl = (bool)(GLUT_ACTIVE_CTRL & s);
  shift = (bool)(GLUT_ACTIVE_SHIFT & s);*/
}

void vsx_widget::mouse_down(float x, float y, int button) {
  //printf("mouse down\n");
	mouse.set_cursor(MOUSE_CURSOR_ARROW);
  mouse.show_cursor();


  vsx_widget_coords coord;
  coord.init(x,y);
  //coord_temp = coord;
  vsx_widget_distance result;
  vsx_widget *t = find_component(coord,result);
  //printf("----------\n");
  if (t != 0) {
  	//printf("component found: %s\n",t->name.c_str());
    // set up coordinates for
    mouse_down_pos = result;
    mouse.position.set(x,y);
    k_focus = t;
    m_focus = t;
    t->event_mouse_down(result,coord,button);
    if (time - t->double_click_d[button] < 0.3 && (last_clicked == t))
    {
      t->double_click_d[button] = 0;
      t->event_mouse_double_click(result,coord,button);
      last_clicked = 0;
    }
    else
    {
      t->double_click_d[button] = time;
      last_clicked = t;
    }
  }
	mouse.set_cursor_implement();
  //printf("done mouse down\n");
}

void vsx_widget::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  if (this != root)
  parent->front(this);
  if (button == 0)
  {
    a_focus = this;
    mouse_down_l = true;
    if (support_scaling)
    if (alt && !ctrl) {
      scaling_start = distance.center;
      scaling_start_size = size;
      scaling_start_pos = target_pos;
    }
  }
  if (button == 2)
  {
    //printf("rdown\n");
    mouse_down_r = true;
  }
}
void vsx_widget::mouse_move(float x, float y) {
//  printf("mx: %f, my: %f\n",x,y);
  mouse.set_cursor(MOUSE_CURSOR_ARROW);
  if (m_focus) {
    vsx_widget_coords coord;
    mouse.position.set(x,y);
    vsx_widget_distance distance;
    m_focus->calculate_mouse_distance(x,y,coord,distance);
    m_focus->event_mouse_move(distance,coord);
  }
  mouse.set_cursor_implement();
}

void vsx_widget::mouse_up(float x, float y, int button) {
  mouse.set_cursor(MOUSE_CURSOR_ARROW);
  if (m_focus) {
    vsx_widget_coords coord;
    vsx_widget_distance distance;
    m_focus->calculate_mouse_distance(x,y,coord,distance);
    if (button == 0) m_focus->mouse_down_l = false;
    if (button == 2) {
      m_focus->mouse_down_r = false;
    }
    m_focus->event_mouse_up(distance,coord,button);
    m_focus = 0;
  }
  mouse.set_cursor_implement();
}




void vsx_widget::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords) {
  //glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
  //printf("distance.corner.x: %f\n",distance.corner.x);
  if (visible) {
    resize_i = 0;
  	float db = dragborder*1.5f;
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
/*
#define MOUSE_CURSOR_HAND 1
#define MOUSE_CURSOR_IBEAM 2
#define MOUSE_CURSOR_SIZE 3
#define MOUSE_CURSOR_NS 4
#define MOUSE_CURSOR_WE 5
#define MOUSE_CURSOR_NESW 6
#define MOUSE_CURSOR_NWSE 7
*/

    if ((resize_i == 2) || (resize_i == 8)) mouse.set_cursor(MOUSE_CURSOR_WE);
    if ((resize_i == 1) || (resize_i == 4)) mouse.set_cursor(MOUSE_CURSOR_NS);
    if (resize_i == 6) mouse.set_cursor(MOUSE_CURSOR_NWSE);
    if (resize_i == 9) mouse.set_cursor(MOUSE_CURSOR_NWSE);
    if (resize_i == 3) mouse.set_cursor(MOUSE_CURSOR_NESW);
    if (resize_i == 12) mouse.set_cursor(MOUSE_CURSOR_NESW);
  }
}

void vsx_widget::calculate_mouse_distance(float x, float y, vsx_widget_coords &coord, vsx_widget_distance &distance) {
    coord.init(x,y);
  vsx_vector world;
  if (render_type == VSX_WIDGET_RENDER_2D) {
    world = coord.screen_global;
  } else
  if (render_type == VSX_WIDGET_RENDER_3D) {
    world = coord.world_global;
  }
  //printf("world.x pre: %f\n",world.x);
  //printf("world.y pre: %f\n",world.y);
  world = world - get_target_pos_p();
  if (coord_type == VSX_WIDGET_COORD_CENTER) {
    distance.center = world;
    distance.corner.x = target_size.x*0.5 - world.x;
    distance.corner.y = target_size.y*0.5 - world.y;
    distance.corner.z = distance.center.z = 0;
  } else {
    distance.corner = world;
    distance.center.x = world.x - target_size.x*0.5f;
    distance.center.y = world.y - target_size.y*0.5f;
    distance.corner.z = distance.center.z = 0;
  }
  //printf("world post: %f , %f\n",distance.center.x,distance.center.y);
  //printf("world.x post: %f\n",world.x);
}

void vsx_widget::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  if (button == 2) {
    if (!menu_temp_disable)
    if (menu) {
      //printf("menu up\n");
      front(menu);
      menu->visible = 2;
      menu->pos = menu->target_pos = coords.screen_global;
    }
    menu_temp_disable = false;
  }
}


void vsx_widget::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) {
  if (mouse_down_l)
  {
    if (support_scaling)
    if (alt && !ctrl) {
      target_size.x += (distance.center.x-mouse_down_pos.center.x)*2.0f;
      if (target_size.x < this->size_min.x) target_size.x = this->size_min.x;
      interpolating_size = true;
      mouse_down_pos = distance;
    }

    if (resize_i)
    resize(distance);
    else move_d(vsx_vector(distance.center.x-mouse_down_pos.center.x,distance.center.y-mouse_down_pos.center.y));
  }
}

void vsx_widget::resize(vsx_widget_distance distance) {
  if (visible) {
    if ((resize_i == 2) || (resize_i == 8)) mouse.set_cursor(MOUSE_CURSOR_WE);
    if ((resize_i == 1) || (resize_i == 4)) mouse.set_cursor(MOUSE_CURSOR_NS);
    if (resize_i == 6) mouse.set_cursor(MOUSE_CURSOR_NWSE);
    if (resize_i == 9) mouse.set_cursor(MOUSE_CURSOR_NWSE);
    if (resize_i == 3) mouse.set_cursor(MOUSE_CURSOR_NESW);
    if (resize_i == 12) mouse.set_cursor(MOUSE_CURSOR_NESW);

    float dx = distance.center.x-mouse_down_pos.center.x;
    float dy = mouse_down_pos.center.y - distance.center.y;
    vsx_vector s_target_pos = target_pos;
    if (resize_i & 1) {
      //float dy = screen.y-target_pos.y;
      //target_pos.y -= dy;
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
      //mouse_down_pos.center.y -= dy;
    }
    if (resize_i & 4) {
      //double dy = target_pos.y-screen.y;
//      float dy = world.y-mouse_down_pos.y;
      if (coord_type == VSX_WIDGET_COORD_CORNER)
      {
        //float dy = mouse_down_pos.center.y-distance.center.y;
        //printf("4 %f %f\n",mouse_down_pos.center.y,distance.center.y);
        target_size.y += dy;
        if (!(target_size.y < size_min.y)) {
          target_pos.y -= dy;
        mouse_down_pos.center.y -= dy*0.5f;
        } else target_size.y = size_min.y;

      } else {
        float dy = -(distance.center.y-mouse_down_pos.center.y);
        //printf("4 %f %f\n",dy,distance.center.y);
        //target_pos.y -= dy*0.5f;
        target_size.y += dy*2.0f;
        mouse_down_pos.center.y = distance.center.y;

      }
      //mouse_down_pos.y += dy;
      //if (target_size.y < size_min.y) target_size.y = size_min.y;
    }
    // LEFT
    if (resize_i & 8) {
      if (coord_type == VSX_WIDGET_COORD_CORNER) {
        //float dx = mouse_down_pos.center.x-distance.center.x;

        target_size.x -= dx;
        //mouse_down_pos.corner.x = distance.corner.x;
        if (target_size.x < size_min.x) {
          target_size.x = size_min.x;
          //target_pos.x += dx;
        } else  {
          target_pos.x += dx;
          mouse_down_pos.center.x += dx*0.5f;
        }
      } else {
        target_size.x -= dx*2.0f;
        mouse_down_pos.center.x = distance.center.x;
      }
    }
    // RIGHT
    if (resize_i & 2) {
       //printf("2\n");
      if (coord_type == VSX_WIDGET_COORD_CENTER) {
        mouse_down_pos.center.x += dx;
        target_size.x += dx*2.0f;
      }
      else {
        //printf("dx: %f\n",dx);
        mouse_down_pos.center.x += dx*0.5f;
        target_size.x += dx;
      }
      if (target_size.x < size_min.x) {
        target_size.x = size_min.x;
      }

      //if (coord_type == VSX_WIDGET_COORD_CENTER)
      //target_pos.x += dx;

//      mouse_down_pos.center.x += dx;

    }
    //if (!target_size.x) target_size.x = this->target_size.x;
    //if (!target_size.y) target_size.y = this->target_size.y;
    /*if (target_size.x < size_min.x) {
      target_size.x = size_min.x;
    } else target_pos.x = s_target_pos.x;
    if (target_size.y < size_min.y) target_size.y = size_min.y;
    else target_pos.y = s_target_pos.y;*/
    resize_to(target_size);
  }
}


//void vsx_widget::event_mouse_move_passive(vsx_vector world, vsx_vector screen) {
/*  glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
  if (visible) {
    resize_i = 0;
    if (allow_resize_y) {
      if ((world.y < pos.y+size.y*0.5+dragborder*4) && (world.y > pos.y+size.y*0.5-dragborder*2)) resize_i += 1;
      if ((world.y < pos.y-size.y*0.5+dragborder*4) && (world.y > pos.y-size.y*0.5-dragborder*2)) resize_i += 4;
    }
    if (allow_resize_x) {
      if ((world.x < pos.x+size.x*0.5+dragborder*4) && (world.x > pos.x+size.x*0.5-dragborder*2)) resize_i += 2;
      if ((world.x < pos.x-size.x*0.5+dragborder*4) && (world.x > pos.x-size.x*0.5-dragborder*2)) resize_i += 8;
    }
    if ((resize_i == 2) || (resize_i == 8)) glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
    if ((resize_i == 1) || (resize_i == 4)) glutSetCursor(GLUT_CURSOR_UP_DOWN);
    if (resize_i == 6) glutSetCursor(GLUT_CURSOR_BOTTOM_RIGHT_CORNER);
    if (resize_i == 9) glutSetCursor(GLUT_CURSOR_TOP_LEFT_CORNER);
    if (resize_i == 3) glutSetCursor(GLUT_CURSOR_TOP_RIGHT_CORNER);
    if (resize_i == 12) glutSetCursor(GLUT_CURSOR_BOTTOM_LEFT_CORNER);
    if (!resize_i)
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
  }*/
//}



//void vsx_widget::mouse_move_passive(double x, double y, bool translated) {
/*  vsx_vector screen = (vsx_vector(x,y,0) / vsx_vec_viewport());
  screen.y = 1-screen.y;

  vsx_vector u;
  u = translate_xy(x,y);
  vsx_widget *t = find_component(u,screen,true);
  if (t != 0) {
    m_o_focus = t;
    if (t->render_type == VSX_WIDGET_RENDER_2D)
    {
      screen.x -= t->pos.x;
      screen.y -= t->pos.y;
      t->event_mouse_move_passive(screen,screen);
      t->mouse_pos = screen;
      return;
    }
    if (render_type == VSX_WIDGET_RENDER_3D)
    {
      vsx_vector uu = u - t->parent->get_pos_p();
      uu.z = t->pos.z;
      mouse.position.set(x,y);
      t->event_mouse_move_passive(uu,screen);
      t->mouse_pos = uu;
      return;
    }
  } else root->event_mouse_move_passive(u,screen);
  */
//}


using namespace std;
void vsx_widget::front(vsx_widget* t) {
  //printf("fronting %s\n",t->name.c_str());
  //for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
//    printf("%s,",(*children_iter)->name.c_str());
//  }printf("\n");

  children_iter = children.end();
  children_iter--;
  if ((!(*children_iter)->topmost) || (t->topmost) || (t->render_type == VSX_WIDGET_RENDER_2D)) {
  //  printf("++topmost\n");
    children.remove(t);
    children.push_back(t);
  } else {
    if (t != *children_iter) {
    //  printf("++searching for topmost\n");
      children.remove(t);
      children_iter = children.end();
      --children_iter;
      //  printf("  moving up: %s\n",(*children_iter)->name.c_str());
      if (children_iter != children.begin())
      while ((*(children_iter))->topmost && children_iter != children.begin()) {
      --children_iter;
//        printf("  moving up: %s\n",(*children_iter)->name.c_str());
      };
      //if (children_iter != children.begin())
      if (!((*children_iter))->topmost)
      ++children_iter;
      children.insert(children_iter,1,(vsx_widget *)t);
    }
  }
//  for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
//    printf("%s,",(*children_iter)->name.c_str());
//  }
//  printf("\n-------------------\n");
  if (parent != this) parent->front(this);
}

void vsx_widget::before_delete() {}
void vsx_widget::on_delete() {}


void vsx_widget::move(double x, double y, double z) {
	event_move_scale();
  if (constrained_x) {
    if (x > parent->size.x/2-size.x/2) x = parent->size.x/2-size.x/2;
    if (x < -(parent->size.x/2-size.x/2)) x = -(parent->size.x/2-size.x/2);
  }
  if (constrained_y) {
    if (y > parent->size.y/2-size.y/2) y = parent->size.y/2-size.y/2;
    if (y < -(parent->size.y/2-size.y/2)) y = -(parent->size.y/2-size.y/2);
  }
  //printf("move: %s  %d %d\n",name.c_str(),allow_move_x,allow_move_y);
  if (allow_move_x) target_pos.x = x;
  if (allow_move_y) target_pos.y = y;
  target_pos.z = z;
  if (support_interpolation) {
    interpolating_pos = true;
  } else {
    pos = target_pos;
  }
}

void vsx_widget::move(vsx_vector t) {
  move(t.x,t.y,t.z);
}

void vsx_widget::move_d(vsx_vector t) {
  if (support_interpolation)
    move(
      target_pos.x+t.x,
      target_pos.y+t.y,
      target_pos.z+t.z
    );
  else
  move(
    pos.x+t.x,
    pos.y+t.y,
    pos.z+t.z
  );
}


void vsx_widget::show_children() {
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
  {
    (*it)->visible = 1;
    (*it)->show_children();
  }
}

void vsx_widget::hide_children() {
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
  {
    (*it)->visible = 0;
    (*it)->hide_children();
  }
}



vsx_widget *vsx_widget::add(vsx_widget *t,vsx_string name) {
  //printf("adding %s\n",name.c_str());
  t->name = name;
  t->parent = this;
  t->cmd_parent = this;
  if (t->topmost) children.push_back(t); else
  if (children.size()) {
    children_iter = children.end();
    --children_iter;
    if (children_iter != children.begin())
    while ((*(children_iter))->topmost && children_iter != children.begin()) {
//      printf("1");
    --children_iter;
    };

    children.insert(children_iter,1,t);
  } else
  children.push_back(t);
  //++children_iter;
  //printf("++adding done\n");
  //
  //log("added child widget: "+name);
  //cout << this->name+" created vsx_widget_component "+name << endl;
  glist[name] = t;
  l_list[name] = t;
  return t;
}

void vsx_widget::vsx_command_process_f() {
  if (this == root) {
    vsx_command_s *c = 0;

    while ( (c = command_q_f.pop()) )
    {
      // cleanup duties :3

      if (c->cmd == "delete") {
        vsx_widget *t = f(s2i(c->cmd_data));
        if (t) {
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
//          log("deleted component "+c->cmd_data+" "+t->name);
					#ifdef VSX_DEBUG
          printf("actual deletion\n",t->id);
          #endif
          if (t->parent->marked_for_deletion) {
          //if (m_focus == t) m_focus = root;
          //if (k_focus == t) k_focus = root;
          //if (a_focus == t) { a_focus = root; m_focus = root; }
          m_focus = k_focus = a_focus = root;
          } else
          {
          	m_focus = t->parent;
          	k_focus = t->parent;
          	a_focus = t->parent;
          }
          delete t;
          #ifdef VSX_DEBUG
          printf("after actual deletion\n",t->id);
          #endif
        }
      }
    }
  }
  vsx_command_queue_f();
  #ifdef VSX_DEBUG
  printf("end ofqf %s\n",name.c_str());
  #endif
}


void vsx_widget::delete_() {
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it) {
  	#ifdef VSX_DEBUG
  	//printf("delete_ %s\n",(*it)->name.c_str());
  	//printf("1");
  	#endif
    (*it)->marked_for_deletion = true;
    #ifdef VSX_DEBUG
    //printf("2");
    #endif
    (*it)->before_delete();
    #ifdef VSX_DEBUG
    //printf("3");
    #endif
    (*it)->delete_();
    #ifdef VSX_DEBUG
    //printf("4");
    #endif
    (*it)->on_delete();
    #ifdef VSX_DEBUG
    //printf("5");
    #endif
    #ifdef VSX_DEBUG
    //printf("6\n");
    #endif

    delete *it;
  }
}

void vsx_widget::_delete() {
  if (marked_for_deletion) return;
  mark_for_deletion();
  before_delete();
  root->command_q_f.add("delete",id);
}

void vsx_widget::mark_for_deletion() {
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it) {
    (*it)->mark_for_deletion();
  }
  marked_for_deletion = true;
}


void vsx_widget::delete_all_by_type(unsigned long t) {
  //std::map<int, vsx_widget*> tt = ilist;
  for (std::map<int, vsx_widget*>::iterator it = ilist.begin();  it != ilist.end(); ++it) {
    if ((*it).second->widget_type == t) {
      (*it).second->_delete();
    }
  }
}



void vsx_widget::set_render_type(unsigned long new_render_type) {
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) {
    (*children_iter)->set_render_type(new_render_type);
  }
  //if (new_render_type == VSX_WIDGET_RENDER_2D) pos.z = 0.0f;
  render_type = new_render_type;
}
void vsx_widget::set_border(float border) {
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) {
    (*children_iter)->set_border(border);
  }
  dragborder = border;
}
void vsx_widget::set_font_size(float fsize) {
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter) {
    (*children_iter)->set_font_size(fsize);
  }
  font_size = fsize;
}


//------------------------------------------------------------------------------
// MOUSE DOWN
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// MOUSE UP
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// MOUSE MOVE
//------------------------------------------------------------------------------



void vsx_widget::stop() {
  if (this == root) {
    vsx_texture tex;
    tex.unload_all_active();
  }
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
  (*children_iter)->stop();
}

void vsx_widget::reinit() {
  if (this == root) {
    vsx_texture tex;
    tex.reinit_all_active();
    myf.reinit_all_active(); // reinit all active fonts plz
  }
  for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
  (*children_iter)->reinit();
}


void vsx_widget::interpolate_pos() {
  float tt = dtime*interpolation_speed*global_interpolation_speed;
  if (tt > 1) { tt = 1; interpolating_pos = false;}
  pos.x = pos.x*(1-tt)+target_pos.x*tt;
  pos.y = pos.y*(1-tt)+target_pos.y*tt;
  if (
    (round(pos.x*2000.0f) == round(target_pos.x*2000.0f)) &&
    (round(pos.y*2000.0f) == round(target_pos.y*2000.0f))
  ) interpolating_pos = false;
}

void vsx_widget::interpolate_size() {
  float tt = dtime*interpolation_speed*global_interpolation_speed;
  if (tt > 1) { tt = 1; interpolating_size = false;}
  size.x = size.x*(1-tt)+target_size.x*tt;
  size.y = size.y*(1-tt)+target_size.y*tt;
  if (
    (round(size.x*2000) == round(target_size.x*2000)) &&
    (round(size.y*2000) == round(target_size.y*2000))
  ) interpolating_size = false;
}



void vsx_widget::resize_to(vsx_vector to_size) {
  if (target_size.x < size_min.x) to_size.x = size_min.x;
  if (target_size.y < size_min.y) to_size.y = size_min.y;
	event_move_scale();
  if (support_interpolation) {
    interpolating_size = true;
    interpolating_pos = true;
    target_size = to_size;
  } else {
    size = target_size = to_size;
    pos = target_pos;
  }
}

void vsx_widget::draw() {
  if (visible) {
    if (render_type == VSX_WIDGET_RENDER_3D) {
      i_draw();
    }
    draw_children_3d();
  }
}

void vsx_widget::draw_2d() {
	#ifdef VSX_DEBUG
	printf("draw 2d: %s\n",name.c_str());
	#endif
  if (visible) {
    if (render_type == VSX_WIDGET_RENDER_2D) {
       i_draw();
    }
    draw_children_2d();
  }
  #ifdef VSX_DEBUG
	printf("end_draw 2d: %s\n",name.c_str());
	#endif
}


// VSX_WIDGET_WINDOW ***************************************************************************************************
// VSX_WIDGET_WINDOW ***************************************************************************************************
// VSX_WIDGET_WINDOW ***************************************************************************************************
// VSX_WIDGET_WINDOW ***************************************************************************************************

#endif
