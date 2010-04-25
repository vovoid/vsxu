#include <vsx_version.h>
#include <vsx_manager.h>
#include <stdlib.h>

#define NO_INTRO
#include "logo_intro.h"

#include "application.h"
#include "info_overlay.h"

// global vars
vsx_string fpsstring = "VSX Ultra "+vsx_string(vsxu_version)+" - �2005 Vovoid";

vsx_manager_abs* manager;


void (*app_set_fullscreen)(int,bool) = 0;
bool (*app_get_fullscreen)(int) = 0;

//float global_time;

#ifndef VSXU_DEMO
vsx_overlay* overlay = 0;
#endif

void app_init(int id) {
#ifndef VSXU_DEMO
  /*for (int i = 1; i < app_argc; i++) {
    vsx_string arg1 = app_argv[i];
    if (arg1 == "--help")
    {
      printf(
          "Usage:\n"
          "  vsxu_player [path_to_vsx_file]\n"
          "\n"
          "Flags: \n"
          "  -dr   Disable randomizer     \n\n"
          "  -p [x,y] window position x,y \n\n"
          "  -s [x,y] window size x,y \n\n"
      );
      exit(0);
    } else
    if (arg1 == "-dr")
    {
      randomizer = false;
    } else
    if (arg1 == "-s" || arg1 == "-p") {
      if (i+1 < app_argc) i++;
    } else
    if (arg1[0] != '-') {
      state_file_list.push_back(arg1);
    }
  }*/
#endif

/*#ifdef VSXU_DEMO
	list->show_progress_bar = true;
#endif*/
#ifndef VSXU_DEMO
	///overlay = new vsx_overlay();
	///overlay->set_manager(manager);
#endif
}

#ifdef VSXU_DEMO
bool first = true;

int start_loaded_modules;

#endif

void app_pre_draw() {}

bool first = true;
bool app_draw(int id)
{
	if (first)
	{
    printf("app1\n");
		//intro = new vsx_logo_intro;
		first = false;
    printf("app2\n");
    manager = manager_factory();
    vsx_string path = PLATFORM_SHARED_FILES;
    printf("application_manager_init\n");
    manager->init( path.c_str() );
    printf("application_after_manager_init\n");
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (manager) manager->render();
	///if (overlay) overlay->render();
	//intro->draw();
	return true;
}

void app_key_down(long key) {
	char key_out[100];
	sprintf(key_out, "%d", (int)key);
	//::MessageBox(0,key_out, key_out, MB_OK);
	printf("key: %d\n", (int)key);
	switch (key) {
	  case 257:
		case 27: exit(0);
		//case 38: list->get_vxe()->start(); break;//list->inc_amp(); break;
		//case 40: list->get_vxe()->stop(); break;//list->dec_amp(); break;
#ifndef VSXU_DEMO
		case 298:
		case 33: manager->inc_speed(); break;
		case 299:
		case 34: manager->dec_speed(); break;
		case 283:
		case 38: manager->inc_fx_level(); break;
		case 284:
		case 40: manager->dec_fx_level(); break;
		case 285:
		case 37: manager->prev_visual(); break;
		case 286:
		case 39: manager->next_visual(); break;
		//case 13: manager->toggle_fullscreen(); overlay->reinit(); break;
		case 70: overlay->set_help(2); break;
		case 82: if (app_ctrl) manager->pick_random_visual(); else manager->toggle_randomizer(); break;
		///case 258:
		///case 112: overlay->set_help(1); break;
#endif
	}
	//printf("key down: %d\n",key);
}

void app_key_up(long key) {}
void app_mouse_move_passive(int x, int y) {}
void app_mouse_move(int x, int y) {}
void app_mouse_down(unsigned long button,int x,int y) {}
void app_mouse_up(unsigned long button,int x,int y) {}
void app_mousewheel(float diff,int x,int y) {}
void app_char(long key) {}

