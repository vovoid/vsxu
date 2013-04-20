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

#ifndef VSX_BSPLINE_H
#define VSX_BSPLINE_H

class vsx_bspline {
public:
  float		current_pos;
  float   real_pos;
  vsx_vector	p0, p1, p2, p3;
  float		minDistNext;
  float		maxDistNext;
  vsx_vector center;
  float		radius;
  long old_pos;
  vsx_array<vsx_vector> points;
  
  vsx_bspline() : real_pos(0.0f),old_pos(0) {
 		current_pos = 0.0f;
  };
  
  void init_random_points() {
    for (int i = 0; i < 250; ++i) {
      points.push_back(
        vsx_vector(
          ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f),
          ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f),
          ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f)
        )
      );
    }
    p0 = points[0];
    p1 = points[1];
    p2 = points[2];
    p3 = points[3];

  }

  ~vsx_bspline() {
  }
  
	vsx_bspline(vsx_vector _center, float _radius, float _minDistNext, float _maxDistNext) {
		current_pos = 0.0f;
		center = _center;
		radius = _radius;
		minDistNext = _minDistNext;
		maxDistNext = _maxDistNext;
	};

	void init(vsx_vector _center, float _radius, float _minDistNext, float _maxDistNext) {
		center = _center;
		radius = _radius;
		minDistNext = _minDistNext;
		maxDistNext = _maxDistNext;
	};
	
	void set_pos(float t) {
    int tt = (int)t;
    //float tt = fmod(t,(float)points.size()-4);
    if (tt != old_pos) {
      old_pos = tt;
      p0 = points[(old_pos)%points.size()];
      p1 = points[(old_pos+1)%points.size()];
      p2 = points[(old_pos+2)%points.size()];
      p3 = points[(old_pos+3)%points.size()];
    }
    real_pos = current_pos = t;
    if (current_pos > 1.0f) current_pos -= (int)current_pos;
  }
  
  void step(float stepn) {
    set_pos(real_pos+stepn);
  }

	vsx_vector calc_coord() {
    vsx_vector v;
		float t = current_pos;
		float t2 = t * t;
		float t3 = t2 * t;

		float k1 = 1.0f - 3.0f * t + 3.0f * t2 - t3;
		float k2 = 4.0f - 6.0f * t2 + 3.0f * t3;
		float k3 = 1.0f + 3.0f * t + 3.0f * t2 - 3.0f * t3;

		v = (p0 * k1 + 
				p1 * k2 + 
				p2 * k3 + 
				p3 * t3) * (1.0f / 6.0f);
		return v;
	};
};

#endif
