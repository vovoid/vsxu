#ifndef VSX_AUDIO_MIXER_CHANNEL_H
#define VSX_AUDIO_MIXER_CHANNEL_H

// interface class for an audio channel / audio source
// Register this in the mixer like so:
// mixer->

class vsx_audio_mixer_channel
{
protected:
  double gain;

public:
  // is the source active?
  // mixer reads this: if not, will not be mixed
  // (mainly to improve performance)
  virtual int is_active() = 0;

  // cannel gain control
  virtual float get_gain() = 0;
  virtual void set_gain(float n) = 0;

  // called by the mixer, returns one sample per call
  virtual int16_t consume_left() = 0;
  virtual int16_t consume_right() = 0;

};

#endif
