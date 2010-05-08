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

#include "log/vsx_log_a.h"

//#include "../resource.h"

#ifndef VSX_NO_CLIENT
#include "vsx_widget/vsx_widget_base.h"
#include "vsx_widget/window/vsx_widget_window.h"
#include "vsx_widget/lib/vsx_widget_lib.h"
#include "vsx_widget/vsx_widget_main.h"
#include "vsx_widget/server/vsx_widget_server.h"
#include "vsx_widget/module_choosers/vsx_widget_module_chooser.h"
#include "vsx_widget/helpers/vsx_widget_object_inspector.h"
#include "vsx_widget/helpers/vsx_widget_preview.h"
#endif
#include "logo_intro.h"
//#define NO_INTRO
#include "application.h"

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <time.h>
#endif


// global vars
vsx_string fpsstring = "VSX Ultra "+vsx_string(vsxu_version)+" - 2010 Vovoid";
vsx_engine* vxe = 0;

// from the perspective (both for gui/server) from here towards the tcp thread
vsx_command_list system_command_queue;
vsx_command_list internal_cmd_in;
vsx_command_list internal_cmd_out;
vsx_widget_desktop *desktop = 0;
bool prod_fullwindow = false;
bool take_screenshot = false;
bool *gui_prod_fullwindow;
vsx_font myf;

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

	//printf("vxe: %d\n", vxe);
  //printf("{CLIENT} Creating engine\n");
  vxe->init();

  //printf("{CLIENT} Starting engine:");
  vxe->start();
}


void load_desktop_a(vsx_string state_name = "") {
  //printf("{CLIENT} creating desktop:");
  desktop = new vsx_widget_desktop;
  //printf(" [DONE]\n");
  internal_cmd_in.clear();
  internal_cmd_out.clear();
  ((vsx_widget_server*)desktop->f("desktop_local"))->cmd_in = &internal_cmd_out;
  ((vsx_widget_server*)desktop->f("desktop_local"))->cmd_out = &internal_cmd_in;
  if (state_name != "") ((vsx_widget_server*)desktop->f("desktop_local"))->state_name = str_replace("/",";",str_replace("//",";",str_replace("_states/","",state_name)));
  internal_cmd_out.add_raw(vsx_string("vsxu_welcome ")+vsx_string(vsxu_ver)+" 0");
  desktop->system_command_queue = &system_command_queue;
  vsx_widget* t_viewer = desktop->f("vsxu_preview");
  if (t_viewer)
  {
  	gui_prod_fullwindow = &((vsx_window_texture_viewer*)desktop->f("vsxu_preview"))->fullwindow;
  	LOG_A("found vsxu_preview widget")
  }
  if (!dual_monitor)
  ((vsx_widget_server*)desktop->f("desktop_local"))->engine = (void*)vxe;
  desktop->front(desktop->f("vsxu_preview"));

  //printf("{CLIENT} starting desktop:");
  desktop->init();
  //printf(" [DONE]\n");
}


void app_init(int id) {
	if (dual_monitor && id == 0) return;
	//if (!dual_monitor && id == 0) return;

	vsx_string own_path;
  vsx_avector<vsx_string> parts;
  vsx_avector<vsx_string> parts2;
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  vsx_string deli = "/";
#else
  vsx_string deli = "\\";
#endif
  own_path = app_argv[0];
  //printf("own_path: %s\n", own_path.c_str() );
  explode(own_path, deli, parts);
  for (unsigned long i = 0; i < parts.size()-1; ++i) {
    parts2.push_back(parts[i]);
  }
  own_path = implode(parts2,deli);
  if (own_path.size()) own_path.push_back('\\');

	//printf("argc: %d %s\n",app_argc,own_path.c_str());
	vxe = new vsx_engine(own_path.c_str());
	gui_prod_fullwindow = &prod_fullwindow;
//	if (!dual_monitor && id == 1) {
  myf.init(PLATFORM_SHARED_FILES+"font/font-ascii_output.png");
	if (dual_monitor) {
		init_vxe();
	}
}


void app_pre_draw() {
	//printf("app_pre_draw\n");

  vsx_command_s *c = 0;
  while (c = system_command_queue.pop())
  {
  	//printf("c->cmd: %s\n",c->cmd.c_str());
    vsx_string cmd = c->cmd;
    vsx_string cmd_data = c->cmd_data;
    if (cmd == "fullscreen_toggle") {
    } else
    if (cmd == "fullscreen") {
      if (desktop)
      desktop->stop();
      internal_cmd_in.addc(c);
    }
		c = 0;
  }

}

// id is 0 for first monitor, 1 for the next etc.

// draw-related variables
class vsxu_draw {
public:
	bool first;
	vsx_string current_fps;
	vsx_timer gui_t;
	int frame_count;
	float gui_g_time;
	double dt;
	double gui_f_time;
	double gui_fullscreen_fpstimer;
	vsx_command_s pgv;
	vsx_logo_intro *intro;
	vsxu_draw() : first(true),frame_count(0), gui_g_time(0), gui_f_time(0), gui_fullscreen_fpstimer(0)
	{};

	void draw() {
		if (desktop)
  	desktop->vsx_command_process_f();

		if (first) {
	    //#ifndef NO_INTRO
	    intro = new vsx_logo_intro;
	    //#endif
	    //printf("creating intro\n");
	  }
	  dt = gui_t.dtime();
		gui_f_time += dt;
		gui_g_time += dt;

		float f_wait;
		bool run_always = false;
	#ifndef VSX_NO_CLIENT
	  if (desktop) {
	    if (desktop->global_framerate_limit == -1) run_always = true;
	    else
	    f_wait = 1.0f/desktop->global_framerate_limit;
	  }
		else
	#endif
		f_wait = 1.0f/100.0f;
	  if (run_always || gui_f_time > f_wait)
	  {
	  ++frame_count;

	#ifndef VSX_NO_CLIENT
	    if (desktop) {
	    	desktop->dtime = gui_f_time;
	    	desktop->time += desktop->dtime;
	    	desktop->frames = frame_count;
	    }
	#endif
	    gui_fullscreen_fpstimer += gui_f_time;
	      current_fps = f2s(round(1.0f/gui_f_time),2);
	    if (gui_fullscreen_fpstimer > 1) {
	      vsx_string h = fpsstring + " @ "+ current_fps+ "fps";
	      gui_fullscreen_fpstimer = 0;
	    }

	  	gui_f_time = 0;
	#ifndef VSX_NO_CLIENT

	    if (!*gui_prod_fullwindow)
	    {
	      if (desktop) {
	        desktop->init_frame();
	      	desktop->draw();
	      	desktop->draw_2d();
	      }
	    } else {
	    }
	    if (!dual_monitor)
	    vxe->process_message_queue(&internal_cmd_in,&internal_cmd_out);
	    if (*gui_prod_fullwindow) {
	#endif
	      glDepthMask(GL_TRUE);
				glClearColor(0.0f,0.0f,0.0f,1.0f);

	      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	    
#ifndef NO_INTRO
	    if (vxe->e_state == VSX_ENGINE_STOPPED)
	    {
	      intro->draw(true);
	    }
	    #endif
	      if (vxe && !dual_monitor) {
	    		vxe->render();
			glDisable(GL_DEPTH_TEST);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();											// Reset The Modelview Matrix
 			glEnable(GL_BLEND);
 			++frame_counter;
 			++delta_frame_counter;
 			//float dt = time2.dtime();
 			delta_frame_time+= dt;
 			total_time += dt;
 			if (delta_frame_counter == 100) {
 				delta_fps = 100.0f/delta_frame_time;
 				delta_frame_counter = 0;
 				delta_frame_time = 0.0f;
 			}
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0,0,0,0.4f);
			glBegin(GL_QUADS);									// Draw A Quad
				glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
				glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
				glVertex3f( 1.0f,0.92f, 0.0f);					// Bottom Right
				glVertex3f(-1.0f,0.92f, 0.0f);					// Bottom Left
			glEnd();											// Done Drawing The Quad
			myf.print(vsx_vector(-1.0f,0.92f)," Fc "+i2s(frame_counter)+" Fps "+f2s(delta_fps)+" T "+f2s(total_time)+" Tfps "+f2s(frame_counter/total_time)+" MC "+i2s(vxe->get_num_modules())+" VSX Ultra (c) 2003-2010 Vovoid",0.05f);
	      }
	      if (desktop && desktop->performance_mode) {
	      	glClear(GL_DEPTH_BUFFER_BIT);
	        desktop->init_frame();
	      	desktop->draw();
	      	desktop->draw_2d();
	      }
	    }
	    #ifndef NO_INTRO
	    //intro->window_width = window_width;
	    //intro->window_height = window_height;
	    #endif
	#ifndef VSX_DEMO
	      //vsx_mouse mouse;
	      //mouse.hide_cursor();
	    #ifndef NO_INTRO
	    intro->draw();
	    #endif
	#else
	    if (vxe->e_state == VSX_ENGINE_STOPPED)
	    {
	      #ifndef NO_INTRO
	      intro->draw(true);
	      #endif
	    }
	    ShowCursor(false);
	#endif
	    if (!first)
	#ifndef VSX_NO_CLIENT
	    if (!desktop)
	#endif

	    pgv.iterations = -1;
	    pgv.process_garbage();
	    if (first) {
	    	if (!dual_monitor) init_vxe();
	      load_desktop_a();
	      first = false;
	    }
	  } else {
	    int zz = (int)((f_wait-gui_f_time)*1000.0f);
	    if (zz < 0) zz = 0;
	    //printf("zz%d ",zz);
	//    Sleep(zz);
	  }
		if (take_screenshot) {
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			char* pixeldata = (char*)malloc( viewport[2] * viewport[3] * 3 );
			take_screenshot = false;
			glReadPixels(0,viewport[3],viewport[2], 0,GL_RGB,GL_UNSIGNED_BYTE, (GLvoid*)pixeldata);
			char filename[256];
      #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
      sprintf(filename, "%d_%d_%d.raw",(int)time(0),viewport[2],viewport[3]);
      #else
			sprintf(filename, "%d_%d_%d.raw",rand(),viewport[2],viewport[3]);
      #endif
			FILE* fp = fopen(filename,"wb");
			fwrite(pixeldata, 1, viewport[2] * viewport[3] * 3, fp);
			fclose(fp);
			printf("took screenshot to: %s\n",filename);
		}
	}
};

vsxu_draw my_draw;


bool app_draw(int id)
{
	if (id == 0) {
		my_draw.draw();
	} else
	{
		if (dual_monitor) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
			vxe->process_message_queue(&internal_cmd_in,&internal_cmd_out);
			vxe->render();
			glDisable(GL_DEPTH_TEST);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();											// Reset The Modelview Matrix
 			glEnable(GL_BLEND);
 			++frame_counter;
 			++delta_frame_counter;
 			float dt = time2.dtime();
 			delta_frame_time+= dt;
 			total_time += dt;
 			if (delta_frame_counter == 100) {
 				delta_fps = 100.0f/delta_frame_time;
 				delta_frame_counter = 0;
 				delta_frame_time = 0.0f;
 			}
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0,0,0,0.4f);
			glBegin(GL_QUADS);									// Draw A Quad
				glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
				glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
				glVertex3f( 1.0f,0.92f, 0.0f);					// Bottom Right
				glVertex3f(-1.0f,0.92f, 0.0f);					// Bottom Left
			glEnd();											// Done Drawing The Quad
			myf.print(vsx_vector(-1.0f,0.92f)," Fc "+i2s(frame_counter)+" Fps "+f2s(delta_fps)+" T "+f2s(total_time)+" Tfps "+f2s(frame_counter/total_time)+" MC "+i2s(vxe->get_num_modules())+" VSX Ultra (c) Vovoid",0.07);
		}
//		 else {
//			printf("was");
//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
//			glLoadIdentity();									// Reset The Current Modelview Matrix
//			glTranslatef(0.0f,0.0f,-6.0f);						// Move Right 3 Units
//	  	glRotatef(sin(global_time*2)*360.0f,0,0,1);
//	  	if ( id == 0) {
//		    glColor4f(1.0,0.0,1.0,1.0);
//		  } else {
//		    glColor4f(0.0,0.0,1.0,1.0);
//		  }
//			glBegin(GL_QUADS);									// Draw A Quad
//				glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
//				glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
//				glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
//				glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
//			glEnd();											// Done Drawing The Quad
//		  global_time = time2.atime();
//		}
	}
	return true;
}

void app_char(long key) {
  if (desktop) {
    desktop->key_down(key,app_alt, app_ctrl, app_shift);
  }
}

void app_key_down(long key) {
  if (desktop) {
  	if (app_alt && app_ctrl && key == 80)
  	take_screenshot = true;
    desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
#ifdef __APPLE__
    desktop->key_down(key,app_alt, app_ctrl, app_shift);
#else
    desktop->key_down(-key,app_alt, app_ctrl, app_shift);
#endif
  }
}

void app_key_up(long key) {
  //if (desktop)
  if (desktop) {
  	//printf("key up %d\n",key);
  	desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
  	desktop->key_up(key,app_alt, app_ctrl, app_shift);
  }
  //printf("key up: %d\n",key);
}

void app_mouse_move_passive(int x, int y) {
	//if (gui_prod_fullwindow)
  //if (*gui_prod_fullwindow) return;
  if (desktop) {
  	desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
  	desktop->mouse_move_passive(x,y);
  }
  //printf("mouse passive pos: %d :: %d\n",x,y);
}

void app_mouse_move(int x, int y) {
  //if (*gui_prod_fullwindow) return;

  GLint	viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  int xx = x;
  int yy = y;
  if (xx < 0) xx = 0;
  if (yy < 0) yy = 0;
  if (xx > viewport[2]) xx = viewport[2]-1;
  if (yy > viewport[3]) yy = viewport[3]-1;
#ifndef VSX_NO_CLIENT
  if (desktop) {
		desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
	  desktop->mouse_move(xx,yy);
  }
#endif
  //printf("mouse pos: %d :: %d\n",x,y);
}

void app_mouse_down(unsigned long button,int x,int y)
{
	if (desktop) {
	desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
	desktop->mouse_down(x,y,button);
	//printf("is alt down: %d\n",app_alt);
	}
  //printf("button %d pressed at %d, %d\n",(int)button,(int)x,(int)y);
}

void app_mouse_up(unsigned long button,int x,int y)
{
	if (desktop) {
		desktop->set_key_modifiers(app_alt, app_ctrl, app_shift);
		desktop->mouse_up(x,y,button);
	}
  //printf("button %d depressed at %d, %d\n",(int)button,(int)x,(int)y);
}

void app_mousewheel(float diff,int x,int y)
{
	if (desktop)
	{
		desktop->mouse_wheel(diff);
	}
  //printf("mousewheel diff: %f\n",diff);
}

