

#pragma once


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
  All this is done via communicating and filling out a vsx_module_specification class. It's quite simple really, if you look
  at the examples you'll get it right away!

  As this code is used in the internal structures of VSXU there's some things that you don't have to bother with.
*/
class vsx_module_specification
{
public:
//-----internal:
  vsx_string<>location; // internal or external - for internal use only, don't care about this
//-
  vsx_string<>identifier; // the name of the module be it internal or a file i.e. "math;oscillator" etc, with a module
                          // found on disk as a DLL/SO this will be the filename of the module.
  vsx_string<>identifier_save; // If not empty, use this name when saving the state rather than identifier.
//----------------------------------------------------------------------------------------------------------------------
  // [description]
  //   The purpouse of this is to tell the end user what a component loaded with this module does.
  //   If it cooks eggs - write something like this:
  //   "This component can cook your eggs using the heat of the external USB water boiler wich this component controls."
  //
  vsx_string<>description;

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

  bool tunnel = false; // very special case for render component that have to be reset after each run.


  /*
  Param specifications - used for the GUI (artiste)

  Looks like:
    name:type?param1=val1&param2=val2

    Notable extra params for a param are:
      nc=1                       - don't allow connections to this param
      default_controller=knob    - what happens when double-clicking on the param
      help=`Helpful Text`        - help text
  */
  vsx_string<>in_param_spec;
  vsx_string<>out_param_spec;

  vsx_string<>component_class; // the class of the component

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
  int output = 0;
};
