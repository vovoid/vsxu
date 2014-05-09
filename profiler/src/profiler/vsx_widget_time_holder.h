/**
* Project: VSXu Profiler - Data collection and data visualizer
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2014
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

#ifndef VSX_WIDGET_TIME_HOLDER_H
#define VSX_WIDGET_TIME_HOLDER_H

class vsx_widget_time_holder
{
  float start;
  float end;
  float cur;

public:

  inline float end_get()
  {
    return end;
  }

  inline void end_set(float n)
  {
    end = n;
  }

  inline void end_inc(float n)
  {
    end += n;
  }

  inline float start_get()
  {
    return start;
  }

  inline void start_set(float n)
  {
    start = n;
  }

  inline void start_inc(float n)
  {
    start += n;
  }

  inline float cur_get()
  {
    return cur;
  }

  inline void cur_set(float n)
  {
    cur = n;
  }

  inline float diff()
  {
    return end - start;
  }

  vsx_widget_time_holder()
    :
    start( -0.5f ),
    end( 5.0f ),
    cur( 0.0f )
  {}
};

#endif
