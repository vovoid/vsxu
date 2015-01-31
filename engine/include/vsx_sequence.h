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


#ifndef VSX_SEQUENCE_H
#define VSX_SEQUENCE_H


#include <vsx_platform.h>
#include <string/vsx_string_helper.h>
#include <vector/vsx_vector2.h>
#include <vector/vsx_vector2_helper.h>
#include "vsx_engine_dllimport.h"

namespace vsx
{

namespace sequence
{

class value_float
{
public:
  float value;

  value_float()
    : value(0.0)
  {}

  value_float(float t)
    : value(t)
  {}

  value_float(const value_float& t)
    : value(t.value)
  {}

  value_float& operator=(const value_float& ss)
  {
    value = ss.value;
    return *this;
  }

  value_float& operator=(float ss)
  {
    value = ss;
    return *this;
  }

  inline value_float operator*(float ss)
  {
    value_float f(*this);
    f.value *= ss;
    return f;
  }

  inline value_float operator+(float ss)
  {
    value_float f(*this);
    f.value += ss;
    return f;
  }

  inline value_float operator-(float ss)
  {
    value_float f(*this);
    f.value -= ss;
    return f;
  }

  inline value_float operator-(const value_float& ss)
  {
    value_float f(*this);
    f.value -= ss.value;
    return f;
  }

  inline value_float operator+(const value_float& ss)
  {
    value_float f(*this);
    f.value += ss.value;
    return f;
  }

  inline float get_float()
  {
    return value;
  }

  inline void set_float(float new_value)
  {
    value = new_value;
  }

  inline vsx_string<> get_string()
  {
    return vsx_string_helper::f2s(value);
  }

  inline void set_string(vsx_string<> new_value)
  {
    value = vsx_string_helper::s2f(new_value);
  }
};

class value_string
{
public:
  vsx_string<> value;

  value_string() {}

  value_string(float t)
  {
    value = vsx_string_helper::f2s(t);
  }

  value_string(const value_string& t)
    : value(t.value)
  {}

  value_string& operator=(const value_string& ss)
  {
    value = ss.value;
    return *this;
  }

  value_string& operator=(vsx_string<> ss)
  {
    value = ss;
    return *this;
  }

  inline value_string operator*(float ss)
  {
    VSX_UNUSED(ss);
    return value_string();
  }

  inline value_string operator+(float ss)
  {
    VSX_UNUSED(ss);
    return value_string();
  }

  inline value_string operator-(float ss)
  {
    VSX_UNUSED(ss);
    return value_string();
  }

  inline value_string operator-(const value_string& ss)
  {
    VSX_UNUSED(ss);
    return value_string();
  }

  inline value_string operator+(const value_string& ss)
  {
    VSX_UNUSED(ss);
    return value_string();
  }

  inline float get_float()
  {
    return 0.0;
  }

  inline void set_float(float new_value)
  {
    VSX_UNUSED(new_value);
  }

  inline vsx_string<> get_string()
  {
    return value;
  }

  inline void set_string(vsx_string<> new_value)
  {
    value = new_value;
  }
};


enum interpolation_t
{
  none,
  linear,
  cosine,
  reserved,
  bezier
};

template <typename T>
class channel
{
  class item
  {
  public:
    T value;
    float delay; // in seconds (float)
    interpolation_t interpolation;
    vsx_vector2f handle1;
    vsx_vector2f handle2;

    vsx_string<> get_value()
    {
      if (interpolation == bezier)
        return
            value.get_string()+":"+
            vsx_string_helper::f2s(handle1.x)+","+
            vsx_string_helper::f2s(handle1.y)+":"+
            vsx_string_helper::f2s(handle2.x)+","+
            vsx_string_helper::f2s(handle2.y);
      return value.get_string();
    }

    item()
      :
      delay(1),
      interpolation(none)
    {}
  };

  vsx_bezier_calc<float> bez_calc;
  vsx_nw_vector< item > items;
  float i_time;
  T cur_val;
  T to_val;
  float cur_delay;
  int cur_interpolation;
  long line_cur;
  float line_time;
  int i_cur;

public:

  channel()
  {
    reset();
    item a;
    a.delay = 0.5f;
    items.push_back(a);
    items.push_back(a);
  }

  channel(const channel& seq)
  {
    channel* sq = (channel*)&seq;
    items.reset_used();
    for (unsigned long i = 0; i < (sq->items.size()); ++i)
      items.push_back(sq->items[i]);
  }

  channel(channel& seq)
  {
    items.reset_used();
    for (unsigned long i = 0; i < seq.items.size(); ++i)
      items.push_back( seq.items[i] );
  }


  channel<T>& operator=(channel<T>& ss)
  {
    items.reset_used();
    for (unsigned long i = 0; i < ss.items.size(); ++i)
      items.push_back( ss.items[i] );
    return *this;
  }

  item& get_item_by_index(size_t index)
  {
    return items[index];
  }

  void reset()
  {
    i_time = 0;
    i_cur = 0;
    to_val = T();
    line_cur = 0;
    line_time = 0;
  }

  void set_time(float time) {
    execute(time-i_time);
  }

  T execute_absolute(float time) {
    return execute(time-i_time);
  }

  T execute(float t_incr)
  {
    if (!items.size())
      return T();

    if (items.size() < 2)
    {
      i_time += t_incr;
      return items[0].value;
    }

    // if run for the first time
    if (i_time == 0 && i_cur == 0)
    {
      cur_val = items[0].value;
      cur_delay = items[0].delay;
      cur_interpolation = items[0].interpolation;
      to_val = items[1].value;
    }
    i_time += t_incr;

    int c = 0;
    line_time += t_incr;

    // The assumption for this algorithm is that you seldom jump quickly from beginning to end,
    // if you were to do that you could use a plain oscillator.
    // So in most cases it saves computing cycles to move from the position we were before.
    // Hence it's not worth always iterating from the beginning when being presented with
    // a new time stamp.

    // deal with moving backwards (dt < 0)
    if (t_incr < 0)
    {
      if ( line_time < 0 )
      {
        // we've passed behind our line, but how long?
        // calculate that here
        while (line_time < 0)
        {
          ++c;
          --line_cur;
          if (line_cur < 0)
          {
            line_cur = 0;
            line_time = 0;
            continue;
          }
          line_time +=  items[line_cur].delay;
        }
      }
      // newly calculated line_cur
      cur_val = items[line_cur].value;
      cur_delay = items[line_cur].delay;
      cur_interpolation = items[line_cur].interpolation;
      to_val = items[line_cur+1].value;
    } else
    {
      // moving forwards (dt > 0)
      if (cur_delay != -1)
      while (
                line_time > cur_delay
              &&
                cur_delay != -1
            )
      {
        ++c;
        line_time -= items[line_cur].delay;
        ++line_cur;
        cur_delay = items[line_cur].delay;
        cur_val = to_val;
        if ( line_cur  >=  (long)items.size()-1 )
        {
          if (line_cur > (long)items.size()-1)
          line_cur = (long)items.size()-1;
          cur_delay = -1;
        }
        else
        {
          if (line_cur >= (long)items.size()-1)
            to_val = items[line_cur].value;
          else
            to_val = items[line_cur+1].value;
        }
      }
      cur_interpolation = items[line_cur].interpolation;
    }

    // positioning complete, now calculate value
    T cv = cur_val;
    T ev = to_val;
    T dv = ev-cv;

    if (cur_interpolation == none)
    {
      if (line_time/cur_delay < 0.99)
        return cv;
      return ev;
    }

    if (cur_interpolation == linear)
    {
      if (cur_delay != 0.0f)
        return cv + dv * (line_time/cur_delay);
      return cv + dv;
    }

    if (cur_interpolation == cosine)
    {
      float ft = line_time/cur_delay * PI_FLOAT;
      float f = (1 - (float)cos(ft) ) * 0.5f ;
      return cv + dv * f;
    }

    if (cur_interpolation == bezier)
    {
      float t = (line_time/cur_delay);

      bez_calc.x0 = 0.0f;
      bez_calc.y0 = cv.get_float();
      bez_calc.x1 = items[line_cur].handle1.x;
      bez_calc.y1 = cv.get_float() + items[line_cur].handle1.y;
      bez_calc.x2 = items[line_cur].handle2.x;
      bez_calc.y2 = ev.get_float() + items[line_cur].handle2.y;
      bez_calc.x3 = 1.0f;
      bez_calc.y3 = ev.get_float();
      bez_calc.init();
      float tt = bez_calc.t_from_x(t);
      return T( bez_calc.y_from_t(tt) );
    }

    return 0.0f;
  }

  vsx_string<> get_string()
  {
    vsx_nw_vector< vsx_string<> > parts;
    for (unsigned long i = 0; i < items.size(); ++i)
    {
      parts.push_back(
        vsx_string_helper::f2s(items[i].delay)+";"+
        vsx_string_helper::f2s(items[i].interpolation)+";"+
        vsx_string_helper::base64_encode((items[i].value.get_string()))
      );
    }
    vsx_string<>deli = "|";
    return implode(parts,deli);
  }

  void set_string(vsx_string<>str)
  {
    items.reset_used();
    vsx_nw_vector< vsx_string<> > rows;
    vsx_string<>deli = "|";
    explode(str, deli, rows);
    vsx_string<>deli2 = ";";
    item n_i;
    for (unsigned long i = 0; i < rows.size(); ++i)
    {
      vsx_nw_vector< vsx_string<> > parts;
      explode(rows[i], deli2,  parts);
      n_i.delay = vsx_string_helper::s2f(parts[0]);
      n_i.interpolation = (interpolation_t)(int)vsx_string_helper::s2f(parts[1]);
      vsx_string<>ff(vsx_string_helper::base64_decode(parts[2]));

      if (n_i.interpolation != bezier)
        n_i.value.set_string(ff);

      if (n_i.interpolation == 4) {
        vsx_nw_vector< vsx_string<> > pld_l;
        vsx_string<>pdeli_l = ":";
        explode(ff, pdeli_l, pld_l);
        n_i.value.set_string(pld_l[0]);
        n_i.handle1 = vsx_vector2_helper::from_string<float>(pld_l[1]);
        n_i.handle2 = vsx_vector2_helper::from_string<float>(pld_l[2]);
      }
      items.push_back(n_i);
    }
    float t = i_time;
    i_time = 0;
    i_cur = 0;
    line_time = 0;
    line_cur = 0;
    execute(t);
  }
};

}

}
#endif
