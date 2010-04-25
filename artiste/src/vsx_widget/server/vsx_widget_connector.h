#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_CONNECTOR_H
#define VSX_WIDGET_CONNECTOR_H

// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************


class vsx_widget_connector : public vsx_widget {
  bool macro_child;
  bool transparent;
  vsx_vector pv;
  double sx,sy,ex,ey; // internal coordinates
  vsx_texture mtex_blob;
  
public:
  static float dim_alpha;
  float dim_my_alpha;
  vsx_vector real_pos; // storing the value of the position of the drawing
                       // so that the anchor can order them in the order they
                       // appear visually. Updated on each frame in the draw func.
  bool alias_conn; // is this an alias connection?
  bool open;
  int order;
  static bool receiving_focus;
//  double smx, smy; // coordinate where mouse has been clicked
//  double mdx, mdy; // delta mouse move
  vsx_widget *destination;
  
  void vsx_command_process_b(vsx_command_s *t);
  
  void show_children() {};
  void hide_children() {};

  void before_delete();
  void on_delete();
  void init();
  int inside_xy_l(vsx_vector &test, vsx_vector &global);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);
  void draw();
  vsx_widget_connector();
};



#endif
#endif
