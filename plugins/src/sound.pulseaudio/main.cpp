/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
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

#include "_configuration.h"
#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__)
  #include <windows.h>
  #include <conio.h>
#else
  //#include "wincompat.h"
#endif
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_float_array.h"
#include "vsx_math_3d.h"

#include "pulse/simple.h"
#include "pulse/error.h"
#include "pulse/gccmacro.h"
#include <pthread.h>
#include "fftreal/fftreal.h"
#include <unistd.h>

int thread_created = 0;
pthread_t         worker_t;
pthread_mutex_t signal_mutex;

typedef struct
{
    float l_mul;
    vsx_float_array spectrum[2];
    vsx_float_array wave[2];    // 512 L 512 R BANZAI!
    //float spectrum[2][512];
    float vu[2];
    float octaves[2][8];
} vsx_paudio_struct;

vsx_paudio_struct pa_audio_data;

  /*
  i = 0..n	1.3 ^ i	   1.3 ^ i - 1	  1.3^i-1 / 1.3^n	  (1.3^i-1 / 1.3^n) * (n-1) + 1
  0	        1	         0	            0	                1
  1	        1,3	       0,3	          0,02346344	      1,211170956
  2	        1,69	     0,69	          0,053965911	      1,4856932
  3	        2,197	     1,197	        0,093619124	      1,842572116
  4	        2,8561	   1,8561	        0,145168301	      2,306514707
  5	        3,71293	   2,71293	      0,212182231	      2,909640075
  6	        4,826809	 3,826809	      0,299300339	      3,693703054
  7	        6,2748517	 5,2748517	    0,412553881	      4,712984927
  8	        8,15730721 7,15730721	    0,559783485	      6,038051361
  9	       10,60449937 9,604499373    0,75118197        7,760637726
  10       13,7858491812,78584918	    1	                10
  */
void normalize_fft(float* fft, vsx_float_array& spectrum) {
  float B1 = pow(8.0,1.0/512.0); //1.004
  float dd = 1*(512.0/8.0);
  float a = 0;
  float b;
  float diff = 0;
  int aa;
  for (int i = 1; i < 512; ++i) {
    (*(spectrum.data))[i] = 0;
    b = (float)((pow(B1,(float)(i))-1.0)*dd);
    diff = b-a;
    aa = (int)floor(a);

    if((int)b == (int)a) {
      (*(spectrum.data))[i] = fft[aa]*3 * diff;
    }
    else
    {
      ++aa;
      (*(spectrum.data))[i] += fft[aa]*3 * (ceil(a) - a);
      while (aa != (int)b) {
        (*(spectrum.data))[i] += fft[aa]*3;
        ++aa;
      }
      (*(spectrum.data))[i] += fft[aa+1]*3 * (b - floor(b));
    }
    a = b;
  }
}

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************

float fft[512];
class vsx_listener : public vsx_module {
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

public:

void module_info(vsx_module_info* info)
{
  info->output = 1;
  info->identifier = "sound;input_visualization_listener||system;sound;vsx_listener";
#ifndef VSX_NO_CLIENT
  info->description = "Simple fft runs at 86.13 fps\n\
HQ fft runs at 43.07 fps\n\
The octaves are 0 = bass, 7 = treble";
  info->in_param_spec = "\
quality:enum?\
  normal_only|high_only|both&help=\
`\
If you don't need both FFT's to run,\n\
disable either of them here. It's a\n\
somewhat CPU-intensive task to do\n\
the FFT for both every frame. \n\
Default is to only run\n\
the normal one.`\
,multiplier:float\
";
  info->out_param_spec = "\
vu:complex{\
vu_l:float,\
vu_r:float\
},\
octaves:complex{\
  left:complex{\
    octaves_l_0:float,\
    octaves_l_1:float,\
    octaves_l_2:float,\
    octaves_l_3:float,\
    octaves_l_4:float,\
    octaves_l_5:float,\
    octaves_l_6:float,\
    octaves_l_7:float\
  },\
  right:complex{\
    octaves_r_0:float,\
    octaves_r_1:float,\
    octaves_r_2:float,\
    octaves_r_3:float,\
    octaves_r_4:float,\
    octaves_r_5:float,\
    octaves_r_6:float,\
    octaves_r_7:float\
  }\
},\
wave:float_array,\
normal:complex{spectrum:float_array},hq:complex{spectrum_hq:float_array}";
  info->component_class = "output";
#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  quality = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"quality");
  quality->set(0);

  multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"multiplier");
  multiplier->set(1);

  //////////////////

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
  wave.data = new vsx_array<float>;
  for (int i = 0; i < 512; ++i) wave.data->push_back(0);
  wave_p->set_p(wave);


  spectrum_p = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum");
  spectrum_p_hq = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum_hq");
  spectrum.data = new vsx_array<float>;
  spectrum_hq.data = new vsx_array<float>;
  for (int i = 0; i < 512; ++i) spectrum.data->push_back(0);
  for (int i = 0; i < 512; ++i) spectrum_hq.data->push_back(0);
  spectrum_p->set_p(spectrum);
  spectrum_p_hq->set_p(spectrum_hq);
  //printf("spectrum size0: %d\n",spectrum.data->size());


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
  pa_audio_data.l_mul = multiplier->get()*engine->amp;
  // set wave
  if (0 == engine->param_float_arrays.size())
  {
    wave_p->set_p(pa_audio_data.wave[0]);
  }
  spectrum_p->set_p(pa_audio_data.spectrum[0]);
  spectrum_p_hq->set_p(pa_audio_data.spectrum[0]);
  vu_l_p->set(pa_audio_data.vu[0]);
  vu_r_p->set(pa_audio_data.vu[1]);

  octaves_l_0_p->set(pa_audio_data.octaves[0][0]);
  octaves_l_1_p->set(pa_audio_data.octaves[0][1]);
  octaves_l_2_p->set(pa_audio_data.octaves[0][2]);
  octaves_l_3_p->set(pa_audio_data.octaves[0][3]);
  octaves_l_4_p->set(pa_audio_data.octaves[0][4]);
  octaves_l_5_p->set(pa_audio_data.octaves[0][5]);
  octaves_l_6_p->set(pa_audio_data.octaves[0][6]);
  octaves_l_7_p->set(pa_audio_data.octaves[0][7]);

  octaves_r_0_p->set(pa_audio_data.octaves[0][0]);
  octaves_r_1_p->set(pa_audio_data.octaves[0][1]);
  octaves_r_2_p->set(pa_audio_data.octaves[0][2]);
  octaves_r_3_p->set(pa_audio_data.octaves[0][3]);
  octaves_r_4_p->set(pa_audio_data.octaves[0][4]);
  octaves_r_5_p->set(pa_audio_data.octaves[0][5]);
  octaves_r_6_p->set(pa_audio_data.octaves[0][6]);
  octaves_r_7_p->set(pa_audio_data.octaves[0][7]);

}
};


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#include <unistd.h>
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

static int worker_signal;

void* worker(void *ptr)
{
  pa_sample_spec ss;
  ss.format = PA_SAMPLE_S16LE;
  ss.rate = 44100;
  ss.channels = 2;
  pa_simple *s = NULL;

  int error;
  int16_t buf[1024];
  float fftbuf[1024];
  size_t fftbuf_it = 0;

  FFTReal* fft = new FFTReal(512);

  vsx_paudio_struct* pa_d = (vsx_paudio_struct*)ptr;
  //int gc = 0;

  /* Create the recording stream */
  if (!(s = pa_simple_new(NULL, "vsxu", PA_STREAM_RECORD, NULL, "r", &ss, NULL, NULL, &error))) {
      //fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
      goto finish;
  }

  pa_d->wave[0].data = new vsx_array<float>;
  pa_d->wave[1].data = new vsx_array<float>;
  for (int i = 0; i < 512; ++i) pa_d->wave[0].data->push_back(0);
  for (int i = 0; i < 512; ++i) pa_d->wave[1].data->push_back(0);

  pa_d->spectrum[0].data = new vsx_array<float>;
  pa_d->spectrum[1].data = new vsx_array<float>;
  for (int i = 0; i < 512; ++i) pa_d->spectrum[0].data->push_back(0);
  for (int i = 0; i < 512; ++i) pa_d->spectrum[1].data->push_back(0);

  for (;;)
  {
      /* Record some data ... */
      if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
          //fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
          goto finish;
      }
      int j = 0;
      // nab left channel for spectrum data
      for (size_t i = 0; i < 512; i++)
      {
        const float &f = (float)buf[j] / 16384.0f;
        (*(pa_d->wave[0].data))[i] = f * pa_d->l_mul;
        fftbuf[fftbuf_it++] = f;
        j++;
        j++;
      }
      fftbuf_it = fftbuf_it % 1024;
      j = 1;
      for (size_t i = 0; i < 512; i++)
      {
        (*(pa_d->wave[1].data))[i] = (float)buf[j] / 16384.0f * pa_d->l_mul;
        j++;
        j++;
      }
      // do some FFT's
      float spectrum[1024];
      float spectrum_dest[512];
      fft->do_fft( (float*)&spectrum, (float*) &fftbuf[0]);
      float re, im;

      for(int ii = 0; ii < 256; ii++)
      {
        re = spectrum[ii];
        im = spectrum[ii + 256];
        spectrum_dest[ii] = (float)sqrt(re * re + im * im) / 256.0f * pa_d->l_mul;
      }

      // calc vu
      float vu = 0.0f;
      for (int ii = 0; ii < 256; ii++)
      {
        vu += spectrum_dest[ii];
      }
      pa_d->vu[0] = vu;
      pa_d->vu[1] = vu;

      for (size_t ii = 0; ii < 512; ii++)
      {
        (*(pa_d->spectrum[0].data))[ii] = spectrum_dest[ii >> 1] * 3.0f * pow(log( 10.0f + 44100.0f * (ii / 512.0f)) ,1.0f);
      }



      //normalize_fft( (float*)spectrum_dest, pa_d->spectrum[0]);

      /*for (size_t ii = 0; ii < 512; ii++)
      {
        //float f = log( 2.0f + 8.0f * (ii / 512.0f) );
        (*(pa_d->spectrum[0].data))[ii] *= pow(log( 2.0f + 8.0f * (ii / 512.0f)) ,3.0f);
      }*/




#define spec_calc(cur_val, start, offset) \
  cur_val = 0.0f;\
  for (int ii = start * 50 + offset; ii < (start+1)*50; ii++) {\
    cur_val += (*(pa_d->spectrum[0].data))[ii];\
  }\
  cur_val = (cur_val / 50.0f)

      spec_calc(pa_d->octaves[0][0], 0, 10);
      spec_calc(pa_d->octaves[0][1], 1, 0);
      spec_calc(pa_d->octaves[0][2], 2, 0);
      spec_calc(pa_d->octaves[0][3], 3, 0);
      spec_calc(pa_d->octaves[0][4], 4, 0);
      spec_calc(pa_d->octaves[0][5], 5, 0);
      spec_calc(pa_d->octaves[0][6], 6, 0);
      spec_calc(pa_d->octaves[0][7], 7, 0);

      //fft->do_fft( (float*)&(pa_d->spectrum[1]), (float*)&(pa_d->wave[1]) );



      //printf("%f\t\t%d\n", ((float*)ptr)[0],gc++);
      /* And write it to STDOUT */
      //if (loop_write(STDOUT_FILENO, buf, sizeof(buf)) != sizeof(buf)) {
          //fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
          //goto finish;
      //}
      //printf("worker signal: %d\n",worker_signal);
    pthread_mutex_lock(&signal_mutex);
    int signal_value = worker_signal;
    pthread_mutex_unlock(&signal_mutex);


    if (signal_value == 1) goto finish;
  }



finish:
  //printf("finishing up...\n");
  if (s)
  {
    pa_simple_free(s);
  }
  //printf("finishing up 2...\n");
  pthread_mutex_lock(&signal_mutex);
  worker_signal = 2;
  pthread_mutex_unlock(&signal_mutex);
  //printf("worker signal: %d\n",worker_signal);
  return 0;
}




//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
__declspec(dllexport) void on_unload_library();
}


vsx_module* create_new_module(unsigned long module) {
  if (!thread_created)
  {
    //printf("creating pulseaudio thread:\n");
    worker_signal = 0;
    pthread_mutex_init(&signal_mutex,NULL);
    pthread_create(&worker_t, NULL, &worker, (void*)&pa_audio_data);
    thread_created++;
  }
  switch (module) {
    case 0: return (vsx_module*)(new vsx_listener);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_listener*)m; break;
  }
}

unsigned long get_num_modules() {
  return 1;
}

void on_unload_library()
{
  if (thread_created)
  {
    pthread_mutex_lock(&signal_mutex);
    worker_signal = 1;
    pthread_mutex_unlock(&signal_mutex);
    void* ret;
    pthread_join(worker_t, &ret);
  }
}

