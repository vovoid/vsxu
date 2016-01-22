#pragma once

#include "vsx_bitmap.h"
#include <math/vsx_math.h>
#include <tools/vsx_req_error.h>

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

    float fracU = (u + 10000) - (int)(u + 10000);
    float fracV = (v + 10000) - (int)(v + 10000);

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
      x = (uint32_t)(x00 * (1.0f - fracU) * (1.0f - fracV) + \
          x01 * (fracU) * (1.0f - fracV) + \
          x10 * (1.0f - fracU) * (fracV) + \
          x11 * (fracU) * (fracV));

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

    float sphV = (float)((acos(-vec[1]) / PI) ) * height;

    float vec2[4] = {
      vec[0],
      0,
      vec[2],
      1
    };
    float scale = v_rlen(vec2);
    v_smult(vec, scale);

    float sphU = (float)(atan2(vec[0], vec[2]) / PI ) * 0.5f * width;

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


  void split_into_cubemap(vsx_bitmap* bitmap)
  {
    req_error(bitmap->width / 6 == bitmap->height, "Not cubemap, should be aspect 6:1");
    req_error(bitmap->channels == 4, "RGB cubemaps not implemented");
    req_error(IS_POWER_OF_TWO(bitmap->height), "Height must be power of two")

    if (bitmap->storage_format == vsx_bitmap::float_storage)
      split_into_cubemap_by_type<float>(bitmap);

    if (bitmap->storage_format == vsx_bitmap::byte_storage)
      split_into_cubemap_by_type<uint32_t>(bitmap);
  }



  void sphere_map_into_cubemap(vsx_bitmap* bitmap)
  {
    int texSize = bitmap->height;
    int texSize1 = texSize - 1;
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
              vec[0] = ((float)u / texSize1 - 0.5f) * 2;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2;
              vec[2] = 1;
              vec[3] = 1;
            break;
            case 1:
              vec[0] = 1;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2;
              vec[2] = -((float)u / texSize1 - 0.5f) * 2;
              vec[3] = 1;
            break;
            case 2:
              vec[0] = -((float)u / texSize1 - 0.5f) * 2;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2;
              vec[2] = -1;
              vec[3] = 1;
            break;
            case 3:
              vec[0] = -1;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2;
              vec[2] = ((float)u / texSize1 - 0.5f) * 2;
              vec[3] = 1;
            break;
            case 4:
              vec[0] = -((float)v / texSize1 - 0.5f) * 2;
              vec[1] = -1;
              vec[2] = ((float)u / texSize1 - 0.5f) * 2;
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
