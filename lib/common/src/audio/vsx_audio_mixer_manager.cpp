#include <audio/vsx_audio_mixer_manager.h>

static vsx_audio_mixer vsx_audio_mixer_manager_singleton;


vsx_audio_mixer* vsx_audio_mixer_manager::get_instance()
{
  return &vsx_audio_mixer_manager_singleton;
}
