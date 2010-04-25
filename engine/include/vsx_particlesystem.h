#ifndef VSX_PARTICLESYSTEM_H
#define VSX_PARTICLESYSTEM_H

typedef struct {
  vsx_vector pos; // current position
  vsx_vector speed; // current speed
  vsx_color color; // color it starts out as (including alpha)
  vsx_color color_end; // color it interpolates to (including alpha)
  vsx_quaternion rotation; // rotation vector
  vsx_quaternion rotation_dir; // rotation vector
  float orig_size; // size upon creation (also used for weight)
  float size; // rendering size
  float time; // how long it has lived
  float lifetime; // how long it can live
  int grounded; // if a particle is grounded it shouldn't move or rotate anymore, lying on the floor
} vsx_particle;



class vsx_particlesystem {
public:
  int timestamp;
//  unsigned long num_particles;
  vsx_array<vsx_particle>* particles;
  vsx_particlesystem() {
    particles = 0;
  }
};  


#endif
