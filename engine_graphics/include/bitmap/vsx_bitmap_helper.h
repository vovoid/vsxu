#ifndef VSX_BITMAP_HELPER_H
#define VSX_BITMAP_HELPER_H

#include "vsx_bitmap.h"

namespace vsx_bitmap_helper
{

  void maintain_same_size(vsx_bitmap& source, vsx_bitmap& destination)
  {
    req((source.width != destination.width) || (source.height != destination.height));

    for (size_t i = 0; i < 6; i++)
    {
      if (destination.data[i] && !source.data[i])
      {
        free(destination.data[i]);
        destination.data[i] = 0x0;
        continue;
      }

      if (!source.data[i])
        continue;

      if (destination.data[i] && source.data[i])
      {
        destination.data[i] = realloc(destination.data[i], source.get_channel_size() * source.width * source.height);
        continue;
      }

      destination.data[i] = malloc(source.get_channel_size() * source.width * source.height);
    }
  }


  void copy(vsx_bitmap& source, vsx_bitmap& destination)
  {
    maintain_same_size(source, destination);
    destination.width = source.width;
    destination.height = source.height;
    destination.depth = source.depth;
    destination.alpha = source.alpha;
    destination.channels = source.channels;
    destination.channels_bgra = source.channels_bgra;
    destination.storage_format = source.storage_format;
    destination.compressed_data = source.compressed_data;
    destination.attached_to_cache = false;
    destination.references = 0;
    for (size_t i = 0; i < 6; i++)
    {
      if (!source.data[i])
        continue;

      memcpy(destination.data[i], source.data[i], source.get_channel_size() * source.width * source.height);
    }
  }
}

#endif
