#ifndef RTAUDIO_RECORD_H
#define RTAUDIO_RECORD_H

float fftbuf[1024];
size_t fftbuf_it = 0;


// rt audio instance
RtAudio* padc_record = 0x0;

// reference counter
size_t rt_record_refcounter = 0;

FFTReal* fftr = 0x0;




int record( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  (void)outputBuffer;
  (void)nBufferFrames;
  (void)streamTime;
  (void)userData;
  if ( status )
  {
    printf("Stream overflow detected!\n");
  }

  #if (PLATFORM == PLATFORM_LINUX)
    const char* cal = "sound.rtaudio";
    prctl(PR_SET_NAME,cal);
  #endif

  int16_t* buf = (int16_t*)inputBuffer;

  vsx_audio_record_buf* pa_d = &pa_audio_data;

  int j = 0;
  // nab left channel for spectrum data
  for (size_t i = 0; i < 512; i++)
  {
    const float &f = (float)buf[j] * one_div_32768;

    (*(pa_d->wave[0].data))[i] = f * pa_d->l_mul;
    fftbuf[fftbuf_it++] = f;
    j++;
    j++;
  }
  fftbuf_it = fftbuf_it % 1024;

  j = 1;

  for (size_t i = 0; i < 512; i++)
  {
    (*(pa_d->wave[1].data))[i] = (float)buf[j] * one_div_32768 * pa_d->l_mul;
    j++;
    j++;
  }
  // do some FFT's
  float spectrum[1024];
  float spectrum_dest[512];
  fftr->do_fft( (float*)&spectrum, (float*) &fftbuf[0]);
  float re, im;

  for(int ii = 0; ii < 256; ii++)
  {
    re = spectrum[ii];
    im = spectrum[ii + 256];
    spectrum_dest[ii] = (float)sqrt(re * re + im * im) * one_div_256 * pa_d->l_mul;
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
    (*(pa_d->spectrum[0].data))[ii] =
        spectrum_dest[ii >> 1]
                            *
                            3.0f
                            *
                            pow(
                              log(
                                10.0f
                                +
                                44100.0f * ( (float)ii * one_div_512 )
                                ),
                              1.0f
                              );
  }



  //normalize_fft( (float*)spectrum_dest, pa_d->spectrum[0]);

  /*for (size_t ii = 0; ii < 512; ii++)
  {
    //float f = log( 2.0f + 8.0f * (ii / 512.0f) );
    (*(pa_d->spectrum[0].data))[ii] *= pow(log( 2.0f + 8.0f * (ii / 512.0f)) ,3.0f);
  }*/


  const float one_div_50 = 1.0f / 50.0f;


  #define spec_calc(cur_val, start, offset) \
    cur_val = 0.0f;\
    for (int ii = start * 50 + offset; ii < (start+1)*50; ii++) {\
      cur_val += (*(pa_d->spectrum[0].data))[ii];\
    }\
    cur_val = (cur_val * one_div_50)

  spec_calc(pa_d->octaves[0][0], 0, 10);
  spec_calc(pa_d->octaves[0][1], 1, 0);
  spec_calc(pa_d->octaves[0][2], 2, 0);
  spec_calc(pa_d->octaves[0][3], 3, 0);
  spec_calc(pa_d->octaves[0][4], 4, 0);
  spec_calc(pa_d->octaves[0][5], 5, 0);
  spec_calc(pa_d->octaves[0][6], 6, 0);
  spec_calc(pa_d->octaves[0][7], 7, 0);

  #undef spec_calc

  return 0;
}



void setup_rtaudio_record()
{
  if (padc_record)
  {
    rt_record_refcounter++;
    return;
  }
  else
  {
    padc_record = new RtAudio((RtAudio::Api)rtaudio_type);
    fftr = new FFTReal(512);
    rt_record_refcounter++;
    #if (PLATFORM == PLATFORM_WINDOWS)
    rt_record_refcounter++;
    #endif
  }

  if ( padc_record->getDeviceCount() < 1 )
  {
    printf("WARNING::::::::      No audio devices found!\n");
    return;
  }

  vsx_audio_record_buf* pa_d = &pa_audio_data;

  pa_d->wave[0].data = new vsx_array<float>;
  pa_d->wave[1].data = new vsx_array<float>;
  for (int i = 0; i < 512; ++i) pa_d->wave[0].data->push_back(0);
  for (int i = 0; i < 512; ++i) pa_d->wave[1].data->push_back(0);

  pa_d->spectrum[0].data = new vsx_array<float>;
  pa_d->spectrum[1].data = new vsx_array<float>;
  for (int i = 0; i < 512; ++i) pa_d->spectrum[0].data->push_back(0);
  for (int i = 0; i < 512; ++i) pa_d->spectrum[1].data->push_back(0);


  RtAudio::StreamParameters parameters;
  parameters.deviceId = padc_record->getDefaultInputDevice();
  parameters.nChannels = 2;
  parameters.firstChannel = 0;
  unsigned int sampleRate = 44100;
  unsigned int bufferFrames = 512;

  RtAudio::StreamOptions options;
      options.streamName = "vsxu";

  try {
    padc_record->openStream(
      NULL,
      &parameters,
      RTAUDIO_SINT16,
      sampleRate,
      &bufferFrames,
      &record,
      NULL,
      &options
    );
    padc_record->startStream();
  }
  catch ( RtError& e ) {
    e.printMessage();
  }
}

void shutdown_rtaudio_record()
{
  if (!padc_record) return;
  if (rt_record_refcounter == 0) return;
  rt_record_refcounter--;

  if (rt_record_refcounter == 0)
  {
    try {
      // Stop the stream
      padc_record->stopStream();
    }
    catch (RtError& e) {
      e.printMessage();
    }

    if ( padc_record->isStreamOpen() ) padc_record->closeStream();
    delete padc_record;
    delete fftr;
    padc_record = 0;
  }
}


#endif // RTAUDIO_RECORD_H
