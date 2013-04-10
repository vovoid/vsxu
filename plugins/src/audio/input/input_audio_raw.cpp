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

#include <sstream>
#include "input_audio_raw.h"

#ifdef __LINUX_PULSE__
#define  AUDIO_API RtAudio::LINUX_PULSE
#else
#define AUDIO_API
#endif

input_audio_raw::input_audio_raw():
  vsx_module(),
  in_multiplier(0x0),
  hidden_in_quality(0x0),
  hidden_out_wave(0x0),
  hidden_out_spectrum(0x0),
  hidden_out_spectrum_hq(0x0),
  amp(1.0f),
  m_currentBuffer(-1),
  m_adc(AUDIO_API),
  m_fft_machine(N_BUFFER_FRAMES),
  m_sample_rate(SAMPLE_RATE),
  m_buffer_frames(N_BUFFER_FRAMES)
{
  pthread_mutex_init(&m_mutex, NULL);
}

input_audio_raw::~input_audio_raw()
{
  pthread_mutex_destroy(&m_mutex);
}

bool input_audio_raw::can_run()
{
  return m_adc.getDeviceCount() >= 1;
}

int input_audio_raw::record(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData)
{
  VSX_UNUSED(outputBuffer);
  VSX_UNUSED(streamTime);
  if ( status )
    std::cout << "Stream overflow/underflow detected!" << std::endl;
    // Do something with the data in the "inputBuffer" buffer.
  else ((input_audio_raw*)userData)->read_data((sample*)inputBuffer, nBufferFrames);

  return 0;
}

void input_audio_raw::read_data(sample* buffer, int nBufferFrames)
{
  //TODO: Make the computation of an output only if it is needed, using flags collected from vsx_module::output
  //printf("Read %d frames\n",nBufferFrames);
  int page = nextBufferPage();

  float amplification = in_multiplier->get() * amp;
  m_vu[page][LEFT] = 0;
  m_vu[page][RIGHT] = 0;

  //Copy the data iinto internal buffers
  for( int i = 0; i < N_CHANNELS; i++ )
    for ( int j = 0; j < nBufferFrames; j++){
      m_tmp_wave[i][j] = buffer[2*j + i]*amplification/16384.0;
      (*(m_wave[page][i].data))[j] = m_tmp_wave[i][j];
      //m_vu[page][i] += m_tmp_wave[i][j]/nBufferFrames;
    }

  //Take the FFT
  m_fft_machine.do_fft(m_tmp_fft_samples[LEFT], m_tmp_wave[LEFT]);
  m_fft_machine.do_fft(m_tmp_fft_samples[RIGHT], m_tmp_wave[RIGHT]);

  //Caclulating the VU values
  float real, imaginary;
  int n_fft_values = nBufferFrames/2;
  for( int i = 0; i < N_CHANNELS; i++ )
    for ( int j = 0; j < n_fft_values; j++){
      real = m_tmp_fft_samples[i][j];
      imaginary = m_tmp_fft_samples[i][j+n_fft_values];
      m_tmp_fft_values[i][j] = (sqrt(real*real + imaginary*imaginary)/n_fft_values);
      m_vu[page][i] += m_tmp_fft_values[i][j];
    }

  //Calculating the spectrum data
  for (int i = 0; i < N_CHANNELS; i++)
    for ( int j = 0; j < nBufferFrames; j++)
      (*(m_spectrum[page][i].data))[j] = m_tmp_fft_values[i][j/2] * 3.0 * pow(log( 10.0f + ((float)SAMPLE_RATE) * (j /(float)nBufferFrames)) ,1.0f);

  //Calculating the Octave data
  int values_per_octave = nBufferFrames/N_OCTAVES;
  for (int i = 0; i < N_CHANNELS; i++)
    for ( int j = 0; j < N_OCTAVES; j++){
      m_octaves[page][i][j] = 0;
      for ( int k = 0; k < values_per_octave; k++)
        m_octaves[page][i][j] +=  (*(m_spectrum[page][i].data))[j*values_per_octave + k];
      m_octaves[page][i][j] /= (values_per_octave - 14);
    }
}


bool input_audio_raw::init()
{
  RtAudio::StreamParameters params;
  params.deviceId = m_adc.getDefaultInputDevice();
  params.nChannels = N_CHANNELS;
  params.firstChannel = LEFT;

  RtAudio::StreamOptions options;
  options.streamName = "vsxu";

  try{
    m_adc.openStream( NULL, &params, RTAUDIO_SINT16, m_sample_rate,
                     &m_buffer_frames, &input_audio_raw::record, this , &options);
    m_adc.startStream();
    return true;
  }
  catch(RtError &e){
    message = (std::string("module||ERROR!")+e.getMessage()).c_str();
    return false;
  }
}

void input_audio_raw::on_delete()
{
  try{
      m_adc.stopStream();
      if(m_adc.isStreamOpen())m_adc.closeStream();
  }
  catch(RtError &e){
    message = (std::string("module||ERROR!")+e.getMessage()).c_str();
  }

  freeBuffers();
}

inline int input_audio_raw::currentBufferPage()
{
  int currentBuffer;
  pthread_mutex_lock(&m_mutex);
    currentBuffer = m_currentBuffer;
  pthread_mutex_unlock(&m_mutex);
  return currentBuffer;
}

inline int input_audio_raw::nextBufferPage()
{
  return (currentBufferPage() + 1)%N_BUFFERS;
}

inline void input_audio_raw::flipBufferPage(){
  pthread_mutex_lock(&m_mutex);
    m_currentBuffer = (m_currentBuffer + 1)%N_BUFFERS;
  pthread_mutex_unlock(&m_mutex);
}

void input_audio_raw::initializeBuffers()
{
  for (int i = 0;i < N_BUFFERS;i++)
    for (int j = 0; j < N_CHANNELS; j++ ){
      m_wave[i][j].data = new vsx_array<float>;
      m_spectrum[i][j].data = new vsx_array<float>;;
    }

  for (int i = 0;i < N_BUFFERS;i++)
    for (int j = 0; j < N_CHANNELS; j++ )
      for(int k = 0; k < N_BUFFER_FRAMES; k++){
        m_wave[i][j].data->push_back(0);
        m_spectrum[i][j].data->push_back(0);
      }
}

void input_audio_raw::freeBuffers()
{
  for (int i = 0;i < N_BUFFERS;i++)
    for (int j = 0; j < N_CHANNELS; j++ ){
      delete m_wave[i][j].data;
      delete m_spectrum[i][j].data;
    }
}


void input_audio_raw::module_info(vsx_module_info* info)
{
  //TODO: Re create the input params based on the available device capabilities for module configuration.
  info->output = 1;
  info->identifier = "audio;input;wave_in||sound;input_visualization_listener||system;sound;vsx_listener";
#ifndef VSX_NO_CLIENT
  info->description = "VU is the volume level for each channel.\nThe octaves are 0 = bass, 7 = treble";
  info->in_param_spec = "multiplier:float";
  info->out_param_spec = "vu:complex{vu_l:float,vu_r:float},\
octaves:complex{\
  left:complex{\
    octaves_l_0:float,octaves_l_1:float,octaves_l_2:float,octaves_l_3:float,\
    octaves_l_4:float, octaves_l_5:float,octaves_l_6:float,octaves_l_7:float},\
  right:complex{\
    octaves_r_0:float,octaves_r_1:float,octaves_r_2:float,octaves_r_3:float,\
    octaves_r_4:float,octaves_r_5:float,octaves_r_6:float,octaves_r_7:float\
  }\
},\
wave_data:complex{wave_left:float_array,wave_right:float_array,},\
spectrum_data:complex{spectrum_left:float_array,spectrum_right:float_array}";
  info->component_class = "output";
#endif

}

void input_audio_raw::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  hidden_in_quality = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"quality");
  hidden_in_quality->set(0);

  in_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"multiplier");
  in_multiplier->set(1);

  out_vu[LEFT] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"vu_l");
  out_vu[RIGHT] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"vu_r");
  out_vu[LEFT]->set(0);
  out_vu[RIGHT]->set(0);

  for(int i = 0 ; i < N_OCTAVES; i++){
    std::stringstream name;
    name.str("");
    name << "octaves_l_"<<i;
    out_octaves[LEFT][i] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,name.str().c_str());
    out_octaves[LEFT][i]->set(0);

    name.str("");
    name << "octaves_r_"<<i;
    out_octaves[RIGHT][i] = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,name.str().c_str());
    out_octaves[RIGHT][i]->set(0);
  }

  out_wave[LEFT] = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"wave_left");
  out_wave[RIGHT] = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"wave_right");
  out_spectrum[LEFT] = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum_left");
  out_spectrum[RIGHT] = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum_right");


  initializeBuffers();
  for (int i = 0; i < N_CHANNELS; i++ ){
    out_wave[i]->set_p(m_wave[0][i]);
    out_spectrum[i]->set_p(m_spectrum[0][i]);
  }

  //Now the params for the backwards compatibility
  hidden_out_wave = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"wave");
  hidden_out_spectrum = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum");
  hidden_out_spectrum_hq = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum_hq");
  hidden_out_wave->set_p(m_wave[0][LEFT]);
  hidden_out_spectrum->set_p(m_spectrum[0][LEFT]);
  hidden_out_spectrum_hq->set_p(m_spectrum[0][LEFT]);

  loading_done = true;
}

void input_audio_raw::run()
{
  flipBufferPage();
  int page = currentBufferPage();

  amp = engine->amp * 0.25f;

  for (int i = 0; i < N_CHANNELS; i++ ){
    out_vu[i]->set(m_vu[page][i]);
    out_wave[i]->set_p(m_wave[page][i]);
    out_spectrum[i]->set_p(m_spectrum[page][i]);
    for(int j = 0; j < N_OCTAVES; j++)
      out_octaves[i][j]->set(m_octaves[page][i][j]);
  }

  hidden_out_wave->set_p(m_wave[page][LEFT]);
  hidden_out_spectrum->set_p(m_spectrum[page][LEFT]);
  hidden_out_spectrum_hq->set_p(m_spectrum[page][LEFT]);

}
