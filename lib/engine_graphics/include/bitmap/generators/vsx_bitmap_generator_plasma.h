#pragma once

#include <inttypes.h>
#include <tools/vsx_singleton_counter.h>
#include <string/vsx_string_helper.h>
#include <color/vsx_color.h>
#include <math/vector/vsx_vector2.h>
#include <bitmap/vsx_bitmap.h>
#include <tools/vsx_thread_pool.h>

class vsx_bitmap_generator_plasma
{
public:

  inline static vsx_string<> generate_cache_handle(
      vsx_vector2f period_red,
      vsx_vector2f period_green,
      vsx_vector2f period_blue,
      vsx_vector2f period_alpha,
      vsx_vector2f offset_red,
      vsx_vector2f offset_green,
      vsx_vector2f offset_blue,
      vsx_vector2f offset_alpha,
      vsx_colorf amp,
      vsx_colorf ofs,
      uint16_t size
  )
  {
    char result_char[192];
    sprintf(
      result_char,
      ":plasma:"
      "%.4f,%.4f " // period red
      "%.4f,%.4f " // period green
      "%.4f,%.4f " // period blue
      "%.4f,%.4f " // period alpha
      "%.4f,%.4f " // offset red
      "%.4f,%.4f " // offset green
      "%.4f,%.4f " // offset blue
      "%.4f,%.4f " // offset alpha
      "%.4f,%.4f,%.4f,%.4f " // amp
      "%.4f,%.4f,%.4f,%.4f " // ofs
      "%d" // size
      ,
      period_red.x, period_red.y,
      period_green.x, period_green.y,
      period_blue.x, period_blue.y,
      period_alpha.x, period_alpha.y,
      offset_red.x, offset_red.y,
      offset_green.x, offset_green.y,
      offset_blue.x, offset_blue.y,
      offset_alpha.x, offset_alpha.y,
      amp.r, amp.g, amp.b, amp.a,
      ofs.r, ofs.g, ofs.b, ofs.a,
      8 << size
    );
    return vsx_string<>(result_char);
  }

  inline static void generate(
      vsx_bitmap* bitmap,
      vsx_vector2f period_red,
      vsx_vector2f period_green,
      vsx_vector2f period_blue,
      vsx_vector2f period_alpha,
      vsx_vector2f offset_red,
      vsx_vector2f offset_green,
      vsx_vector2f offset_blue,
      vsx_vector2f offset_alpha,
      vsx_colorf amp,
      vsx_colorf ofs,
      uint16_t size
  )
  {
    bitmap->filename =
      vsx_bitmap_generator_plasma::generate_cache_handle(
          period_red,
          period_green,
          period_blue,
          period_alpha,
          offset_red,
          offset_green,
          offset_blue,
          offset_alpha,
          amp,
          ofs,
          size
      );

    uint16_t i_size = 8 << size;

    float amp_r = amp.r * 127.0f;
    float amp_g = amp.g * 127.0f;
    float amp_b = amp.b * 127.0f;
    float amp_a = amp.a * 127.0f;

    float ofs_r = ofs.r * 127.0f;
    float ofs_g = ofs.g * 127.0f;
    float ofs_b = ofs.b * 127.0f;
    float ofs_a = ofs.a * 127.0f;

    bitmap->data_set( malloc( sizeof(uint32_t) * i_size * i_size ) );

    uint32_t* p = (uint32_t*)bitmap->data_get();
    int hsize = i_size >> 1;
    float size_f = (float)(2.0f*PI)/(float)i_size;

    for(int y = -hsize; y < hsize; ++y)
      for(int x = -hsize; x < hsize; ++x, p++)
      {
        long r =
          (long)round(
            fmod(
              fabs(
                (
                  (
                    sin( ( x * size_f + offset_red.x)   * period_red.x)
                    *
                    sin((y * size_f + offset_red.y)   * period_red.y)
                  )
                  + 1.0f
                )
                  * amp_r
                + ofs_r
              )
              ,
              255.0
            )
          );

        long g =
          (long)round(
            fmod(
              fabs(
                (
                  (
                    sin( ( x * size_f + offset_green.x)   * period_green.x)
                    *
                    sin((y * size_f + offset_green.y)   * period_green.y)
                  )
                  + 1.0f
                )
                  * amp_g
                + ofs_g
              )
              ,
              255.0
            )
          );

        long b =
          (long)round(
            fmod(
              fabs(
                (
                  (
                    sin( ( x * size_f + offset_blue.x)   * period_blue.x)
                    *
                    sin((y * size_f + offset_blue.y)   * period_blue.y)
                  )
                  + 1.0f
                )
                  * amp_b
                + ofs_b
              )
              ,
              255.0
            )
          );

        long a =
          (long)round(
            fmod(
              fabs(
                (
                  (
                    sin( ( x * size_f + offset_alpha.x)   * period_alpha.x)
                    *
                    sin((y * size_f + offset_alpha.y)   * period_alpha.y)
                  )
                  + 1.0f
                )
                  * amp_a
                + ofs_a
              )
              ,
              255.0
            )
          );
        *p = 0x01000000 * a | b * 0x00010000 | g * 0x00000100 | r;
      }
    bitmap->width = i_size;
    bitmap->height = i_size;
    bitmap->timestamp = vsx_singleton_counter::get();
  }


  inline static void generate_thread(
      vsx_bitmap* bitmap,
      vsx_vector2f period_red,
      vsx_vector2f period_green,
      vsx_vector2f period_blue,
      vsx_vector2f period_alpha,
      vsx_vector2f offset_red,
      vsx_vector2f offset_green,
      vsx_vector2f offset_blue,
      vsx_vector2f offset_alpha,
      vsx_colorf amp,
      vsx_colorf ofs,
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
        vsx_vector2f period_red,
        vsx_vector2f period_green,
        vsx_vector2f period_blue,
        vsx_vector2f period_alpha,
        vsx_vector2f offset_red,
        vsx_vector2f offset_green,
        vsx_vector2f offset_blue,
        vsx_vector2f offset_alpha,
        vsx_colorf amp,
        vsx_colorf ofs,
        uint16_t size
      )
      {
        generate(bitmap, period_red, period_green, period_blue, period_alpha, offset_red, offset_green, offset_blue, offset_alpha, amp, ofs, size);
        bitmap->data_ready.fetch_add(1);
        bitmap->lock.release();
      },
      bitmap,
      period_red,
      period_green,
      period_blue,
      period_alpha,
      offset_red,
      offset_green,
      offset_blue,
      offset_alpha,
      amp,
      ofs,
      size
    );
  }
};
