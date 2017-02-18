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

#include <audio/vsx_sample_raw.h>

class vsx_module_raw_sample_play : public vsx_module
{
  // in
  vsx_module_param_resource* filename;
  vsx_module_param_int* format;
  vsx_module_param_float* gain;
  vsx_module_param_int* show_waveform_in_sequencer;

  // out

  // private
  vsx_sample_raw main_sample;

  vsx_module_engine_float_array full_pcm_data_l;
  vsx_module_engine_float_array full_pcm_data_r;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "sound;raw_sample_play";

    info->description =
      "Plays 16-bit signed int PCM\n"
      "RAW files; mono or stereo."
    ;

    info->in_param_spec =
      "filename:resource,"
      "format:enum?mono|stereo,"
      "gain:float?default_controller=controller_slider,"
      "show_waveform_in_sequencer:enum?no|yes"
    ;

    info->out_param_spec =
      "";

    info->component_class =
      "output";

    info->output = 1;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    (void)out_parameters;

    filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename->set("");

    format = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"format");

    gain = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"gain");
    gain->set(1.0);

    show_waveform_in_sequencer = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"show_waveform_in_sequencer");
    show_waveform_in_sequencer->set( 1 );

    loading_done = true;
  }

  bool init()
  {
    setup_rtaudio_play();
    vsx_printf(L"rtaudio play\n");
    vsx_audio_mixer& main_mixer = *vsx_audio_mixer_manager::get_instance();
    main_mixer.register_channel( &main_sample );
    return true;
  }

  void param_set_notify(const vsx_string<>& name)
  {
    if (name == "filename")
    {
      main_sample.set_filesystem( engine_state->filesystem );
      main_sample.load_filename( filename->get() );

      // store the sample data in float array
      //full_pcm_data_l.array[index]
      const float one_div_32767 = 1.0 / 32767.0;
      vsx_ma_vector<int16_t>* data = main_sample.get_data();
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
    vsx_audio_mixer& main_mixer = *vsx_audio_mixer_manager::get_instance();
    main_mixer.unregister_channel( &main_sample );
    shutdown_rtaudio_play();
  }


  void run()
  {
    if (show_waveform_in_sequencer->get())
    {
      engine_state->param_float_arrays[2] = &full_pcm_data_l;
      engine_state->param_float_arrays[3] = &full_pcm_data_r;
    }


    if (fabs(engine_state->vtime - main_sample.get_time()) > 0.08)
    {
      main_sample.goto_time(engine_state->vtime);
      float cur_sample_time = main_sample.get_time();
      VSX_UNUSED(cur_sample_time);
    }

    if (engine_state->state == VSX_ENGINE_PLAYING)
    {
      if (engine_state->dtime < 0.0f)
      {
        main_sample.goto_time( engine_state->vtime );
      }
      main_sample.play();
      main_sample.set_pitch_bend(engine_state->speed);
    }
    if (engine_state->state == VSX_ENGINE_STOPPED)
    {
      main_sample.stop();
      if (engine_state->dtime != 0.0f)
      {
        main_sample.goto_time( engine_state->vtime );
      }
    }
    main_sample.set_stereo_type( format->get() + 1 );
    main_sample.set_gain( gain->get() );
  }
};

