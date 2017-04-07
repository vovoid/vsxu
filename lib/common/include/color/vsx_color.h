/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once 

#include <math/vector/vsx_vector3.h>
#include <vsx_platform.h>

template<typename T = float>
class vsx_color
{
public:

  union
  {
    T h;
    T r;
  };
  union
  {
    T s;
    T g;
  };
  union
  {
    T v;
    T b;
  };
  T a;

  inline void hsv(T h, T s, T v)
  {
    if ( s == 0 )                       //HSV values = From 0 to 1
    {
      r = v;                      //RGB results = From 0 to 255
      g = v;
      b = v;
      return;
    }
    float var_h = h * 6;
    int var_i = (int)floor( var_h );             //Or ... var_i = floor( var_h )
    float var_1 = v * ( 1 - s );
    float var_2 = v * ( (T)1 - s * ( var_h - (T)var_i ) );
    float var_3 = v * ( (T)1 - s * ( (T)1 - ( var_h - (T)var_i ) ) );
    float var_r, var_g, var_b;
    if      ( var_i == 0 ) { var_r = v     ; var_g = var_3 ; var_b = var_1; }
    else if ( var_i == 1 ) { var_r = var_2 ; var_g = v     ; var_b = var_1; }
    else if ( var_i == 2 ) { var_r = var_1 ; var_g = v     ; var_b = var_3; }
    else if ( var_i == 3 ) { var_r = var_1 ; var_g = var_2 ; var_b = v;     }
    else if ( var_i == 4 ) { var_r = var_3 ; var_g = var_1 ; var_b = v;     }
    else                   { var_r = v     ; var_g = var_1 ; var_b = var_2; }

    r = var_r;                  //RGB results = From 0 to 255
    g = var_g;
    b = var_b;
  }

  inline void get_hsv(vsx_color* result)
  {
    float h = 0.0f, s, v;
    float var_r = r;
    float var_g = g;
    float var_b = b;
    float var_min, var_max;
    var_min = var_r;
    var_max = var_r;

    if (var_r <= var_g && var_r <= var_b) var_min = var_r; else
    if (var_g <= var_b && var_g <= var_r) var_min = var_g; else
    if (var_b <= var_r && var_b <= var_g) var_min = var_b;

    if (var_r >= var_g && var_r >= var_b) var_max = var_r; else
    if (var_g >= var_b && var_g >= var_r) var_max = var_g; else
    if (var_b >= var_r && var_b >= var_g) var_max = var_b;

    float del_max = var_max - var_min;             //Delta RGB value
    v = var_max;

    if ( FLOAT_EQUALS(del_max, 0.0f) )                     //This is a gray, no chroma...
    {
      h = 0;                                //HSV results = From 0 to 1
      s = 0;
    }
    else                                    //Chromatic data...
    {
      s = del_max / var_max;

      float del_R = ( ( ( var_max - var_r ) / 6.0f ) + ( del_max / 2.0f ) ) / del_max;
      float del_G = ( ( ( var_max - var_g ) / 6.0f ) + ( del_max / 2.0f ) ) / del_max;
      float del_B = ( ( ( var_max - var_b ) / 6.0f ) + ( del_max / 2.0f ) ) / del_max;

      if      ( FLOAT_EQUALS(var_r, var_max) ) h = del_B - del_G;
      else if ( FLOAT_EQUALS(var_g, var_max) ) h = ( 1.0f / 3.0f ) + del_R - del_B;
      else if ( FLOAT_EQUALS(var_b, var_max) ) h = ( 2.0f / 3.0f ) + del_G - del_R;

    }
    result->h = h;
    result->s = s;
    result->v = v;
  }

  // alpha blend 2 other vectors and store result in us
  // assumed it's colors so r,g,b,a will be blended
  // b is value to blend into
  // p at 0 is no blend, 1 is full blend into the other value
  void alpha_blend(vsx_color *aa, vsx_color *bb,float p)
  {
    if (p > 1) p = 1;
    r = aa->r*(1-p)+bb->r*p;
    g = aa->g*(1-p)+bb->g*p;
    b = aa->b*(1-p)+bb->b*p;
    a = aa->a*(1-p)+bb->a*p;
  }

  void set(const float &ir=0, const float &ig=0, const float &ib=0, const float &ia=0)
  {
    r=ir;
    g=ig;
    b=ib;
    a=ia;
  }

  void set_percentages(const T rp, const T gp, const T bp, const T ap)
  {
    r = rp * (T)0.01;
    g = gp * (T)0.01;
    b = bp * (T)0.01;
    a = ap * (T)0.01;
  }


  inline vsx_color operator +(const vsx_color &t)
  {
    vsx_color temp;
    temp.r = r + t.r;
    temp.g = g + t.g;
    temp.b = b + t.b;
    temp.a = a + t.a;
    return temp;
  }

  inline vsx_color operator +=(const vsx_color &t)
  {
    r += t.r;
    g += t.g;
    b += t.b;
    a += t.a;
    return *this;
  }

  inline vsx_color operator *=(const float &t)
  {
    r *= t;
    g *= t;
    b *= t;
    a *= t;
    return *this;
  }

  inline vsx_color operator -=(const vsx_color &t)
  {
    r -= t.r;
    g -= t.g;
    b -= t.b;
    a -= t.a;
    return *this;
  }

  inline vsx_color operator -()
  {
    vsx_color temp;
    temp.r = -r;
    temp.g = -g;
    temp.b = -b;
    temp.a = -a;
    return temp;
  }

  inline vsx_color operator -(const vsx_color &t)
  {
    vsx_color temp;
    temp.r = r - t.r;
    temp.g = g - t.g;
    temp.b = b - t.b;
    temp.a = a - t.a;
    return temp;
  }

  inline vsx_color operator *(const vsx_color &t)
  {
    vsx_color temp;
    temp.r = r * t.r;
    temp.g = g * t.g;
    temp.b = b * t.b;
    temp.a = a * t.a;
    return temp;
  }

  inline vsx_color operator *(const float &t)
  {
    vsx_color temp;
    temp.r = r * t;
    temp.g = g * t;
    temp.b = b * t;
    return temp;
  }

  vsx_color()
    :
      a(1)
  {
  }

  vsx_color(const T n[4])
  {
    r = n[0];
    g = n[1];
    b = n[2];
    a = n[3];
  }


  vsx_color(float rr, float gg = 0.0f, float bb = 0.0f, float aa = 0.0f)
    :
      r(rr),
      g(gg),
      b(bb),
      a(aa)
  {
  }

};

typedef vsx_color<float> vsx_colorf;
typedef vsx_color<double> vsx_colord;

#define vsx_color_transparent vsx_colorf(0.0f, 0.0f, 0.0f, 0.0f)
#define vsx_color_white vsx_colorf(1.0f, 1.0f, 1.0f, 1.0f)
#define vsx_color_black vsx_colorf(0.0f, 0.0f, 0.0f, 1.0f)
#define vsx_color_red vsx_colorf(1.0f, 0.0f, 0.0f, 1.0f)
#define vsx_color_green vsx_colorf(0.0f, 1.0f, 0.0f, 1.0f)
#define vsx_color_blue vsx_colorf(0.0f, 0.0f, 1.0f, 1.0f)



