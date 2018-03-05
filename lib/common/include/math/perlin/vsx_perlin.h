
#pragma once

#include <stdlib.h>
#include <math/vector/vsx_vector2.h>
#include <math/vector/vsx_vector3.h>
#include <math/vsx_rand.h>

/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

namespace vsx
{
namespace math
{
/**
 * FT = float or double
 * SAMPLE_SIZE should be power of 2
 */
template<typename FT = float, uint32_t seed_value = 0, size_t SAMPLE_SIZE = 1024>
class perlin
{
  uint32_t B = SAMPLE_SIZE;
  uint32_t BM = SAMPLE_SIZE - 1;
  vsx_rand rand;

  int octaves = 8;
  FT frequency = 8;
  FT amplitude = 1;

  int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
  FT g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
  FT g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
  FT g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];

public:

  perlin() {
    rand.srand(seed_value);
    init();
  }

  /**
   * @brief perlin
   * @param octaves 1|2|3..16
   * @param freq 1.0 .. 8.0
   * @param amp 0.0 .. 1.0
   */
  perlin(int octaves, FT freq, FT amp)
  {
    this->octaves = octaves;
    this->frequency = freq;
    amplitude = amp;
    rand.srand(seed_value);
    init();
  }

  FT get_1d(FT x)
  {
    return perlin_noise_1d(x);
  }

  FT get_2d(FT x, FT y)
  {
    FT vec[2];
    vec[0] = x;
    vec[1] = y;
    return perlin_noise_2d(vec);
  }

  FT get_3d(FT x, FT y, FT z)
  {
    FT vec[3];
    vec[0] = x;
    vec[1] = y;
    vec[2] = y;
    return perlin_noise_3d(vec);
  }

  FT get(vsx_vector2<FT> v)
  {
    FT vec[2];
    vec[0] = v.x;
    vec[1] = v.y;
    return perlin_noise_2d(vec);
  }

  FT get(vsx_vector3<FT> v)
  {
    FT vec[3];
    vec[0] = v.x;
    vec[1] = v.y;
    vec[2] = v.z;
    return perlin_noise_3d(vec);
  }

  void init()
  {
    uint32_t i, j, k;

    for (i = 0 ; i < B ; i++)
    {
      p[i] = i;

      g1[i] = rand.frand_c() * 2;

      for (j = 0 ; j < 2 ; j++)
        g2[i][j] = rand.frand_c() * 2;
      normalize2(g2[i]);

      for (j = 0 ; j < 3 ; j++)
        g3[i][j] = rand.frand_c() * 2;
      normalize3(g3[i]);
    }

    while (--i)
    {
      k = p[i];
      p[i] = p[j = rand.rand() % B];
          p[j] = k;
    }

    for (i = 0 ; i < B + 2 ; i++)
    {
      p[B + i] = p[i];
      g1[B + i] = g1[i];
      for (j = 0 ; j < 2 ; j++)
        g2[B + i][j] = g2[i][j];
      for (j = 0 ; j < 3 ; j++)
        g3[B + i][j] = g3[i][j];
    }
  }

private:

    FT perlin_noise_1d(FT value)
    {
      int terms = octaves;
      FT result = 0.0f;
      FT amp = amplitude;

      value *= frequency;

      for( int i=0; i < terms; i++ )
      {
        result += noise1(value) * amp;
        value *= 2.0f;
        amp *= 0.5f;
      }

      return result;
    }
    FT perlin_noise_2d(FT vec[2])
    {
      int terms = octaves;
      FT result = 0.0f;
      FT amp = amplitude;

      vec[0] *= frequency;
      vec[1] *= frequency;

      for( int i=0; i<terms; i++ )
      {
        result += noise2(vec) * amp;
        vec[0] *= 2.0f;
        vec[1] *= 2.0f;
        amp *= 0.5f;
      }

      return result;
    }
    FT perlin_noise_3d(FT vec[3])
    {
      int terms = octaves;
      FT result = 0.0f;
      FT amp = amplitude;

      vec[0] *= frequency;
      vec[1] *= frequency;
      vec[2] *= frequency;

      for( int i=0; i<terms; i++ )
      {
        result += noise3(vec) * amp;
        vec[0] *= 2.0f;
        vec[1] *= 2.0f;
        vec[2] *= 2.0f;
        amp *= 0.5f;
      }

      return result;
    }

#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )
#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
  t = vec[i] + N;\
  b0 = ((int)t) & BM;\
  b1 = (b0+1) & BM;\
  r0 = t - (int)t;\
  r1 = r0 - 1.0f;

  FT noise1(FT arg)
  {
    int bx0, bx1;
    FT rx0, rx1, sx, t, u, v, vec[1];

    vec[0] = arg;

    setup(0, bx0,bx1, rx0,rx1);

    sx = s_curve(rx0);

    u = rx0 * g1[ p[ bx0 ] ];
    v = rx1 * g1[ p[ bx1 ] ];

    return lerp(sx, u, v);
  }

  FT noise2(FT vec[2])
  {
    int bx0, bx1, by0, by1, b00, b10, b01, b11;
    FT rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
    int i, j;

    setup(0,bx0,bx1,rx0,rx1);
    setup(1,by0,by1,ry0,ry1);

    i = p[bx0];
    j = p[bx1];

    b00 = p[i + by0];
    b10 = p[j + by0];
    b01 = p[i + by1];
    b11 = p[j + by1];

    sx = s_curve(rx0);
    sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

    q = g2[b00];
    u = at2(rx0,ry0);
    q = g2[b10];
    v = at2(rx1,ry0);
    a = lerp(sx, u, v);

    q = g2[b01];
    u = at2(rx0,ry1);
    q = g2[b11];
    v = at2(rx1,ry1);
    b = lerp(sx, u, v);
#undef at2
    return lerp(sy, a, b);
  }

  FT noise3(FT vec[3])
  {
    int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    FT rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
    int i, j;

    setup(0, bx0,bx1, rx0,rx1);
    setup(1, by0,by1, ry0,ry1);
    setup(2, bz0,bz1, rz0,rz1);

    i = p[ bx0 ];
    j = p[ bx1 ];

    b00 = p[ i + by0 ];
    b10 = p[ j + by0 ];
    b01 = p[ i + by1 ];
    b11 = p[ j + by1 ];

    t  = s_curve(rx0);
    sy = s_curve(ry0);
    sz = s_curve(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

    q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
    q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
    a = lerp(t, u, v);

    q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
    q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
    b = lerp(t, u, v);

    c = lerp(sy, a, b);

    q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
    q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
    a = lerp(t, u, v);

    q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
    q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
    b = lerp(t, u, v);
#undef at3
    d = lerp(sy, a, b);

    return lerp(sz, c, d);
  }
#undef setup
#undef lerp
#undef s_curve

  void normalize2(FT v[2])
  {
    FT s;

    s = (FT)sqrt(v[0] * v[0] + v[1] * v[1]);
    s = 1.0f/s;
    v[0] = v[0] * s;
    v[1] = v[1] * s;
  }

  void normalize3(FT v[3])
  {
    FT s;

    s = (FT)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    s = 1.0f/s;

    v[0] = v[0] * s;
    v[1] = v[1] * s;
    v[2] = v[2] * s;
  }


};

}
}
#undef N
#undef NP
#undef NM
