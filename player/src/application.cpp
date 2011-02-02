#define PLATFORM_SHARED_FILES_STL
#include <string>
#include <stdlib.h>
// vsxu includes
#include <vsx_version.h>
#include <vsx_platform.h>
#include <vsx_manager.h>
#include <logo_intro.h>

// local includes
#include "application.h"

// info_overlay is the text on top of the graphics
// you might want to remove this.
#include "vsx_overlay.h"

vsx_manager_abs* manager;

vsx_logo_intro* intro;

vsx_overlay* overlay = 0;
bool first = true;

/* uncomment for manual sound injection
float sound_wave_test[513];
float sound_freq_test[513];
*/

void (*app_set_fullscreen)(int,bool) = 0;
bool (*app_get_fullscreen)(int) = 0;

void app_init(int id) {}
void app_pre_draw() {}

bool app_draw(int id)
{
	if (first)
	{
		first = false;
    // create a new manager
    manager = manager_factory();
    std::string path = PLATFORM_SHARED_FILES_STLSTRING;
    // init manager with the shared path and sound input type.
    // manual sound injection: manager->init( path.c_str() , "media_player");
    manager->init( path.c_str() , "pulseaudio");
    // create a new text overlay
    overlay = new vsx_overlay;
    overlay->set_manager(manager);
    // create a new intro (Luna logo) object
    intro = new vsx_logo_intro;
    if (disable_randomizer) manager->set_randomizer(false);
	}

/* uncomment for manual sound injection
  for (unsigned long i = 0; i < 512; i++)
  {
    sound_wave_test[i] = (float)(rand()%65535-32768)*(1.0f/32768.0f);
  }
  for (unsigned long i = 0; i < 512; i++)
  {
    sound_freq_test[i] = (float)(rand()%65535)*(1.0f/65535.0f);
  }
  manager->set_sound_freq(&sound_freq_test[0]);
  manager->set_sound_wave(&sound_wave_test[0]);
  */
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (manager) manager->render();
	if (overlay) overlay->render();
	intro->draw();
	return true;
}

void app_key_down(long key) 
{
  #ifdef VSXU_DEBUG
  	printf("key: %d\n", (int)key);
  #endif
  switch (key) 
  {
    // esc:
	  case 257:
		case 27:
      if (manager) manager_destroy(manager);
      exit(0);
    // pgup:
		case 298:
		case 33: 
      manager->inc_speed(); 
      break;
    // pgdn:
		case 299:
		case 34: 
      manager->dec_speed(); 
      break;
    // arrow up:
		case 283:
    case 38: 
      manager->inc_fx_level(); overlay->show_fx_graph(); 
      break;
    // arrow down:
		case 284:
    case 40: 
      manager->dec_fx_level(); overlay->show_fx_graph(); 
      break;
    // arrow left:
		case 285:
		case 37: 
      manager->prev_visual(); 
      break;
    // arrow right:
		case 286:
		case 39: 
      manager->next_visual(); 
      break;
    // "F1" key:
    case 258:
      overlay->set_help(1); 
      break;
    // "F" key:
    case 112:
    case 70: 
      overlay->set_help(2); 
      break;
    // "R" key:
		case 82:
      if (app_ctrl) 
      {
        manager->pick_random_visual(); 
      }
      else 
      {
        manager->toggle_randomizer(); 
        overlay->show_randomizer_status();
      }
      break;
	}
}

void app_key_up(long key) {}
void app_mouse_move_passive(int x, int y) {}
void app_mouse_move(int x, int y) {}
void app_mouse_down(unsigned long button,int x,int y) {}
void app_mouse_up(unsigned long button,int x,int y) {}
void app_mousewheel(float diff,int x,int y) {}
void app_char(long key) {}

