#ifndef RTAUDIO_PLAY_H
#define RTAUDIO_PLAY_H

#include "vsx_audio_mixer.h"


// rt audio instance
RtAudio* padc_play = 0x0;

// reference counter
size_t rt_play_refcounter = 0;




// singleton main mixer
vsx_audio_mixer main_mixer;

int play_callback
(
  void *outputBuffer,
  void *inputBuffer,
  unsigned int nBufferFrames,
  double streamTime,
  RtAudioStreamStatus status,
  void *userData
)
{
  (void)inputBuffer;
  (void)streamTime;
  (void)userData;

  int16_t *buffer = (int16_t *) outputBuffer;

  if ( status )
    printf("Stream underflow detected!\n");

  //vsx_printf("buffer frames: %d latency: %d\n", nBufferFrames, padc_play->getStreamLatency());

  // Write interleaved audio data.
  for ( unsigned int i=0; i < nBufferFrames; i++ )
  {
    *buffer = (int16_t)main_mixer.consume_left();
    buffer++;
    *buffer = (int16_t)main_mixer.consume_right();
    buffer++;
  }
  return 0;
}

void setup_rtaudio_play()
{
  if (padc_play)
  {
    rt_play_refcounter++;
    return;
  }
  else
  {
    padc_play = new RtAudio((RtAudio::Api)rtaudio_type);
    rt_play_refcounter++;
    #if (PLATFORM == PLATFORM_WINDOWS)
    rt_play_refcounter++;
    #endif
  }

  if ( padc_play->getDeviceCount() < 1 )
  {
    printf("WARNING::::::::      No audio devices found!\n");
    return;
  }


  RtAudio::StreamParameters parameters;

  parameters.deviceId = padc_play->getDefaultInputDevice();
  parameters.nChannels = 2;
  parameters.firstChannel = 0;
  unsigned int sampleRate = 44100;
  unsigned int bufferFrames = 64;
  double data[2];

  //

  RtAudio::StreamOptions options;
  options.flags = RTAUDIO_MINIMIZE_LATENCY;
  options.streamName = "vsxu";

  try
  {
    padc_play->openStream
    (
      &parameters,
      NULL,
      RTAUDIO_SINT16,
      sampleRate,
      &bufferFrames,
      &play_callback,
      (void *)&data,
      &options
    );
    padc_play->startStream();
    padc_play->getStreamLatency();
  }
  catch ( RtError& e )
  {
    e.printMessage();
  }

}

void shutdown_rtaudio_play()
{
  if (!padc_play) return;
  if (rt_play_refcounter == 0) return;
  rt_play_refcounter--;

  if (rt_play_refcounter == 0)
  {
    try
    {
      // Stop the stream
      padc_play->stopStream();
    }
    catch (RtError& e)
    {
      e.printMessage();
    }

    if ( padc_play->isStreamOpen() )
      padc_play->closeStream();
    delete padc_play;
    padc_play = 0;
  }
}




#endif // RTAUDIO_PLAY_H
