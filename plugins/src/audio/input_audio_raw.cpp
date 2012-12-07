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

#include "input_audio_raw.h"

input_audio_raw::input_audio_raw():
  m_fft_machine(N_BUFFER_FRAMES),
  m_buffer_frames(N_BUFFER_FRAMES),
  m_sample_rate(SAMPLE_RATE),
  vsx_module()
{
}

input_audio_raw::~input_audio_raw()
{
}

bool input_audio_raw::can_run()
{
  return m_adc.getDeviceCount() >= 1;
}

int input_audio_raw::record(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData)
{
  if ( status )
    std::cout << "Stream overflow detected!" << std::endl;
    // Do something with the data in the "inputBuffer" buffer.
    ((input_audio_raw*)userData)->read_data((double*)inputBuffer, nBufferFrames);

  return 0;
}

void input_audio_raw::read_data(double* buffer, int nBufferFrames)
{
  printf("Read %d frames\n",nBufferFrames);
}


bool input_audio_raw::init()
{
  RtAudio::StreamParameters params;
  params.deviceId = m_adc.getDefaultInputDevice();
  params.nChannels = N_CHANNELS;
  params.firstChannel = LEFT;

  try{
    m_adc.openStream( NULL, &params, RTAUDIO_FLOAT32, m_sample_rate,
                     &m_buffer_frames, &input_audio_raw::record, this );
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
    if(m_adc.isStreamOpen())
      m_adc.stopStream();
  }
  catch(RtError &e){
    message = (std::string("module||ERROR!")+e.getMessage()).c_str();
  }

  delete m_wave.data;
  delete m_spectrum.data;
  delete m_spectrum_hq.data;
}


void input_audio_raw::module_info(vsx_module_info* info)
{
  //TODO: Re create the input params based on the available devices
  info->output = 1;
  info->identifier = "audio;input;raw";
#ifndef VSX_NO_CLIENT
  info->description = "Simple fft runs at 86.13 fps\nHQ fft runs at 43.07 fps\nThe octaves are 0 = bass, 7 = treble";
  info->in_param_spec = "quality:enum?normal_only|high_only|both&help=\
`If you don't need both FFT's to run,\ndisable either of them here. It's a\nsomewhat CPU-intensive task to do\nthe FFT for both every frame. \n\
Default is to only run\nthe normal one.`\
,multiplier:float";
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
wave:float_array,normal:complex{spectrum:float_array},hq:complex{spectrum_hq:float_array}";
  info->component_class = "output";
#endif

}

void input_audio_raw::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  in_quality = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"quality");
  in_quality->set(0);

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

  out_wave = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"wave");
  out_spectrum = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum");
  out_spectrum_hq = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum_hq");

  m_wave.data = new vsx_array<float>;
  m_spectrum.data = new vsx_array<float>;
  m_spectrum_hq.data = new vsx_array<float>;
  
  for(int i = 0; i < N_BUFFER_FRAMES; i++){
    m_wave.data->push_back(0);
    m_spectrum.data->push_back(0);
    m_spectrum_hq.data->push_back(0);
  }
  out_wave->set_p(m_wave);
  out_spectrum->set_p(m_spectrum);
  out_spectrum_hq->set_p(m_spectrum_hq);
  
  loading_done = true;
}

void input_audio_raw::run()
{
}