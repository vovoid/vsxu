#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_LIB_H
#define VSX_WIDGET_LIB_H

#include "vsx_texture.h"
#include "vsx_font.h"
#include "vsx_widget_base.h"

class vsx_widget_popup_menu : public vsx_widget {
  bool oversub;
public:
  vsx_command_list menu_items;
  int id_over;
  int over;
  vsx_command_s *current_command_over;
  double row_size;
  void init();
  void vsx_command_process_b(vsx_command_s *t);
  void add_commands(vsx_command_s* command);
  
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void i_draw();
  vsx_widget_popup_menu() { target_size.x = size.x = 0.4; }
  void on_delete();
};

// VSX_WIDGET_BUTTON ***************************************************************************************************
// VSX_WIDGET_BUTTON ***************************************************************************************************
// VSX_WIDGET_BUTTON ***************************************************************************************************
// VSX_WIDGET_BUTTON ***************************************************************************************************

class vsx_widget_button : public vsx_widget {
  bool outside;
  //vsx_font myf;
public:
  double border;
  void init();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void i_draw();
  vsx_widget_button() {render_type=VSX_WIDGET_RENDER_2D;};
};

// VSX_WIDGET_LABEL ****************************************************************************************************
// VSX_WIDGET_LABEL ****************************************************************************************************
// VSX_WIDGET_LABEL ****************************************************************************************************
// VSX_WIDGET_LABEL ****************************************************************************************************

	enum align{
		a_left,
		a_center,
		a_right
	};

class vsx_widget_2d_label : public vsx_widget {
public:
  bool inside_xyd(vsx_vector world, vsx_vector screen) { return false; }
	align halign;
  void init();
  void draw_2d();
};

// VSX_WIDGET_PAGER ***************************************************************************************************
// VSX_WIDGET_PAGER ***************************************************************************************************
// VSX_WIDGET_PAGER ***************************************************************************************************
// VSX_WIDGET_PAGER ***************************************************************************************************

class vsx_widget_2d_pager : public vsx_widget {
  bool outside;
  //vsx_font myf;
  int side;
public:
  int cur_page, max_pages;
  int type; // 0 = 2d, 1 = 3d
  double border;
  void init();
  //int inside_xy_l(vsx_vector &test, vsx_vector &global);
  //void event_mouse_down(vsx_vector world, vsx_vector screen, int button);
  //void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void i_draw();
  void increase();
  void decrease();
  vsx_widget_2d_pager();
};


#endif
#endif
