#pragma once

#include <inttypes.h>
#include <tools/vsx_singleton_counter.h>
#include <string/vsx_string_helper.h>
#include <color/vsx_color.h>
#include <bitmap/vsx_bitmap.h>
#include <tools/vsx_thread_pool.h>
#include <lib/engine_graphics/thirdparty/perlin/perlin.h>

class vsx_bitmap_generator_perlin_noise
{
public:

  inline static vsx_string<> generate_cache_handle(
    bool blob_enable,
    float blob_arms,
    float blob_attenuation,
    float blob_star_flower,
    float blob_angle,
    int rand_seed,
    int octave,
    int frequency,
    float perlin_strength,
    bool alpha,
    vsx_color<> color,
    bool storage_float,
    uint16_t size
  )
  {
    char result_char[128];
    sprintf(result_char, ":perlin:%d %.4f %.4f %.4f %.4f %d %d %d %.4f %d %.4f %.4f %.4f %.4f %d %d",
      blob_enable,
      blob_arms,
      blob_attenuation,
      blob_star_flower,
      blob_angle,
      rand_seed,
      octave,
      frequency,
      perlin_strength,
      alpha,
      color.r,
      color.g,
      color.b,
      color.a,
      storage_float,
      8 << size
    );
    return vsx_string<>(result_char);
  }

  inline static void generate(
      vsx_bitmap* bitmap,
      bool blob_enable,
      float blob_arms,
      float blob_attenuation,
      float blob_star_flower,
      float blob_angle,
      int rand_seed,
      int octave,
      int frequency,
      float perlin_strength,
      bool alpha,
      vsx_color<> color,
      bool storage_float,
      uint16_t size
  )
  {
    Perlin* perlin = new Perlin( octave + 1, (float)frequency + 1.0f, 1.0f, rand_seed );

    int i_size = 8 << size;
    float f_size = (float)i_size;

    float divisor = 1.0f / (float)i_size;

    int hsize = i_size / 2;

    if (storage_float)
    {
      bitmap->data_set( malloc( sizeof(float) * i_size * i_size ) );
      float *p = (float*)bitmap->data_get();
      float yp = 0.0f;
      float ddiv = 1.0f / (((float)hsize)+1.0f);
      for (int y = -hsize; y < hsize; ++y)
      {
        float xp = 0.0f;
        for (int x = -hsize; x < hsize; ++x)
        {
          float dist = 1.0f;
          if (blob_enable)
          {
            float xx = (f_size/(f_size-2.0f))*((float)x)+0.5f;
            float yy = (f_size/(f_size-2.0f))*((float)y)+0.5f;
            float dd = sqrt(xx*xx + yy*yy);
            if (dd > (float)hsize)
            {
              dist = 0.0f;
            }
            else
            {
              float xx = (f_size/(f_size-2.0f))*((float)x)+0.5f;
              float yy = (f_size/(f_size-2.0f))*((float)y)+0.5f;
              float dd = sqrt(xx*xx + yy*yy);
              float dstf = dd * ddiv;
              float phase =
                (float)pow(
                  1.0f -
                    (
                      (float)fabs(
                        (float)cos(
                          blob_angle + blob_arms * (float)atan2(xx,yy)
                        )
                      )
                      *
                      (blob_star_flower + (1 - blob_star_flower) * dstf)
                    )
                  ,
                  blob_attenuation
                );
              if (phase > 2.0f)
                phase = 1.0f;

              dist = (float)cos(dstf * PI/2.0f)*phase;
              dist = CLAMP(dist, 0.0f, 1.0f);
            }
          }

          float pf = dist * (float)pow( (perlin->Get(xp,yp)+1.0f) * 0.5f, perlin_strength);
          if (alpha)
          {
            p[0] = color.r;
            p[1] = color.g;
            p[2] = color.b;
            p[3] = CLAMP( pf * color.a, 0.0f, 1.0f);
          } else {
            p[0] = color.r * pf;
            p[1] = color.g * pf;
            p[2] = color.b * pf;
            p[3] = CLAMP( color.a, 0.0f, 1.0f);
          }
          ++p;
          ++p;
          ++p;
          ++p;
          xp += divisor;
        }
        yp += divisor;
      }
    } else
    {
      // integer storage
      bitmap->data_set( malloc( sizeof(uint32_t) * i_size * i_size ) );
      uint32_t *p = (uint32_t*)bitmap->data_get();
      float yp = 0.0f;
      float ddiv = 1.0f / (((float)hsize)+1.0f);

      for (int y = -hsize; y < hsize; ++y)
      {
        float xp = 0.0f;
        for (int x = -hsize; x < hsize; ++x)
        {
          float dist = 1.0f;
          if (blob_enable)
          {
            float xx = (f_size/(f_size-2.0f))*((float)x)+0.5f;
            float yy = (f_size/(f_size-2.0f))*((float)y)+0.5f;
            float dd = sqrt(xx*xx + yy*yy);
            float dstf = dd * ddiv;
            float phase =
              (float)pow(
                1.0f -
                  (
                    (float)fabs(
                      (float)cos(
                        blob_angle + blob_arms * (float)atan2(xx,yy)
                      )
                    )
                    *
                    (blob_star_flower + (1 - blob_star_flower) * dstf)
                  )
                ,
                blob_attenuation
              );

            if (phase > 2.0f)
              phase = 1.0f;

            dist = (float)cos(dstf * PI/2.0f)*phase;
            dist = CLAMP(dist, 0.01f, 1.0f);
          }
          float pf =
            pow(
              0.5f *
                (perlin->Get(xp,yp) + 1.0f)
              ,
              perlin_strength
            )
            * 255.0f * dist;

          if (alpha)
          {
            long pr = (long)CLAMP( 255.0f * color.r, 0, 255);
            long pg = (long)CLAMP( 255.0f * color.g, 0, 255);
            long pb = (long)CLAMP( 255.0f * color.b, 0, 255);
            long pa = (long)CLAMP( color.a * pf, 0, 255);
            *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
          } else
          {
            long pr = (long)CLAMP( color.r * pf, 0, 255);
            long pg = (long)CLAMP( color.g * pf, 0, 255);
            long pb = (long)CLAMP( color.b * pf, 0, 255);
            long pa = (long)CLAMP( 255.0f * color.a, 0, 255);
            *p = 0x01000000 * pa | pb * 0x00010000 | pg * 0x00000100 | pr;
          }
          ++p;
          xp += divisor;
        }
        yp += divisor;
      }
    }
    delete perlin;

    bitmap->width = i_size;
    bitmap->height = i_size;
    bitmap->timestamp = vsx_singleton_counter::get();
  }

  inline static void generate_thread(
    vsx_bitmap* bitmap,
    bool blob_enable,
    float blob_arms,
    float blob_attenuation,
    float blob_star_flower,
    float blob_angle,
    int rand_seed,
    int octave,
    int frequency,
    float perlin_strength,
    bool alpha,
    vsx_color<> color,
    bool storage_float,
    uint16_t size
  )
  {
    if (bitmap->references > 1)
      ret(bitmap->timestamp = vsx_singleton_counter::get());

    bitmap->data_ready = 0;
    bitmap->lock.aquire();
    vsx_thread_pool::instance()->add(
      [=]
      (
        vsx_bitmap* bitmap,
        bool blob_enable,
        float blob_arms,
        float blob_attenuation,
        float blob_star_flower,
        float blob_angle,
        int rand_seed,
        int octave,
        int frequency,
        float perlin_strength,
        bool alpha,
        vsx_color<> color,
        bool storage_float,
        uint16_t size
      )
      {
        generate(bitmap, blob_enable, blob_arms, blob_attenuation,
                 blob_star_flower, blob_angle, rand_seed,
                 octave, frequency, perlin_strength, alpha,
                 color, storage_float, size );

        bitmap->data_ready.fetch_add(1);
        bitmap->lock.release();
      },
      bitmap,
      blob_enable,
      blob_arms,
      blob_attenuation,
      blob_star_flower,
      blob_angle,
      rand_seed,
      octave,
      frequency,
      perlin_strength,
      alpha,
      color,
      storage_float,
      size
    );
  }
};
