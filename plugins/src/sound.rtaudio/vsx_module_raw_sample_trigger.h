/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
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

#include "vsx_sample_raw.h"

class vsx_module_raw_sample_trigger : public vsx_module
{
  // in
  vsx_module_param_resource* filename;
  vsx_module_param_float* trigger;
  vsx_module_param_float* pitch;
  vsx_module_param_float* gain;
  vsx_module_param_int* format;
  // out

  // private
  vsx_sample_raw main_sample;
  float trigger_old;

public:

  void module_info(vsx_module_info* info)
  {
    info->output = 1;
    info->identifier = "sound;raw_sample_trigger";
    info->description =
      "Plays 16-bit signed int PCM\n"
      "RAW files; mono or stereo."
    ;

    info->in_param_spec =
      "filename:resource,"
      "trigger:float,"
      "pitch:float,"
      "gain:float,"
      "format:enum?mono|stereo"
    ;
    info->out_param_spec = "";
    info->component_class = "output";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    (void)out_parameters;
    filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename->set("");

    trigger = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trigger");
    pitch = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"pitch");
    gain = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"gain");
    gain->set(1.0f);

    format = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"format");

    trigger_old = 0.0f;
    loading_done = true;
  }

  bool init()
  {
    setup_rtaudio_play();
    main_mixer.register_channel( &main_sample );
    return true;
  }

  void param_set_notify(const vsx_string& name)
  {
    if (name == "filename")
    {
      main_sample.set_filesystem( engine->filesystem );
      main_sample.load_filename( filename->get() );
    }
  }

  void on_delete()
  {
    main_mixer.unregister_channel( &main_sample );
    shutdown_rtaudio_play();
  }


  void run()
  {
    if (param_updates)
    {
      param_updates = 0;
    }

    main_sample.set_pitch_bend( trigger->get() + pitch->get() * 2.0f );
    main_sample.set_gain( gain->get() );

    main_sample.set_stereo_type( format->get() + 1 );

    if (trigger->get() < 0.1f)
    {
      main_sample.stop();
    }

    if (trigger_old < 1.0f && trigger->get() >= 1.0f)
    {
      main_sample.trigger();
    }

    trigger_old = trigger->get();
  }
};

