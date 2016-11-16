#pragma once

#include "profiler_desktop.h"

// draw-related variables
class vsx_profiler_draw {
public:
  vsx_profiler_desktop* desktop;
  vsx_string<>current_fps;
  vsx_timer gui_t;
  vsx_timer engine_render_time;
  int frame_count;
  int movie_frame_count;
  float gui_g_time;
  double dt;
  double gui_fullscreen_fpstimer;
  double max_fps;
  double min_fps;
  double max_render_time;
  double min_render_time;
  vsx_font myf;
  vsx_command_list system_command_queue;

  vsx_profiler* profiler;

  vsx_profiler_draw() :
    frame_count(0),
    gui_g_time(0),
    gui_fullscreen_fpstimer(0),
    max_fps(0),
    min_fps(1000000),
    max_render_time(-1),
    min_render_time(1000),
    system_command_queue(false)
  {}
  ~vsx_profiler_draw() {}


  void init()
  {
    profiler = vsx_profiler_manager::get_instance()->get_profiler();
    desktop = new vsx_profiler_desktop;
    desktop->system_command_queue = &system_command_queue;
    desktop->init();
  }

  void draw()
  {
    dt = gui_t.dtime();
    gui_g_time += dt;

    ++frame_count;

    vsx_widget_time::get_instance()->set_dtime( dt );
    vsx_widget_time::get_instance()->increase_time( dt );
    desktop->vsx_command_process_f();

    desktop->frames = frame_count;



      profiler->sub_begin("init_frame");
          desktop->init_frame();
      profiler->sub_end();

      profiler->sub_begin("desktop draw");
          desktop->draw();
      profiler->sub_end();

      profiler->sub_begin("draw 2d");
          desktop->draw_2d();
      profiler->sub_end();

      vsx_command_process_garbage();
  }
};
