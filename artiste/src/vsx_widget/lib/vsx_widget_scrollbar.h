#ifndef VSX_WIDGET_SCROLLBAR_H
#define VSX_WIDGET_SCROLLBAR_H


class vsx_widget_scrollbar : public vsx_widget {
  float scroll_handle_size;
  vsx_vector click_down;
  float cur_pos_click_down;
  float cur_pos;
  float value; // 
public:
  int scroll_type; // 0 = horizontal, 1 = vertical
  float scroll_window_size;
  float scroll_max; // scroll from 0 to what?
  float* control_value;
  vsx_widget_scrollbar();
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);

  float shz;
  void i_draw();

};

#endif
