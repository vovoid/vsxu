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

#include "vsx_sample_ogg.h"

class vsx_module_ogg_sample_play : public vsx_module
{
  // in
  vsx_module_param_resource* filename;
  vsx_module_param_int* format;

  // out

  // private
  vsx_sample_ogg main_sample;

  vsx_engine_float_array full_pcm_data_l;
  vsx_engine_float_array full_pcm_data_r;

public:

  void module_info(vsx_module_info* info)
  {
    info->output = 1;
    info->identifier = "sound;ogg_sample_play";
    info->description =
      "Plays 16-bit signed int PCM\n"
      "OGG vorbis files; mono or stereo."
    ;

    info->in_param_spec =
      "filename:resource,"
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

    format = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"format");

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

      // store the sample data in float array
      //full_pcm_data_l.array[index]
      const float one_div_32767 = 1.0 / 32767.0;
      vsx_array<int16_t>* data = main_sample.get_data();
      size_t index_data = 0;
      for (size_t i = 0; i < (data->size() >> 1); i++)
      {
        full_pcm_data_l.array[i] = (float)(*data)[index_data] * one_div_32767;
        index_data++;
        full_pcm_data_r.array[i] = (float)(*data)[index_data] * one_div_32767;
        index_data++;
      }
    }
  }

  void on_delete()
  {
    main_mixer.unregister_channel( &main_sample );
    shutdown_rtaudio_play();
  }


  void run()
  {

    engine->param_float_arrays[2] = &full_pcm_data_l;
    engine->param_float_arrays[3] = &full_pcm_data_r;


    if (fabs(engine->vtime - main_sample.get_time()) > 0.08)
    {
      main_sample.goto_time(engine->vtime);
      float cur_sample_time = main_sample.get_time();
    }

    if (engine->state == VSX_ENGINE_PLAYING)
    {
      if (engine->dtime < 0.0f)
      {
        main_sample.goto_time( engine->vtime );
      }
      main_sample.play();
    }
    if (engine->state == VSX_ENGINE_STOPPED)
    {
      //vsx_printf("engine is stopped at %f\n", engine->vtime );
      main_sample.stop();
      if (engine->dtime != 0.0f)
      {
        main_sample.goto_time( engine->vtime );
      }
    }
    main_sample.set_stereo_type( format->get() + 1 );
  }
};

