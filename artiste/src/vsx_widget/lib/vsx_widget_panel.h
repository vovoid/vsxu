#ifndef VSX_WIDGET_PANEL_H
#define VSX_WIDGET_PANEL_H

class vsx_widget_panel : public vsx_widget {
public:
  bool size_from_parent;
  bool pos_from_parent;

  void calc_size();
  virtual int inside_xy_l(vsx_vector &test, vsx_vector &global);
  vsx_vector calc_pos();
  void base_draw();
  virtual void i_draw() { base_draw(); }
  vsx_widget_panel();
};

#define VSX_WIDGET_SPLIT_PANEL_VERT 0 
#define VSX_WIDGET_SPLIT_PANEL_HORIZ 1 

class vsx_widget_split_panel : public vsx_widget_panel {
  // 2 x base_editor
public:
  float split_pos; // 0 -> 1
  float splitter_size;
  vsx_vector size_max;
  vsx_widget_panel* one;
  vsx_widget_panel* two;
  int orientation;
  float sy;
  void i_draw();
  void set_border(float border);

  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);

  int inside_xy_l(vsx_vector &test, vsx_vector &global);

  vsx_widget_split_panel();

};





#endif
