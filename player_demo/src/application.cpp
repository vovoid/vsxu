#define PLATFORM_SHARED_FILES_STL
#include <string>
#include <stdlib.h>
// vsxu includes
#include <vsx_version.h>
#include <vsx_platform.h>
#include <vsx_engine.h>
#include "logo_intro.h"

// local includes
#include "application.h"

// info_overlay is the text on top of the graphics
// you might want to remove this.


bool first = true;

/* uncomment for manual sound injection
float sound_wave_test[513];
float sound_freq_test[513];
*/

void (*app_set_fullscreen)(int,bool) = 0;
bool (*app_get_fullscreen)(int) = 0;

void app_init(int id)
{
  VSX_UNUSED(id);
}

void app_pre_draw()
{
}

vsx_engine* vxe_local = 0;
vsx_command_list cmd_in, cmd_out;

vsx_logo_intro* intro;

bool app_draw(int id)
{
  VSX_UNUSED(id);
	if (first)
	{
		first = false;
    intro = new vsx_logo_intro;
    vxe_local = new vsx_engine("");
    vxe_local->set_no_send_client_time(true);
    vxe_local->start();
    vsx_string<>path = PLATFORM_SHARED_FILES;
    vxe_local->load_state(path+"example-prods/vovoid-luna-reactivation.vsx");
  }
  
  vxe_local->process_message_queue(&cmd_in,&cmd_out);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  vxe_local->render();
  if
  (
      vxe_local->get_engine_state() == VSX_ENGINE_LOADING
      &&
      vxe_local->get_engine_info()->vtime < 1.0f
  )
  intro->draw(true,false);
  else
  intro->draw(false,false);
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
      exit(0);
	}
}

void app_key_up(long key)
{
  VSX_UNUSED(key);
}
void app_mouse_move_passive(int x, int y)
{
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_mouse_move(int x, int y)
{
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_mouse_down(unsigned long button,int x,int y)
{
  VSX_UNUSED(button);
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_mouse_up(unsigned long button,int x,int y)
{
  VSX_UNUSED(button);
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_mousewheel(float diff,int x,int y)
{
  VSX_UNUSED(diff);
  VSX_UNUSED(x);
  VSX_UNUSED(y);
}
void app_char(long key)
{
  VSX_UNUSED(key);
}

