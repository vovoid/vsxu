#ifndef VSX_SAMPLE_H
#define VSX_SAMPLE_H

#include "vsx_audio_mixer_channel.h"

#define VSX_SAMPLE_STATE_STOPPED 0
#define VSX_SAMPLE_STATE_PLAYING 1

#define VSX_SAMPLE_MONO 1
#define VSX_SAMPLE_STEREO 2

#define ONE_DIV_32767 1.0/32767.0

// read-ahead samples, should be the same as buffer size
#define DRIFT 64.0

// no file operations in vorbis loader
#define STB_VORBIS_NO_STDIO 1

#define PB_LENGTH 16384.0f



class vsx_sample : public vsx_audio_mixer_channel
{
  double position;
protected:
  vsx_array<int16_t> data;
  double pitch_bend;
  double gain;
  int16_t prev_left_value;
  int state;
  int stereo_type;
  int rate;
  double play_bit;

public:

  vsx_sample()
  :
    position(0.0f),
    pitch_bend(0.0f),
    gain(1.0f),
    prev_left_value(0),
    state(VSX_SAMPLE_STATE_STOPPED),
    stereo_type(VSX_SAMPLE_MONO),
    rate(44100),
    play_bit(-1.0f)
  {
  }

  vsx_array<int16_t>* get_data()
  {
    return &data;
  }

  inline float get_position()
  {
    return position;
  }

  inline void set_position(float n)
  {
    position = n;
  }

  inline float get_pitch_bend()
  {
    return pitch_bend;
  }

  inline void set_pitch_bend(float n)
  {
    pitch_bend = n;
  }

  inline float get_gain()
  {
    return gain;
  }

  inline void set_gain(float n)
  {
    gain = n;
  }

  // should this be mixed by the mixer?
  virtual int is_active()
  {
    return get_state();
  }

  inline int get_state()
  {
    if (!state && play_bit > -1.0f)
      return 1;
    return state;
  }

  inline void play()
  {
    pitch_bend = 1.0f;
    state = VSX_SAMPLE_STATE_PLAYING;
  }

  inline void stop()
  {
    state = VSX_SAMPLE_STATE_STOPPED;
  }

  inline void rewind()
  {
    position = 0;
    state = VSX_SAMPLE_STATE_STOPPED;
  }

  inline void trigger()
  {
    position = 0;
    state = VSX_SAMPLE_STATE_PLAYING;
  }

  inline void goto_time(float t)
  {
    //vsx_printf("t: %f, rate: %f, stereo_type: %f\n", t, (float)rate, (float)stereo_type);
    position = t * (float)rate * (float)stereo_type;

    //vsx_printf("playbit in goto time: %f\n", play_bit);
    if (play_bit < 0.0 && state == VSX_SAMPLE_STATE_STOPPED)
    {
      play_bit = PB_LENGTH;
    }

  }

  inline float get_time()
  {
    return position / ((float)rate * (float)stereo_type);
  }


  inline void set_stereo_type(int n)
  {
    stereo_type = n;
  }

  virtual inline int16_t consume_left()
  {
    if (!data.size())
      return 0;

    if (state == VSX_SAMPLE_STATE_STOPPED)
    {
      if (play_bit > -1.0f)
      {
        play_bit -= 1.0f;
        float pp = position + (PB_LENGTH - play_bit) * (float)stereo_type ;
        return data[ round(pp) ];
      }
      return 0;
    }

    // move our sample pointer
    position += pitch_bend * (float)stereo_type;

    // safeguard
    if (position < 0.0f)
    {
      position = 0.0f;
      return 0;
    }

    // safeguard
    if (position > data.size()-(2.0 + DRIFT * stereo_type))
    {
      position = data.size()-(2.0 + (double)DRIFT * (double)stereo_type);
      return 0; // silence
    }

    // set value so that the consume_right
    // method can use it if we're a mono sample


    float i_pos = position + DRIFT * (double)stereo_type * (double)state;
    if (i_pos < 0.0f) i_pos = 0.0;

    float start_val = data[ floor(i_pos) ] * ONE_DIV_32767;
    float end_val = data[ ceil(i_pos) ] * ONE_DIV_32767;
    float factor = i_pos - floor(i_pos);
    float res = (factor) * end_val + (1.0 - factor) * start_val;




    prev_left_value = (int16_t) ( res * 32767.0f );
    return prev_left_value;
  }

  virtual inline int16_t consume_right()
  {
    if (!data.size())
      return 0;

    if (stereo_type == VSX_SAMPLE_MONO)
      return prev_left_value;

    if (state == VSX_SAMPLE_STATE_STOPPED)
      return 0;

    float i_pos = position + DRIFT * (double)stereo_type * (double)state;
    if (i_pos < 0.0f) i_pos = 0.0;

    float start_val = data[ floor(i_pos) + 1.0f ] * ONE_DIV_32767;
    float end_val = data[ ceil(i_pos) + 1.0f ] * ONE_DIV_32767;
    float factor = i_pos - floor(i_pos);
    float res = (factor) * end_val + ( 1.0 - factor) * start_val;


    return
      (int16_t) ( res * 32767.0f );
  }


  virtual void load_filename(vsx_string filename) = 0;

};




#endif
