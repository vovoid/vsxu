#include "vsx_gl_global.h"
#include <map>
#include <list>
#include <vector>
#include "vsx_command.h"
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_font.h"
#include "../vsx_widget_base.h"
#include "vsx_widget_scrollbar.h"


vsx_widget_scrollbar::vsx_widget_scrollbar() {
  vsx_widget::vsx_widget();
  scroll_max = 1;
  scroll_window_size = 0.5;
  value = 0;
  control_value = 0;
  coord_type = VSX_WIDGET_COORD_CORNER;
}  

void vsx_widget_scrollbar::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  click_down = distance.corner;
  //printf("cd_world.y = %f\n",click_down.y);
  //printf("cd_world_local.y = %f\n",distance.corner.y);
  cur_pos_click_down = cur_pos;
  //printf("cur_pos_click_down %f\n",cur_pos_click_down);
//    vsx_widget::event_mouse_down(world,screen,button);
}  

void vsx_widget_scrollbar::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) {
//  vsx_vector pp = parent->get_pos_p();
    
  if (scroll_type == 0) { // horizontal scroll
    cur_pos = distance.corner.x + cur_pos_click_down-click_down.x;
    if (cur_pos < 0) cur_pos = 0;
    if (cur_pos > (1-shz)*size.x) cur_pos = (1-shz)*size.x;
    if ((1-shz)*size.x != 0)
    value = ((cur_pos/((1-shz)*size.x))*(float)scroll_max);
    else value = 0;
  }
  else
  { // vertical scroll
    cur_pos = (-distance.corner.y + cur_pos_click_down+click_down.y);///(size.y*shz);//;
    //cur_pos = (click_down.y-world.y+pp.y+cur_pos_click_down)/(size.y*shz);//;
    //printf("cur_pos: %f\n",cur_pos);
    if (cur_pos < 0.0f) cur_pos = 0.0f;
    if (cur_pos > (1-shz)*size.y) cur_pos = (1-shz)*size.y;
//      value = cur_pos*scroll_max;
    value = cur_pos/((1-shz)*size.y)*scroll_max;
  }
  if (control_value) *control_value = value;
}

void vsx_widget_scrollbar::i_draw() {
  if (visible) {
    scroll_handle_size = scroll_window_size;// scroll_window_size/scroll_max;
    //printf("scroll_handle_size: %f  scroll_max: %f\n",scroll_handle_size, scroll_max);
    //if (scroll_handle_size < scroll_max) scroll_max = scroll_handle_size;
    if (control_value)
    value = *control_value;
    if (scroll_handle_size > scroll_max) {
      //printf("adjusting handle\n");
      scroll_max = scroll_handle_size;
    }
    if (size.y < 0) return;
    if (size.x < 0) return;
      

    vsx_vector p = parent->get_pos_p()+pos;
    p.z = pos.z;
    shz = scroll_handle_size/(scroll_max);
    glColor4f(skin_color[0].r,skin_color[0].g,skin_color[0].b,skin_color[0].a);
    draw_box(p,size.x,size.y);
    glColor4f(1,1,1,1);
    if (scroll_type == 0) {
      // horizontal bar
      cur_pos = (size.x-size.x*(shz))*(value/scroll_max);
      p.x += cur_pos;
      draw_box(p,size.x*shz,size.y);
    } else
    {
      // vertical
      cur_pos = (size.y-size.y*(shz))*(value/scroll_max);
      p.y = -cur_pos+p.y+size.y;
      draw_box(p,size.x,-size.y*shz);
    }
  }
}
