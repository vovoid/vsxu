#ifndef VSX_AUDIO_MIXER_MANAGER_H
#define VSX_AUDIO_MIXER_MANAGER_H

#include <container/vsx_ma_vector.h>
#include <audio/vsx_audio_mixer.h>

class vsx_audio_mixer_manager
{
public:

  static vsx_audio_mixer* get_instance();

};

#endif
