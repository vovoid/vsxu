/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
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


#include "_configuration.h"
#include <vsx_gl_global.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
#ifndef _WIN32
#include <unistd.h>
#define Sleep sleep
#endif
#include <math.h>


// TODO: a module that uses 2 sequences to make a blob with different interpolators (not just sin)
//       and also an intensity multiplier per degree

// TODO: implement andrew mccanns RSA xor algorithm
// for every y
// for every x
// temp = abs(x+y) xor abs(x-y)
// pixel[x,y] = (temp^7) mod 257;

#include "module_bitmap_blob.h"
#include "module_bitmap_concentric_circles.h"
#include "module_bitmap_perlin_noise.h"
#include "module_bitmap_plasma.h"
#include "module_bitmap_subplasma.h"


//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules(vsx_module_engine_environment* environment);
}

#ifndef MOD_CM
#define MOD_CM vsx_module_bitmap_generators_cm
#define MOD_DM vsx_module_bitmap_generators_dm
#define MOD_NM vsx_module_bitmap_generators_nm
#endif


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);

  switch(module)
  {
    case 0: return (vsx_module*)(new module_bitmap_generators_blob);
    case 1: return (vsx_module*)(new module_bitmap_generators_concentric_circles);
    case 2: return (vsx_module*)(new module_bitmap_plasma);
    case 3: return (vsx_module*)(new module_bitmap_subplasma);
    case 4: return (vsx_module*)(new module_bitmap_perlin_noise);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module)
{

  switch(module) {
    case 0: delete (module_bitmap_generators_blob*)m; break;
    case 1: delete (module_bitmap_generators_concentric_circles*)m; break;
    case 2: delete (module_bitmap_plasma*)m; break;
    case 3: delete (module_bitmap_subplasma*)m; break;
    case 4: delete (module_bitmap_perlin_noise*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment)
{
  VSX_UNUSED(environment);
  return 5;
}
