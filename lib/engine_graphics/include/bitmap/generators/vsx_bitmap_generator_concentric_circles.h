#pragma once

#include <inttypes.h>
#include <tools/vsx_singleton_counter.h>
#include <string/vsx_string_helper.h>
#include <color/vsx_color.h>
#include <bitmap/vsx_bitmap.h>
#include <tools/vsx_thread_pool.h>

class vsx_bitmap_generator_concentric_circles
{
public:

  inline static vsx_string<> generate_cache_handle(
    float frequency,
    float attenuation,
    vsx_color<> color,
    bool alpha,
    uint16_t size
  )
  {
    char result_char[96];
    sprintf(result_char, ":cnccr:%.4f %.4f %.4f,%.4f,%.4f,%.4f %d %d",
      frequency,
      attenuation,
      color.r,
      color.g,
      color.b,
      color.a,
      alpha,
      8 << size
    );
    return vsx_string<>(result_char);
  }

  inline static void generate(
      vsx_bitmap* bitmap,
      float frequency,
      float attenuation,
      vsx_color<> color,
      bool alpha,
      uint16_t size
  )
  {
    bitmap->filename =
      vsx_bitmap_generator_concentric_circles::generate_cache_handle(
        frequency,
        attenuation,
        color,
        alpha,
        size
      );

    int i_size = 8 << size;
    frequency *= 0.5f;
    bitmap->data_set( malloc( sizeof(uint32_t) * i_size * i_size ) );
    uint32_t *p = (uint32_t*)bitmap->data_get();
    int hsize = i_size >> 1;
    float one_div_hsize = 1.0f / ((float)hsize+1);

    for (int y = -hsize; y < hsize; ++y)
      for (int x = -hsize; x < hsize; ++x,p++)
      {
        float xx = (size/(size-2.0f))*((float)x)+0.5f;
        float yy = (size/(size-2.0f))*((float)y)+0.5f;
        float dd = sqrt(xx*xx + yy*yy);

        float dstf = dd * one_div_hsize;

        float dist = (float)(pow(fabs(cos(dstf * PI * frequency)), (float)attenuation) * cos(dstf * PI * 0.5));

        if (alpha)
        {
          long pr = CLAMP( (long)(255.0f * color.r), 0, 255);
          long pg = CLAMP( (long)(255.0f * color.g), 0, 255);
          long pb = CLAMP( (long)(255.0f * color.b), 0, 255);
          long pa = CLAMP( (long)(255.0f * dist * color.a), 0, 255);
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        } else
        {
          long pr = CLAMP( (long)(255.0f * dist * color.r), 0, 255);
          long pg = CLAMP( (long)(255.0f * dist * color.g), 0, 255);
          long pb = CLAMP( (long)(255.0f * dist * color.b), 0, 255);
          long pa = (long)(255.0f * color.a);
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        }
      }

    bitmap->width = i_size;
    bitmap->height = i_size;
    bitmap->timestamp = vsx_singleton_counter::get();
  }

  inline static void generate_thread(
      vsx_bitmap* bitmap,
      float frequency,
      float attenuation,
      vsx_color<> color,
      bool alpha,
      uint16_t size
  )
  {
    if (bitmap->references > 1)
      ret(bitmap->timestamp = vsx_singleton_counter::get());

    bitmap->data_ready = 0;
    bitmap->lock.aquire();
    vsx_thread_pool::instance()->add(
      [=]
      (vsx_bitmap* bitmap, float frequency, float attenuation, vsx_color<> color, bool alpha, uint16_t size)
      {
        generate(bitmap, frequency, attenuation, color, alpha, size);
        bitmap->data_ready.fetch_add(1);
        bitmap->lock.release();
      },
      bitmap,
      frequency,
      attenuation,
      color,
      alpha,
      size
    );
  }
};
