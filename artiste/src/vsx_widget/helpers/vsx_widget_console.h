#ifndef VSX_WIDGET_CONSOLE_H
#define VSX_WIDGET_CONSOLE_H

// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************
// VSX_WIDGET_CONSOLE **************************************************************************************************


class vsx_widget_2d_console : public vsx_widget {
public:
  double rows;
  //vsx_font myf;
  vsx_string h;
  double texty;
  bool up;
  double fontsize;
  double tx;
  double ypos;
  double yposs;
  double ythrust;
  double height;
  double htime;
  vsx_widget *editor;
  vsx_widget *destination;
  void init();
  void vsx_command_process_b(vsx_command_s *t);
  int inside_xy_l(vsx_vector &test, vsx_vector &global);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void draw_2d();
  void set_destination(vsx_widget* dest);
};

#endif
