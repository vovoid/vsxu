/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSX_BEZIER_CALC_H
#define VSX_BEZIER_CALC_H


// recursive third-grade newton raphson numerical analysis solver

class vsx_bezier_calc {
  float a;
  float b;
  float c;
  float d;
  float e;
  float f;
  float g;
  float h;

public:
  float x0;
  float y0;
  float x1;
  float y1;
  float x2;
  float y2;
  float x3;
  float y3;

/*  vsx_bezier_calc() {
    x0 = 0;
    y0 = 0;
    x1 = 0;
    y1 = 1;

    x2 = 1;
    y2 = 1;
    x3 = 1;
    y3 = 0;
  }*/
  
  void init() {
    a = x3 - 3.0f*x2 + 3.0f*x1 - x0; 
    e = y3 - 3.0f*y2 + 3.0f*y1 - y0;
    b = 3.0f*x2 - 6.0f*x1 + 3.0f*x0;
    f = 3.0f*y2 - 6.0f*y1 + 3.0f*y0;
    c = 3.0f*x1 - 3.0f*x0;
    g = 3.0f*y1 - 3.0f*y0;
    d = x0;
    h = y0;
  }
  
   

  float t_from_x(float x_find, float t = 0.5f, int iter = 5) {
    if (iter < 0) return t; else
    {
      if (iter == 5) {
        t = x_find;
      }
      float current_slope = 1.0f/(3.0f*a*t*t + 2.0f*b*t + c);
      float cur_x = t*(t*(a*t + b) + c) + d;
      float nextguess = t + (x_find-cur_x)*(current_slope);
      return t_from_x(x_find,nextguess,iter-1);
    }  
  }
  
  float x_from_t(float t) {
    return t*(t*(a*t + b) + c) + d;
  }

  float y_from_t(float t) {
    return t*(t*(e*t + f) + g) + h;
  }

};

#endif
