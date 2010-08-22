#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__)
  #include <windows.h>
  #include <conio.h>
#else
  //#include "wincompat.h"
#endif
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_float_array.h"
#include "main.h"
#include "vsx_math_3d.h"

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
  /*if (!fmod_init) {
    //printf("Initializing fmod...\n");
    if (!FSOUND_Init(44100, 32, FSOUND_INIT_ACCURATEVULEVELS ))
    {
      printf("Error!\n");
      printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));
      FSOUND_Close();
      //return false;
    }
    fmod_init = true;
  } */
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
  //SAudioData* dat = aa->getCurrentData(multiplier->get(),quality->get()+1);
  float l_mul = multiplier->get()*engine->amp * 0.4f;
  // set wave
  if (0 == engine->param_float_arrays.size())
  {
    int i;
    /*for (i = 0; i < 512; ++i) {
        (*(wave.data))[i] = (float)(rand()%1000) * 0.0005 * l_mul;
    }
    wave_p->set_p(wave);

    for (i = 0; i < 512; ++i) {
        (*(spectrum.data))[i] = (float)(rand()%1000) * 0.0005 * l_mul;
    }*/
  } else
  {
    vsx_engine_float_array* lv_wave_data = engine->param_float_arrays[0];
    vsx_engine_float_array* lv_freq_data = engine->param_float_arrays[1];


    for (i = 0; i < 512; ++i) {
        (*(wave.data))[i] = (*lv_wave_data).array[i] * l_mul;
    }
    wave_p->set_p(wave);

    for (i = 0; i < 512; ++i) {
        (*(spectrum.data))[i] = (*lv_freq_data).array[i] * l_mul;
    }
  }
  spectrum_p->set_p(spectrum);
  spectrum_p_hq->set_p(spectrum);
  //int start = 0;
  float cur_val = 0.0f;

  float vu = 0.0f;


#define spec_calc(obj, start) \
  for (i = start * 64; i < (start+1)*64; i++) {\
    cur_val += (*(spectrum.data))[ round((float)i * 0.5f) ];\
  }\
  cur_val = (cur_val / 64.0f);\
  vu += cur_val;\
  obj->set(cur_val);

  spec_calc(octaves_l_0_p, 0)
  spec_calc(octaves_l_1_p, 1)
  spec_calc(octaves_l_2_p, 2)
  spec_calc(octaves_l_3_p, 3)
  spec_calc(octaves_l_4_p, 4)
  spec_calc(octaves_l_5_p, 5)
  spec_calc(octaves_l_6_p, 6)
  spec_calc(octaves_l_7_p, 7)

  spec_calc(octaves_r_0_p, 0)
  spec_calc(octaves_r_1_p, 1)
  spec_calc(octaves_r_2_p, 2)
  spec_calc(octaves_r_3_p, 3)
  spec_calc(octaves_r_4_p, 4)
  spec_calc(octaves_r_5_p, 5)
  spec_calc(octaves_r_6_p, 6)
  spec_calc(octaves_r_7_p, 7)

  vu = 5.5f * vu / 8;

  vu_l_p->set(vu * l_mul);
  vu_r_p->set(vu * l_mul);


//printf("module_listener::run %d\n",__LINE__);
	/*vu_l_p->set(dat->vu[0] * l_mul);
	vu_r_p->set(dat->vu[1] * l_mul);
//	printf("module_listener::run %d\n",__LINE__);
  octaves_l_0_p->set(dat->octaveSpectrum[0][0]*l_mul);
  octaves_l_1_p->set(dat->octaveSpectrum[0][1]*l_mul);
  octaves_l_2_p->set(dat->octaveSpectrum[0][2]*l_mul);
  octaves_l_3_p->set(dat->octaveSpectrum[0][3]*l_mul);
  octaves_l_4_p->set(dat->octaveSpectrum[0][4]*l_mul);
  octaves_l_5_p->set(dat->octaveSpectrum[0][5]*l_mul);
  octaves_l_6_p->set(dat->octaveSpectrum[0][6]*l_mul);
  octaves_l_7_p->set(dat->octaveSpectrum[0][7]*l_mul);

  octaves_r_0_p->set(dat->octaveSpectrum[1][0]*l_mul);
  octaves_r_1_p->set(dat->octaveSpectrum[1][1]*l_mul);
  octaves_r_2_p->set(dat->octaveSpectrum[1][2]*l_mul);
  octaves_r_3_p->set(dat->octaveSpectrum[1][3]*l_mul);
  octaves_r_4_p->set(dat->octaveSpectrum[1][4]*l_mul);
  octaves_r_5_p->set(dat->octaveSpectrum[1][5]*l_mul);
  octaves_r_6_p->set(dat->octaveSpectrum[1][6]*l_mul);
  octaves_r_7_p->set(dat->octaveSpectrum[1][7]*l_mul);*/
  /*
  //printf("module_listener::run %d\n",__LINE__);
  if ((quality->get()+1) & 1) {
    for (int i = 0; i < 512; ++i) {
      fft[i] = dat->spectrum[0][i/2]*l_mul;
    }
//    printf("module_listener::run %d\n",__LINE__);
    normalize_fft(fft,spectrum);
    //printf("module_listener::run %d\n",__LINE__);
    spectrum_p->set_p(spectrum);
  }
  //printf("module_listener::run %d\n",__LINE__);

  if ((quality->get()+1) & 2) {
    for (int i = 0; i < 512; ++i) {
      fft[i] = dat->spectrum_512[0][i]*l_mul;
    }
    normalize_fft(fft,spectrum_hq);
    spectrum_p_hq->set_p(spectrum_hq);
  }*/
  //printf("module_listener::run %d\n",__LINE__);
  //printf("wave: %f\n",dat->complexSpectrum[0][12]);
  //float* fft = FSOUND_DSP_GetSpectrum();
//  FSOUND_SAMPLE* samp = FSOUND_GetCurrentSample(channel);
//  if (samp)
//  printf("sample length: %d\n",FSOUND_Sample_GetLength(samp));
}
};



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#if BUILDING_DLL
vsx_module* create_new_module(unsigned long module) {
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
#endif

