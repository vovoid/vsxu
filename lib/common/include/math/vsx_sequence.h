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

#include <vsx_platform.h>

#if COMPILER == COMPILER_VISUAL_STUDIO
#pragma warning( push )
#pragma warning( disable : 4521)
#endif

#include <debug/vsx_error.h>
#include <string/vsx_string_helper.h>
#include <math/vector/vsx_vector2.h>
#include <math/vector/vsx_vector2_helper.h>
#include <vsx_common_dllimport.h>

namespace vsx
{

namespace sequence
{

class value_abs
{
protected:
  float value_f;
  vsx_string<> value_s;

public:

  value_abs()
    : value_f(0.0)
  {}

  virtual float get_float()
  {
    return value_f;
  }

  virtual void set_float(float new_value)
  {
    value_f = new_value;
  }

  virtual vsx_string<> get_string()
  {
    return value_s;
  }

  virtual void set_string(vsx_string<> new_value)
  {
    value_s = new_value;
  }

  virtual value_abs& operator=(const value_abs& ss)
  {
    value_s = ss.value_s;
    value_f = ss.value_f;
    return *this;
  }

};

class value_float
    :
    public value_abs
{
public:

  value_float()
  {}

  value_float(float t)
  {
    value_f = t;
  }

  value_float(const value_float& t)
  {
    value_f = t.value_f;
  }

  value_float& operator=(const value_float& ss)
  {
    value_f = ss.value_f;
    return *this;
  }

  value_float& operator=(float ss)
  {
    value_f = ss;
    return *this;
  }

  inline value_float operator*(float ss)
  {
    value_float f(*this);
    f.value_f *= ss;
    return f;
  }

  inline value_float operator+(float ss)
  {
    value_float f(*this);
    f.value_f += ss;
    return f;
  }

  inline value_float operator-(float ss)
  {
    value_float f(*this);
    f.value_f -= ss;
    return f;
  }

  inline value_float operator-(const value_float& ss)
  {
    value_float f(*this);
    f.value_f -= ss.value_f;
    return f;
  }

  inline value_float operator+(const value_float& ss)
  {
    value_float f(*this);
    f.value_f += ss.value_f;
    return f;
  }

  void set_string(vsx_string<> new_value)
  {
    value_f = vsx_string_helper::s2f(new_value);
  }

  virtual vsx_string<> get_string()
  {
    return vsx_string_helper::f2s(value_f);
  }


  static vsx_string<> get_type_name()
  {
    return "float";
  }
};

class value_string
    :
    public value_abs
{
public:

  value_string() {}

  value_string(float t)
  {
    value_s = vsx_string_helper::f2s(t);
  }

  value_string(const value_string& t)
  {
    value_s = t.value_s;
  }

  value_string& operator=(const value_string& ss)
  {
    value_s = ss.value_s;
    return *this;
  }

  value_string& operator=(vsx_string<> ss)
  {
    value_s = ss;
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

  static vsx_string<> get_type_name()
  {
    return "string";
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
    float delay; // seconds
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
  vsx_string<> name;
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

  void set_name(const vsx_string<> n)
  {
    name = n;
  }

  const vsx_string<>& get_name()
  {
    return name;
  }

  value_abs execute_absolute(float time) {
    return execute(time-i_time);
  }

  value_abs execute(float t_incr)
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
      if (line_time/cur_delay < 0.99f)
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

    return T();
  }

  vsx_string<> get_string()
  {
    vsx_nw_vector< vsx_string<> > parts;
    for (unsigned long i = 0; i < items.size(); ++i)
    {
      parts.push_back(
        vsx_string_helper::f2s(items[i].delay)+";"+
        vsx_string_helper::f2s((float)(int)items[i].interpolation)+";"+
        vsx_string_helper::base64_encode((items[i].get_value()))
      );
    }
    vsx_string<>deli = "|";
    return vsx_string_helper::implode(parts,deli);
  }

  void set_string(vsx_string<>str)
  {
    items.reset_used();
    vsx_nw_vector< vsx_string<> > keyframes;
    vsx_string_helper::explode_single(str, '|', keyframes);
    foreach(keyframes, i)
    {
      vsx_nw_vector< vsx_string<> > keyframe_parts;
      vsx_string_helper::explode_single(keyframes[i], ';', keyframe_parts);

      item n_i;
      n_i.delay = vsx_string_helper::s2f(keyframe_parts[0]);
      n_i.interpolation = (interpolation_t)(int)vsx_string_helper::s2f(keyframe_parts[1]);

      vsx_string<> value_string(vsx_string_helper::base64_decode(keyframe_parts[2]));

      if (n_i.interpolation != bezier)
        n_i.value.set_string(value_string);

      if (n_i.interpolation == 4) {
        vsx_nw_vector< vsx_string<> > value_parts;
        vsx_string_helper::explode_single(value_string, ':', value_parts);
        n_i.value.set_string(value_parts[0]);
        n_i.handle1 = vsx_vector2_helper::from_string<float>(value_parts[1]);
        n_i.handle2 = vsx_vector2_helper::from_string<float>(value_parts[2]);
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

  void load_from_file(vsx::filesystem* filesystem, vsx_string<> filename)
  {
    if (!filesystem->is_file(filename))
      return;

    vsx_string<> all_data = vsx_string_helper::read_from_file( filename );
    vsx_nw_vector< vsx_string<> > rows;
    vsx_string_helper::explode_single(all_data, '\n', rows);

    if (T::get_type_name() != rows[0])
      VSX_ERROR_RETURN_S("mismatching type for sequence ", filename.c_str() );

    set_string( rows[1] );
  }
};

}

}

#if COMPILER == COMPILER_VISUAL_STUDIO
#pragma warning( pop )
#endif
