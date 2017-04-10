#pragma once

#include "vsx_launcher_desktop.h"

// draw-related variables
class vsx_launcher_draw
{
public:
  std::unique_ptr<vsx_launcher_desktop> desktop;
  vsx_string<> current_fps;
  vsx_timer gui_t;
  vsx_timer engine_render_time;
  int frame_count = 0;
  float gui_g_time = 0;
  vsx_command_list system_command_queue;

  vsx_launcher_draw() :
    system_command_queue(false)
  {}

  void init()
  {
    desktop = std::unique_ptr<vsx_launcher_desktop>(new vsx_launcher_desktop);
    desktop->system_command_queue = &system_command_queue;
    desktop->init();
  }

  void draw()
  {
    double dt = gui_t.dtime();
    gui_g_time += dt;
    ++frame_count;
    vsx_widget_time::get_instance()->set_dtime( dt );
    vsx_widget_time::get_instance()->increase_time( dt );
    desktop->vsx_command_process_f();
    desktop->frames = frame_count;
    desktop->init_frame();
    desktop->draw();
    desktop->draw_2d();
    vsx_command_process_garbage();
  }
};
