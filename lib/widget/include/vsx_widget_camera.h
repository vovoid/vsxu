/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#ifndef VSX_WIDGET_CAMERA_H
#define VSX_WIDGET_CAMERA_H

#include <math/vector/vsx_vector3.h>
#include "vsx_widget_dllimport.h"

class WIDGET_DLLIMPORT vsx_widget_camera
{
  double xps, yps, zps; //current speed
  double xpd, ypd, zpd; //current direction
  double xpp, ypp, zpp; // additional movement

  vsx_vector3<> camera_target;

  double xp;
  double yp;
  double zp;
  double zpf;
  double zpa;

  float interpolation_speed;
  float key_speed;

  bool interpolating;

public:

  vsx_widget_camera();

  void move_camera(vsx_vector3<> world);
  void start_interpolation();
  void stop_interpolation();

  vsx_vector3<> get_pos()
  {
    return vsx_vector3<>(xp, yp, zp);
  }

  void set_pos( vsx_vector3<> destination )
  {
    camera_target = destination;
    interpolating = true;
  }

  vsx_vector3<> get_pos_2d()
  {
    return vsx_vector3<>(xp, yp, 0.0);
  }

  void set_distance(double d);

  void increase_zps(double d)
  {
    zps += d;

    if (zps > 2.0)
      zps = 2.0;

    if (zps < -2.0)
      zps = -2.0;
  }

  void set_movement_x(double d)
  {
    xpd = d;
  }

  void set_movement_y(double d)
  {
    ypd = d;
  }

  void set_movement_z(double d)
  {
    zpd = d;
  }

  void set_extra_movement( vsx_vector3f m)
  {
    xpp = m.x;
    ypp = m.y;
    zpp = m.z;
  }

  void set_key_speed(double d)
  {
    key_speed = d;
  }

  float get_pos_x()
  {
    return xp;
  }

  float get_pos_y()
  {
    return yp;
  }

  float get_pos_z()
  {
    return zp;
  }

  void run();
};


#endif
