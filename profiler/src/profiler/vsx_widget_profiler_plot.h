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

#ifndef VSX_WIDGET_PROFILER_PLOT_H
#define VSX_WIDGET_PROFILER_PLOT_H

// widget
#include <vsx_widget.h>

// engine_graphics
#include <vsx_vbo_bucket.h>


#include "vsx_widget_time_holder.h"

#include "vsx_profiler_consumer.h"
typedef struct
{
  GLuint a;
} line_index_single;


class vsx_widget_profiler_plot : public vsx_widget
{
  // profiler for profiling ourselves
  vsx_profiler* profiler;

  vsx_vbo_bucket<line_index_single, 1, GL_LINE_STRIP, GL_STREAM_DRAW> draw_bucket_a;
  vsx_vbo_bucket<line_index_single, 1, GL_LINE_STRIP, GL_STREAM_DRAW> draw_bucket_b;
  vsx_vbo_bucket<line_index_single, 1, GL_LINE_STRIP, GL_STREAM_DRAW> draw_bucket_c;
  vsx_vbo_bucket<line_index_single, 1, GL_LINE_STRIP, GL_STREAM_DRAW> draw_bucket_d;

  vsx_profiler_consumer_chunk* selected_chunk;

  vsx_avector<vsx_profiler_consumer_plot> consumer_chunks;

  vsx_vector<> mouse_pos;

  void update_vbo();

public:

  vsx_widget_profiler_plot()
    :
      selected_chunk( 0x0 )
  {}

  void init();

  void load_plot(uint64_t plot_id);

  void update();

  void i_draw();

  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);

  void event_mouse_wheel(float y);

  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void interpolate_size();


};


#endif
