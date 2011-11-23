#ifndef INFO_OVERLAY_H_
#define INFO_OVERLAY_H_

#include "vsx_font.h"
#include "vsx_manager.h"
#include "vsx_timer.h"
#include "vsxfst.h"
#include <logo_intro.h>


class vsx_overlay {
	vsx_timer time2;
	vsx_font* myf;
	unsigned long frame_counter;
	unsigned long delta_frame_counter;
	float delta_frame_time;
	float delta_fps;
	float total_time;
	float title_timer;	
  float show_randomizer_timer;
	float scroll_pos;
	float dt;
	float intro_timer;
  vsx_manager_abs* manager;
  int help_id;
  float fx_alpha;
  bool first;
  vsx_logo_intro* intro;
	
public:
	
	vsx_overlay();
  void set_manager(vsx_manager_abs* new_manager);
	void reinit();
	void render();
  void set_help(int id);
  void print_help();
	void show_fx_graph();
  void show_randomizer_status();
};

#endif /*INFO_OVERLAY_H_*/
