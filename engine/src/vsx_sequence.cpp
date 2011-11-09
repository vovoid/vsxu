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

#include<stdio.h>
#if !defined(__APPLE__) && !defined(linux)
#include<conio.h>
#endif
#include<stdlib.h>
//#ifndef VSX_STRINGLIB_NOSTL
//  #include <list>
//  #include <vector>
//#endif
#include "vsxfst.h"
#include "vsx_math_3d.h"
#include "vsx_array.h"
#include "vsx_avector.h"
#include "vsx_bezier_calc.h"
#include "vsx_sequence.h"

vsx_sequence::vsx_sequence() {
  //printf("vsx_sequence main constructor %d\n",this);
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

vsx_sequence::vsx_sequence(vsx_sequence& seq) {
  //printf("sequence copy constructor\n");
  for (unsigned long i = 0; i < seq.items.size(); ++i) {
    //printf("copying value: %f\n",seq.items[i].value);
    items[i] = seq.items[i];
    //printf("resulting value: %f\n",items[i].value);
  }
  timestamp = seq.timestamp;
}

vsx_sequence::vsx_sequence(const vsx_sequence& seq) {
  //printf("sequence copy constructor\n");
	vsx_sequence* sq = (vsx_sequence*)&seq;
  for (unsigned long i = 0; i < (sq->items.size()); ++i) {
    //printf("copying value: %f\n",seq.items[i].value);
    items[i] = sq->items[i];
    //printf("resulting value: %f\n",items[i].value);
  }
  timestamp = seq.timestamp;
}

vsx_sequence& vsx_sequence::operator=(vsx_sequence& ss) {
  //printf("sequence = operator\n");
  for (unsigned long i = 0; i < ss.items.size(); ++i) {
    //printf("copying value: %f\n",ss.items[i].value);
    items[i] = ss.items[i];
    //printf("resulting value: %f\n",items[i].value);
  }
  timestamp = ss.timestamp;
  return *this;
}

vsx_sequence::~vsx_sequence() {
  //printf("vsx sequence destructor %d\n",this);
}

float vsx_sequence::execute(float t_incr) {
  //printf("-----------\nt_incr: %f\ni_time: %f\n",t_incr,i_time);
  //printf("items.size: %d\n",items.size());

  if (!items.size()) return 0;
  if (items.size() < 2)
  {
    i_time += t_incr;
    //printf("return %d\n",__LINE__);
    return items[0].value;
  }
  else
  {
    // if run for the first time
    if (i_time == 0 && i_cur == 0)
    {
      //printf("running first foobar\n");
      cur_val = items[0].value;
      cur_delay = items[0].delay;
      cur_interpolation = items[0].interpolation;
      //if (items.size() > 1)
      to_val = items[1].value;
    }
    i_time += t_incr;
    //printf("a\n");
    int c = 0;
    line_time += t_incr;
    //printf("line_cur: %d\n",line_cur);
    //printf("cur_delay: %f\n",cur_delay);
    //printf("cur_val: %f\n",cur_val);
    //printf("line_time: %f\n",line_time);

    // The assumption for this algorithm is that you seldom jump quickly from beginning to end,
    // if you were to do that you could use a plain oscillator.
    // So in most cases it saves computing cycles to move from the position we were before.
    // Hence it's not worth always iterating from the beginning when being presented with
    // a new time stamp.

    // deal with moving backwards (dt < 0)
    if (t_incr < 0)
    {
      //bool stop = false;
      if (
      				(line_time) < 0
      			/*&&
      				line_cur != 0*/
      	 )
      {
        // we've passed behind our line, but how long?
        // calculate that here
        while (line_time < 0)
        {
          ++c;
          --line_cur;
          if (line_cur < 0) {
            //printf("__f__ %d ",c);
            line_cur = 0;
            line_time = 0;
            //cur_val = items[line_cur].value;
            //cur_delay = items[line_cur].delay;
            //cur_interpolation = items[line_cur].interpolation;
            //to_val = items[line_cur+1].value;
          } else {
            //printf("__a__ %d ",c);
            line_time +=  items[line_cur].delay;
            //cur_val = items[line_cur].value;
            //cur_delay = items[line_cur].delay;
            //cur_interpolation = items[line_cur].interpolation;
            //to_val = items[line_cur+1].value;
          }
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
    //printf("line_time: %f\n",line_time);

    // 0 = no interpolation
    // 1 = linear interpolation
    // 2 = cosine interpolation
    // 3 = reserved
    // 4 = bezier
    if (cur_interpolation == 4)
    {
      float t = (line_time/cur_delay);

      //printf("handle1.x: %f\n",lines[line_cur].handle1.x);
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
      //printf("return %d\n",__LINE__);
      return rv;
      //printf("rv: %f\n",rv);
      //param->set_string(f2s(rv));

    } else
    if (cur_interpolation == 0)
    {
      if (line_time/cur_delay < 0.99)
      {
      	//printf("return %d\n",__LINE__);
      return cv;
      }
      else
      {
//      	printf("return %d\n",__LINE__);
      return ev;
      }
    }
    else
    if (cur_interpolation == 1)
    {
      //printf("return %d\n",__LINE__);
      if (cur_delay != 0.0f)
      return cv+dv*(line_time/cur_delay);
      else return cv+dv;
    }
    else
    if (cur_interpolation == 2)
    {
      float ft = line_time/cur_delay*PI_FLOAT;
      float f = (1 - (float)cos(ft)) * 0.5f;
      //printf("return %d\n",__LINE__);
      return cv*(1-f) + ev*f;
    }
  }
  return 0.0f;
}

vsx_string vsx_sequence::get_string() {
  #ifdef VSXU_EXE
  //printf("sequence internal get string\n");
  #else
  //printf("sequence internal get string MODULE\n");
  #endif
  vsx_avector<vsx_string> parts;
  for (unsigned long i = 0; i < items.size(); ++i) {
    //printf("value: %f\n",items[i].value);
    parts.push_back(f2s(items[i].delay)+";"+f2s(items[i].interpolation)+";"+base64_encode((items[i].get_value())));
  }
  vsx_string deli = "|";
  return implode(parts,deli);
}

void vsx_sequence::set_string(vsx_string str) {
  //printf("sequence internal set string\n");
  //printf("string to set: %s\n",str.c_str());
  items.clear();
  vsx_avector<vsx_string> rows;
  vsx_string deli = "|";
  explode(str, deli, rows);
  vsx_string deli2 = ";";
  vsx_sequence_item n_i;
  //for (std::vector<vsx_string>::iterator it = rows.begin(); it != rows.end(); ++it) {
  for (unsigned long i = 0; i < rows.size(); ++i)
  {
    //printf("inner string: %s\n",(*it).c_str());
    vsx_avector<vsx_string> parts;
    explode(rows[i], deli2,  parts);
    //printf("parts size: %d\n",parts.size());
    n_i.delay = s2f(parts[0]);
    n_i.interpolation = (int)s2f(parts[1]);
    vsx_string ff(base64_decode(parts[2]));
    if (n_i.interpolation < 4) {
			//printf("value: %s\n",ff.c_str());
      n_i.value = s2f(ff);
    } else
    if (n_i.interpolation == 4) {
      vsx_avector<vsx_string> pld_l;
      vsx_string pdeli_l = ":";
      explode(ff, pdeli_l, pld_l);
      n_i.value = s2f(pld_l[0]);
      n_i.handle1.from_string(pld_l[1]);
      n_i.handle2.from_string(pld_l[2]);
    }
    items.push_back(n_i);
  }
  float t = i_time;
  //printf("i_time: %f\n",i_time);
  i_time = 0;
  i_cur = 0;
  line_time = 0;
  line_cur = 0;
  execute(t);
  ++timestamp;
//  _sleep(1000);
}

void vsx_sequence::reset() {
//  timestamp = 0;
  i_time = 0;
  i_cur = 0;
  to_val = 0;
  line_cur = 0;
  line_time = 0;
}

