#include <fmod.h>
#include <fmod_errors.h>
#include <math.h>
#include "caudioanalyzer_fmod.h"
#include "fmod_holder.h"
#include <stdio.h>
#include <string.h>
//*************************************************************
//*************************************************************
//*************************************************************


CAudioAnalyzer::CAudioAnalyzer() {

  if (!fmod_init)
  {
	printf("fmod system init Caudioanalyzer\n");
    result = FMOD_System_Create(&fsystem);
    ERRCHECK(result);

    result = FMOD_System_GetVersion(fsystem, &version);
    ERRCHECK(result);
    fmod_init = true;
  }
  //printf("fmod init %d\n",__LINE__);
  if (version < FMOD_VERSION)
  {
    printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
    return;
  }
#ifdef _WIN32
  FMOD_System_SetOutput(fsystem, FMOD_OUTPUTTYPE_AUTODETECT);
  //result = FMOD_System_GetRecordNumDrivers(system, &numdrivers);
      /*ERRCHECK(result);

      printf("---------------------------------------------------------\n");
      printf("Choose a RECORD driver\n");
      printf("---------------------------------------------------------\n");
      for (count=0; count < numdrivers; count++)
      {
          char name[256];

          result = FMOD_System_GetRecordDriverInfo(system, count, name, 256, 0);
          ERRCHECK(result);

          printf("%d : %s\n", count + 1, name);
      }
*/
  //result = FMOD_System_SetRecordDriver(system, 0);  // default input is recorded from
#endif
#ifdef _linux
  //FMOD_System_SetOutput(fsystem, FMOD_OUTPUTTYPE_AUTODETECT);
  FMOD_System_SetOutput(fsystem, FMOD_OUTPUTTYPE_ALSA);
  int num_drivers;
  FMOD_System_GetRecordNumDrivers(
   fsystem,
   &num_drivers
 );
  printf("number of drivers: %d\n", num_drivers);

  for (int i = 0; i < num_drivers; i++)
  {
  char driver_name[256];
  FMOD_System_GetRecordDriverInfo(
    fsystem,
    i,
    driver_name,
    256,
    0
  );
  printf("driver name: %s\n", driver_name);
}

  //FMOD_System_SetOutput(fsystem, FMOD_OUTPUTTYPE_ESD);
  // next line is testing pulseAudio driver
  //result = FMOD_System_SetRecordDriver(fsystem, 0);  // default input is recorded from
#endif
  ERRCHECK(result);
  try {
  result = FMOD_System_Init(fsystem, 32, FMOD_INIT_NORMAL, NULL);
  }
  catch(...) {
	printf("FMOD system init failed!\n");
  }
  //printf("fmod init %d\n",__LINE__);
  ERRCHECK(result);
  memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));

  exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
  exinfo.numchannels      = 2;
  exinfo.format           = FMOD_SOUND_FORMAT_PCM16;
  //printf("fmod init %d\n",__LINE__);
  //FMOD_CAPS rcap;
  //int minfreq;
  //int maxfreq;
  //int num_driv;
  //FMOD_System_GetNumDrivers(fsystem, &num_driv);
  //FMOD_System_GetRecordDriverCaps(fsystem, 2, &rcap, &minfreq, &maxfreq);
  //printf("maxfreq: %d, %d\n",num_driv, minfreq);

#ifdef _WIN32
  exinfo.defaultfrequency = 44100;
#endif
#ifdef _linux
  exinfo.defaultfrequency = 48000;
#endif
  exinfo.length = sizeof(short int) * exinfo.defaultfrequency * 2;
  result = FMOD_System_CreateSound(fsystem, 0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &sound);
  ERRCHECK(result);
  //printf("fmod init %d\n",__LINE__);
  result = FMOD_System_RecordStart(fsystem, 0, sound, true);
  ERRCHECK(result);
  //printf("fmod init %d\n",__LINE__);
  pFFTObject512 = new FFTReal(512);
  pFFTObject1024 = new FFTReal(1024);
  //printf("fmod init %d\n",__LINE__);
  lastrecordpos = 0;
}


//*************************************************************
//*************************************************************
//*************************************************************


CAudioAnalyzer::~CAudioAnalyzer() {
  /*result = FMOD_System_RecordStop(fsystem);
  ERRCHECK(result);

  result = FMOD_Sound_Release(sound);
  ERRCHECK(result);

  result = FMOD_System_Release(fsystem);
  ERRCHECK(result);*/
  //printf("a");

  //waveInStop(hWaveIn);
	/*MM_ERR(
		waveInReset(hWaveIn);
	)*/
	//printf("b");
	//MM_ERR(
//		waveInClose(hWaveIn);
//	)
	//printf("c");

	//delete pFFTObject;

	//printf("e");
	/*while(!lstWaveHdrs->empty()) {
		LPWAVEHDR pHdr = lstWaveHdrs->back();

		lstWaveHdrs->pop_back();
		delete pHdr->lpData;
		delete pHdr;
	}*/
}


//*************************************************************
//*************************************************************
//*************************************************************


SAudioData* CAudioAnalyzer::getCurrentData(float inputLevel,int quality) {

  result = FMOD_Sound_Lock(sound, 0, sizeof(short int) * exinfo.defaultfrequency * 2, (void**)&read_pointer1, (void**)&read_pointer2, &read_bytes1, &read_bytes2);
  FMOD_System_GetRecordPosition(fsystem, 0, &lastrecordpos);
  lastrecordpos *= 2;
  //printf("sample data: %d\n",read_pointer1[lastrecordpos]);
  ERRCHECK(result);
  //printf("read bytes 1 %d\n", read_bytes1);
  //printf("lastrecordpos: %d\n",lastrecordpos);
  // now shovel the data into the audioData arrays
  static float scale = inputLevel / (float)(1 << (BITS_PER_SAMPLE - 1));
  //printf("scale: %f\n",scale);
	static int ch, i;
  static int pos = 0;// = lastrecordpos-BUFFER_SIZE * 2;
  //
  for (ch = 0; ch < 2; ch++)
  {
    pos = lastrecordpos-(BUFFER_SIZE) * 2+ch;
    //pos = lastrecordpos+ch;
//    pos = ch;// = lastrecordpos-BUFFER_SIZE * 2;
    if (pos < 0) pos += exinfo.defaultfrequency * 2;
    //if (ch == 0)
    int pass_1 = pos+(BUFFER_SIZE*2);
    int pass_2 = 0;
    if (pass_1 >  exinfo.defaultfrequency * 2)
    pass_2 = pass_1 %  exinfo.defaultfrequency * 2;
    if (pass_2) pass_1 = exinfo.defaultfrequency * 2-pass_2;
//    printf("pass 1: %d\n", pass_1);
    //printf("pass 2: %d\n", pass_2);

    int i_pointer = 0;
    //printf("pos: %d %d %d\n", pos, pass_1, pass_2);
    while (pos < pass_1)
    {
    	//if (i_pointer < 10) printf("%d ",read_pointer1[pos]);
      audioData.wave[ch][i_pointer++] = (float)read_pointer1[pos] * scale;
      pos+=2;
    }
    pos = 0;
    while (pos < pass_2)
    {
    	//if (i_pointer < 10) printf("%d ",read_pointer1[pos]);
      audioData.wave[ch][i_pointer++] = (float)read_pointer1[pos] * scale;
      pos+=2;
    }
  }

    /*pos = lastrecordpos;
    for (i = 0; i < BUFFER_SIZE; i++)
    {
      for (ch = 0; ch < 2; ch++)
      {
      audioData.wave[ch][i] = (float)read_pointer1[pos] * scale;
      pos++;
      pos = pos % 2048; // keep it within 0-2047
    }
  }*/

  //pos = lastrecordpos;
  /*for (i = 0; i < BUFFER_SIZE*2; i++)
  for (ch = 0; ch < 2; ch++)
  {
	  audioData.wave_1024[ch][i] = (float)read_pointer1[pos] * scale;
	  pos++;
	  pos = pos % 2048; // keep it within 0-2047
	}*/
  FMOD_Sound_Unlock(sound, read_pointer1, read_pointer2, read_bytes1, read_bytes2);
  if (quality & 1) {
    //printf("running fft qual 1\n");
		for(ch = 0; ch < CHANNELS; ch++) {
			pFFTObject512->do_fft(
				(float*)(void*)audioData.complexSpectrum[ch],
				(float*)(void*)audioData.wave[ch]);
			float re, im;
			for(i = 0; i < BUFFER_SIZE / 2; i++) {
				re = audioData.complexSpectrum[ch][i];
				im = audioData.complexSpectrum[ch][i + (BUFFER_SIZE >> 1)];
				audioData.spectrum[ch][i] = (float)sqrt(re * re + im * im) / (BUFFER_SIZE / 2);
			}
		}


		/*for(ch = 0; ch < CHANNELS; ch++) {
			audioData.vu[ch] = 0;
			for(i = 0; i < OCTAVE_SPECTRUM_SIZE; i++) {
				audioData.octaveSpectrum[ch][i] = 0.0f;
				for(int j = (1 << i); j < (1 << (i + 1)); j++) {
					audioData.octaveSpectrum[ch][i] += audioData.spectrum[ch][j];
				}
				audioData.vu[ch] += audioData.octaveSpectrum[ch][i];
			}
		}*/
  }
  if (quality & 2) {  // high quality
		for(ch = 0; ch < CHANNELS; ch++) {
			pFFTObject1024->do_fft(
				(float*)(void*)audioData.complexSpectrum_1024[ch],
				(float*)(void*)audioData.wave_1024[ch]);

			float re, im;
			for(i = 0; i < BUFFER_SIZE; i++) {
				re = audioData.complexSpectrum_1024[ch][i];
				im = audioData.complexSpectrum_1024[ch][i + (BUFFER_SIZE*2 >> 1)];
				audioData.spectrum_512[ch][i] = (float)sqrt(re * re + im * im) / (BUFFER_SIZE);
			}
		}
		for(ch = 0; ch < CHANNELS; ch++) {
			audioData.vu[ch] = 0;
			for(i = 0; i < OCTAVE_SPECTRUM_SIZE; i++) {
				audioData.octaveSpectrum[ch][i] = 0.0f;
				for(int j = (1 << i); j < (1 << (i + 1)); j++) {
					audioData.octaveSpectrum[ch][i] += audioData.spectrum_512[ch][j*2];
				}
				audioData.vu[ch] += audioData.octaveSpectrum[ch][i];
			}
		}
  }

	return &audioData;
}


//*************************************************************
//*************************************************************
//*************************************************************

