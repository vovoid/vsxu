/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
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

#include "_configuration.h"
#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_quaternion.h"


class vsx_module_plugin_fluid : public vsx_module {
  int i;
  float time;
  vsx_particlesystem* particles;
  vsx_module_param_particlesystem* in_particlesystem; 
  vsx_module_param_float3* actor;
  vsx_module_param_float* strength;
  vsx_module_param_int* draw_velocity;
  // out
  vsx_module_param_particlesystem* result_particlesystem; 

  #define IX(i,j) ((i)+(N+2)*(j))
  
  int N;
  float dt, diff, visc;
  float force, source;
  int dvel;
  
  float * u, * v, * u_prev, * v_prev;
  float * dens, * dens_prev;
  
  //int win_id;
  //int win_x, win_y;
  //int mouse_down[3];
  float omx, omy;//, mx, my;
  
  #define IX(i,j) ((i)+(N+2)*(j))
  #define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
  #define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) {
  #define END_FOR }}
  
  void add_source ( int Nl, float * x, float * s, float dtt )
  {
    int ii, size=(Nl+2)*(Nl+2);
    for ( ii=0 ; ii<size ; ii++ ) x[ii] += dtt*s[ii];
  }
  
  void set_bnd ( int NL, int b, float * x )
  {
    int i;
  
    for ( i=1 ; i<=NL ; i++ ) {
      x[IX(0  ,i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)];
      x[IX(NL+1,i)] = b==1 ? -x[IX(NL,i)] : x[IX(NL,i)];
      x[IX(i,0  )] = b==2 ? -x[IX(i,1)] : x[IX(i,1)];
      x[IX(i,NL+1)] = b==2 ? -x[IX(i,NL)] : x[IX(i,NL)];
    }
    x[IX(0  ,0  )] = 0.5f*(x[IX(1,0  )]+x[IX(0  ,1)]);
    x[IX(0  ,NL+1)] = 0.5f*(x[IX(1,NL+1)]+x[IX(0  ,NL)]);
    x[IX(NL+1,0  )] = 0.5f*(x[IX(NL,0  )]+x[IX(NL+1,1)]);
    x[IX(NL+1,NL+1)] = 0.5f*(x[IX(NL,NL+1)]+x[IX(NL+1,NL)]);
  }
  
  void lin_solve ( int NL, int b, float * x, float * x0, float a, float c )
  {
    int i, j, k;
  
    for ( k=0 ; k<20 ; k++ ) {
      FOR_EACH_CELL
        x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/c;
      END_FOR
      set_bnd ( NL, b, x );
    }
  }
  
  void diffuse ( int NL, int b, float * x, float * x0, float diff_l, float dtt )
  {
    float a=dtt*diff_l*NL*NL;
    lin_solve ( NL, b, x, x0, a, 1+4*a );
  }
  
  void advect ( int NL, int b, float * d, float * d0, float * u, float * v, float dt )
  {
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1, dt0;
  
    dt0 = dt*NL;
    FOR_EACH_CELL
      x = i-dt0*u[IX(i,j)]; y = j-dt0*v[IX(i,j)];
      if (x<0.5f) x=0.5f; if (x>NL+0.5f) x=NL+0.5f; i0=(int)x; i1=i0+1;
      if (y<0.5f) y=0.5f; if (y>NL+0.5f) y=NL+0.5f; j0=(int)y; j1=j0+1;
      s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
      d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+
             s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
    END_FOR
    set_bnd ( NL, b, d );
  }
  
  void project ( int NL, float * u, float * v, float * p, float * div )
  {
    int i, j;
  
    FOR_EACH_CELL
      div[IX(i,j)] = -0.5f*(u[IX(i+1,j)]-u[IX(i-1,j)]+v[IX(i,j+1)]-v[IX(i,j-1)])/NL;
      p[IX(i,j)] = 0;
    END_FOR 
    set_bnd ( NL, 0, div ); set_bnd ( NL, 0, p );
  
    lin_solve ( NL, 0, p, div, 1, 4 );
  
    FOR_EACH_CELL
      u[IX(i,j)] -= 0.5f*NL*(p[IX(i+1,j)]-p[IX(i-1,j)]);
      v[IX(i,j)] -= 0.5f*NL*(p[IX(i,j+1)]-p[IX(i,j-1)]);
    END_FOR
    set_bnd ( NL, 1, u ); set_bnd ( NL, 2, v );
  }
  
  /*void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt )
  {
    add_source ( N, x, x0, dt );
    SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
    SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt );
  }*/
  
  void vel_step ( int NL, float * u, float * v, float * u0, float * v0, float visc, float dt )
  {
    add_source ( NL, u, u0, dt ); add_source ( NL, v, v0, dt );
    SWAP ( u0, u ); diffuse ( NL, 1, u, u0, visc, dt );
    SWAP ( v0, v ); diffuse ( NL, 2, v, v0, visc, dt );
    project ( NL, u, v, u0, v0 );
    SWAP ( u0, u ); SWAP ( v0, v );
    advect ( NL, 1, u, u0, u0, v0, dt ); advect ( NL, 2, v, v0, u0, v0, dt );
    project ( NL, u, v, u0, v0 );
  }
  
  void free_data ( void )
  {
    if ( u ) free ( u );
    if ( v ) free ( v );
    if ( u_prev ) free ( u_prev );
    if ( v_prev ) free ( v_prev );
    if ( dens ) free ( dens );
    if ( dens_prev ) free ( dens_prev );
  }

  void clear_data ( void )
  {
    int i, size=(N+2)*(N+2);

    for ( i=0 ; i<size ; i++ ) {
      u[i] = v[i] = u_prev[i] = v_prev[i] = dens_prev[i] = 0.0f;
      dens[i] = rand()%1000 * 0.0005f;
    }
  }

  int allocate_data ( void )
  {
    int size = (N+2)*(N+2);

    u     = (float *) malloc ( size*sizeof(float) );
    v     = (float *) malloc ( size*sizeof(float) );
    u_prev    = (float *) malloc ( size*sizeof(float) );
    v_prev    = (float *) malloc ( size*sizeof(float) );
    dens    = (float *) malloc ( size*sizeof(float) );  
    dens_prev = (float *) malloc ( size*sizeof(float) );

    if ( !u || !v || !u_prev || !v_prev || !dens || !dens_prev ) {
      fprintf ( stderr, "cannot allocate data\n" );
      return ( 0 );
    }

    return ( 1 );
  }

  
  void draw_velocity_func ( void )
 {
   int i, j;
   float x, y, h;

   h = 1.0f/N;

   glColor4f ( 1.0f, 1.0f, 1.0f,0.2f );
   glLineWidth ( 1.0f );

   glBegin ( GL_LINES );

     for ( i=1 ; i<=N ; i++ ) {
       x = (i-0.5f)*h;
       for ( j=1 ; j<=N ; j++ ) {
         y = (j-0.5f)*h;
 //printf("foo");
         glVertex3f ( x * N, 0, y * N );
         glVertex3f ( N*(x+u[IX(i,j)]), 0, N*(y+v[IX(i,j)]) );
       }
     }

   glEnd ();
 }

  
  
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "particlesystems;modifiers;particle_fluid_deformer";
    info->description = "";
    info->out_param_spec = "particlesystem:particlesystem";
    info->in_param_spec = "in_particlesystem:particlesystem,actor:float3,strength:float,draw_velocity:enum?no|yes";
    info->component_class = "particlesystem";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");
    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");
  
    actor = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "actor");
    omx = 0.0f;
    omy = 0.0f;
    strength = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "strength");
    strength->set(20.0f);
    draw_velocity = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "draw_velocity");
    N = 40;
    dt = 0.1f;
    diff = 0.0f;
    visc = 0.001f;
    force = 20.8f;
    source = 10.0f;
    if ( !allocate_data () ) exit ( 1 );
  clear_data ();
    
  }
  
  void run() {
    particles = in_particlesystem->get_addr();  
    if (particles) {
    
      // get positions from the user
      float px = actor->get(0);
      float py = actor->get(1);

      int i, j, size = (N+2)*(N+2);

      for ( i=0 ; i<size ; i++ ) {
        u_prev[i] = v_prev[i] = dens_prev[i] = 0.0f;
      }

      //if ( !mouse_down[0] && !mouse_down[1] ) return;

      i = (int)((       px )*N+1);
      j = (int)((( py))*N+1);

      if ( i<1 || i>N || j<1 || j>N ) return;

      if (omx-px != 0.0f || omy-py != 0.0f)
      {
        //printf("pos changed! %f %f",px,omx);
      //if ( mouse_down[0] ) {
        u[IX(i,j)] = force * (px-omx);
        v[IX(i,j)] = force * (py-omy);
      }
      
      omx = px;
      omy = py;

      dt = 0.01f;//engine->dtime;
      
      vel_step ( N, u, v, u_prev, v_prev, visc, dt );

      //if ( mouse_down[1] ) {
//        d[IX(i,j)] = source;
      //}
      
      float _strength = strength->get();

      // go through all particles, they travel within 0.0..N
      for (unsigned long i = 0; i <  particles->particles->size(); ++i) {
        // add the delta-time to the time of the particle
        float mpx = (*particles->particles)[i].pos.x;
        float mpy = (*particles->particles)[i].pos.z;
        int dpx = (int)round(mpx);
        int dpy = (int)round(mpy);
        //printf("dpxy: %f, %f\n",dpx,dpy);
        
        if (dpx+1 > N) dpx = N;
        if (dpx < 1) dpx = 1;
        if (dpy+1 > N) dpy = N;
        if (dpy < 1) dpy = 1;
        //float fpx = (float)floor(px);
        //float fpy = (float)floor(py);
        
        //float ax,ay,bx,by,cx,cy,dx,dy;
        //cx = ax = mpx-fpx;
        //ay = mpy-fpy;
        //dx = bx = 1.0f-ax;
        //by = ay;
        //dy = cy = 1.0f-ay;

        //float va = 1.0f-ax*ay;
        //float vb = 1.0f-bx*by;
        //float vc = 1.0f-cx*cy;
        //float vd = 1.0f-dx*dy;
        
        
        (*particles->particles)[i].speed.x = u[IX(dpx,dpy)] * _strength;// + u[IX(dpx+1,dpy)]*vb + u[IX(dpx,dpy+1)]*vc + u[IX(dpx+1,dpy+1)]*vd;

        (*particles->particles)[i].speed.z = v[IX(dpx,dpy)] * _strength;// + v[IX(dpx+1,dpy)]*vb + v[IX(dpx,dpy+1)]*vc + v[IX(dpx+1,dpy+1)]*vd;

        //(*particles->particles)[i].pos.x += px*engine->dtime;
        //(*particles->particles)[i].pos.y = 0;//py*engine->dtime;
        //(*particles->particles)[i].pos.z += pz*engine->dtime;
      }
      if (draw_velocity->get()) draw_velocity_func();
      // in case some modifier has decided to base some mesh or whatever on the particle system
      // increase the timsetamp so that module can know that it has to copy the particle system all
      // over again.
      // bear in mind however that particlsystems are not commonly copied (unless you want to base something on it)
      // as a particle lives on to the next frame. So modifiers should just work with the values and increase or
      // decrease them. for instance, wind is applied adding or subtracting a value.. or possibly modifying the
      // speed component.
      
      // set the resulting value
      result_particlesystem->set_p(*particles);
      return;
      // now all left is to render this, that will be done one of the modules of the rendering branch
    }
    result_particlesystem->valid = false;
  }
};


class vsx_module_plugin_wind : public vsx_module {
	float time;
	vsx_particlesystem* particles;
	vsx_module_param_particlesystem* in_particlesystem;	
	vsx_module_param_float3* wind;
	// out
	vsx_module_param_particlesystem* result_particlesystem;	

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "particlesystems;modifiers;basic_wind_deformer";
    info->description = "";
    info->out_param_spec = "particlesystem:particlesystem";
    info->in_param_spec = "in_particlesystem:particlesystem,wind:float3";
    info->component_class = "particlesystem";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");
  //  in_particlesystem->set(particles);
  
    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");
  //  result_particlesystem->set(particles);
  
    // oh, all these parameters.. ..
  	wind = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"wind");
  	// set the position of the particle spray emitter to the center
  	wind->set(0,0);
  	wind->set(0,1);
  	wind->set(0,2);
    // default is 100 particles, should be enough for most effects (tm)
  }
  
  void run() {
    particles = in_particlesystem->get_addr();  
    if (particles) {
    
      // get positions from the user
      float px = wind->get(0);
      float py = wind->get(1);
      float pz = wind->get(2);
      
      // go through all particles
      for (unsigned long i = 0; i <  particles->particles->size(); ++i) {
        // add the delta-time to the time of the particle
        (*particles->particles)[i].pos.x += px*engine->dtime;
        (*particles->particles)[i].pos.y += py*engine->dtime;
        (*particles->particles)[i].pos.z += pz*engine->dtime;
      } 
      // in case some modifier has decided to base some mesh or whatever on the particle system
      // increase the timsetamp so that module can know that it has to copy the particle system all
      // over again.
      // bear in mind however that particlsystems are not commonly copied (unless you want to base something on it)
      // as a particle lives on to the next frame. So modifiers should just work with the values and increase or
      // decrease them. for instance, wind is applied adding or subtracting a value.. or possibly modifying the
      // speed component.
      
      // set the resulting value
      result_particlesystem->set_p(*particles);
      return;
      // now all left is to render this, that will be done one of the modules of the rendering branch
    }
    result_particlesystem->valid = false;
  }
};


class vsx_module_plugin_gravity : public vsx_module {
	float time;
	vsx_particlesystem* particles;
	vsx_module_param_particlesystem* in_particlesystem;	
	vsx_module_param_float3* center;
	vsx_module_param_float3* amount;
	vsx_module_param_float3* friction;
	vsx_module_param_int* mass_type;
	vsx_module_param_int* time_source;	
	vsx_module_param_float* uniform_mass;
	// out
	vsx_module_param_particlesystem* result_particlesystem;	

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "particlesystems;modifiers;basic_gravity";
    info->description = "";
    info->out_param_spec = "particlesystem:particlesystem";
    info->in_param_spec = "in_particlesystem:particlesystem,\
    basic_parameters:complex{\
      center:float3,\
      amount:float3,\
      friction:float3,\
      time_source:enum?sequencer|real\
    },\
    mass_calculations:complex{\
      mass_type:enum?individual|uniform,\
      uniform_mass:float\
    }";
    info->component_class = "particlesystem";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");
  //  in_particlesystem->set(particles);
  
    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");
  //  result_particlesystem->set(particles);
    mass_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"mass_type");
    time_source = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"time_source");
    uniform_mass = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"uniform_mass");
    uniform_mass->set(0.1f);
    // oh, all these parameters.. ..
  	center = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"center");
  	center->set(0.0f,0);
  	center->set(0.0f,1);
  	center->set(0.0f,2);
  	amount = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"amount");
  	amount->set(0.0f,0);
  	amount->set(1.0f,1);
  	amount->set(0.0f,2);
  	friction = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"friction");
  	friction->set(1.5f,0);
  	friction->set(1.5f,1);
  	friction->set(1.5f,2);
    // default is 100 particles, should be enough for most effects (tm)
  }
  
  void run() {
  float ddtime;
  if (time_source->get()) {
     ddtime = engine->real_dtime;
  } else ddtime = engine->dtime;
    particles = in_particlesystem->get_addr();  
    if (particles) {
    
      // get positions from the user
      float cx = center->get(0);
      float cy = center->get(1);
      float cz = center->get(2);
      
      // go through all particles
      float orig_size;
      float fricx = 1.0f-friction->get(0) * ddtime;
      float fricy = 1.0f-friction->get(1) * ddtime;
      float fricz = 1.0f-friction->get(2) * ddtime;
      float ax = amount->get(0)*ddtime;
      float ay = amount->get(1)*ddtime;
      float az = amount->get(2)*ddtime;
      if (mass_type->get() == 0) {
        unsigned long nump = particles->particles->size();
        vsx_particle* pp = particles->particles->get_pointer();
        for (unsigned long i = 0; i <  nump; ++i) {
          if ((*pp).time < (*pp).lifetime) {
            // add the delta-time to the time of the particle
            orig_size = 1.0f / (*pp).orig_size;
            //printf("orig size: %f\n", orig_size);
            (*pp).speed.x += ax*((cx - (*pp).pos.x) * orig_size);
            (*pp).speed.x *= fricx;

            (*pp).speed.y += ay*((cy - (*pp).pos.y) * orig_size);
            (*pp).speed.y *= fricy;

            (*pp).speed.z += az*((cz - (*pp).pos.z) * orig_size);
            (*pp).speed.z *= fricz;
          }
          pp++;
        } 
      } else
      {
        orig_size = 1.0f / uniform_mass->get();
        unsigned long nump = particles->particles->size();
        vsx_particle* pp = particles->particles->get_pointer();
        for (unsigned long i = 0; i <  nump; ++i) {
          if ((*pp).time < (*pp).lifetime) {
            // add the delta-time to the time of the particle
            (*pp).speed.x += ax*((cx - (*pp).pos.x) * orig_size);
            (*pp).speed.x *= fricx;

            (*pp).speed.y += ay*((cy - (*pp).pos.y) * orig_size);
            (*pp).speed.y *= fricy;

            (*pp).speed.z += az*((cz - (*pp).pos.z) * orig_size);
            (*pp).speed.z *= fricz;
          }
          pp++;
        } 
      }
      
      // set the resulting value
      result_particlesystem->set_p(*particles);
      return;
      // now all left is to render this, that will be done one of the modules of the rendering branch
    }
    result_particlesystem->valid = false;
  }
};


class vsx_module_plugin_particle_2d_rotator : public vsx_module {
  float time;
  vsx_particlesystem* particles;
  vsx_module_param_particlesystem* in_particlesystem;
  vsx_module_param_quaternion* rotation_dir;
  // out
  vsx_module_param_particlesystem* result_particlesystem;

  vsx_array<vsx_quaternion> rotation;
  vsx_array<vsx_quaternion> rotation_delta;
  vsx_quaternion q1;
  vsx_quaternion* q_out;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "particlesystems;modifiers;particle_2d_rotator";
    info->description = "";
    info->out_param_spec = "particlesystem:particlesystem";
    info->in_param_spec = "in_particlesystem:particlesystem,rotation_dir:quaternion";
    info->component_class = "particlesystem";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");

    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");

    rotation_dir = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"wind");
    // set the position of the particle spray emitter to the center
    rotation_dir->set(0.0f,0);
    rotation_dir->set(0.0f,1);
    rotation_dir->set(0.0f,2);
    rotation_dir->set(1.0f,3);
    // default is 100 particles, should be enough for most effects (tm)
  }

  void run() {
    particles = in_particlesystem->get_addr();
    if (particles) {
      if (rotation.size() != particles->particles->size())
      {
        rotation.reset_used();
        rotation_delta.reset_used();
        rotation.allocate(particles->particles->size());
        rotation_delta.allocate(particles->particles->size());
      }
      for (unsigned long i = 0; i < particles->particles->size(); ++i)
      {
        q_out = &(*particles->particles)[i].rotation;
        q1.x = 0.0f;
        q1.w = 1.0f;
        q1.y = rotation_dir->get(0) * engine->dtime;
        q1.z = 0.0f;
        q1.normalize();
        q_out->mul(*q_out, q1);

        q1.x = 0.0f;
        q1.w = 1.0f;
        q1.y = 0.0f;
        q1.z = rotation_dir->get(0) * engine->dtime;
        q1.normalize();
        q_out->mul(*q_out, q1);
      }
      // get positions from the user
      /*float px = wind->get(0);
      float py = wind->get(1);
      float pz = wind->get(2);

      // go through all particles
      for (unsigned long i = 0; i <  particles->particles->size(); ++i) {
        // add the delta-time to the time of the particle
        (*particles->particles)[i].pos.x += px*engine->dtime;
        (*particles->particles)[i].pos.y += py*engine->dtime;
        (*particles->particles)[i].pos.z += pz*engine->dtime;
      }*/
      // in case some modifier has decided to base some mesh or whatever on the particle system
      // increase the timsetamp so that module can know that it has to copy the particle system all
      // over again.
      // bear in mind however that particlsystems are not commonly copied (unless you want to base something on it)
      // as a particle lives on to the next frame. So modifiers should just work with the values and increase or
      // decrease them. for instance, wind is applied adding or subtracting a value.. or possibly modifying the
      // speed component.

      // set the resulting value
      result_particlesystem->set_p(*particles);
      return;
      // now all left is to render this, that will be done one of the modules of the rendering branch
    }
    result_particlesystem->valid = false;
  }
};

//_____________________________________________________________________________________________________________

class vsx_module_particle_size_noise : public vsx_module {
  int i;
	vsx_particlesystem* particles;
	vsx_module_param_particlesystem* in_particlesystem;	
	vsx_module_param_float* strength;
	vsx_module_param_int* size_type;
	// out
  vsx_module_param_particlesystem* result_particlesystem;
  vsx_rand rand;
  vsx_array<float> f_randpool;
  float* f_randpool_pointer;
public:
  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "particlesystems;modifiers;size_noise";
    info->description = "Makes particles randomly differ in size.";
    info->out_param_spec = "particlesystem:particlesystem";
    info->in_param_spec = "in_particlesystem:particlesystem,strength:float?min=0,size_type:enum?multiply|add";
    info->component_class = "particlesystem";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");
  //  in_particlesystem->set(particles);
  
    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");
  //  result_particlesystem->set(particles);
  
    // oh, all these parameters.. ..
  	strength = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"strength");
  	size_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size_type");
  	// set the position of the particle spray emitter to the center
  	strength->set(1);
    // default is 100 particles, should be enough for most effects (tm)
  }
  
  void run() {
    particles = in_particlesystem->get_addr();
    if (particles) {
      float sx = strength->get(0);

      unsigned long nump = particles->particles->size();
      if (nump != f_randpool.size()<<1)
      {
        for (unsigned long i = f_randpool.size()<<1; i < nump<<1; i++)
        {
          f_randpool[i] = rand.frand();
        }
      }
      f_randpool_pointer = f_randpool.get_pointer() + rand.rand()%nump;
      if (size_type->get()) {
        vsx_particle* pp = particles->particles->get_pointer();

        for (unsigned long i = 0; i <  nump; ++i) {
          (*pp).size = (*pp).orig_size + ( (*(f_randpool_pointer++)) *sx);
        }
      } else {
        vsx_particle* pp = particles->particles->get_pointer();
        for (unsigned long i = 0; i <  nump; ++i) {
          (*pp).size = (*pp).orig_size * ( (*(f_randpool_pointer++)) *sx);
          pp++;
        }
      }
      result_particlesystem->set_p(*particles);
      return;
    }
    result_particlesystem->valid = false;
  }
};


class vsx_module_particle_floor : public vsx_module {
  int i;
	vsx_particlesystem* particles;
	vsx_module_param_particlesystem* in_particlesystem;	
	vsx_module_param_int* x_floor;
	vsx_module_param_int* y_floor;
	vsx_module_param_int* z_floor;
	vsx_module_param_int* x_bounce;
	vsx_module_param_int* y_bounce;
	vsx_module_param_int* z_bounce;
	vsx_module_param_float* x_loss;
	vsx_module_param_float* y_loss;
	vsx_module_param_float* z_loss;
	vsx_module_param_int* refraction;
	vsx_module_param_float3* refraction_amount;
	vsx_module_param_float3* floor;
	// out
	vsx_module_param_particlesystem* result_particlesystem;	

	vsx_array<float> f_randpool;
  float* f_randpool_pointer;

public:
  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "particlesystems;modifiers;floor";
    info->description = "Stops particles when they reach either of the walls";
    info->out_param_spec = "particlesystem:particlesystem";
    info->in_param_spec = "in_particlesystem:particlesystem,\
    axis:complex{\
      x:complex{\
        x_floor:enum?no|yes,\
        x_bounce:enum?no|yes,\
        x_loss:float\
      },\
      y:complex{\
        y_floor:enum?no|yes,\
        y_bounce:enum?no|yes,\
        y_loss:float\
      },\
      z:complex{\
        z_floor:enum?no|yes,\
        z_bounce:enum?no|yes,\
        z_loss:float\
      },\
      refraction:enum?no|yes,\
      refraction_amount:float3\
    }\
    ,\
    floor:float3\
    ";
    info->component_class = "particlesystem";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");
    
    x_floor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"x_floor");
    x_floor->set(0);
    y_floor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"y_floor");
    y_floor->set(0);
    z_floor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"z_floor");
    z_floor->set(0);

    x_bounce = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"x_bounce");
    x_bounce->set(0);
    y_bounce = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"y_bounce");
    y_bounce->set(0);
    z_bounce = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"z_bounce");
    z_bounce->set(0);

    x_loss = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_loss");
    x_loss->set(5.0f);
    y_loss = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_loss");
    y_loss->set(5.0f);
    z_loss = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"z_loss");
    z_loss->set(5.0f);

    refraction = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"refraction");
    refraction->set(0);
    refraction_amount = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"refraction_amount");
    refraction_amount->set(0.0f,0);
    refraction_amount->set(0.0f,1);
    refraction_amount->set(0.0f,2);
    
    floor = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"floor");
    floor->set(0.0f,0);
    floor->set(0.0f,1);
    floor->set(0.0f,2);
  
    result_particlesystem = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem");
  }
  
  void run() {
    //printf("size-noise runnah\n");
    particles = in_particlesystem->get_addr();  
    if (particles) {
      
      //printf("size-noise runnah2\n");
      float fx = floor->get(0);
      float fy = floor->get(1);
      float fz = floor->get(2);
      bool xf = x_floor->get();
      bool yf = y_floor->get();
      bool zf = z_floor->get();
      bool xb = x_bounce->get();
      bool yb = y_bounce->get();
      bool zb = z_bounce->get();
      float xl = 1.0f-x_loss->get()*0.01f;
      float yl = 1.0f-y_loss->get()*0.01f;
      float zl = 1.0f-z_loss->get()*0.01f;
      
      unsigned long nump = particles->particles->size();
      if (nump != f_randpool.size() * 10)
      {
        for (unsigned long i = f_randpool.size() * 10; i < nump * 10; i++)
        {
          f_randpool[i] = ((float)(rand()%1000000)*0.000001f);
        }
      }
      f_randpool_pointer = f_randpool.get_pointer() + rand()%nump;

      vsx_particle* pp = particles->particles->get_pointer();
      for (unsigned long i = 0; i < nump; ++i) {
        if (xf) {
          if ((*pp).pos.x < fx) {
            (*pp).pos.x = fx;
            if (xb) {
              (*pp).speed.x = -(*particles->particles)[i].speed.x*xl*(*(f_randpool_pointer++));
              if (refraction->get()) {
                (*pp).speed.y += refraction_amount->get(1)*((  (*(f_randpool_pointer++)) - 0.5f));
                (*pp).speed.z += refraction_amount->get(2)*((  (*(f_randpool_pointer++)) - 0.5f));
              }
            } else {
              (*pp).speed.x = 0.0f;
            }
          }
        }
        if (yf) {
          if ((*pp).pos.y < fy)
          {
            (*pp).pos.y = fy;
            if (yb) 
            {
              if ( fabs((*pp).speed.y) > 0.00001f )
              {
                (*pp).speed.y = -( (*pp).speed.y*yl)*(*(f_randpool_pointer++));
                if (refraction->get()) 
                {
                  (*pp).speed.x += refraction_amount->get(0)*(( (*(f_randpool_pointer++))-0.5f));
                  (*pp).speed.x *= (*pp).speed.y*0.1f;
                  (*pp).speed.z += refraction_amount->get(2)*(( (*(f_randpool_pointer++))-0.5f));
                  (*pp).speed.z *= (*pp).speed.y*0.1f;
                }
              }
            } else {
              (*pp).speed.y = 0.0f;
            }
          }
        }
        if (zf) {
          if ( (*pp).pos.z < fz) {
            (*pp).pos.z = fz;
            if (zb) {
              (*pp).speed.z = -(*pp).speed.z*zl*(*(f_randpool_pointer++));
              if (refraction->get()) {
                (*pp).speed.x += refraction_amount->get(0)*(( (*(f_randpool_pointer++))-0.5f));
                (*pp).speed.y += refraction_amount->get(1)*(( (*(f_randpool_pointer++))-0.5f));
              }
            } else {
              (*pp).speed.z = 0.0f;
            }
          }
        }
        ++pp;
      }
      result_particlesystem->set_p(*particles);
      return;
    }
    result_particlesystem->valid = false;
  }
};

//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


unsigned long get_num_modules() {
  return 5;
}  

vsx_module* create_new_module(unsigned long module) {
  switch (module) {
    case 0: return (vsx_module*)(new vsx_module_plugin_wind);
    case 1: return (vsx_module*)(new vsx_module_particle_size_noise);
    case 2: return (vsx_module*)(new vsx_module_plugin_gravity);
    case 3: return (vsx_module*)(new vsx_module_particle_floor);
    case 4: return (vsx_module*)(new vsx_module_plugin_fluid);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_plugin_wind*)m; break;
    case 1: delete (vsx_module_particle_size_noise*)m; break;
    case 2: delete (vsx_module_plugin_gravity*)m; break;
    case 3: delete (vsx_module_particle_floor*)m; break;
    case 4: delete (vsx_module_plugin_fluid*)m; break;
  }
}
