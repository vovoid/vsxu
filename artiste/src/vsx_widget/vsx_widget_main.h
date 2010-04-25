#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_MAIN_H
#define VSX_WIDGET_MAIN_H

// VSX_WIDGET_DESKTOP **************************************************************************************************
// VSX_WIDGET_DESKTOP **************************************************************************************************
// VSX_WIDGET_DESKTOP **************************************************************************************************
// VSX_WIDGET_DESKTOP **************************************************************************************************
class vsx_widget_desktop : public vsx_widget {
  float logo_time;
  bool first_draw;
  // move-around-the-desktop stuff
  double xp, yp;
  double xps, yps, zps; //current speed
  double xpd, ypd, zpd; //current direction
  //double xs, ys;
  double zp;
  double zpf;
  double zpa;
  vsx_widget *tv; // vme television (tm)

  bool interpolating;
  vsx_vector camera_target;
	vsx_widget* sv;
public:
  int auto_undo;
  double xpp,ypp,zpp;
#ifndef VSXU_PLAYER
  vsx_widget* assistant;
#endif
	vsx_widget *console;
  vsx_command_list *system_command_queue;
#ifndef VSXU_PLAYER
  vsx_texture mtex;
#endif
  void load_configuration();
  void save_configuration();
  void init();
  void reinit();
  bool key_down(signed long key, bool n_alt, bool n_ctrl, bool n_shift);
  bool key_up(signed long key, bool alt, bool ctrl, bool shift);
  void event_mouse_wheel(float y);
  void vsx_command_process_b(vsx_command_s *t);
  //void event_mouse_move_passive(vsx_vector world, vsx_vector screen);
  void draw();
  void draw_2d();
  void move_camera(vsx_vector world);
  vsx_widget_desktop();
};








#endif
#endif
