
#include <vsx_application_control.h>
#include <vsx_application_input_state_manager.h>
#include <bitmap/vsx_bitmap_writer.h>
#include <time/vsx_timer.h>
#include <vsx_gl_global.h>
#include <vsx_logo_intro.h>
#include <profiler/vsx_profiler_manager.h>
#include <vsx_font.h>
#include <vsx_engine.h>
#include <artiste_desktop.h>
#include <server/vsx_widget_server.h>
#include <helpers/vsx_widget_preview.h>
#include <log/vsx_log_a.h>

class vsx_artiste_draw {
public:

  bool first = true;

  vsx_string<> current_fps;

  // time
  float global_time = 0.0f;
  vsx_timer time2;
  vsx_timer gui_t;
  vsx_timer engine_render_time;

  // movie
  bool record_movie = false;
  int movie_frame_count;

  // fps
  vsx_string<>fpsstring = "VSX Ultra "+vsx_string<>(VSXU_VERSION)+" - 2012 Vovoid";
  int frame_count = 0;
  float gui_g_time = 0.0f;
  double dt;
  double gui_f_time = 0.0;
  double gui_fullscreen_fpstimer = 0.0;
  double max_fps = 0.0;
  double min_fps = 10000.0;
  double max_render_time = -1.0;
  double min_render_time = 1000.0;
  unsigned long frame_counter = 0;
  unsigned long delta_frame_counter = 0;
  float delta_frame_time = 0.0f;
  float delta_fps = 0.0f;
  float total_time = 0.0f;

  bool prod_fullwindow = false;
  bool take_screenshot = false;
  bool* gui_prod_fullwindow;
  bool gui_prod_fullwindow_helptext = true;
  bool reset_time_measurements = false;

  // desktop
  vsx_artiste_desktop *desktop = 0;
  vsx_logo_intro *intro;
  vsx_font myf;

  // engine
  vsx_module_list_abs* module_list = 0x0;
  vsx_engine* vxe = 0x0;
  vsx_command_list system_command_queue;
  vsx_command_list internal_cmd_in;
  vsx_command_list internal_cmd_out;


  VSXP_CLASS_DECLARE

  vsx_artiste_draw()
    :
      system_command_queue(false),
      internal_cmd_in(false),
      internal_cmd_out(false)
  {
    VSXP_CLASS_CONSTRUCTOR
  }

  void load_desktop_a(vsx_string<> state_name = "")
  {
    //printf("{CLIENT} creating desktop:");
    desktop = new vsx_artiste_desktop;
    //printf(" [DONE]\n");
    internal_cmd_in.clear_normal();
    internal_cmd_out.clear_normal();
    // connect server widget to command lists
    ((vsx_widget_server*)desktop->find("desktop_local"))->cmd_in = &internal_cmd_out;
    ((vsx_widget_server*)desktop->find("desktop_local"))->cmd_out = &internal_cmd_in;
    if (state_name != "")
      ((vsx_widget_server*)desktop->find("desktop_local"))->state_name =
        vsx_string_helper::str_replace<char>
        (
          "/",
          ";",
          vsx_string_helper::str_replace<char>(
            "//",
            ";",
            vsx_string_helper::str_replace<char>(
              "_states/",
              "",
              state_name
            )
          )
        );
    internal_cmd_out.add_raw(vsx_string<>("vsxu_welcome ")+vsx_string<>(VSXU_VER)+" 0", true);
    desktop->system_command_queue = &system_command_queue;
    vsx_widget* t_viewer = desktop->find("vsxu_preview");
    if (t_viewer)
    {
      gui_prod_fullwindow = ((vsx_window_texture_viewer*)desktop->find("vsxu_preview"))->get_fullwindow_ptr();
      LOG_A("found vsxu_preview widget");
    }
    ((vsx_widget_server*)desktop->find("desktop_local"))->engine = (void*)vxe;
    desktop->front(desktop->find("vsxu_preview"));

    desktop->init();
  }

  void init()
  {
    module_list = vsx_module_list_factory_create();
    vxe = new vsx_engine(module_list);
    gui_prod_fullwindow = &prod_fullwindow;
    myf.load( PLATFORM_SHARED_FILES + vsx_string<>("font/font-ascii_output.png"), vsx::filesystem::get_instance());
  }

  void uninit()
  {
    myf.unload();
    vxe->stop();
    delete vxe;
    vsx_module_list_factory_destroy( module_list );
    desktop->stop();
    desktop->on_delete();
    delete desktop;
    vsx_command_process_garbage_exit();
  }

  void pre_draw()
  {
    vsx_command_s *c = 0;
    while ( (c = system_command_queue.pop()) )
    {
      vsx_string<>cmd = c->cmd;
      if (cmd == "system.shutdown")
        vsx_application_control::get_instance()->shutdown_request();

      if (cmd == "fullscreen") {
        if (desktop)
          desktop->stop();
        internal_cmd_in.add(c);
      }
      c = 0;
    }
  }

  void draw() {
    VSXP_CLASS_LOCAL_INIT

    VSXP_S_BEGIN("artiste vsxu_draw")
    if (record_movie)
    {
      vxe->set_constant_frame_progression(1.0f / 60.0f);
      vxe->time_play();
    }

    if (desktop)
    {
      if (*gui_prod_fullwindow && !desktop->performance_mode)
        vxe->set_no_send_client_time(true);
      else
        vxe->set_no_send_client_time(false);

      VSXP_S_BEGIN("desktop cmd_process_f")
        desktop->vsx_command_process_f();
      VSXP_S_END
    }

    if (first)
      intro = new vsx_logo_intro;
    dt = gui_t.dtime();
    gui_f_time += dt;
    gui_g_time += dt;

    float f_wait;
    bool run_always = false;
    if (desktop)
    {
      if (desktop->global_framerate_limit == -1)
        run_always = true;
      else
        f_wait = 1.0f/desktop->global_framerate_limit;
    }
    else
      f_wait = 1.0f/100.0f;

    if (run_always || gui_f_time > f_wait)
    {
      ++frame_count;

      vsx_widget_time::get_instance()->set_dtime( dt );
      vsx_widget_time::get_instance()->increase_time( dt );

      if (desktop) {
        desktop->frames = frame_count;
      }
      gui_fullscreen_fpstimer += gui_f_time;
      current_fps = vsx_string_helper::f2s(round(1.0f/gui_f_time),2);
      if (gui_fullscreen_fpstimer > 1)
      {
        vsx_string<>h = fpsstring + " @ "+ current_fps+ "fps";
        gui_fullscreen_fpstimer = 0;
      }

      gui_f_time = 0;
      if (!*gui_prod_fullwindow)
      {
        if (desktop)
        {
          desktop->init_frame();
          desktop->draw();
          desktop->draw_2d();
        }
      }
      VSXP_S_BEGIN("vxe->process_msg")
        vxe->process_message_queue(&internal_cmd_in,&internal_cmd_out,false,false,60.0f);
      VSXP_S_END

      if (*gui_prod_fullwindow)
      {

        glDepthMask(GL_TRUE);
        glClearColor(0.0f,0.0f,0.0f,1.0f);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        if (vxe)
        {
          engine_render_time.start();

          VSXP_S_BEGIN("vxe->render")
            vxe->render();
          VSXP_S_END

          VSXP_S_BEGIN("draw overlay")
          float frame_time = engine_render_time.dtime();
          glDisable(GL_DEPTH_TEST);

          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          glMatrixMode(GL_MODELVIEW);
          glLoadIdentity();
          glEnable(GL_BLEND);
          ++frame_counter;
          ++delta_frame_counter;

          delta_frame_time+= dt;
          total_time += dt;
          if (delta_frame_counter == 100) {
            delta_fps = 100.0f/delta_frame_time;
            delta_frame_counter = 0;
            delta_frame_time = 0.0f;
          }

          if (gui_prod_fullwindow_helptext)
          {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0,0,0,0.4f);
            glBegin(GL_QUADS);                  // Draw A Quad
              glVertex3f(-1.0f, 1.0f, 0.0f);          // Top Left
              glVertex3f( 1.0f, 1.0f, 0.0f);          // Top Right
              glVertex3f( 1.0f,0.92f, 0.0f);          // Bottom Right
              glVertex3f(-1.0f,0.92f, 0.0f);          // Bottom Left
            glEnd();                      // Done Drawing The Quad
            if (reset_time_measurements)
            {
              max_fps = 0.0f;
              min_fps = 1000.0f;
              max_render_time = 0.0f;
              min_render_time = 1000.0f;
              reset_time_measurements = false;
            }
            if (delta_fps > max_fps) max_fps = delta_fps;
            if (delta_fps < min_fps) min_fps = delta_fps;
            if (frame_time > max_render_time) max_render_time = frame_time;
            if (frame_time < min_render_time) min_render_time = frame_time;
            myf.print(vsx_vector3<>(-0.99f,0.92f+0.04f),
                      vsx_string<>(VSXU_VERSION) + " " +
                      VSXU_VERSION_COPYRIGHT +
                      "- Alt+T=toggle this text - "
                      "Ctrl+Alt+P (take screenshot) - "
                      "Alt+F (toggle perf. mode) :: "
                      "Frame count: "+vsx_string_helper::i2s(frame_counter)+" | "+
                      "Elapsed time: "+vsx_string_helper::f2s(total_time)+" | "+
                      "Modules: "+vsx_string_helper::i2s(vxe->get_num_modules())+" | "
                      "Avg. FPS: "+vsx_string_helper::f2s(frame_counter/total_time)
                      ,0.025f);
            myf.print
            (
              vsx_vector3<>(
                -0.99f,
                0.88f + 0.05f
              ),
              "[Cur/Min/Max] "
              "FPS: ("+vsx_string_helper::f2s(delta_fps)+"/"+vsx_string_helper::f2s(min_fps)+"/"+vsx_string_helper::f2s(max_fps)+") "
              "Frame render time: ("+vsx_string_helper::f2s(frame_time)+"/"+vsx_string_helper::f2s(min_render_time)+"/"+vsx_string_helper::f2s(max_render_time)+") "
              "Ctrl+T to reset"
              ,0.025f
            );
          }
          VSXP_S_END
        }
        if (desktop && desktop->performance_mode)
        {
          glClear(GL_DEPTH_BUFFER_BIT);
          desktop->init_frame();
          desktop->draw();
          desktop->draw_2d();
        }
      }
      #ifndef NO_INTRO
        intro->draw();
      #endif
      vsx_command_process_garbage();
      if (first)
      {
        vxe->start();
        load_desktop_a();
        first = false;
      }
    } else
    {
      int zz = (int)((f_wait-gui_f_time)*1000.0f);
      if (zz < 0)
        zz = 0;
    }
    //------------------------------------------------------------------
    // movie recording
    //------------------------------------------------------------------
    if (record_movie)
    {
      unsigned char* pixeldata = (unsigned char*)malloc( vsx_gl_state::get_instance()->viewport_get_height() * vsx_gl_state::get_instance()->viewport_get_width() * 3 );
      unsigned char* pixeldata_flipped = (unsigned char*)malloc( vsx_gl_state::get_instance()->viewport_get_height() * vsx_gl_state::get_instance()->viewport_get_width() * 3 );
      take_screenshot = false;
      glReadPixels(0, 0, vsx_gl_state::get_instance()->viewport_get_width(), vsx_gl_state::get_instance()->viewport_get_height(),GL_RGB,GL_UNSIGNED_BYTE, (GLvoid*)pixeldata);
      int x3 = vsx_gl_state::get_instance()->viewport_get_width() * 3;
      int hi = vsx_gl_state::get_instance()->viewport_get_height();
      for (int y = 0; y < hi; y++)
        for (int x = 0; x < x3; x++)
          pixeldata_flipped[y*x3+x] = pixeldata[ (hi-y)*x3+x];

      vsx_bitmap bitmap;
      bitmap.width  = vsx_gl_state::get_instance()->viewport_get_width();
      bitmap.height = vsx_gl_state::get_instance()->viewport_get_height();
      bitmap.data_set(pixeldata_flipped, 0, 0, 3 * bitmap.width * bitmap.height);

      char mfilename[32];
      sprintf(mfilename, "%05d", movie_frame_count);
      vsx_bitmap_writer::write(&bitmap,
        vsx_data_path::get_instance()->data_path_get() + "videos" + DIRECTORY_SEPARATOR +
        vsx_string<>(mfilename)+"_" +
        vsx_string_helper::i2s(vsx_gl_state::get_instance()->viewport_get_width()) + "_" + vsx_string_helper::i2s(vsx_gl_state::get_instance()->viewport_get_height()) +
        ".tga"
      );
      free(pixeldata);
      free(pixeldata_flipped);
      movie_frame_count++;
    }

    //------------------------------------------------------------------
    // screenshots
    //------------------------------------------------------------------

    if (take_screenshot)
    {
      unsigned char* pixeldata = (unsigned char*)malloc( vsx_gl_state::get_instance()->viewport_get_height() * vsx_gl_state::get_instance()->viewport_get_width() * 3 );
      unsigned char* pixeldata_flipped = (unsigned char*)malloc( vsx_gl_state::get_instance()->viewport_get_height() * vsx_gl_state::get_instance()->viewport_get_width() * 3 );
      take_screenshot = false;
      glReadPixels(0, 0, vsx_gl_state::get_instance()->viewport_get_width(), vsx_gl_state::get_instance()->viewport_get_height(),GL_RGB,GL_UNSIGNED_BYTE, (GLvoid*)pixeldata);

      int x3 = vsx_gl_state::get_instance()->viewport_get_width() * 3;
      int hi = vsx_gl_state::get_instance()->viewport_get_height();
      for (int y = 0; y < hi; y++)
        for (int x = 0; x < x3; x++)
          pixeldata_flipped[y*x3+x] = pixeldata[ (hi-y)*x3+x];

      vsx_bitmap bitmap;
      bitmap.width  = vsx_gl_state::get_instance()->viewport_get_width();
      bitmap.height = vsx_gl_state::get_instance()->viewport_get_height();
      bitmap.data_set(pixeldata_flipped, 0, 0, 3 * bitmap.width * bitmap.height);

      vsx_bitmap_writer::write( &bitmap,
        vsx_data_path::get_instance()->data_path_get() + "screenshots" + DIRECTORY_SEPARATOR +
        vsx_string_helper::i2s(time(0x0)) +"_"+
        vsx_string_helper::i2s(vsx_gl_state::get_instance()->viewport_get_width()) + "_" + vsx_string_helper::i2s(vsx_gl_state::get_instance()->viewport_get_height()) +
        ".tga"
      );
      free(pixeldata);
      free(pixeldata_flipped);
    }
    VSXP_S_END
  }

  bool is_engine_fullscreen()
  {
    return
      !desktop->performance_mode
      &&
      *gui_prod_fullwindow;
  }

  void key_down_event(long scancode)
  {
    if (vsx_input_keyboard.pressed_alt() && vsx_input_keyboard.pressed_ctrl() && vsx_input_keyboard.pressed_shift() && scancode == VSX_SCANCODE_P)
    {
      if (record_movie == false)
        movie_frame_count = 0;
      record_movie = !record_movie;
      return;
    }

    if (vsx_input_keyboard.pressed_alt() && vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift() && scancode == VSX_SCANCODE_P)
      take_screenshot = true;

    if (*gui_prod_fullwindow && vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift() && scancode == VSX_SCANCODE_T)
      gui_prod_fullwindow_helptext = !gui_prod_fullwindow_helptext;

    if (*gui_prod_fullwindow && !vsx_input_keyboard.pressed_alt() && vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift() && scancode == VSX_SCANCODE_T)
      reset_time_measurements = true;
  }
};
