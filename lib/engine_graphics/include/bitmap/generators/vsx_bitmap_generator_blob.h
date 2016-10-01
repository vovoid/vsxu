#pragma once

#include <inttypes.h>
#include <tools/vsx_singleton_counter.h>
#include <string/vsx_string_helper.h>
#include <color/vsx_color.h>
#include <bitmap/vsx_bitmap.h>
#include <tools/vsx_thread_pool.h>

class vsx_bitmap_generator_blob
{
public:

  inline static vsx_string<> generate_cache_handle(
    float arms,
    float attenuation,
    float star_flower,
    float angle,
    vsx_color<> color,
    bool alpha,
    uint16_t size
  )
  {
    char result_char[96];
    sprintf(result_char, ":blob:%.4f %.4f %.4f %.4f %.4f,%.4f,%.4f,%.4f %d %d",
      arms,
      attenuation,
      star_flower,
      angle,
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
    float arms,
    float attenuation,
    float star_flower,
    float angle,
    vsx_color<> color,
    bool alpha,
    uint16_t size
  )
  {
    bitmap->filename =
      vsx_bitmap_generator_blob::generate_cache_handle(
        arms,
        attenuation,
        star_flower,
        angle,
        color,
        alpha,
        size
      );


    int i_size = 8 << size;
    arms *= 0.5f;
    bitmap->data_set( malloc( sizeof(uint32_t) * i_size * i_size ) );
    uint32_t *p = (uint32_t*)bitmap->data_get();
    float size_f = (float)i_size;
    float size_div_size_minus_two = (size_f/(size_f-2.0f));
    int hsize = i_size >> 1;
    for(int y = hsize; y > -hsize; --y)
    {
      for(int x = -hsize; x < hsize; ++x, p++)
      {
        float xx = size_div_size_minus_two * ((float)x)+0.5f;
        float yy = size_div_size_minus_two * ((float)y)+0.5f;
        float dd = sqrt(xx*xx + yy*yy);
        float dstf = dd/((float)hsize+1);
        float phase = (float)pow(1.0f - fabsf(cosf(angle+arms*atan2f(xx,yy)))*(star_flower+(1-star_flower)*(((dstf)))),attenuation);
        if (phase > 2.0f)
          phase = 1.0f;

        float pf = (255.0f * (cos(((dstf * PI_FLOAT/2.0f)))*phase));

        if (pf > 255.0f)
          pf = 255.0f;

        if (pf < 0.0f)
          pf = 0.0f;

        *p = (long)pf;
        float dist = cos(dstf * PI_FLOAT/2.0f)*phase;
        if (alpha)
        {
          long pr = MAX(0,MIN(255,(long)(255.0f * color.r)));
          long pg = MAX(0,MIN(255,(long)(255.0f * color.g)));
          long pb = MAX(0,MIN(255,(long)(255.0f * color.b)));
          long pa = MAX(0,MIN(255,(long)(255.0f * dist * color.a)));
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        } else
        {
          long pr = MAX(0,MIN(255,(long)(255.0f * dist * color.r)));
          long pg = MAX(0,MIN(255,(long)(255.0f * dist * color.g)));
          long pb = MAX(0,MIN(255,(long)(255.0f * dist * color.b)));
          long pa = (long)(255.0f * color.a);
          *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
        }
      }
    }
    bitmap->width = i_size;
    bitmap->height = i_size;
    bitmap->timestamp = vsx_singleton_counter::get();
  }

  inline static void generate_thread(
    vsx_bitmap* bitmap,
    float arms,
    float attenuation,
    float star_flower,
    float angle,
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
      (vsx_bitmap* bitmap, float arms, float attenuation, float star_flower, float angle, vsx_color<> color, bool alpha, uint16_t size)
      {
        generate(bitmap, arms, attenuation, star_flower, angle, color, alpha, size);
        bitmap->data_ready.fetch_add(1);
        bitmap->lock.release();
      },
      bitmap,
      arms,
      attenuation,
      star_flower,
      angle,
      color,
      alpha,
      size
    );
  }
};
