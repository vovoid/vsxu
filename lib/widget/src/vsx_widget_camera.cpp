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

// global includes
#include <math/vector/vsx_vector3.h>
#include <string/vsx_string.h>

// widget
#include "vsx_widget_camera.h"
#include "vsx_widget_time.h"
#include "vsx_widget_global_interpolation.h"

vsx_widget_camera::vsx_widget_camera()
  :
    xps( 0.0f ),
    yps( 0.0f ),
    zps( 0.0f ),
    xpd( 0.0f ),
    ypd( 0.0f ),
    zpd( 0.0f ),
    xpp( 0.0f ),
    ypp( 0.0f ),
    zpp( 0.0f ),
    xp( 0.0f ),
    yp( 0.0f ),
    zp( 1.9f ),
    zpf( 0.0f ),
    zpa( 0.0f ),
    interpolation_speed( 10.0f ),
    key_speed( 3.0f ),
    interpolating(false)
{
}

void vsx_widget_camera::move_camera(vsx_vector3<> world)
{
  camera_target = world;
  interpolating = true;
}


void vsx_widget_camera::set_distance(double d)
{
  camera_target.x = (float)xp;
  camera_target.y = (float)yp;
  camera_target.z = (float)d;
  interpolating = true;
}

void vsx_widget_camera::run()
{
  double dtime = vsx_widget_time::get_instance()->get_dtime();
  double global_interpolation_speed = vsx_widget_global_interpolation::get_instance()->get();

  if (interpolating)
  {
    double tt = dtime * 10.0 * global_interpolation_speed;
    if (tt > 1) { tt = 1; interpolating = false;}
    xp = xp*(1.0-tt) + (double)camera_target.x * tt;
    yp = yp*(1.0-tt) + (double)camera_target.y * tt;
    zp = zp*(1.0-tt) + (double)camera_target.z * tt;
    if (
      ((int)round(xp*2000) == (int)round(camera_target.x*2000)) &&
      ((int)round(yp*2000) == (int)round(camera_target.y*2000)) &&
      ((int)round(zp*2000) == (int)round(camera_target.z*2000))
    )
      interpolating = false;
    return;
  }

  double acc = 4.0;
  double dec = 3.0;

  // interpolation falloff control
  double tt = dtime * (double)interpolation_speed * global_interpolation_speed;
  tt = clamp(tt, 0, 1);

  if (fabs(zpd) > 0.0) {
    double sgn = SGN(zpd);
    zps += dtime * acc * sgn * global_interpolation_speed;
    zps = CLAMP(zps, -1.2, 1.2);
  }

  if(fabs(zpd) < 0.00001) {
    double sgn = SGN(zps);
    zps -= dtime * dec * sgn * global_interpolation_speed;
    zps = MAX(zps * sgn, 0) * sgn;
  }

  zp +=
      zps * fabs(zp - 1.1) * (double)key_speed * dtime +
      zpp * (zp - 1.0);
  zpp = zpp*(1-tt);

  zp = CLAMP(zp, 1.2, 100.0);


  if(!DOUBLE_EQUALS(xpd, 0.0)) {
    double sgn = SGN(xpd);
    xps += dtime * acc * sgn * global_interpolation_speed;
    xps = CLAMP(xps, -1, 1);
  }
  if(DOUBLE_EQUALS(xpd, 0.0)) {
    double sgn = SGN(xps);
    xps -= dtime * dec * sgn * global_interpolation_speed;
    xps = MAX(xps * sgn, 0) * sgn;
  }

  xp +=
      xps * fabs(zp - 1.1) * (double)key_speed * dtime * 0.6
      +
      xpp * (zp - 1.0);
  xpp = xpp * (1 - tt);

  xp = CLAMP(xp, -10, 10);


  if(!DOUBLE_EQUALS(ypd, 0.0)) {
    double sgn = SGN(ypd);
    yps += dtime * acc * sgn * global_interpolation_speed;
    yps = CLAMP(yps, -1, 1);
  }
  if(DOUBLE_EQUALS(ypd, 0.0)) {
    double sgn = SGN(yps);
    yps -= dtime * dec * sgn * global_interpolation_speed;
    yps = MAX(yps * sgn, 0) * sgn;
  }
  yp += yps * fabs(zp - 1.1) * (double)key_speed * dtime*0.6 + ypp*(zp - 1.0);
  ypp = ypp*(1-tt);

  yp = CLAMP(yp, -10, 10);

}
