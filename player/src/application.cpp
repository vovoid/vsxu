#include <vsx_version.h>
#include <vsx_manager.h>
#include <stdlib.h>

#define NO_INTRO
#include "logo_intro.h"

#include "application.h"
#include "info_overlay.h"


// info_overlay is the text on top of the graphics
// you might want to remove this.
 

// global vars
vsx_string fpsstring = "VSX Ultra "+vsx_string(vsxu_version)+" - (c) Vovoid";
vsx_manager_abs* manager;
vsx_logo_intro* intro;

void (*app_set_fullscreen)(int,bool) = 0;
bool (*app_get_fullscreen)(int) = 0;

vsx_overlay* overlay = 0;

void app_init(int id) 
{

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

}


void app_pre_draw() {}

bool first = true;
bool app_draw(int id)
{
	if (first)
	{
		//intro = new vsx_logo_intro;
		first = false;
    manager = manager_factory();
    vsx_string path = PLATFORM_SHARED_FILES;
    manager->init( path.c_str() );
    overlay = new vsx_overlay;
    overlay->set_manager(manager);
    intro = new vsx_logo_intro;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (manager) manager->render();
	if (overlay) overlay->render();
	intro->draw();
	return true;
}

void app_key_down(long key) 
{
	char key_out[100];
	sprintf(key_out, "%d", (int)key);
	//::MessageBox(0,key_out, key_out, MB_OK);
#ifdef VSXU_DEBUG
	printf("key: %d\n", (int)key);
#endif
	switch (key) 
  {
	  case 257:
		case 27: exit(0);
		case 298:
		case 33: manager->inc_speed(); break;
		case 299:
		case 34: manager->dec_speed(); break;
		case 283:
    case 38: manager->inc_fx_level(); overlay->show_fx_graph(); break;
		case 284:
    case 40: manager->dec_fx_level(); overlay->show_fx_graph(); break;
		case 285:
		case 37: manager->prev_visual(); break;
		case 286:
		case 39: manager->next_visual(); break;
    case 258:overlay->set_help(1); break;
    case 112:
    case 70: overlay->set_help(2); break;
		case 82: if (app_ctrl) manager->pick_random_visual(); else manager->toggle_randomizer(); break;
	}
}

void app_key_up(long key) {}
void app_mouse_move_passive(int x, int y) {}
void app_mouse_move(int x, int y) {}
void app_mouse_down(unsigned long button,int x,int y) {}
void app_mouse_up(unsigned long button,int x,int y) {}
void app_mousewheel(float diff,int x,int y) {}
void app_char(long key) {}

