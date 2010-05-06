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
#include "main.h"
#include "vsx_math_3d.h"
#include "caudioanalyzer_fmod.h"
#include "fmod_holder.h"

bool fmod_init = false;

//CAudioAnalyzer aa;// = new CAudioAnalyzer();
CAudioAnalyzer *aa =0;
FMOD_SYSTEM           *fsystem;
	//new CAudioAnalyzer();


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


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}



class vsx_module_sound_stream_play : public vsx_module {
  float d_time, v_time;
  // in
	vsx_module_param_int* play_mode;
	vsx_module_param_int* time_mode;
	vsx_module_param_int* spectrum_enabled;
	vsx_module_param_resource* filename;
	vsx_module_param_float* fx_level;
	// out
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
	vsx_module_param_float_array* sample_l_p;
	vsx_module_param_float_array* sample_r_p;
	vsx_module_param_float_array* spectrum_p;
	vsx_module_param_float_array* octave_spectrum_p;
	vsx_module_param_float* st;
	// internal
  vsx_float_array spectrum;
  vsx_string cur_filename;
  int i_state;
  int wave_pos;
  FMOD_CREATESOUNDEXINFO exinfo;
  FMOD_SOUND            *sound;
  FMOD_CHANNEL          *channel;
  FMOD_RESULT            result;
  int                    key, driver, numdrivers, count;
  unsigned int           version;
  float *fft;
  vsx_engine_float_array full_pcm_data_l;
  vsx_engine_float_array full_pcm_data_r;
public:
  vsx_float_array sample_l;
  vsx_float_array sample_r;
  vsx_float_array sample_l_int;
  vsx_float_array sample_r_int;


void module_info(vsx_module_info* info)
{
  info->output = 1;

  info->identifier = "system;sound;stream_play";
#ifndef VSX_NO_CLIENT
  info->description = "Plays a stream (mp3/wav/xm).";
  info->in_param_spec = "filename:resource,\
time_mode:enum?async|sync&help=`If sync is set, the position of the\nsong will be that of the engine time\n- useful for demos and presentations.\nIf async, it ignores the engine time\nand you can play songs independent from the time.`,\
play_mode:enum?play|pause,\
spectrum_enabled:enum?no|yes,\
fx_level:float\
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
  spectrum:float_array,sample_l:float_array,sample_r:float_array";
  info->component_class = "output";
#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  fft = new float[512];
  // special for controlling the system time
  st = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"_st");
  st->set(-1.0f);

  play_mode = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"play_mode");
  play_mode->set(1);
  time_mode = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_mode");
  spectrum_enabled = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"spectrum_enabled");
  filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
  filename->set("");
	fx_level = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fx_level");
	fx_level->set(1.0f);
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
  spectrum_p = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"spectrum");
  sample_l_p = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"sample_l");
  sample_r_p = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"sample_r");
  spectrum.data = new vsx_array<float>;
  for (int i = 0; i < 512; ++i) spectrum.data->push_back(0);
  spectrum_p->set_p(spectrum);

  //printf("initializing sample data\n");
  sample_l.data = new vsx_array<float>;
  sample_r.data = new vsx_array<float>;
  sample_l_int.data = new vsx_array<float>;
  sample_r_int.data = new vsx_array<float>;
  //printf("fmod data %d\n", sample_l.data);
  for (int i = 0; i < 512; ++i) {
  	sample_l.data->push_back(1);
  	sample_r.data->push_back(1);
  }
  for (int i = 0; i < 2048; ++i) {
    sample_l_int.data->push_back(1);
    sample_r_int.data->push_back(1);
  }
  //printf("fmod data %d\n", sample_l.data);
  sample_l_p->set_p(sample_l);
  sample_r_p->set_p(sample_r);
  wave_pos = 0;
  channel = 0;
  //printf("spectrum size0: %d\n",spectrum.data->size());
}

bool init() {
  //printf("fmod module init\n");
  //channel = -1;
  //FSOUND_Stream_SetBufferSize(1000);

  i_state = VSX_ENGINE_STOPPED;
  return true;
}

void on_delete() {
  delete spectrum.data;
  if (sound) FMOD_Sound_Release(sound);
}

void run() {
  engine->param_float_arrays[2] = &full_pcm_data_l;
  engine->param_float_arrays[3] = &full_pcm_data_r;
  if (!fmod_init) {
    //printf("Initializing fmod...\n");
    result = FMOD_System_Create(&fsystem);
    ERRCHECK(result);

    result = FMOD_System_GetVersion(fsystem, &version);
    ERRCHECK(result);
    FMOD_System_SetOutput(fsystem, FMOD_OUTPUTTYPE_ALSA);
    //FMOD_System_SetSoftwareFormat(fsystem,48000,FMOD_SOUND_FORMAT_PCM16,2,0,FMOD_DSP_RESAMPLER_LINEAR);
    ERRCHECK(result);
    result = FMOD_System_SetDriver(fsystem, 0);
    ERRCHECK(result);
    result = FMOD_System_Init(fsystem, 32, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
    fmod_init = true;
  }
  //printf("fmod_%d\n",__LINE__);
  if (cur_filename != filename->get() || !loading_done) {
    cur_filename = filename->get();
    if (cur_filename != "")
    {
    //  printf("fmod_%d\n",__LINE__);
      if (!loading_done) {
        loading_done = true;
      } else {
        FMOD_Sound_Release(sound);
        sound = 0;
      }
      //printf("opening stream: %s\n",cur_filename.c_str());
      //printf("fmod LOADING stream_%d\n",__LINE__);
      vsxf_handle *fp;
      if ((fp = engine->filesystem->f_open(cur_filename.c_str(), "rb")) == NULL)
        return;
//      printf("fmod_%d\n",__LINE__);
      //printf("fmod %d\n",__LINE__);
      unsigned long fsize = engine->filesystem->f_get_size(fp);
      char* fdata = new char[fsize+1];
      unsigned long bread = engine->filesystem->f_read(fdata, engine->filesystem->f_get_size(fp), fp);
  //    printf("fmod_%d\n",__LINE__);
      //printf("fmod %d\n",__LINE__);
      if (bread == fsize) {
        //printf("fmod_%d\n",__LINE__);
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        //printf("fmod_%d\n",__LINE__);
        //printf("fmod %d\n",__LINE__);
        exinfo.length = fsize;
        //printf("fmod_%d\n",__LINE__);
        result = FMOD_System_CreateSound(fsystem, fdata, FMOD_2D | FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_ACCURATETIME, &exinfo, &sound);
        //printf("fmod_%d\n",__LINE__);

        // find length of sample in bytes
        unsigned int byte_length;
        FMOD_Sound_GetLength(sound, &byte_length, FMOD_TIMEUNIT_PCMBYTES);
        //printf("song length in bytes: %d\n", byte_length);
        void* sample_data;
        void* sample_data_ext;
        unsigned int len1, len2;
        FMOD_Sound_Lock(
          sound,
          0,
          byte_length,
          &sample_data,
          &sample_data_ext,
          &len1,
          &len2
        );
        if (len1 == byte_length)
        {
          //printf("length is matching! \n");
          signed short* sample_data_short = (signed short*)sample_data;
          size_t k = byte_length / sizeof(signed short);
          size_t index = 0;
          float divisor = 1.0f / 32768.0f;
          for (size_t i = 0; i < k; i+=2)
          {

            full_pcm_data_l.array[index] = sample_data_short[i  ] * divisor;
            full_pcm_data_r.array[index] = sample_data_short[i+1] * divisor;
            //printf("%f ", full_pcm_data_l.array[index]);
            index++;
          }
          //printf("index reached: %d\n", index);
        }

        FMOD_Sound_Unlock(
            sound,
            sample_data,
            sample_data_ext,
            len1,
            len2
        );





        ERRCHECK(result);
     //   printf("fmod_%d\n",__LINE__);
        //printf("fmod %d\n",__LINE__);
        if (time_mode->get() == 0) {
       //   printf("fmod_%d\n",__LINE__);
          result = FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound, 0, &channel);
          //printf("fmod_%d\n",__LINE__);
          //printf("fmod %d\n",__LINE__);
          ERRCHECK(result);
          //printf("fmod_%d\n",__LINE__);
          //channel = FSOUND_Stream_PlayEx(FSOUND_FREE, stream, NULL, TRUE);
          //FSOUND_SetPaused(channel,FALSE);
        }
      }
    }
  }

  if (time_mode->get() == 0) {
    switch (play_mode->get()) {
      case 0:
        //printf("fmod %d\n",__LINE__);
        FMOD_Channel_SetPaused(channel,false);
        //printf("fmod %d\n",__LINE__);
      break;
      case 1:
        //printf("fmod %d\n",__LINE__);
        FMOD_Channel_SetPaused(channel,true);
        //printf("fmod %d\n",__LINE__);
      break;
    }
  } else
  if (time_mode->get() == 1) {
    // sync mode
    if (engine->state != -1)
    if (i_state != engine->state) {
      //printf("fmod %d\n",__LINE__);
      i_state = engine->state;
      //printf("engine_state: %d\n", engine->state);
      if (i_state == VSX_ENGINE_STOPPED) {
        //printf("fmod %d\n",__LINE__);
        FMOD_Channel_SetPaused(channel, true);
        //printf("fmod %d\n",__LINE__);
      }
      else
      if (i_state == VSX_ENGINE_PLAYING) {
        if (!channel)
        {
          //printf("fmod %d\n",__LINE__);
          result = FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound, 0, &channel);
          ERRCHECK(result);
          //printf("fmod %d\n",__LINE__);
        }
        //printf("fmod %d\n",__LINE__);
        FMOD_Channel_SetPaused(channel,false);
        //printf("fmod %d\n",__LINE__);
      }
    }
    if (i_state == VSX_ENGINE_STOPPED && engine->dtime != 0) {
      st->set(-1.0f);
      if (channel) {
        //printf("fmod %d\n",__LINE__);
        if (engine->vtime == 0)
        FMOD_Channel_SetPosition(channel, (int)round(((engine->vtime)*1000.0)),FMOD_TIMEUNIT_MS );
//        FSOUND_Stream_SetTime(stream, (int)round(((engine->vtime)*1000.0)));
        else
        FMOD_Channel_SetPosition(channel, (int)round(((engine->vtime)*1000.0)),FMOD_TIMEUNIT_MS );
      }
    }
    if (i_state == VSX_ENGINE_PLAYING) {
      //printf("fmod %d channel: %d\n",__LINE__,channel);
      unsigned int chpos;
      if (FMOD_Channel_GetPosition(channel, &chpos, FMOD_TIMEUNIT_MS) == FMOD_OK)
      {
        //printf("fmod %d %d\n",__LINE__,chpos);
        float ft = (float)chpos;
        //printf("channel pos: %f\n",ft);
        if (ft != 0.0f)
        st->set((ft)/1000.0f);
        else
        st->set(-1.0f);
      }
    }
  }

  //printf("fmod %d\n",__LINE__);
  result = FMOD_System_GetSpectrum(fsystem, (float*)&fft[0],512, 0, FMOD_DSP_FFT_WINDOW_RECT);
  ERRCHECK(result);
  //float* fft = FSOUND_DSP_GetSpectrum();
  //normalize_fft((float*)&fft,spectrum);
  //printf("fmod %d\n",__LINE__);
  //printf("fmod data %d\n", sample_l.data);
  FMOD_System_GetWaveData(fsystem, sample_l_int.data->get_pointer(),2048, 0);
  //printf("fmod %d\n",__LINE__);
  FMOD_System_GetWaveData(fsystem, sample_r_int.data->get_pointer(),2048, 1);
  int i = 0;

  int wpos;
    wpos = 256 * (wave_pos%7);
  while (i < 512) {
    //printf("wpos: %d %d",wpos,i);
    //fflush(stdout);
    (*(sample_l.data))[i] = (*(sample_l_int.data))[wpos];
    (*(sample_r.data))[i] = (*(sample_r_int.data))[wpos];
    //sample_r.data[i] = sample_r_int.data[wpos];
    wpos++;
    i++;
  }
  wave_pos++;
  //printf("fmod %d\n",__LINE__);
  int j = 0;
  int k = 0;
  float sum = 0.0f;
  float vusum = 0.0f;
  for (int i = 0; i < 512; ++i) {
  	sum += (*(spectrum.data))[i];
  	vusum += (*(spectrum.data))[i];
  	++j;
  	if (j == 64) {
  		j = 0;
  		sum /= 64;
  		sum *= fx_level->get();
  		switch (k) {
  			case 0:
  				octaves_l_0_p->set(sum);
  				octaves_r_0_p->set(sum);
  				break;
  			case 1:
  				octaves_l_1_p->set(sum);
  				octaves_r_1_p->set(sum);
  				break;
  			case 2:
  				octaves_l_2_p->set(sum);
  				octaves_r_2_p->set(sum);
  				break;
  			case 3:
  				octaves_l_3_p->set(sum);
  				octaves_r_3_p->set(sum);
  				break;
  			case 4:
  				octaves_l_4_p->set(sum);
  				octaves_r_4_p->set(sum);
  				break;
  			case 5:
  				octaves_l_5_p->set(sum);
  				octaves_r_5_p->set(sum);
  				break;
  			case 6:
  				octaves_l_6_p->set(sum);
  				octaves_r_6_p->set(sum);
  				break;
  			case 7:
  				octaves_l_7_p->set(sum);
  				octaves_r_7_p->set(sum);
  				break;
  		}
  		++k;
  	}
  }

	vusum /= 512;
	vusum *= fx_level->get();
	vu_l_p->set(vusum);
	vu_r_p->set(vusum);

  spectrum_p->set_p(spectrum);
  sample_l_p->set_p(sample_l);
  sample_r_p->set_p(sample_r);
//  FSOUND_SAMPLE* samp = FSOUND_GetCurrentSample(channel);
//  if (samp)
//  printf("sample length: %d\n",FSOUND_Sample_GetLength(samp));
}
};

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************

class vsx_module_sound_sample_play : public vsx_module {
  float d_time, v_time;
  // in
  vsx_module_param_float* trigger;
  vsx_module_param_float* pitch;
  vsx_module_param_resource* filename;
  // out
  // internal
  vsx_string cur_filename;
  int i_state;
  bool first_play; // to find out base frequency
  float base_frequency;
  FMOD_CREATESOUNDEXINFO exinfo;
  FMOD_SOUND            *sound;
  FMOD_CHANNEL          *channel;
  FMOD_RESULT            result;
  int                    key, driver, numdrivers, count;
  float                    prev_trigger;
  unsigned int           version;

public:
  vsx_float_array sample_l;
  vsx_float_array sample_r;

void module_info(vsx_module_info* info)
{
  info->output = 1;

  info->identifier = "sound;sample_trigger";
#ifndef VSX_NO_CLIENT
  info->description = "Plays a sample, possibly multiple times (mp3/wav/xm etc.)";
  info->in_param_spec = "filename:resource,\
trigger:float,\
pitch:float\
";
  info->component_class = "output";
#endif
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  first_play = true;
  trigger = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"trigger");
  pitch = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"pitch");
  pitch->set(0);
  filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
  filename->set("");
  prev_trigger = 0;
  channel = 0;
}

bool init() {
  return true;
}

void on_delete() {
}

void run() {
  if (!fmod_init)
  {
    //printf("Initializing fmod...\n");
    result = FMOD_System_Create(&fsystem);
    ERRCHECK(result);

    result = FMOD_System_GetVersion(fsystem, &version);
    ERRCHECK(result);
    FMOD_System_SetOutput(fsystem, FMOD_OUTPUTTYPE_ALSA);
//    FMOD_System_SetDSPBufferSize(fsystem,2048,8);
    //FMOD_System_SetDSPBufferSize(fsystem,256,8);

    ERRCHECK(result);
    result = FMOD_System_SetDriver(fsystem, 0);
    ERRCHECK(result);
    result = FMOD_System_Init(fsystem, 32, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
    fmod_init = true;
  }

  if (cur_filename != filename->get() || !loading_done) {
    cur_filename = filename->get();
    if (cur_filename != "") {
      if (!loading_done) {
        loading_done = true;
      } else {
        FMOD_Sound_Release(sound);
        sound = 0;
      }
      //printf("opening stream: %s\n",cur_filename.c_str());
      vsxf_handle *fp;
      if ((fp = engine->filesystem->f_open(cur_filename.c_str(), "rb")) == NULL)
        return;
      //printf("fmod %d\n",__LINE__);
      unsigned long fsize = engine->filesystem->f_get_size(fp);
      char* fdata = new char[fsize+1];
      unsigned long bread = engine->filesystem->f_read(fdata, engine->filesystem->f_get_size(fp), fp);
      //printf("fmod %d\n",__LINE__);
      if (bread == fsize) {
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        //printf("fmod %d\n",__LINE__);
        exinfo.length = fsize;
        result = FMOD_System_CreateSound(fsystem, fdata, FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_CREATESAMPLE, &exinfo, &sound);
        //if (FMOD_OK != result)
        //{
//          printf("FMOD failed System_CreateSound: %d\n",res);
          //exit(0);
        //}
        ERRCHECK(result);
        //printf("fmod %d\n",__LINE__);
      }
    }
  }
  if (sound != 0)
  if (prev_trigger <= 0.0 && trigger->get() > 0.0)
  {
    //FMOD_CHANNEL *chan = 0;
    result = FMOD_System_PlaySound(fsystem, FMOD_CHANNEL_FREE, sound, 0, &channel);
    if (first_play) {
      FMOD_Channel_GetFrequency(channel,&base_frequency);
    }
   /* float pitch_val = pitch->get();
    if (pitch_val > 1.0f) pitch_val = 1.0f;
    if (pitch_val < -1.0f) pitch_val = -1.0f;
    float new_freq = base_frequency + base_frequency*0.2f + 1 * pitch_val*base_frequency;
    FMOD_Channel_SetFrequency(channel, new_freq);*/
    //printf("fmod %d\n",__LINE__);
    ERRCHECK(result);
    //channel = FSOUND_Stream_PlayEx(FSOUND_FREE, stream, NULL, TRUE);
    //FSOUND_SetPaused(channel,FALSE);
  }
  if (channel)
  {
    float pitch_val = pitch->get();
    if (pitch_val > 1.0f) pitch_val = 1.0f;
    if (pitch_val < -1.0f) pitch_val = -1.0f;
    if (pitch_val > 0.0f) pitch_val *= 1.5f;
    float new_freq = base_frequency + 0.6 * pitch_val*base_frequency;
    FMOD_Channel_SetFrequency(channel, new_freq);
  }
  prev_trigger = trigger->get();
}
};


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
  //printf("fmod module init\n");
/*  channel = -1;
#if defined(WIN32) || defined(_WIN64) || defined(__CYGWIN32__) || defined(__WATCOMC__)
    FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
#elif defined(__linux__)
    FSOUND_SetOutput(FSOUND_OUTPUT_OSS);
#endif
  FSOUND_SetDriver(0);
  FSOUND_Record_SetDriver(0);
  FSOUND_Stream_SetBufferSize(1000);
  FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(),TRUE);
//  i_state = VSX_ENGINE_STOPPED;
  sample = FSOUND_Sample_Alloc(FSOUND_FREE,44100*5,FSOUND_NORMAL,44100,255,128,255);
  FSOUND_Record_StartSample(sample,TRUE);
*/
  return true;
}

void on_delete() {
  delete spectrum.data;
}



//float vsx_module_sound_record_fft::amp = 1.0f;
void run() {
	if (aa == 0) aa = new CAudioAnalyzer();
  SAudioData* dat = aa->getCurrentData(1.0f,quality->get()+1);
  float l_mul = multiplier->get()*engine->amp;
  // set wave
  for (int i = 0; i < 512; ++i) {
    (*(wave.data))[i] = dat->wave[0][i]*l_mul;
  }
  wave_p->set_p(wave);


  float vu = 0.0f;
  if ((quality->get()+1) & 1) {
    for (int i = 0; i < 512; ++i) {
    	vu += dat->spectrum[0][i/2]*l_mul;
    	(*(spectrum.data))[i] = dat->spectrum[0][i/2] * l_mul * log(10.0f + 44100.0f * ( float)i / 512.0f) * l_mul;
    }
//    printf("module_listener::run %d\n",__LINE__);
    //normalize_fft(fft,spectrum);
    //printf("module_listener::run %d\n",__LINE__);
    spectrum_p->set_p(spectrum);
  }

#define spec_calc(cur_val, start) \
	cur_val = 0.0f;\
	for (int ii = start * 50; ii < (start+1)*50; ii++) {\
		cur_val += (*(spectrum.data))[ii];\
	}\
	cur_val = (cur_val) / 50.0f

//printf("module_listener::run %d\n",__LINE__);
	vu_l_p->set(vu);
	vu_r_p->set(vu);
  spec_calc(dat->octaveSpectrum[0][0],0);
  spec_calc(dat->octaveSpectrum[0][1],1);
  spec_calc(dat->octaveSpectrum[0][2],2);
  spec_calc(dat->octaveSpectrum[0][3],3);
  spec_calc(dat->octaveSpectrum[0][4],4);
  spec_calc(dat->octaveSpectrum[0][5],5);
  spec_calc(dat->octaveSpectrum[0][6],6);
  spec_calc(dat->octaveSpectrum[0][7],7);

//	printf("module_listener::run %d\n",__LINE__);
  octaves_l_0_p->set(dat->octaveSpectrum[0][0]);
  octaves_l_1_p->set(dat->octaveSpectrum[0][1]);
  octaves_l_2_p->set(dat->octaveSpectrum[0][2]);
  octaves_l_3_p->set(dat->octaveSpectrum[0][3]);
  octaves_l_4_p->set(dat->octaveSpectrum[0][4]);
  octaves_l_5_p->set(dat->octaveSpectrum[0][5]);
  octaves_l_6_p->set(dat->octaveSpectrum[0][6]);
  octaves_l_7_p->set(dat->octaveSpectrum[0][7]);
  octaves_r_0_p->set(dat->octaveSpectrum[0][0]);
  octaves_r_1_p->set(dat->octaveSpectrum[0][1]);
  octaves_r_2_p->set(dat->octaveSpectrum[0][2]);
  octaves_r_3_p->set(dat->octaveSpectrum[0][3]);
  octaves_r_4_p->set(dat->octaveSpectrum[0][4]);
  octaves_r_5_p->set(dat->octaveSpectrum[0][5]);
  octaves_r_6_p->set(dat->octaveSpectrum[0][6]);
  octaves_r_7_p->set(dat->octaveSpectrum[0][7]);

  //printf("module_listener::run %d\n",__LINE__);
  //printf("module_listener::run %d\n",__LINE__);

  if ((quality->get()+1) & 2) {
    for (int i = 0; i < 512; ++i) {
      fft[i] = dat->spectrum_512[0][i]*l_mul;
    }
    normalize_fft(fft,spectrum_hq);
    spectrum_p_hq->set_p(spectrum_hq);
  }
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
    case 0: return (vsx_module*)(new vsx_module_sound_stream_play);
    case 1: return (vsx_module*)(new vsx_listener);
    case 2: return (vsx_module*)(new vsx_module_sound_sample_play);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_sound_stream_play*)m; break;
    case 1: delete (vsx_listener*)m; break;
    case 2: delete (vsx_module_sound_sample_play*)m; break;
  }
}


unsigned long get_num_modules() {
  return 3;
}
#endif

