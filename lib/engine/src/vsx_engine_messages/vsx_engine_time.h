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

// ***************************************
// Set time loop point
// ***************************************
// 0=time_set_loop_point 1=[time:float]
if (cmd == "time_set_loop_point")
{
  loop_point_end = vsx_string_helper::s2f(c->parts[1]);
  goto process_message_queue_end;
}



// ***************************************
// Set Time Progression Factor (Speed)
// ***************************************
// 0=seq_pool 1=time_set_loop_point 2=[time:float]
if (cmd == "time_set_speed")
{
  // sanity
  if ( c->parts.size() <= 1 )
    goto process_message_queue_end;

  set_speed( vsx_string_helper::s2f(c->parts[1]) );

  goto process_message_queue_end;
}




if (cmd == "play")
{
  time_play();
  goto process_message_queue_end;
}



if (cmd == "stop")
{
  current_state = VSX_ENGINE_STOPPED;
  goto process_message_queue_end;
}



if (cmd == "rewind")
{
  current_state = VSX_ENGINE_REWIND;
  goto process_message_queue_end;
}



if (cmd == "fps_d" || cmd == "fps")
{
  cmd_out->add_raw("fps_d " + vsx_string_helper::f2s((float)frame_delta_fps), VSX_COMMAND_GARBAGE_COLLECT);
  goto process_message_queue_end;
}




if (cmd == "time_set")
{
  float dd = engine_info.vtime - vsx_string_helper::s2f(c->parts[1]);
  if (dd > 0.0f)
  {
    engine_info.dtime = -dd;
    goto process_message_queue_end;
  }
  engine_info.dtime = fabs(dd);
  goto process_message_queue_end;
}
