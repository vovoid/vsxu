/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
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

/*
  This is the module defenition of VSXU. This file contains a few things you don't have to
  bother with as they're used by the engine and had to be declared in here. They have comments
  like 
  //----internal
  [some internal code]
  //-
  [normal code]
  
*/

#ifndef VSX_MODULE_H
#define VSX_MODULE_H

#ifndef VSX_NO_GL
  #include "vsx_gl_global.h"
#endif

#include "vsxfst.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef _WIN32
#define _declspec(MICROSOFT_DLL_LOADING_CODE_SUCKS_BAD)
#endif
/*
Environment information
*/
typedef struct {
	// engine_parameter[0] is module shared data path, data/config files that are used to define behaviour and existence of modules.
  //                     Examples: The render.glsl module is a good example. It returns one available module per file it finds
  //                               in its data directory.
	vsx_avector<vsx_string> engine_parameter;
} vsx_engine_environment;

// engine float arrays, data flowing from the engine to a module
// 0 is reserved for sound data, wave (512 floats)
// 1 is reserved for sound data, freq (512 floats)
typedef struct {
	vsx_avector<float> array;
} vsx_engine_float_array;

/*
  The information that the module get from the engine. All modules share a pointer
  to this struct.
*/

#define VSX_ENGINE_LOADING -1
#define VSX_ENGINE_STOPPED 0
#define VSX_ENGINE_PLAYING 1
#define VSX_ENGINE_REWIND 2

typedef struct  {
  vsxf* filesystem;
    
  int state; // stopped or playing
  float amp; // engine effect amplification - can be used freely by the modules
             // but should primarily control 
  // sequenced values
  float dtime;  // delta time - since last frame in seconds and fractions of seconds
  float vtime;  // total time - since the engine started, seconds and fractions of seconds
  // realtime values
  float real_dtime;
  float real_vtime;
  // time control from module
  int request_play;
  int request_stop;
  int request_rewind;
  // this is used to send delta hints to control a sequencer in recording mode
  vsx_avector<float> sequencer_hints;
  // this is to send parameters from the implementor of the engine down to the modules, like sound data etc
  // should only be used for implementation-specific modules that are not part of a normal vsxu distribution.
  // item 0 is reserved for realtime PCM data for visualization (from host app to module)
  // item 1 is reserved for realtime FFT data for visualization (from host app to module)
  // item 2 is reserved for Full song PCM data for the sequencer, Left Channel (from module to host app)
  // item 3 is reserved for Full song PCM data for the sequencer, Right Channel (from module to host app)
  // item 4..999 are reserved for Vovoid use
  vsx_avector<vsx_engine_float_array*> param_float_arrays;
} vsx_module_engine_info;

/*
  Modules are the workhorses of VSXU. To understand how they work, you have to know the concept of module and
  component. The server component is the entity that deals with inter-component communication and the logic involved
  in feedback loops, parameter queueing and similar tasks. Each component can have only one module loaded - and it's
  the module that does the actual work. For the user there is only _component_ and no modules. We chose c++ for our
  modules as we want full speed. Maybe we'll implement creating modules in some scripting language as well, but that's
  not a primary objective right now. 
    
  About speed - the engine will go about and call every single component (wich will call its module) in the big grid and
  it does this every frame. Some modules will therefore be run more than once per frame. Therefore it's utterly important
  that you know what you're doing so you don't lock up the engine! All heavy tasks that take more than 0.001 seconds, such
  as loading a 100 Mbyte file, writing stuff to disk, generating a texture in software mode - should be done IN A THREAD.
  None of the functions called by the engine should take time.
  
  There's an example on how to do this in the module repository called "module_thread" that generates a texture. Also
  note that you can't use the STL in threads, it'll go terribly wrong. Using a thread is a bit of a pain but it's the
  only way to do this. VSXU is a realtime system and needs to be treated like one. If you have any questions about this
  we'd be glad to help you, just come by the forums.
  
  So in order to present a module to the component you need to tell the component a few things.
  "Hi, I'm a module!"
  - wich parameters do i (i'm a module) accept? of what types?
  - how should these be presented to the user? (in the GUI)
  - wich of them should be presented to the user at all? (in the GUI)
  All this is done via communicating and filling out a vsx_module_info class. It's quite simple really, if you look
  at the examples you'll get it right away!
  
  As this code is used in the internal structures of VSXU there's some things that you don't have to bother with.
*/


class vsx_module_info {
public:
//-----internal:
  vsx_string location; // internal or external - for internal use only, don't care about this
//-
  vsx_string identifier; // the name of the module be it internal or a file i.e. "math;oscillator" etc, with a module
                          // found on disk as a DLL/SO this will be the filename of the module.
//----------------------------------------------------------------------------------------------------------------------
  // [description]
  //   The purpouse of this is to tell the end user what a component loaded with this module does. 
  //   If it cooks eggs - write something like this:
  //   "This component can cook your eggs using the heat of the external USB water boiler wich this component controls."
  //   
  vsx_string description;
  
  /* 
   This is the tricky part. The param specifications. Here you define what parameters are going to be published in the
   GUI. This takes the form of 2 strings - one for inputs and one for outputs.
   Here's an example, it's much easier to dissect that: (Characters {blabla} are to be replaced with something.)
  
     size:float,angle:float,texture_in:texture
  
   Eacn parameter is separated by a ','  a comma. So, by knowing that we can draw the conclusion that the above
   is 3 parameters. The first one, called "size" is of the type "float". The second one called "angle" is of the
   type "float". The third one is a "texture" called "texture_in".
  
   Easy? Once you get it - it will be. There's more to it though. Here's another example:
   
     render_in:render,texture_resolution:complex[screen_width:int{64,128,256,512,1024},screen_height:int{64,128,256,512,1024}]
   
   First we have a standalone render-parameter. Then comes something new, called a "complex parameter". This is really
   simple - it's just a collection of parameters - a tree. It can have infinite depth in theory but I think 4 or 5
   levels should be enough. This is for parameters that will seldom be used or simply configurations.
   As you've probably noted by now, the parameters are declared like name:type. Here's the name, "texture_resolution"
   and then the type wich is complex[name:type,name2:type...] like usual params. In addition to this, this int can only
   hold fixed values provided in the list in between the {} brackets. 
   
   One final thing, NO SPACES IN THIS STRING UNLESS DEALING WITH STRING VALUES!

   See the examples on how to use it, it's easier that way, there's comments in there too.
   
   Here's the basic data types:
     float  - flaot values - 0.1,0.2,0.3123,4442123.123981723 etc.
     float3 - see vsx_math_3d.h - it's the vsx_vector class!
     int    - ordinal values- 0,1,2,3,4
     mesh   
     texture
     blob
     string
 */
 
 	bool tunnel; // very special case for render component that have to be reset after each run.

  
  vsx_string in_param_spec;
  vsx_string out_param_spec;

  vsx_string component_class; // the class of the component

  /* [output]
    Some modules deal with output. For instance, if you have a sattelite card or whatever in your computer and want to send
    stuff to it - you'll be writing a special kind of module - an output module.
    The way VSXU works, all other modules can be input modules. If they're not connected anywhere, they do nothing.
    The output modules however will be called every frame to do their thing. They're "last in the chain". This means
    that vsxu goes through all these every frame. They in turn need parameters from other components. So, a float
    value can end up taking part in both sound and visual outputs. You don't care about that here. Also an output
    module shouldn't have any out-parameters. THIS IS A RULE.
    
    A few examples of output modules:
      - Midi-out
      - Sound-out
      - Screen
      - RS232 (serial) - just an example, hopefully this won't be needed ;)
    
    Valid values are: 0 or 1 for now.
  */
  int output;
  
  // constructor
  vsx_module_info() {
    output = 0; // being an input type is the default
    tunnel = false;
  }
  
};

class vsx_module {
public:
  // this property will be set by the engine, it allows access to certain parts of the engine for the modules.
  vsx_module_engine_info* engine;


	vsx_string message; // message from module to client regarding this parameter
	                    // will be emptied by the engine so just put something in it :)
	                    
	// format is as follows:
  // module||The message goes here...        <-- module message, "module" reserved word
  // render_in||Parameter info goes here...  <-- parameter info, "render_in" - name of parameter


  
  // A counter that is increased once per parameter value update including all that can be set by the user.
  // 
  // The parameter is updated so a module can know
  // wether or not to re-create all its data in cases it is costly.
  // When connections are made the channel will update this value.
  // Do not rely on this for modules that need to know wether or not a parameter
  // that is set by set_p, those channels don't update this value.
  int param_updates;
  
  // param notify, run whenever the engine sets a param from a command
  // not with the interpolator! added because of the need of redeclaring in-params
  // during state load before trying to make connections.
  virtual void param_set_notify(const vsx_string& name) {};
  
  // is the module loaded and ready to render in high performance?
  // this will be used to determine if loading is completed for a demo for instance, or an effect.
  // The engine will not star the time until loading of all modules are done. 
  // So consecutive frames will be rendered with a black overlay allowing all threads and heavy processing
  // tasks to finish. If you know you do initial processing that takes more than one tenth of a frame
  // it's good practice to use this and set it to true once you've done your first run.
  // If you don't do heavy processing, set it to true in the declare_params() method.
  bool loading_done;
  // this is the function that the engine will call to get information about the module
  virtual void module_info(vsx_module_info* info) {};
  // when it's time to allocate memory for parameters (when a module is to be used for real)
  // you have to implement this method to declare parameters. This is very related to the module_info function
  // as that one declares the GUI aspects of these parameters.
  // DO NOT allocate your own memory in here, do it in the init() function
	virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {};
	
	bool redeclare_in;
	virtual void redeclare_in_params(vsx_module_param_list& in_parameters) {};
	bool redeclare_out;
	virtual void redeclare_out_params(vsx_module_param_list& out_parameters) {};
	
	
	// avector with all the filenames that this module needs
	// this is needed to build .vsx packages
	vsx_avector<vsx_string> resources;
	// the init method, in here you should do stuff that needs to be done only once -
  // - allocate + initialize memory
  // - read configuration files
	// - calculate stuff.
  // Note though: this is called from the main thread so for heavy processing - create a thread instead.
	virtual bool init() {return true;};

  // status screens, these are nice for the live performer as the module gets a chance to draw something
  // on top of everything else in a second render pass by the engine. This can be used for debugging modules
  // when setting up a production environment. One example is the screen module that can print the current
  // fps in one of the corners of the screen - as small as possible - so noone will notice if you just 
  // enable/disable that very quickly. Another example would be to show the real signal levels of a sound
  // module that gets sound from the soundcard.
  
  // first, you need to return how many different status screens you have
	virtual unsigned long status_screen_get_count() {return 0;};
	// keep track of wich one is active
	unsigned long status_screen_id;
	// get the name of the different status screens, this being declared like this
	// gives you the opportunity to dynamically name your status screens
	virtual vsx_string status_screen_get_name(unsigned long id) {return "";};
	// the engine calls this one to tell the module wich one it wants to be drawn
	virtual void status_screen_set(unsigned long id) {status_screen_id = id;};
	// then if it's enabled at all (the engine keeps track of this) it will call
	// this draw method.
	virtual void status_screen_draw() {};


  // these methods are run before and after all the other methods.
  // used mostly for turning on pbuffers and other states, openGL stuff on/off etc.
  // you usually don't have to bother with these :)
  // However, if you for some reason want to prevent the module from running,
  // you can implement activate_offscreen and return false. However this will block off a whole
  // chain of modules, so it's best to leave this to vsxu and the system
  // via the blocker module.
  // The opposite is also important, if you want to do stuff in here, don't forget to
  // either go return vsx_module::deactivate_offscree(); or return true yourself.
  // Deactivate offscreen will not be run if activate_offscreen returns false.
	virtual bool activate_offscreen() { return true; };
	virtual void deactivate_offscreen() {};
	
	// the following methods are used a bit differently in different kinds of modules.
	// I suggest you see the different examples of modules to find out what's appropriate :)
	// run is run once per frame
	virtual void run() {}; 
	// output is run for each connection connected to the module
	virtual void output(vsx_module_param_abs* param) {}; // this method usually performs what is going on screen and/or sets the outparams
	
	// this will be run when the engine stops, this will up until now only happen when going into 
	// fullscreen mode so the modules with pbuffers in them can react to the mode change
	virtual void stop() {};
	virtual void start() {};
	// clean up everything but the parameters in here - stuff you allocated yourself
	virtual void on_delete() {};
	vsx_module() { redeclare_in = false; redeclare_out = false; param_updates = 1; loading_done = false; engine = 0; };
	virtual ~vsx_module() {};
};

// hope this documentation isn't _that_ horrible ;)
// oh, it is. well whaaatever!




#endif

