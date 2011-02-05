#ifndef OGL_GRAPPA_CLONE_H
#define OGL_GRAPPA_CLONE_H
#ifndef VSXU_MAC_XCODE
#include <GL/glew.h>
#else
#include "glew.h"
#endif
#include <cstdlib>
#include <cmath>
#include <vector>
#include "vsx_math_3d.h"

//////////////////////////////////////////////////

/*class Vec3d{
public:
//  Vec3d() : x(0.0f), y(0.0f), z(0.0f) {}
  Vec3d(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
  float x, y, z;
};*/

//////////////////////////////////////////////////

class Mass {

public:
  Mass() : friction(0.07f), mass(1.0f) {};

  void init(vsx_vector pos, vsx_vector spd, float m = 1.0f) {
    position = pos;
    speed = spd;
    mass = m;
    friction = 1.5f;
  }

  inline void update(float timeDiff) {
    //printf("timeDiff: %f\n", mass);
    speed.x += (center.x - position.x) / mass * timeDiff;
    speed.y += (center.y - position.y) / mass * timeDiff;
    speed.z += (center.z - position.z) / mass * timeDiff;
//    float ft = (friction * timeDiff);
    //if (ft < 0.00001f)
    speed.x *= 1.0f - (friction * timeDiff);
    speed.y *= 1.0f - (friction * timeDiff);
    speed.z *= 1.0f - (friction * timeDiff);
    position.x += speed.x * timeDiff;
    position.y += speed.y * timeDiff;
    position.z += speed.z * timeDiff;
  }


  const vsx_vector& getPos() const {return position;}

  void setCenter(const float &x, const float &y, const float &z);

  float friction;
  float mass;
  vsx_vector position, speed;
  vsx_vector center;
};

//////////////////////////////////////////////////
//#define NUMBER 2
#define BUFF_LEN 1024

class gravity_lines {
public:
  bool first;
  int num_lines;
  float color0[4];
  float color1[4];
  float width;
  float wind_y;

  virtual void init();
  virtual void un_init() {
  	for (unsigned long i = 0; i < oldPos.size(); ++i) {
  		delete oldPos[i];
    }
  }
  virtual void reset_pos(float x, float y, float z);
  virtual void update(float delta_time, float x, float y, float z);
  virtual void render();

  float curr_time, last_step_time, step_freq;
  float friction;

  unsigned long offs;
  std::vector<Mass> masses;
  std::vector<vsx_vector*> oldPos;
  //vsx_vector oldPos[NUMBER][BUFF_LEN];
  ~gravity_lines();
};

#endif
