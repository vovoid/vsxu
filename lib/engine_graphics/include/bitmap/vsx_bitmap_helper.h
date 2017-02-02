#pragma once

#include "vsx_bitmap.h"

namespace vsx_bitmap_helper
{

  void maintain_same_size(vsx_bitmap& source, vsx_bitmap& destination)
  {
    req((source.width != destination.width) || (source.height != destination.height));

    for (size_t mip_map_level = 0; mip_map_level < vsx_bitmap::mip_map_level_max; mip_map_level++)
    for (size_t cube_map_side = 0; cube_map_side < 6; cube_map_side++)
    {
      if (destination.data_get(mip_map_level, cube_map_side) && !source.data_get(mip_map_level, cube_map_side))
      {
        destination.data_free(mip_map_level, cube_map_side);
        continue;
      }

      if (!source.data_get(mip_map_level,cube_map_side))
        continue;

      if (destination.data_get(mip_map_level,cube_map_side) && source.data_get(mip_map_level, cube_map_side) )
      {
        destination.data_set(
          realloc(
            destination.data_get(mip_map_level, cube_map_side),
            source.get_channel_size() * source.width * source.height
          ),
          mip_map_level,
          cube_map_side
        );
        continue;
      }

      destination.data_set(
          malloc(source.get_channel_size() * source.width * source.height),
          mip_map_level,
          cube_map_side
      );
    }
  }


  void copy(vsx_bitmap& source, vsx_bitmap& destination)
  {
    source.lock.aquire();
    maintain_same_size(source, destination);
    destination.width = source.width;
    destination.height = source.height;
    destination.depth = source.depth;
    destination.alpha = source.alpha;
    destination.channels = source.channels;
    destination.channels_bgra = source.channels_bgra;
    destination.storage_format = source.storage_format;
    destination.compression = source.compression;
    destination.attached_to_cache = false;
    destination.references = 0;

    for (size_t mip_map_level = 0; mip_map_level < vsx_bitmap::mip_map_level_max; mip_map_level++)
      for (size_t cube_map_side = 0; cube_map_side < 6; cube_map_side++)
      {
        if (!source.data_get(mip_map_level, cube_map_side))
          continue;

        void* destination_data = destination.data_get(mip_map_level, cube_map_side);
        if (!destination_data)
        {
          destination.data_allocate( mip_map_level, cube_map_side, source.width, source.height, source.storage_format, source.channels );
          destination_data = destination.data_get(mip_map_level, cube_map_side);
        }

        memcpy(
              destination_data,
              source.data_get(mip_map_level, cube_map_side),
              source.get_channel_size() * source.width * source.height);
      }
    source.lock.release();
  }
}
