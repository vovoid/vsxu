#include "_configuration.h"
#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include <pthread.h>
#ifndef _WIN32
#include <unistd.h>
#define Sleep sleep
#endif
#include "main.h"
#include <math.h>


// TODO: a module that uses 2 sequences to make a blob with different interpolators (not just sin)
//       and also an intensity multiplier per degree

// TODO: implement andrew mccanns RSA xor algorithm
// for every y
// for every x
// temp = abs(x+y) xor abs(x-y)
// pixel[x,y] = (temp^7) mod 257;

#include "module_bitmap_blob.h"
#include "noise.h"
#include "plasma.h"
#include "subplasma.h"
#include "blend.h"

#ifdef BUILDING_DLL
vsx_module* create_new_module(unsigned long module) {
  if (module > 4)
  {
    module_bitmap_blend* b = new module_bitmap_blend;
    b->blend_type = module-5;
    return (vsx_module*)b;
  }

  switch(module) {
    case 0: { module_bitmap_blob* b = new module_bitmap_blob; b->c_type = 0; return (vsx_module*)b; }
    case 1: { module_bitmap_blob* b = new module_bitmap_blob; b->c_type = 1; return (vsx_module*)b; }
    case 2: return (vsx_module*)(new module_bitmap_add_noise);
    case 3: return (vsx_module*)(new module_bitmap_plasma);
    case 4: return (vsx_module*)(new module_bitmap_subplasma);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  if (module > 4)
  {
    delete (module_bitmap_blend*)m;
    return;
  }

  switch(module) {
    case 1: case 0: delete (module_bitmap_blob*)m; break;
    case 2: delete (module_bitmap_add_noise*)m; break;
    case 3: delete (module_bitmap_plasma*)m; break;
    case 4: delete (module_bitmap_subplasma*)m; break;
  }
}

unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 5+24;
}
#endif
