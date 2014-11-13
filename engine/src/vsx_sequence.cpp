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

#include<stdio.h>
#if !defined(__APPLE__) && !defined(linux)
#include<conio.h>
#endif
#include<stdlib.h>
#include "vsxfst.h"
#include <container/vsx_array.h>
#include <container/vsx_avector.h>
#include "vsx_bezier_calc.h"
#include "vsx_sequence.h"
#include <vector/vsx_vector3_helper.h>

vsx_sequence::vsx_sequence()
{
  reset();
  vsx_sequence_item a;
  a.delay = 0.5f;
  a.value = 1;
  a.interpolation = 0;
  items.push_back(a);
  a.delay = 0.5f;
  a.value = 1;
  a.interpolation = 0;
  items.push_back(a);
}

vsx_sequence::vsx_sequence(vsx_sequence& seq)
{
  for (unsigned long i = 0; i < seq.items.size(); ++i)
  {
    items[i] = seq.items[i];
  }
  timestamp = seq.timestamp;
}

vsx_sequence::vsx_sequence(const vsx_sequence& seq)
{
  vsx_sequence* sq = (vsx_sequence*)&seq;

  for (unsigned long i = 0; i < (sq->items.size()); ++i)
  {
    items[i] = sq->items[i];
  }

  timestamp = seq.timestamp;
}

vsx_sequence& vsx_sequence::operator=(vsx_sequence& ss)
{
  for (unsigned long i = 0; i < ss.items.size(); ++i)
  {
    items[i] = ss.items[i];
  }
  timestamp = ss.timestamp;
  return *this;
}

vsx_sequence::~vsx_sequence()
{
}

float vsx_sequence::execute(float t_incr)
{

  if (!items.size())
    return 0;

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
        //if
      }
    }
    cur_interpolation = items[line_cur].interpolation;
  }


  // positioning complete, now calculate value
  float cv = cur_val;
  float ev = to_val;
  float dv = ev-cv;


  // 0 = no interpolation
  // 1 = linear interpolation
  // 2 = cosine interpolation
  // 3 = reserved
  // 4 = bezier
  if (cur_interpolation == 4)
  {
    float t = (line_time/cur_delay);

    bez_calc.x0 = 0.0f;
    bez_calc.y0 = cv;
    bez_calc.x1 = items[line_cur].handle1.x;
    bez_calc.y1 = cv+items[line_cur].handle1.y;
    bez_calc.x2 = items[line_cur].handle2.x;
    bez_calc.y2 = ev+items[line_cur].handle2.y;
    bez_calc.x3 = 1.0f;
    bez_calc.y3 = ev;
    bez_calc.init();
    float tt = bez_calc.t_from_x(t);
    float rv = bez_calc.y_from_t(tt);
    return rv;
  }


  if (cur_interpolation == 0)
  {
    if (line_time/cur_delay < 0.99)
    {
      return cv;
    }
    return ev;
  }


  if (cur_interpolation == 1)
  {
    if (cur_delay != 0.0f)
    return cv+dv*(line_time/cur_delay);
    else return cv+dv;
  }


  if (cur_interpolation == 2)
  {
    float ft = line_time/cur_delay*PI_FLOAT;
    float f = (1 - (float)cos(ft)) * 0.5f;
    return cv*(1-f) + ev*f;
  }

  return 0.0f;
}

vsx_string<>vsx_sequence::get_string()
{
  vsx_avector< vsx_string<> > parts;
  for (unsigned long i = 0; i < items.size(); ++i)
  {
    parts.push_back(vsx_string_helper::f2s(items[i].delay)+";"+vsx_string_helper::f2s(items[i].interpolation)+";"+base64_encode((items[i].get_value())));
  }
  vsx_string<>deli = "|";
  return implode(parts,deli);
}


void vsx_sequence::set_string(vsx_string<>str)
{
  items.clear();
  vsx_avector< vsx_string<> > rows;
  vsx_string<>deli = "|";
  explode(str, deli, rows);
  vsx_string<>deli2 = ";";
  vsx_sequence_item n_i;
  for (unsigned long i = 0; i < rows.size(); ++i)
  {
    vsx_avector< vsx_string<> > parts;
    explode(rows[i], deli2,  parts);
    n_i.delay = vsx_string_helper::s2f(parts[0]);
    n_i.interpolation = (int)vsx_string_helper::s2f(parts[1]);
    vsx_string<>ff(base64_decode(parts[2]));
    if (n_i.interpolation < 4) {
      n_i.value = vsx_string_helper::s2f(ff);
    } else
    if (n_i.interpolation == 4) {
      vsx_avector< vsx_string<> > pld_l;
      vsx_string<>pdeli_l = ":";
      explode(ff, pdeli_l, pld_l);
      n_i.value = vsx_string_helper::s2f(pld_l[0]);
      n_i.handle1 = vsx_vector3_helper::from_string<float>(pld_l[1]);
      n_i.handle2 = vsx_vector3_helper::from_string<float>(pld_l[2]);
    }
    items.push_back(n_i);
  }
  float t = i_time;
  i_time = 0;
  i_cur = 0;
  line_time = 0;
  line_cur = 0;
  execute(t);
  ++timestamp;
}

void vsx_sequence::reset()
{
  i_time = 0;
  i_cur = 0;
  to_val = 0;
  line_cur = 0;
  line_time = 0;
}

