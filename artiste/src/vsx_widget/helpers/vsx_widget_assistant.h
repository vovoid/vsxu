#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_ASSISTANT_H
#define VSX_WIDGET_ASSISTANT_H


#ifndef VSXU_PLAYER
class vsxu_assistant : public vsx_widget {
  vsx_widget *inspected;
//-----------------------------------------------
  unsigned long timestamp;
  bool reload;
  bool auto_;
  float tt,alpha;
  vsx_widget *pager;
  vsx_texture texture;
  vsx_vector pos_, size_;
  vsx_string text;
  vsx_widget* cur_focus;
  float size_multiplier;
  vsx_vector clickpoint, endpoint, cursize, endsize;
  std::vector<vsx_string> course;
  
  float temp_size;
  
public:
  void temp_hide();
  void temp_show();
  void i_draw();
  void vsx_command_process_b(vsx_command_s *t);
  void init();
  void reinit();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void toggle_size();
  vsxu_assistant();
};
#endif

#endif
#endif
