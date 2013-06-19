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


#ifndef VSX_PARTICLESYSTEM_H
#define VSX_PARTICLESYSTEM_H

typedef struct
{
  vsx_vector pos; // current position
  vsx_vector creation_pos; // position the particle had when created
  vsx_vector speed; // current speed
  vsx_color color; // color it starts out as (including alpha)
  vsx_color color_end; // color it interpolates to (including alpha)
  vsx_quaternion rotation; // rotation vector
  vsx_quaternion rotation_dir; // rotation vector
  float orig_size; // size upon creation (also used for weight)
  float size; // rendering size
  float time; // how long it has lived
  float lifetime; // how long it can live
  float one_div_lifetime;
  int grounded; // if a particle is grounded it shouldn't move or rotate anymore, lying on the floor
} vsx_particle;

class vsx_particlesystem {
public:
  int timestamp;
  vsx_array<vsx_particle>* particles;

  vsx_particlesystem() {
    particles = 0;
    timestamp = 0;
  }
};  


#endif
