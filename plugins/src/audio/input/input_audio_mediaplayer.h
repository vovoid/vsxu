/**
* @author Dinesh Manajipet, Vovoid Media Technologies - Copyright (C) 2012-2020
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


#ifndef INPUT_AUDIO_MEDIAPLAYER_H
#define INPUT_AUDIO_MEDIAPLAYER_H

#include <fftreal.h>

#define N_BUFFER_FRAMES 512
#define SAMPLE_RATE 44100
#define N_CHANNELS 2
#define LEFT 0
#define RIGHT 1
#define N_OCTAVES 8

#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"

class input_audio_mediaplayer : public vsx_module
{
  vsx_module_param_float*       in_multiplier;

  vsx_module_param_float*       out_vu[N_CHANNELS];
  vsx_module_param_float*       out_octaves[N_CHANNELS][N_OCTAVES];
  vsx_module_param_float_array* out_wave[N_CHANNELS];
  vsx_module_param_float_array* out_spectrum[N_CHANNELS];

  //Hidden params for maintaining backwards compatibility with the older audio input module
  vsx_module_param_int*         hidden_in_quality;
  vsx_module_param_float_array* hidden_out_wave;
  vsx_module_param_float_array* hidden_out_spectrum;
  vsx_module_param_float_array* hidden_out_spectrum_hq;

  //Internal data buffers
  vsx_float_array m_wave[N_CHANNELS], m_spectrum[N_CHANNELS];

  //Internal objects needed for calculating vu and octave data
  FFTReal m_fft_machine;
  float m_tmp_wave[N_CHANNELS][N_BUFFER_FRAMES];
  float m_tmp_fft_samples[N_CHANNELS][N_BUFFER_FRAMES];
  float m_tmp_fft_values[N_CHANNELS][N_BUFFER_FRAMES];

public:
  input_audio_mediaplayer();
  ~input_audio_mediaplayer();

  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
  void on_delete();
};


#endif // INPUT_AUDIO_MEDIAPLAYER_H
