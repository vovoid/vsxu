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


#ifndef INPUT_AUDIO_RAW_H
#define INPUT_AUDIO_RAW_H

#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"

#include <RtAudio/RtAudio.h>
#include <RtAudio/RtError.h>
#include <fftreal.h>
#include <pthread.h>

typedef signed short sample;

#define N_BUFFERS 2
#define N_BUFFER_FRAMES 512
#define SAMPLE_RATE 44100
#define N_CHANNELS 2
#define LEFT 0
#define RIGHT 1
#define N_OCTAVES 8

class input_audio_raw : public vsx_module
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
  vsx_float_array m_wave[N_BUFFERS][N_CHANNELS], m_spectrum[N_BUFFERS][N_CHANNELS];
  float m_vu[N_BUFFERS][N_CHANNELS], m_octaves[N_BUFFERS][N_CHANNELS][N_OCTAVES];

  float amp;

  //Double buffer related properties
  pthread_mutex_t m_mutex;

  int m_currentBuffer;
  void initializeBuffers();
  void freeBuffers();
  int currentBufferPage();
  int nextBufferPage();
  void flipBufferPage();

  RtAudio m_adc;

  //Internal objects needed for calculating vu and octave data
  FFTReal m_fft_machine;
  float m_tmp_wave[N_CHANNELS][N_BUFFER_FRAMES];
  float m_tmp_fft_samples[N_CHANNELS][N_BUFFER_FRAMES];
  float m_tmp_fft_values[N_CHANNELS][N_BUFFER_FRAMES];

  unsigned int m_sample_rate;
  unsigned int m_buffer_frames;

  static int record( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                     double streamTime, RtAudioStreamStatus status, void *userData );

  void read_data(sample* buffer , int nBufferFrames);

public:
  input_audio_raw();
  ~input_audio_raw();

  bool can_run();
  bool init();
  void on_delete();

  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
};

#endif // INPUT_AUDIO_RAW_H
