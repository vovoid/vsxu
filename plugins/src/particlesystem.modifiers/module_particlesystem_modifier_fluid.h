class module_particlesystem_modifier_fluid : public vsx_module
{
  int i;
  float time;
  vsx_particlesystem<>* particles;
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

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "particlesystems;modifiers;particle_fluid_deformer";

    info->description =
      "";

    info->out_param_spec =
      "particlesystem:particlesystem";

    info->in_param_spec =
      "in_particlesystem:particlesystem,"
      "actor:float3,"
      "strength:float,"
      "draw_velocity:enum?no|yes"
    ;

    info->component_class =
      "particlesystem";
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

      dt = 0.01f;//engine_state->dtime;

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

        //(*particles->particles)[i].pos.x += px*engine_state->dtime;
        //(*particles->particles)[i].pos.y = 0;//py*engine_state->dtime;
        //(*particles->particles)[i].pos.z += pz*engine_state->dtime;
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


