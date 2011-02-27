#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vsx_gl_global.h"
#include "pthread.h"
#include "vsx_command.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_timer.h"
#include "vsx_math_3d.h"
#include "vsxfst.h"
#include "vsx_font.h"
#include <vsx_version.h>
#include "vsx_engine.h"


#include "application.h"
#include "vsxg.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <time.h>
#endif

#include "vsx_command_client_server.h"

vsx_command_list_server cl_server;
// global vars
vsx_string fpsstring = "VSX Ultra "+vsx_string(vsxu_version)+" - 2010 Vovoid";
vsx_engine* vxe = 0;

// from the perspective (both for gui/server) from here towards the tcp thread
vsx_command_list internal_cmd_in;
vsx_command_list internal_cmd_out;
bool prod_fullwindow = false;
bool take_screenshot = false;
bool record_movie = false;
bool *gui_prod_fullwindow;
bool gui_prod_fullwindow_helptext = true;

unsigned long frame_counter = 0;
unsigned long delta_frame_counter = 0;
float delta_frame_time = 0.0f;
float delta_fps;
float total_time = 0.0f;

void (*app_set_fullscreen)(int,bool) = 0;
bool (*app_get_fullscreen)(int) = 0;

float global_time;
vsx_timer time2;

void init_vxe() {
  printf("starting engine..\n");
  vxe->init();
  vxe->start();
}

void load_desktop_a(vsx_string state_name = "") {
  internal_cmd_in.clear();
  internal_cmd_out.clear();
//  internal_cmd_out.add_raw(vsx_string("vsxu_welcome ")+vsx_string(vsxu_ver)+" 0");
}


void app_init(int id) {
	vsx_string own_path;
  vsx_avector<vsx_string> parts;
  vsx_avector<vsx_string> parts2;
  vsx_string deli = "/";
  own_path = app_argv[0];
  //printf("own_path: %s\n", own_path.c_str() );
  explode(own_path, deli, parts);
  for (unsigned long i = 0; i < parts.size()-1; ++i) {
    parts2.push_back(parts[i]);
  }
  own_path = implode(parts2,deli);
  if (own_path.size()) own_path.push_back(deli[0]);

  //#ifdef VSXU_DEBUG
  printf("own path: %s   \n", own_path.c_str() );
  //#endif
	//printf("argc: %d %s\n",app_argc,own_path.c_str());
	vxe = new vsx_engine(own_path.c_str());
  vsx_es_shader_wrapper_init_shaders();
//  myf.init(PLATFORM_SHARED_FILES+"font/font-ascii_output.png");
}


void app_pre_draw() {
}

// id is 0 for first monitor, 1 for the next etc.

// draw-related variables
class vsxu_draw {
public:
	bool first;
	vsx_string current_fps;
	vsx_timer gui_t;
	int frame_count;
  int movie_frame_count;
	float gui_g_time;
	double dt;
	double gui_f_time;
	double gui_fullscreen_fpstimer;
	vsx_command_s pgv;
	vsxu_draw() : first(true),frame_count(0), gui_g_time(0), gui_f_time(0), gui_fullscreen_fpstimer(0)
	{};

	void draw() {
	  dt = gui_t.dtime();
		gui_f_time += dt;
		gui_g_time += dt;

		float f_wait;
		bool run_always = false;
		f_wait = 1.0f/100.0f;
	  if (run_always || gui_f_time > f_wait)
	  {

      if (first) {
        init_vxe();
        cl_server.set_command_lists(&internal_cmd_in,&internal_cmd_out);
        cl_server.start();
        first = false;
      }
      
      ++frame_count;

	    gui_fullscreen_fpstimer += gui_f_time;
	      current_fps = f2s(round(1.0f/gui_f_time),2);
	    if (gui_fullscreen_fpstimer > 1) {
	      vsx_string h = fpsstring + " @ "+ current_fps+ "fps";
	      gui_fullscreen_fpstimer = 0;
	    }

	  	gui_f_time = 0;

      glDepthMask(GL_TRUE);
      glClearColor(0.0f,0.0f,0.0f,1.0f);

      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	    
      if (vxe) {
        vxe->process_message_queue(&internal_cmd_in,&internal_cmd_out);
        vxe->render();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();											// Reset The Modelview Matrix
        glEnable(GL_BLEND);
        ++frame_counter;
        ++delta_frame_counter;
      }
    }
    pgv.iterations = -1;
    pgv.process_garbage();
	}
};

vsxu_draw my_draw;


bool app_draw(int id)
{
  my_draw.draw();
	return true;
}

void app_char(long key) {
}

void app_key_down(long key) {
}

void app_key_up(long key) {
}

void app_mouse_move_passive(int x, int y) {
}

void app_mouse_move(int x, int y) {
}

void app_mouse_down(unsigned long button,int x,int y)
{
}

void app_mouse_up(unsigned long button,int x,int y)
{
}

void app_mousewheel(float diff,int x,int y)
{
}

