#pragma once

#include "vsx_bitmap.h"
#include <math/vsx_math.h>
#include <tools/vsx_req_error.h>

class vsx_bitmap_transform
{

  template <typename T>
  class rgb_pixel
  {
    T r;
    T g;
    T b;
  };

  template <typename T>
  class rgba_pixel
  {
    T r;
    T g;
    T b;
    T a;
  };


  template < typename T = uint32_t >
  inline void split_into_cubemap_by_type(vsx_bitmap* bitmap)
  {
    void* source_data = bitmap->data_get();
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 0, sizeof(T) * (bitmap->height * bitmap->height));
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 1, sizeof(T) * (bitmap->height * bitmap->height));
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 2, sizeof(T) * (bitmap->height * bitmap->height));
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 3, sizeof(T) * (bitmap->height * bitmap->height));
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 4, sizeof(T) * (bitmap->height * bitmap->height));
    bitmap->data_set( malloc( sizeof(T) * (bitmap->height * bitmap->height) ), 0, 5, sizeof(T) * (bitmap->height * bitmap->height));

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
    bitmap->width = bitmap->height;
    free(source_data);
  }

  void inline v_smult(GLfloat *v, float s){
    v[0] *= s;
    v[1] *= s;
    v[2] *= s;
  }

  float v_rlen(float *v){
    return 1.0f / (GLfloat)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  }

  uint32_t sphere_map_get_color(uint32_t* source_data, float u, float v, int width, int height)
  {
    uint32_t vv = (uint32_t)floor(v);
    uint32_t uu = (uint32_t)floor(u);
    uint32_t c00 = source_data[ (vv    ) % height * width + (uu    ) % width];
    uint32_t c01 = source_data[ (vv    ) % height * width + (uu + 1) % width];
    uint32_t c10 = source_data[ (vv + 1) % height * width + (uu    ) % width];
    uint32_t c11 = source_data[ (vv + 1) % height * width + (uu + 1) % width];

    float fracU = (u + 10000.0f) - (u + 10000.0f);
    float fracV = (v + 10000.0f) - (v + 10000.0f);

    uint32_t a, r, g, b;
    uint32_t a00, a01, a10, a11;
    uint32_t r00, r01, r10, r11;
    uint32_t g00, g01, g10, g11;
    uint32_t b00, b01, b10, b11;

    #define sep_col(col, a, r, g, b)\
      a = (((col) >> 24) & 255);\
      r = (((col) >> 16) & 255);\
      g = (((col) >> 8) & 255);\
      b = (((col) >> 0) & 255);

    #define interpol(x, x00, x01, x10, x11, fracU, fracV)\
      x = (uint32_t)((float)x00 * (1.0f - fracU) * (1.0f - fracV) + \
          (float)x01 * (fracU) * (1.0f - fracV) + \
          (float)x10 * (1.0f - fracU) * (fracV) + \
          (float)x11 * (fracU) * (fracV));

    sep_col(c00, a00, r00, g00, b00);
    sep_col(c01, a01, r01, g01, b01);
    sep_col(c10, a10, r10, g10, b10);
    sep_col(c11, a11, r11, g11, b11);

    interpol(a, a00, a01, a10, a11, fracU, fracV);
    interpol(r, r00, r01, r10, r11, fracU, fracV);
    interpol(g, g00, g01, g10, g11, fracU, fracV);
    interpol(b, b00, b01, b10, b11, fracU, fracV);

    return (a << 24) | (r << 16) | (g << 8) | b;
  }

  uint32_t sphere_map_get_color_sphere(uint32_t* source_data, float *vec, int width, int height)
  {
    v_norm(vec);

    float sphV = (float)((acos(-vec[1]) / PI) ) * (float)height;

    float vec2[4] = {
      vec[0],
      0,
      vec[2],
      1
    };
    float scale = v_rlen(vec2);
    v_smult(vec, scale);

    float sphU = (float)(atan2(vec[0], vec[2]) / PI ) * 0.5f * (float)width;

    return sphere_map_get_color(source_data, sphU, sphV, width, height);
  }

public:

  void flip_vertically(vsx_bitmap* bitmap)
  {
    size_t width = bitmap->width;
    size_t height = bitmap->height;
    size_t channels = bitmap->channels;

    if (bitmap->storage_format == vsx_bitmap::float_storage)
    {
      GLfloat* data2 = (GLfloat*)malloc(sizeof(GLfloat) * width * height * channels);
      int dy = 0;
      size_t sxbpp = width * channels;
      for (int y = (int)height-1; y >= 0; --y)
      {
        for (unsigned long x = 0; x < (int)width * channels; ++x)
          data2[dy*sxbpp + x] = ((GLfloat*)bitmap->data_get())[y*sxbpp + x];
        ++dy;
      }
      bitmap->data_free();
      bitmap->data_set( (void*)data2 );
      return;
    }

    unsigned char* data2 = (unsigned char*)malloc(width * height * channels);
    size_t dy = 0;
    size_t stride_x = width * channels;
    for (int y = (int)height-1; y >= 0; --y)
    {
      size_t dysxbpp = dy * stride_x;
      size_t ysxbpp = y * stride_x;
      for (size_t x = 0; x < width*channels; ++x)
        data2[dysxbpp + x] = ((unsigned char*)bitmap->data_get())[ysxbpp + x];
      ++dy;
    }
    bitmap->data_free();
    bitmap->data_set( (void*)data2 );
  }


  void split_into_cubemap(vsx_bitmap* bitmap)
  {
    req_error(bitmap->width / 6 == bitmap->height, "Not cubemap, should be aspect 6:1");
    req_error(IS_POWER_OF_TWO(bitmap->height), "Height must be power of two")

    if (bitmap->channels == 4)
    {
      if (bitmap->storage_format == vsx_bitmap::float_storage)
        split_into_cubemap_by_type< rgba_pixel<float> >(bitmap);

      if (bitmap->storage_format == vsx_bitmap::byte_storage)
        split_into_cubemap_by_type< rgba_pixel<char> >(bitmap);
      return;
    }
    if (bitmap->channels == 3)
    {
      if (bitmap->storage_format == vsx_bitmap::float_storage)
        split_into_cubemap_by_type< rgb_pixel<float> >(bitmap);

      if (bitmap->storage_format == vsx_bitmap::byte_storage)
        split_into_cubemap_by_type<rgb_pixel<char> >(bitmap);
      return;
    }

    vsx_printf(L"Error, unsupported bitmap channel count: %d\n", bitmap->channels);
  }


  void bgr_to_rgb(vsx_bitmap* bitmap)
  {
    req(bitmap->storage_format == vsx_bitmap::byte_storage);
    for_n(side, 0, bitmap->sides_count_get())
    {
      unsigned char* data = (unsigned char*)bitmap->data_get(0, side);
      for_n(i, 0, bitmap->width * bitmap->height)
      {
        unsigned char temp = *(data + 2);
        *(data + 2) = *data;
        *data = temp;
        data += bitmap->channels;
      }
    }
  }

  template <typename T = char >
  bool is_alpha_channel_used(vsx_bitmap* bitmap, const T below_value)
  {
    reqrv(bitmap->channels == 4, false);
    reqrv(bitmap->storage_format == vsx_bitmap::byte_storage, false);
    for_n(side, 0, bitmap->sides_count_get())
    {
      T* source_data = (T*)bitmap->data_get(0, side);
      for_n(i, 0, bitmap->width * bitmap->height)
      {
        // r
        source_data++; // -> g
        source_data++; // -> b
        source_data++; // -> a
        if (*source_data < below_value)
          return true;
        source_data++; // -> r
      }
    }
    return false;
  }

  bool can_alpha_channel_be_removed(vsx_bitmap* bitmap)
  {
    reqrv(bitmap->channels == 4, false);
    if (bitmap->storage_format == vsx_bitmap::byte_storage)
      if (is_alpha_channel_used<char>(bitmap, (char)255))
        return false;
    if (bitmap->storage_format == vsx_bitmap::float_storage)
      if ( is_alpha_channel_used<float>(bitmap, 1.0f) )
        return false;
    return true;
  }

  void alpha_channel_remove(vsx_bitmap* bitmap)
  {
    req(bitmap->channels == 4);
    req(bitmap->storage_format == vsx_bitmap::byte_storage);
    size_t num_sides = bitmap->sides_count_get();
    for_n(side, 0, num_sides)
    {
      unsigned char* source_data = (unsigned char*)bitmap->data_get(0, side);
      unsigned char* dest_data = (unsigned char*)malloc(bitmap->width * bitmap->height * 3);
      unsigned char* source_data_p = source_data;
      bitmap->data_set(dest_data, 0, side, bitmap->width * bitmap->height * 3);
      for_n(i, 0, bitmap->width * bitmap->height)
      {
        *dest_data = *source_data_p; // r
        dest_data++; source_data_p++;
        *dest_data = *source_data_p; // g
        dest_data++; source_data_p++;
        *dest_data = *source_data_p; // b
        dest_data++; source_data_p++;
        source_data_p++;
      }
      free(source_data);
    }
    bitmap->channels = 3;
  }


  void sphere_map_into_cubemap(vsx_bitmap* bitmap)
  {
    int texSize = bitmap->height;
    float texSize1 = (float)texSize - 1.0f;
    int u, v;
    uint32_t* source_data = (uint32_t*)bitmap->data_get();

    bitmap->data_set( malloc( sizeof(uint32_t) * (bitmap->height * bitmap->height) ), 0, 0);
    bitmap->data_set( malloc( sizeof(uint32_t) * (bitmap->height * bitmap->height) ), 0, 1);
    bitmap->data_set( malloc( sizeof(uint32_t) * (bitmap->height * bitmap->height) ), 0, 2);
    bitmap->data_set( malloc( sizeof(uint32_t) * (bitmap->height * bitmap->height) ), 0, 3);
    bitmap->data_set( malloc( sizeof(uint32_t) * (bitmap->height * bitmap->height) ), 0, 4);
    bitmap->data_set( malloc( sizeof(uint32_t) * (bitmap->height * bitmap->height) ), 0, 5);

    float vec[4];

    for (size_t cube_map_side = 0; cube_map_side < 6; cube_map_side++)
      for(v = 0; v < texSize; ++v)
        for(u = 0; u < texSize; ++u)
        {
          switch(cube_map_side)
          {
            case 0:
              vec[0] = ((float)u / texSize1 - 0.5f) * 2.0f;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2.0f;
              vec[2] = 1;
              vec[3] = 1;
            break;
            case 1:
              vec[0] = 1;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2.0f;
              vec[2] = -((float)u / texSize1 - 0.5f) * 2.0f;
              vec[3] = 1;
            break;
            case 2:
              vec[0] = -((float)u / texSize1 - 0.5f) * 2.0f;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2.0f;
              vec[2] = -1;
              vec[3] = 1;
            break;
            case 3:
              vec[0] = -1;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2.0f;
              vec[2] = ((float)u / texSize1 - 0.5f) * 2.0f;
              vec[3] = 1;
            break;
            case 4:
              vec[0] = -((float)v / texSize1 - 0.5f) * 2.0f;
              vec[1] = -1;
              vec[2] = ((float)u / texSize1 - 0.5f) * 2.0f;
              vec[3] = 1;
            break;
            case 5:
              vec[0] = ((float)v / texSize1 - 0.5f) * 2;
              vec[1] = 1;
              vec[2] = ((float)u / texSize1 - 0.5f) * 2;
              vec[3] = 1;
            break;
          }
          ((uint32_t*)bitmap->data_get(0, cube_map_side))[v * texSize + u] = sphere_map_get_color_sphere(source_data, vec, bitmap->width, bitmap->height);
        }
    free(source_data);
    bitmap->width = bitmap->height;
  }

  static vsx_bitmap_transform* get_instance()
  {
    static vsx_bitmap_transform vtdt;
    return &vtdt;
  }

};
