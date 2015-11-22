#ifndef VSX_TEXTURE_DATA_TRANSFORM_H
#define VSX_TEXTURE_DATA_TRANSFORM_H

#include "vsx_texture_data.h"

class vsx_texture_data_transform
{

  template < typename T = char >
  inline void split_into_cubemap_by_type(vsx_texture_data* data)
  {
    data->data[0] = malloc( sizeof(T) * (data->height * data->height) );
    data->data[1] = malloc( sizeof(T) * (data->height * data->height) );
    data->data[2] = malloc( sizeof(T) * (data->height * data->height) );
    data->data[3] = malloc( sizeof(T) * (data->height * data->height) );
    data->data[4] = malloc( sizeof(T) * (data->height * data->height) );
    data->data[5] = malloc( sizeof(T) * (data->height * data->height) );

    for (size_t side_offset = 0; side_offset < 6; side_offset++)
    {
      for (size_t y = 0; y < data->height; y++)
      {
        memcpy(
          // destination
          ((T*)data->data[side_offset]) + y * data->height
          ,

          // source
          (T*)&((T*)data->data[0])[ data->width * y ] // row offset
          +
          data->height * side_offset,            // horiz offset

          sizeof(T) * data->height // count
        );
      }
    }
  }

public:
  inline void flip_vertically(vsx_texture_data* data)
  {
    size_t width = data->width;
    size_t height = data->height;
    size_t channels = data->channels;

    if (data->storage_format == vsx_texture_data::float_storage)
    {
      GLfloat* data2 = new GLfloat[width * height * channels];
      int dy = 0;
      int sxbpp = width * channels;
      for (int y = height-1; y >= 0; --y)
      {
        for (unsigned long x = 0; x < width * channels; ++x)
          data2[dy*sxbpp + x] = ((GLfloat*)data->data[0])[y*sxbpp + x];
        ++dy;
      }
      free(data->data[0]);
      data->data[0] = (void*)data2;
      return;
    }

    unsigned char* data2 = new unsigned char[width * height * channels];
    int dy = 0;
    int stride_x = width * channels;
    for (int y = height-1; y >= 0; --y)
    {
      int dysxbpp = dy * stride_x;
      int ysxbpp = y * stride_x;
      for (size_t x = 0; x < width*channels; ++x)
      {
        data2[dysxbpp + x] = ((unsigned char*)data->data[0])[ysxbpp + x];
      }
      ++dy;
    }
    free(data->data[0]);
    data->data[0] = (void*)data2;
  }


  inline void split_into_cubemap(vsx_texture_data* data)
  {
    if ( data->width / 6 != data->height )
      VSX_ERROR_RETURN("Error: not cubemap, should be aspect 6:1");

    if (data->channels != 4)
      VSX_ERROR_RETURN("Error: RGB cubemaps not implemented");

    if (data->storage_format == vsx_texture_data::float_storage)
      split_into_cubemap_by_type<float>(data);

    if (data->storage_format == vsx_texture_data::byte_storage)
      split_into_cubemap_by_type<uint32_t>(data);
  }

  static vsx_texture_data_transform* get_instance()
  {
    static vsx_texture_data_transform vtdt;
    return &vtdt;
  }

};

#endif
