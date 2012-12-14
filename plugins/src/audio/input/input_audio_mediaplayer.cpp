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
#include "input_audio_mediaplayer.h"

input_audio_mediaplayer::input_audio_mediaplayer():
  m_fft_machine(N_BUFFER_FRAMES)
{
  m_wave[LEFT].data = 0;
}

input_audio_mediaplayer::~input_audio_mediaplayer()
{

}

void input_audio_mediaplayer::module_info(vsx_module_info* info)
{
  info->output = 1;
  info->identifier = "sound;input_visualization_listener||system;sound;vsx_listener";
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


void input_audio_mediaplayer::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
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

  //Initialize Buffers
  m_wave[LEFT].data = new vsx_array<float>;
  m_wave[RIGHT].data = new vsx_array<float>;
  m_spectrum[LEFT].data = new vsx_array<float>;;
  m_spectrum[RIGHT].data = new vsx_array<float>;;

  for (int i = 0; i < N_CHANNELS; i++ )
    for(int j = 0; j < N_BUFFER_FRAMES; j++){
      m_wave[i].data->push_back(0);
      m_spectrum[i].data->push_back(0);
    }

  for (int i = 0; i < N_CHANNELS; i++ ){
    out_wave[i]->set_p(m_wave[i]);
    out_spectrum[i]->set_p(m_spectrum[i]);
  }

  //Now the params for the backwards compatibility
  hidden_out_wave = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"wave");
  hidden_out_spectrum = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum");
  hidden_out_spectrum_hq = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum_hq");
  hidden_out_wave->set_p(m_wave[LEFT]);
  hidden_out_spectrum->set_p(m_spectrum[LEFT]);
  hidden_out_spectrum_hq->set_p(m_spectrum[LEFT]);

  loading_done = true;
}

void input_audio_mediaplayer::on_delete()
{
  if(!m_wave[LEFT].data)
    return;

  delete m_wave[LEFT].data;
  delete m_wave[RIGHT].data;
  delete m_spectrum[LEFT].data;
  delete m_spectrum[RIGHT].data;

  m_wave[LEFT].data = 0;
}

void input_audio_mediaplayer::run()
{
  //Do nothing if there is nothing to compute
  if(!engine->param_float_arrays.size())
    return;

  float amplification = in_multiplier->get()*engine->amp*0.4f;

  //TODO: Fix engine to take in both left and right audio data channels
  vsx_engine_float_array* lv_wave_data = engine->param_float_arrays[0];
  
  //Copy the data from engine into internal buffers
  for( int i = 0; i < N_CHANNELS; i++ )
    for ( int j = 0; j < N_BUFFER_FRAMES; j++){
      m_tmp_wave[i][j] = (lv_wave_data->array[j])*amplification;
      (*(m_wave[i].data))[j] = m_tmp_wave[i][j];
      //m_vu[i] += m_tmp_wave[i][j]/nBufferFrames;
    }

  //Take the FFT
  m_fft_machine.do_fft(m_tmp_fft_samples[LEFT], m_tmp_wave[LEFT]);
  m_fft_machine.do_fft(m_tmp_fft_samples[RIGHT], m_tmp_wave[RIGHT]);

  //Caclulating the VU values
  float real, imaginary, vu;
  int n_fft_values = N_BUFFER_FRAMES/2;
  for( int i = 0; i < N_CHANNELS; i++ ){
    vu = 0;
    for ( int j = 0; j < n_fft_values; j++){
      real = m_tmp_fft_samples[i][j];
      imaginary = m_tmp_fft_samples[i][j+n_fft_values];
      m_tmp_fft_values[i][j] = (sqrt(real*real + imaginary*imaginary)/n_fft_values);
      vu += m_tmp_fft_values[i][j];
    }
    out_vu[i]->set(vu);
  }

  //Calculating the spectrum data
  for (int i = 0; i < N_CHANNELS; i++){
    for ( int j = 0; j < N_BUFFER_FRAMES; j++)
      (*(m_spectrum[i].data))[j] = m_tmp_fft_values[i][j/2] * 3.0 * pow(log( 10.0f + ((float)SAMPLE_RATE) * (j /(float)N_BUFFER_FRAMES)) ,1.0f);
    out_spectrum[i]->set_p(m_spectrum[i]);
  }

  //Calculating the Octave data
  float current_octave =0;
  int values_per_octave = N_BUFFER_FRAMES/N_OCTAVES;
  for (int i = 0; i < N_CHANNELS; i++)
    for ( int j = 0; j < N_OCTAVES; j++){
      current_octave = 0;
      for ( int k = 0; k < values_per_octave; k++)
        current_octave +=  (*(m_spectrum[i].data))[j*values_per_octave + k];
      current_octave /= values_per_octave;
      out_octaves[i][j]->set(current_octave);
    }

  //Backward compatibility values
  hidden_out_wave->set_p(m_wave[LEFT]);
  hidden_out_spectrum->set_p(m_spectrum[LEFT]);
  hidden_out_spectrum_hq->set_p(m_spectrum[LEFT]);

}
