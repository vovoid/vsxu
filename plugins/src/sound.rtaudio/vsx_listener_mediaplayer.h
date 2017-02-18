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

class vsx_listener_mediaplayer : public vsx_module
{
  // in
  vsx_module_param_int* quality;

  // out
  vsx_module_param_float* multiplier;
  float old_mult;

  vsx_module_param_float* vu_l_p;
  vsx_module_param_float* vu_r_p;
  vsx_module_param_float* octaves_l_0_p;
  vsx_module_param_float* octaves_l_1_p;
  vsx_module_param_float* octaves_l_2_p;
  vsx_module_param_float* octaves_l_3_p;
  vsx_module_param_float* octaves_l_4_p;
  vsx_module_param_float* octaves_l_5_p;
  vsx_module_param_float* octaves_l_6_p;
  vsx_module_param_float* octaves_l_7_p;
  vsx_module_param_float* octaves_r_0_p;
  vsx_module_param_float* octaves_r_1_p;
  vsx_module_param_float* octaves_r_2_p;
  vsx_module_param_float* octaves_r_3_p;
  vsx_module_param_float* octaves_r_4_p;
  vsx_module_param_float* octaves_r_5_p;
  vsx_module_param_float* octaves_r_6_p;
  vsx_module_param_float* octaves_r_7_p;
  vsx_module_param_float_array* wave_p;
  vsx_float_array wave;

  // normal engine
  vsx_float_array spectrum;
  vsx_module_param_float_array* spectrum_p;
  vsx_float_array octave_spectrum;
  vsx_module_param_float_array* octave_spectrum_p;

  // hq engine
  vsx_float_array spectrum_hq;
  vsx_module_param_float_array* spectrum_p_hq;
  vsx_float_array octave_spectrum_hq;
  vsx_module_param_float_array* octave_spectrum_p_hq;
  float fftbuf[1024];
  size_t fftbuf_it;
  FFTReal* fft;

public:

void module_info(vsx_module_specification* info)
{
  info->identifier =
    "sound;input_visualization_listener"
    "||"
    "system;sound;vsx_listener"
  ;

  info->description =
    "Simple fft runs at 86.13 fps\n"
    "HQ fft runs at 43.07 fps\n"
    "The octaves are 0 = bass, 7 = treble"
  ;

  info->in_param_spec =
    "quality:enum?normal_only|high_only|both"
    "&help="
      "`"
        "If you don't need both FFT's to run,\n"
        "disable either of them here. It's a\n"
        "somewhat CPU-intensive task to do\n"
        "the FFT for both every frame. \n"
        "Default is to only run\n"
        "the normal one."
      "`"
    ","
    "multiplier:float"
  ;

  info->out_param_spec =
    "vu:complex"
    "{"
      "vu_l:float,"
      "vu_r:float"
    "},"
    "octaves:complex"
    "{"
      "left:complex"
      "{"
        "octaves_l_0:float,"
        "octaves_l_1:float,"
        "octaves_l_2:float,"
        "octaves_l_3:float,"
        "octaves_l_4:float,"
        "octaves_l_5:float,"
        "octaves_l_6:float,"
        "octaves_l_7:float"
      "},"
      "right:complex"
      "{"
        "octaves_r_0:float,"
        "octaves_r_1:float,"
        "octaves_r_2:float,"
        "octaves_r_3:float,"
        "octaves_r_4:float,"
        "octaves_r_5:float,"
        "octaves_r_6:float,"
        "octaves_r_7:float"
      "}"
    "},"
    "wave:float_array,"
    "normal:complex"
    "{"
      "spectrum:float_array"
    "},"
    "hq:complex"
    "{"
      "spectrum_hq:float_array"
    "}"
  ;

  info->component_class = "output";

  info->output = 1;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  quality = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"quality");
  quality->set(0);

  multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"multiplier");
  multiplier->set(1);

  vu_l_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"vu_l");
  vu_r_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"vu_r");
  vu_l_p->set(0);
  vu_r_p->set(0);
  octaves_l_0_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_0");
  octaves_l_1_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_1");
  octaves_l_2_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_2");
  octaves_l_3_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_3");
  octaves_l_4_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_4");
  octaves_l_5_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_5");
  octaves_l_6_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_6");
  octaves_l_7_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_l_7");
  octaves_r_0_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_0");
  octaves_r_1_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_1");
  octaves_r_2_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_2");
  octaves_r_3_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_3");
  octaves_r_4_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_4");
  octaves_r_5_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_5");
  octaves_r_6_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_6");
  octaves_r_7_p = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"octaves_r_7");
  octaves_l_0_p->set(0);
  octaves_l_1_p->set(0);
  octaves_l_2_p->set(0);
  octaves_l_3_p->set(0);
  octaves_l_4_p->set(0);
  octaves_l_5_p->set(0);
  octaves_l_6_p->set(0);
  octaves_l_7_p->set(0);
  octaves_r_0_p->set(0);
  octaves_r_1_p->set(0);
  octaves_r_2_p->set(0);
  octaves_r_3_p->set(0);
  octaves_r_4_p->set(0);
  octaves_r_5_p->set(0);
  octaves_r_6_p->set(0);
  octaves_r_7_p->set(0);
  wave_p = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"wave");
  wave.data = new vsx_ma_vector<float>;
  for (int i = 0; i < 512; ++i) wave.data->push_back(0);
  wave_p->set_p(wave);


  spectrum_p = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum");
  spectrum_p_hq = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum_hq");
  spectrum.data = new vsx_ma_vector<float>;
  spectrum_hq.data = new vsx_ma_vector<float>;
  for (int i = 0; i < 512; ++i) spectrum.data->push_back(0);
  for (int i = 0; i < 512; ++i) spectrum_hq.data->push_back(0);
  spectrum_p->set_p(spectrum);
  spectrum_p_hq->set_p(spectrum_hq);
  //printf("spectrum size0: %d\n",spectrum.data->size());
  fftbuf_it = 0;
  fft = new FFTReal(512);
  loading_done = true;
}

bool init() {
  return true;
}

void on_delete() {
  delete spectrum.data;
}

int echo_log(const char* message, int a) {
  FILE* fp = fopen("/tmp/vsxu_libvisual.log", "a");
  fprintf(fp,"echo %s, %d\n", message ,a);
  fclose(fp);
  return 5;
}

int i;

  void run() {
    float l_mul = multiplier->get()*engine_state->amp*0.4f;
    // set wave
    if (0 == engine_state->param_float_arrays.size())
    {
      // enable to test using random data when not getting sound
      //int i;
      /*for (i = 0; i < 512; ++i) {
          (*(wave.data))[i] = (float)(rand()%1000) * 0.0005 * l_mul;
      }
      wave_p->set_p(wave);

      for (i = 0; i < 512; ++i) {
          (*(spectrum.data))[i] = (float)(rand()%1000) * 0.0005 * l_mul;
      }*/
    } else
    {
      vsx_module_engine_float_array* lv_wave_data = engine_state->param_float_arrays[0];
      //vsx_module_engine_float_array* lv_freq_data = engine_state->param_float_arrays[1];

      // Process incoming wave data
      for (i = 0; i < 512; ++i)
      {
        const float &f = (*lv_wave_data).array[i];
        // add to wave buffer
        (*(wave.data))[i] =  f * l_mul;
        // add to spectrum buffer
        fftbuf[fftbuf_it++] = f;
      }
      wave_p->set_p(wave);

      // Spectrum analysis
      fftbuf_it = fftbuf_it % 1024;
      // do some FFT's
      float dspectrum[1024];
      float spectrum_dest[512];
      fft->do_fft( (float*)&dspectrum, (float*) &fftbuf[0]);
      float re, im;

      for(int ii = 0; ii < 256; ii++)
      {
        re = dspectrum[ii];
        im = dspectrum[ii + 256];
        spectrum_dest[ii] = (float)sqrt(re * re + im * im) / 256.0f * l_mul;
      }

      // calc vu
      float vu = 0.0f;
      for (int ii = 0; ii < 256; ii++)
      {
        vu += spectrum_dest[ii];
      }
      vu_l_p->set(vu);
      vu_r_p->set(vu);

      for (size_t ii = 0; ii < 512; ii++)
      {
        (*(spectrum.data))[ii] = spectrum_dest[ii >> 1] * 3.0f * pow(log( 10.0f + 44100.0f * (ii / 512.0f)) ,1.0f);
      }

    }
    spectrum_p->set_p(spectrum);
    spectrum_p_hq->set_p(spectrum);

    // calculate octaves
    float cur_val = 0.0f;
    #define spec_calc(obj, start,offset) \
      cur_val = 0.0f; \
      for (i = start * 50 + offset; i < (start+1)*50; i++) {\
        cur_val += (*(spectrum.data))[ round((float)i * 0.5f) ];\
      }\
      cur_val = (cur_val / 50.0f);\
      obj->set(cur_val);
    //#

    spec_calc(octaves_l_0_p, 0,10)
    spec_calc(octaves_l_1_p, 1,0)
    spec_calc(octaves_l_2_p, 2,0)
    spec_calc(octaves_l_3_p, 3,0)
    spec_calc(octaves_l_4_p, 4,0)
    spec_calc(octaves_l_5_p, 5,0)
    spec_calc(octaves_l_6_p, 6,0)
    spec_calc(octaves_l_7_p, 7,0)

    #undef spec_calc

    octaves_r_0_p->set(octaves_l_0_p->get());
    octaves_r_1_p->set(octaves_l_1_p->get());
    octaves_r_2_p->set(octaves_l_2_p->get());
    octaves_r_3_p->set(octaves_l_3_p->get());
    octaves_r_4_p->set(octaves_l_4_p->get());
    octaves_r_5_p->set(octaves_l_5_p->get());
    octaves_r_6_p->set(octaves_l_6_p->get());
    octaves_r_7_p->set(octaves_l_7_p->get());
  }
};

