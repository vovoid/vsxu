#ifndef VSX_APPLICATION_H
#define VSX_APPLICATION_H

#include <sys/time.h>


//extern vsx_engine* vme;
//#define vsxu_version "VSXu 0.1.14 beta"
//extern vsx_string vsxu_name;
//extern vsx_string vsxu_version;
//extern char* vsxu_ver;


extern bool app_ctrl;
extern bool app_alt;
extern bool app_shift;
extern bool dual_monitor;
extern bool disable_randomizer;
extern bool option_preload_all;
extern bool no_overlay;

extern int app_argc;
extern char** app_argv;

//extern int app_num_displays; // 0 means the primary one used, 1 the extra
extern void (*app_set_fullscreen)(int,bool);
extern bool (*app_get_fullscreen)(int);



void app_init(int id); // which context is used?

void app_pre_draw();

bool app_draw(int id);

void app_char(long key);
void app_key_down(long key);
void app_key_up(long key);

void app_mouse_move_passive(int x, int y);
void app_mouse_move(int x, int y);

// buttons: 0 = left, 1 = middle, 2 = right
void app_mouse_down(unsigned long button,int x,int y);
void app_mouse_up(unsigned long button,int x,int y);

// -1 to -5 or whatever up to +1
void app_mousewheel(float diff,int x,int y);

#endif
