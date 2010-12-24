#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_PREVIEW_H
#define VSX_WIDGET_PREVIEW_H

class vsx_window_texture_viewer : public vsx_widget_window {
  vsx_widget *inspected;
  vsx_widget *label1;
  vsx_widget *server;
  float frame_delta;
  float frame_count;
  float fps;
  void** engine;

public:
#ifndef VSXU_PLAYER  
  vsx_texture texture;
#endif

 	bool fullscreen;
 	bool fullwindow;
 	vsx_string modestring;
 	vsx_string modestring_default;
 	bool run;
  vsx_vector pos_, size_;
  vsx_command_list modestring_commands;
	void draw() {};
  void draw_2d();
  void init();
  void set_server(vsx_widget* new_server);
  virtual bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);
  virtual void vsx_command_process_b(vsx_command_s *t);

  vsx_window_texture_viewer();
};


#endif
#endif
