/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
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

#include <vsx_gl_state.h>

#include <vsx_argvector.h>
#include <filesystem/vsx_filesystem.h>
#include <debug/vsx_error.h>
#include <vsx_param.h>
#include <vsx_module_cache_helper.h>

#include "vsx_module_engine_state.h"
#include "vsx_module_engine_environment.h"
#include "vsx_module_specification.h"
#include "vsx_module_operation.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef _WIN32
#define _declspec(MICROSOFT_DLL_LOADING_CODE_SUCKS_BAD)
#endif



class vsx_module
{
public:
  //-----internal:
  int module_id; // module id, used when destroying a module. Managed by engine. Should be treated as read-only!
  vsx_string<> module_identifier; // when created, holds module identifier. Managed by engine. Should be treated as read-only!


  // this property will be set by the engine, it allows access to certain parts of the engine for the modules.
  vsx_module_engine_state* engine_state = 0x0;


  vsx_string<> user_message; // message from module to engine
                        // will be emptied by the engine the frame after it has been set

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
  int param_updates = 1;

  // param notify, run whenever the engine sets a param from a command
  // not with the interpolator! added because of the need of redeclaring in-params
  // during state load before trying to make connections.
  virtual void param_set_notify(const vsx_string<>& name)
  {
    VSX_UNUSED(name);
  }

  // is the module loaded and ready to render in high performance?
  // this will be used to determine if loading is completed for a demo for instance, or an effect.
  // The engine will not star the time until loading of all modules are done.
  // So consecutive frames will be rendered with a black overlay allowing all threads and heavy processing
  // tasks to finish. If you know you do initial processing that takes more than one tenth of a frame
  // it's good practice to use this and set it to true once you've done your first run.
  // If you don't do heavy processing, set it to true in the declare_params() method.
  bool loading_done = false;
  // this is the function that the engine will call to get information about the module
  virtual void module_info(vsx_module_specification* info)
  {
    VSX_UNUSED(info);
  }
  // when it's time to allocate memory for parameters (when a module is to be used for real)
  // you have to implement this method to declare parameters. This is very related to the module_info function
  // as that one declares the GUI aspects of these parameters.
  // DO NOT allocate your own memory in here, do it in the init() function
  virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(in_parameters);
    VSX_UNUSED(out_parameters);
  }

  // Redeclaration of parameters
  // If you need to redeclare in our out parameters during runtime,
  // you implement the appropriate method and when it's time, youset
  // redeclare_[in|out] = true;
  // The engine will then:
  // - pick this up
  // - call redelcare_[in|out]_params
  // - call module_info again
  // When module_info is called again, you can supply new parameter specifications
  // for the GUI.
  bool redeclare_in = false;
  virtual void redeclare_in_params(vsx_module_param_list& in_parameters)
  {
    VSX_UNUSED(in_parameters);
  }

  bool redeclare_out = false;
  virtual void redeclare_out_params(vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(out_parameters);
  }


  virtual void declare_operations(vsx_nw_vector<vsx_module_operation*>& operations )
  {
    VSX_UNUSED(operations);
  }

  virtual void destroy_operations(vsx_nw_vector<vsx_module_operation*>& operations)
  {
    VSX_UNUSED(operations);
  }

  virtual void run_operation(vsx_module_operation& operation)
  {
    VSX_UNUSED(operation);
  }


  // avector with all the filenames that this module needs
  // this is needed to build .vsx packages
  vsx_nw_vector< vsx_string<> > resources;
  // the init method, in here you should do stuff that needs to be done only once -
  // - allocate + initialize memory
  // - read configuration files
  // - calculate stuff.
  // Note though: this is called from the main thread so for heavy processing - create a thread instead.
  virtual bool init()
  {
    return true;
  }

  // status screens, these are nice for the live performer as the module gets a chance to draw something
  // on top of everything else in a second render pass by the engine. This can be used for debugging modules
  // when setting up a production environment. One example is the screen module that can print the current
  // fps in one of the corners of the screen - as small as possible - so noone will notice if you just
  // enable/disable that very quickly. Another example would be to show the real signal levels of a sound
  // module that gets sound from the soundcard.

  // first, you need to return how many different status screens you have
  virtual unsigned long status_screen_get_count()
  {
    return 0;
  }
  // keep track of wich one is active
  unsigned long status_screen_id;
  // get the name of the different status screens, this being declared like this
  // gives you the opportunity to dynamically name your status screens
  virtual vsx_string<>status_screen_get_name(unsigned long id)
  {
    VSX_UNUSED(id);
    return "";
  }
  // the engine calls this one to tell the module wich one it wants to be drawn
  virtual void status_screen_set(unsigned long id)
  {
    status_screen_id = id;
  }
  // then if it's enabled at all (the engine keeps track of this) it will call
  // this draw method.
  virtual void status_screen_draw()
  {
  }


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
  virtual bool activate_offscreen()
  {
    return true;
  }
  virtual void deactivate_offscreen()
  {
  }

  // validates wether or not this module can be run at all
  // If you need to check for certain hardware present, or OpenGL extensions - this is the function
  // to do it in. If you ccan't operate properly, simply implement this function and return false.
  virtual bool can_run()
  {
    return true;
  }

  // the following methods are used a bit differently in different kinds of modules.
  // I suggest you see the different examples of modules to find out what's appropriate :)
  // run is run once per frame
  virtual void run()
  {
  }
  // output is run for each connection connected to the module
  // this method usually performs what is going on screen and/or sets the outparams
  virtual void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
  }

  // this will be run when the engine stops, this will up until now only happen when going into
  // fullscreen mode so the modules with pbuffers in them can react to the mode change
  virtual void stop()
  {
  }

  virtual void start()
  {
  }
  // clean up everything but the parameters in here - stuff you allocated yourself
  virtual void on_delete()
  {
  }

  virtual ~vsx_module()
  {
  }
};

// hope this documentation isn't _that_ horrible ;)
// oh, it is. well whaaatever!


#endif

