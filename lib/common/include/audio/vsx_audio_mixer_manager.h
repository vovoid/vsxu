#ifndef VSX_AUDIO_MIXER_MANAGER_H
#define VSX_AUDIO_MIXER_MANAGER_H

#include <container/vsx_ma_vector.h>
#include <audio/vsx_audio_mixer.h>

#include <vsx_common_dllimport.h>

class vsx_audio_mixer_manager
{
public:

  COMMON_DLLIMPORT static vsx_audio_mixer* get_instance();

};

#endif
