/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_math_3d.h"
#include <vsx_platform.h>
#if PLATFORM == PLATFORM_LINUX
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vsx_platform.h>
#include <linux/joystick.h>
#define JOY_DEV "/dev/input/js"
#define JOY_DEV1 "/dev/input/js0"
#define JOY_DEV2 "/dev/input/js1"

#endif

//-----------------------------------------------------------------------
//---- SHUTDOWN ---------------------------------------------------------
//-----------------------------------------------------------------------

class vsx_module_system_shutdown : public vsx_module {
  // in
	vsx_module_param_float* float_in;
	// out
	vsx_module_param_render* render_out;
	// internal
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "system;shutdown";
  info->description = "If the shutdown value is above 1.0\n\
VSXu is shut down. This is\n\
mainly intended to end automatic\n\
presentations such as demos etc.";
  info->out_param_spec = "render_out:render";
  info->in_param_spec = "shutdown:float";
  info->component_class = "system";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
	float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"shutdown");
	float_in->set(0);
	render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  //--------------------------------------------------------------------------------------------------	
}

void run() {
  if (float_in->get() > 1.0) exit(0);
  render_out->set(1);
}

};


//-----------------------------------------------------------------------
//---- SHUTDOWN ---------------------------------------------------------
//-----------------------------------------------------------------------

class vsx_module_system_trig_sequencer : public vsx_module {
  // in
  vsx_module_param_float* trig_play;
  vsx_module_param_float* trig_stop;
  vsx_module_param_float* trig_rewind;
  // internal
  float prev_play;
  float prev_stop;
  float prev_rewind;
public:

void module_info(vsx_module_info* info)
{
  info->output = 1;
  info->identifier = "system;system_sequencer_control";
  info->description = "";
  info->out_param_spec = "";
  info->in_param_spec = "trig_play:float,trig_stop:float,trig_rewind:float";
  info->component_class = "system";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  trig_play = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trig_play");
  trig_stop = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trig_stop");
  trig_rewind = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trig_rewind");
  //--------------------------------------------------------------------------------------------------  
}

void run() {
  if (prev_play == 0.0f && trig_play->get() > 0.0f) {
    engine->request_play = 1;
  }
  prev_play = trig_play->get();
  
  if (prev_stop == 0.0f && trig_stop->get() > 0.0f) {
    engine->request_stop = 1;
  }
  prev_stop = trig_stop->get();

  if (prev_rewind == 0.0f && trig_rewind->get() > 0.0f) {
    engine->request_rewind = 1;
  }
  prev_rewind = trig_rewind->get();
  
}

};


//#define _linux
//-----------------------------------------------------------------------
//---- JOYSTICK ---------------------------------------------------------
//-----------------------------------------------------------------------
#if (PLATFORM == PLATFORM_LINUX)
struct js_event js;

class joystick_info {
public:
  vsx_avector<vsx_module_param_float*> axes;
  vsx_avector<vsx_module_param_float*> buttons;
  vsx_module_param_string* name;

  int joy_fd;
  int *axis;
  int num_of_axis;
  int num_of_buttons;
  int x;
  char *button, name_of_joystick[80];
  joystick_info() {
    joy_fd = -2;
    num_of_buttons = 0;
    num_of_axis = 0;
  }
};

class vsx_module_system_joystick : public vsx_module {
  // in
  //vsx_module_param_float* float_in;
  // out
  //vsx_module_param_render* render_out;
//  vsx_avector<vsx_module_param_float*> axes;
//  vsx_avector<vsx_module_param_float*> buttons;
  // internal
//  int joy_fd, *axis, num_of_axis, num_of_buttons, x;
//  char *button, name_of_joystick[80];

  vsx_avector<joystick_info> joysticks;
  int joystick_count;
public:
  
  vsx_module_system_joystick() {
    joystick_count = 0;
  }
  
  bool init_joysticks()
  {
//    printf("start init joysticks\n");
    for (int i = 0; i < 10; i++)
    {
      joystick_info ji;
      if (joysticks[i].joy_fd == -2) // uninitialized!
      if ( ( ji.joy_fd = open( (vsx_string(JOY_DEV)+i2s(i)).c_str() , O_RDONLY)) != -1 )
      {
        // enumerate joystick
        fcntl( ji.joy_fd, F_SETFL, O_NONBLOCK );

        ioctl( ji.joy_fd, JSIOCGAXES, &ji.num_of_axis );
        ioctl( ji.joy_fd, JSIOCGBUTTONS, &ji.num_of_buttons );
        ioctl( ji.joy_fd, JSIOCGNAME(80), &ji.name_of_joystick );
        /*printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
          , ji.name_of_joystick
          , ji.num_of_axis
          , ji.num_of_buttons );*/
        joysticks[i] = ji;
//        printf("ji.joy_fd: %d\n", ji.joy_fd);
        
        joystick_count++;
      }
    }
/*    joy_fd = -1;
    num_of_axis = 0;
    num_of_buttons = 0;
    if( ( joy_fd = open( JOY_DEV1 , O_RDONLY)) == -1 )
    {
      printf( "Couldn't open joystick\n" );
      return false;
    }
    fcntl( joy_fd, F_SETFL, O_NONBLOCK );

    ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
    ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
    ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );
    printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
      , name_of_joystick
      , num_of_axis
      , num_of_buttons );*/
    return true;
  }

void module_info(vsx_module_info* info)
{
  info->output = 1;
  info->identifier = "system;joystick";
  info->description = "Joystick input";
  info->in_param_spec = "";
  info->component_class = "system";
  
  init_joysticks();

  if (joystick_count == 0)
  {
    info->out_param_spec = "no_joystick_detected:complex{}";
    return;
  }
  
  for (int j = 0; j < joystick_count; j++)
  {
    if (j != 0) info->out_param_spec += ",";
    info->out_param_spec += "joystick_"+i2s(j)+":complex{j_"+i2s(j)+"_name:string,axes:complex{";
    for (int i = 0; i < joysticks[j].num_of_axis; i++) {
      if (i != 0) info->out_param_spec += ",";
      info->out_param_spec += "j_"+i2s(j)+"_axis"+i2s(i)+":float"; 
    }
    info->out_param_spec += "},";
    info->out_param_spec += "buttons:complex{"; 
//    printf("num of buttons: %d\n", joysticks[j].num_of_buttons);
    for (int i = 0; i < joysticks[j].num_of_buttons; i++) {
      if (i != 0) info->out_param_spec += ",";
      info->out_param_spec += "j_"+i2s(j)+"_button"+i2s(i)+":float"; 
    }
    info->out_param_spec += "}}";
  }
//  printf("%s\n", info->out_param_spec.c_str());
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  
  init_joysticks();

  //if (!init_joystick()) return;  
  //close( joy_fd );
  //joy_fd = -1;
  for (int j = 0; j < joystick_count; j++)
  {  
    printf("FOO num of axes: %d\n", joysticks[j].num_of_axis);
    joysticks[j].axis = (int *) calloc( joysticks[j].num_of_axis, sizeof( int ) );
    joysticks[j].button = (char *) calloc( joysticks[j].num_of_buttons, sizeof( char ) );

    joysticks[j].name = (vsx_module_param_string*)out_parameters.create(VSX_MODULE_PARAM_ID_STRING, vsx_string("j_"+i2s(j)+"_name").c_str());
    joysticks[j].name->set(vsx_string(joysticks[j].name_of_joystick));
    for (int i = 0; i < joysticks[j].num_of_buttons; i++) {
      joysticks[j].buttons[i] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,(vsx_string("j_"+i2s(j)+"_button")+i2s(i)).c_str());
      joysticks[j].buttons[i]->set(0.0f);
    }
    for (int i = 0; i < joysticks[j].num_of_axis; i++) {
      joysticks[j].axes[i] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,(vsx_string("j_"+i2s(j)+"_axis")+i2s(i)).c_str());
      joysticks[j].axes[i]->set(0.0f);
    }
  }
  //float_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"shutdown");
  //float_in->set(0);
  //render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  //--------------------------------------------------------------------------------------------------  
}

void run() {
  //if (joy_fd == -1)
//    if (!init_joystick())
    //return;
  for (int j = 0; j < joystick_count; j++)
  {
//    printf("reading from : %d\n", joysticks[j].joy_fd);
    for (int k = 0; k < 10; k++)
    {
      js.type = 0;
      /* read the joystick state */
      if (read(joysticks[j].joy_fd, &js, sizeof(struct js_event)))
      {
  
        /* see what to do with the event */
        switch (js.type & ~JS_EVENT_INIT)
        {
          case JS_EVENT_AXIS:
            //printf("js axis-number: %d\n", js.number);
            if (js.number < joysticks[j].num_of_axis)
            joysticks[j].axis   [ js.number ] = js.value;
          break;
          case JS_EVENT_BUTTON:
            //printf("js button-number: %d\n", js.number);
            if (js.number < joysticks[j].num_of_buttons)
            joysticks[j].button [ js.number ] = js.value;
          break;
        }
        //return;
      
        for (int i = 0; i < joysticks[j].num_of_buttons; i++) {
          joysticks[j].buttons[i]->set( (float) joysticks[j].button[i]);
        }
    
        for (int i = 0; i < joysticks[j].num_of_axis; i++) {
          joysticks[j].axes[i]->set( (float) joysticks[j].axis[i] / 32768.0f);
        }
      }
    }
  }
  
  /* print the results */
/*printf( "X: %6d  Y: %6d  ", axis[0], axis[1] );

if( num_of_axis > 2 )
  printf("Z: %6d  ", axis[2] );
  
if( num_of_axis > 3 )
  printf("R: %6d  ", axis[3] );

if( num_of_axis > 4 )
  printf("R1: %6d  ", axis[4] );

if( num_of_axis > 5 )
  printf("R2: %6d  ", axis[5] );
  
for( x=0 ; x<num_of_buttons ; ++x )
  printf("B%d: %d  ", x, button[x] );
printf("\n");
*/  
  //if (float_in->get() > 1.0) exit(0);
  //render_out->set(1);
}

};
#endif
//-----------------------------------------------------------------------
//---- TIME -------------------------------------------------------------
//-----------------------------------------------------------------------

class vsx_module_system_time : public vsx_module {
  // in
	// out
	vsx_module_param_float* time_out;
	vsx_module_param_float* dtime_out;
	vsx_module_param_float* r_time_out;
	vsx_module_param_float* r_dtime_out;
	// internal

void module_info(vsx_module_info* info)
{
  info->identifier = "system;time";
//#ifndef VSX_NO_CLIENT
  info->description = "The outputs of this module need some\n\
explanation. The normal time is bound\n\
to the timeline used when creating a\n\
presentation. The 'real' is pure\n\
operating system time, that is -\n\
will be updated even if the sequenced\n\
time is stopped. In most cases you\n\
|||\n\
want to use the normal time unless you're\n\
creating a presentation and know that\n\
you need real time. Time is total\n\
time elapsed, dtime is the time that\n\
passed since the last frame.";
  info->out_param_spec = "normal:complex{time:float,dtime:float},real:complex{r_time:float,r_dtime:float}";
  info->component_class = "system";
//#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
	time_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"time");
	time_out->set(0.0f);
	dtime_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"dtime");
	dtime_out->set(0.0f);
	r_time_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"r_time");
	r_time_out->set(0.0f);
	r_dtime_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"r_dtime");
	r_dtime_out->set(0.0f);
}

void run() {
  time_out->set(engine->vtime);
  dtime_out->set(engine->dtime);
  r_time_out->set(engine->real_vtime);
  r_dtime_out->set(engine->real_dtime);
}

};

//-----------------------------------------------------------------------
//---- CLOCK -------------------------------------------------------------
//-----------------------------------------------------------------------

class vsx_module_system_clock : public vsx_module {
	// out
	vsx_module_param_float* year;
	vsx_module_param_float* month;
	vsx_module_param_float* day_of_week;
	vsx_module_param_float* day;
	vsx_module_param_float* hour;
	vsx_module_param_float* hour12;
	vsx_module_param_float* minute;
	vsx_module_param_float* second;
	vsx_module_param_float* millisecond;
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "system;clock";
//#ifndef VSX_NO_CLIENT
	info->in_param_spec ="";
  //info->description = "";
  info->out_param_spec = "clock:complex{year:float,\
month:float,\
dayofweek:float,\
day:float,\
hour:float,\
hour12:float,\
minute:float,\
second:float,\
millisecond:float}";
  info->component_class = "system";

//#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
	year = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"year");
	year->set(0);
	month = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"month");
	month->set(0);
	day_of_week = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"dayofweek");
	day_of_week->set(0);
	day = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"day");
	day->set(0);
	hour = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"hour");
	hour->set(0);
	hour12 = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"hour12");
	hour12->set(0);
	minute = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"minute");
	minute->set(0);
	second = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"second");
	second->set(0);
	millisecond = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"millisecond");
	millisecond->set(0);
}
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
SYSTEMTIME time;
#endif

void run() {
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
	float sec = (float)time.wSecond+(float)time.wMilliseconds*0.001;
	float min = (float)time.wMinute+sec/60.0f;
	float hr = (float)time.wHour + min/60.0f;
	GetLocalTime(&time);
	year->set(time.wYear);
	month->set(time.wMonth);
	day_of_week->set(time.wDayOfWeek);
	day->set(time.wDay);
	hour->set(hr);
	hour12->set(fmod(hr,12.0f));
	minute->set(min);
	second->set(sec);
	millisecond->set(time.wMilliseconds);
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
	time_t result;
  result = time(NULL);
  tm* t = localtime(&result);
  struct timeval now;
  gettimeofday(&now, 0);
  float sec = (float)t->tm_sec + 0.000001*(double)now.tv_usec;
  float min = (float)t->tm_min + sec / 60.0f;
  float hr = (float)t->tm_hour + min / 60.0f;
  year->set(t->tm_year);
  month->set(t->tm_mon);
  day_of_week->set(t->tm_wday);
  day->set(t->tm_mday);
  hour->set(hr);
  hour12->set(fmod(hr,12.0f));
  minute->set(min);
  second->set(sec);
  millisecond->set(0.001 * (double)now.tv_usec);
  
  //float sec = (float)tm.tm_sec + fmod(engine->real_vtime, 1.0f);
  //float min = (float)tm.tm_sec + fmod(engine->real_vtime, 1.0f);
#endif
}

};




//-----------------------------------------------------------------------
//---- BLOCK CHAIN ------------------------------------------------------
//-----------------------------------------------------------------------



class vsx_module_block_chain : public vsx_module {
  // in
  vsx_module_param_float* block;
	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal
public:
void module_info(vsx_module_info* info)
{
  info->identifier = "system;blocker";
//#ifndef VSX_NO_CLIENT
  info->description = "Blocks a rendering chain.\
If the control value is less\
than 0.5 the chain is blocked,\
if higher or equal it's run.";
  info->in_param_spec = "render_in:render,block:float";
  info->out_param_spec = "render_out:render";
  info->component_class = "system";
//#endif
  info->tunnel = true; // always run this
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  block = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"block");
  block->set(1.0f);
  
	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
	render_in->set(0);
	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	render_result->set(0);
}

bool activate_offscreen() {
  if (engine->state == VSX_ENGINE_LOADING) return true;
  if (block->get() >= 0.5)
  return true;
  else
  return false;
}

};


class vsx_module_block_chain_load : public vsx_module {
  // in
  vsx_module_param_float* fadeout_time;
  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  vsx_module_param_float* fadeout_out;
  // internal
  public:
    void module_info(vsx_module_info* info)
    {
      info->identifier = "system;blocker_loading";
      //#ifndef VSX_NO_CLIENT
      info->description = "Runs a rendering chain\
      Only when the engine is loading and time\
      seconds after that. Useful for \"loading\" overlays.";
      info->in_param_spec = "render_in:render,fadeout_time:float";
      info->out_param_spec = "render_out:render,fadeout_out:float";
      info->component_class = "system";
      //#endif
      info->tunnel = true; // always run this
    }
    
    void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
    {
      loading_done = true;
      fadeout_time = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fadeout_time");
      fadeout_time->set(1.0f);
      
      render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
      render_in->set(0);
      render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
      render_result->set(0);
      fadeout_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fadeout_out");
      fadeout_out->set(1.0f);
    }
    
    bool activate_offscreen() {
      if (engine->state == VSX_ENGINE_LOADING) return true;
      if (engine->state == VSX_ENGINE_PLAYING)
      {
        if (engine->vtime < fadeout_time->get())
        {
          fadeout_out->set(1.0f - engine->vtime/fadeout_time->get());
          return true;
        }
      }
      return false;
    }
    
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_system_shutdown);
    case 1: return (vsx_module*)(new vsx_module_system_time);
    case 2: return (vsx_module*)(new vsx_module_block_chain);
    case 3: return (vsx_module*)(new vsx_module_system_clock);
    case 4: return (vsx_module*)(new vsx_module_system_trig_sequencer);
    case 5: return (vsx_module*)(new vsx_module_block_chain_load);
#if PLATFORM == PLATFORM_LINUX
    case 6: return (vsx_module*)(new vsx_module_system_joystick);
#endif
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_system_shutdown*)m; break;
    case 1: delete (vsx_module_system_time*)m; break;
    case 2: delete (vsx_module_block_chain*)m; break;
    case 3: delete (vsx_module_system_clock*)m; break;
    case 4: delete (vsx_module_system_trig_sequencer*)m; break;
    case 5: delete (vsx_module_block_chain_load*)m; break;
#if PLATFORM == PLATFORM_LINUX
    case 6: delete (vsx_module_system_joystick*)m; break;
#endif
  }
}

unsigned long get_num_modules() {
#if PLATFORM == PLATFORM_LINUX
  return 7;
#else
  return 6;
#endif
}  

