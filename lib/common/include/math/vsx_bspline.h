/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2017
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

#include <inttypes.h>
#include <container/vsx_ma_vector.h>

/**
 * T is value type being interpolated. Can be any scalar value; float, vector2, vector3 etc.
 * FT is interpolation factor type - should be int, float or double etc.
 */
template <class T, typename FT = float>
class vsx_bspline
{

  T get_internal(FT t, T p0, T p1, T p2, T p3)
  {
    FT t2 = t * t;
    FT t3 = t2 * t;

    FT k1 = 1.0f - 3.0f * t + 3.0f * t2 - t3;
    FT k2 = 4.0f - 6.0f * t2 + 3.0f * t3;
    FT k3 = 1.0f + 3.0f * t + 3.0f * t2 - 3.0f * t3;

    return (p0 * k1 +
        p1 * k2 +
        p2 * k3 +
        p3 * t3) * (1.0f / 6.0f);
  }

public:
  FT current_pos = 0.0f;
  FT real_pos = 0.0f;
  T	p0, p1, p2, p3;
  size_t old_pos = 0;
  vsx_ma_vector< T > points;
  	
  inline void set_pos(FT t)
  {
    size_t new_int_pos = (size_t)t;
    if (new_int_pos != old_pos)
    {
      p0 = points[(new_int_pos)  %points.size()];
      p1 = points[(new_int_pos+1)%points.size()];
      p2 = points[(new_int_pos+2)%points.size()];
      p3 = points[(new_int_pos+3)%points.size()];
      old_pos = new_int_pos;
    }
    real_pos = current_pos = t;
    if (current_pos > 1.0f)
      current_pos -= (int)current_pos;
  }
  
  inline void step(FT stepn)
  {
    set_pos(real_pos+stepn);
  }

  inline T get_current()
  {
    return get_internal(current_pos, p0, p1, p2, p3);
  }

  /**
   * @brief get
   * @param t a value between 0 and 1
   * @return
   */
  inline T get(FT t)
  {
    reqrv(points.size() > 4, T() );
    reqrv(t > 0, T());
    FT t2 = CLAMP(t, 0, 1) * (FT)(points.size() - 4);
    size_t new_int_pos = (size_t)t2;
    T p0 = points[(new_int_pos)  %points.size()];
    T p1 = points[(new_int_pos+1)%points.size()];
    T p2 = points[(new_int_pos+2)%points.size()];
    T p3 = points[(new_int_pos+3)%points.size()];
    return get_internal(fmod(t2,1.0), p0, p1, p2, p3);
  }



};
