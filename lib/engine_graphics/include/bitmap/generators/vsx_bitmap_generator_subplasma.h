#pragma once

#include <inttypes.h>
#include <tools/vsx_singleton_counter.h>
#include <string/vsx_string_helper.h>
#include <color/vsx_color.h>
#include <bitmap/vsx_bitmap.h>
#include <tools/vsx_thread_pool.h>
#include <math/vsx_rand.h>

class vsx_bitmap_generator_subplasma
{

  static unsigned char catmullrom_interpolate(int v0, int v1, int v2, int v3, float xx)
  {
    int a =v0 - v1;
    int P =v3 - v2 - a;
    int Q =a - P;
    int R =v2 - v0;
    int t=(int)(v1+xx*(R+xx*(Q+xx*P)));
    if (t > 255) return 255; else
    if (t < 0) return 0; else
    return (unsigned char)t;
  }

public:

  inline static vsx_string<> generate_cache_handle(
      int rand_seed,
      int amplitude,
      uint16_t size
  )
  {
    char result_char[96];
    sprintf(result_char, ":subpl:%d %d %d",
      rand_seed,
      amplitude,
      8 << size
    );
    return vsx_string<>(result_char);
  }

  inline static void generate(
    vsx_bitmap* bitmap,
    int rand_seed,
    int amplitude,
    uint16_t size
  )
  {
    bitmap->filename =
      vsx_bitmap_generator_subplasma::generate_cache_handle(
        rand_seed,
        amplitude,
        size
      );

    int i_size = 8 << size;

    bitmap->data_set( malloc( sizeof(uint32_t) * i_size * i_size ) );

    unsigned char* SubPlasma = new unsigned char[ i_size * i_size];
    for (int x = 0; x < i_size * i_size; ++x)
      SubPlasma[x] = 0;

    int np = 2 << amplitude;

    unsigned int musize = i_size - 1;
    unsigned int mmu = (unsigned int)(((float)i_size/(float)np));
    unsigned int mm1 = mmu - 1;
    unsigned int mm2 = mmu * 2;
    float mmf = (float)mmu;

    vsx_rand rand;

    rand.srand(rand_seed);
    for (int y=0; y < np; y++)
      for (int x=0; x < np; x++)
        SubPlasma[x*mmu + y*mmu*i_size] = rand.rand();

    for (int y=0; y<np; y++)
      for (int x=0; x < i_size; x++)
      {
        int p=x&(~mm1);
        int zy=y*mmu*i_size;
        SubPlasma[x+zy] = catmullrom_interpolate(
          SubPlasma[((p-mmu)&musize)+zy],
          SubPlasma[((p   )&musize)+zy],
          SubPlasma[((p+mmu)&musize)+zy],
          SubPlasma[((p+mm2)&musize)+zy],
          (x&mm1)/mmf);
      }

    int sl = size + 3;
    for (int y=0; y < i_size; y++)
      for (int x=0; x < i_size; x++) {
        int p=y&(~(mm1));
        SubPlasma[x+y*i_size] = catmullrom_interpolate(
          SubPlasma[x+(((p-mmu)&musize)<<sl)],
          SubPlasma[x+(((p   )&musize)<<sl)],
          SubPlasma[x+(((p+mmu)&musize)<<sl)],
          SubPlasma[x+(((p+mm2)&musize)<<sl)],
          (y&(mm1))/mmf);
      }

    uint32_t *p = (uint32_t*)bitmap->data_get();

    for (int x = 0; x < i_size * i_size; ++x, p++)
      *p = 0xFF000000 | ((uint32_t)SubPlasma[x]) << 16 | (uint32_t)SubPlasma[x] << 8 | (uint32_t)SubPlasma[x];

    delete[] SubPlasma;
    bitmap->width = i_size;
    bitmap->height = i_size;
    bitmap->timestamp = vsx_singleton_counter::get();
  }

  inline static void generate_thread(
    vsx_bitmap* bitmap,
    int rand_seed,
    int amplitude,
    uint16_t size
  )
  {
    if (bitmap->references > 1)
      ret(bitmap->timestamp = vsx_singleton_counter::get());

    bitmap->data_ready = 0;
    bitmap->lock.aquire();
    vsx_thread_pool::instance()->add(
      [=]
      (vsx_bitmap* bitmap, int rand_seed, int amplitude, uint16_t size)
      {
        generate(bitmap, rand_seed, amplitude, size);
        bitmap->data_ready.fetch_add(1);
        bitmap->lock.release();
      },
      bitmap,
      rand_seed,
      amplitude,
      size
    );
  }
};
