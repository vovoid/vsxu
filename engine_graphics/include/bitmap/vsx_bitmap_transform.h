#ifndef VSX_BITMAP_TRANSFORM_H
#define VSX_BITMAP_TRANSFORM_H

#include "vsx_bitmap.h"
#include <vsx_math.h>
#include <tools/vsx_req.h>

class vsx_bitmap_transform
{

  template < typename T = char >
  inline void split_into_cubemap_by_type(vsx_bitmap* bitmap)
  {
    void* source_data = bitmap->data_get();
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 0);
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 1);
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 2);
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 3);
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 4);
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 5);

    for (size_t cube_map_side = 0; cube_map_side < 6; cube_map_side++)
    {
      for (size_t y = 0; y < bitmap->height; y++)
      {
        memcpy(
          // destination
          ((T*)bitmap->data_get(0, cube_map_side) ) + y * bitmap->height
          ,

          // source
          (T*)&((T*)source_data)[ bitmap->width * y ] // row offset
          +
          bitmap->height * cube_map_side,            // horiz offset

          sizeof(T) * bitmap->height // count
        );
      }
    }
    free(source_data);
  }

public:
  inline void flip_vertically(vsx_bitmap* bitmap)
  {
    size_t width = bitmap->width;
    size_t height = bitmap->height;
    size_t channels = bitmap->channels;

    if (bitmap->storage_format == vsx_bitmap::float_storage)
    {
      GLfloat* data2 = (GLfloat*)malloc(sizeof(GLfloat) * width * height * channels);
      int dy = 0;
      int sxbpp = width * channels;
      for (int y = height-1; y >= 0; --y)
      {
        for (unsigned long x = 0; x < width * channels; ++x)
          data2[dy*sxbpp + x] = ((GLfloat*)bitmap->data_get())[y*sxbpp + x];
        ++dy;
      }
      bitmap->data_free();
      bitmap->data_set( (void*)data2 );
      return;
    }

    unsigned char* data2 = (unsigned char*)malloc(width * height * channels);
    int dy = 0;
    int stride_x = width * channels;
    for (int y = height-1; y >= 0; --y)
    {
      int dysxbpp = dy * stride_x;
      int ysxbpp = y * stride_x;
      for (size_t x = 0; x < width*channels; ++x)
        data2[dysxbpp + x] = ((unsigned char*)bitmap->data_get())[ysxbpp + x];
      ++dy;
    }
    bitmap->data_free();
    bitmap->data_set( (void*)data2 );
  }


  inline void split_into_cubemap(vsx_bitmap* data)
  {
    req_error(data->width / 6 == data->height, "Not cubemap, should be aspect 6:1");
    req_error(data->channels == 4, "RGB cubemaps not implemented");
    req_error(IS_POWER_OF_TWO(data->height), "Height must be power of two")

    if (data->storage_format == vsx_bitmap::float_storage)
      split_into_cubemap_by_type<float>(data);

    if (data->storage_format == vsx_bitmap::byte_storage)
      split_into_cubemap_by_type<uint32_t>(data);
  }

  static vsx_bitmap_transform* get_instance()
  {
    static vsx_bitmap_transform vtdt;
    return &vtdt;
  }

};

#endif
