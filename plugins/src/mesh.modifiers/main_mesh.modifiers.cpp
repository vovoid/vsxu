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
#include <vsx_param.h>
#include <module/vsx_module.h>
#include <math/vsx_float_array.h>
#include <math/quaternion/vsx_quaternion.h>

// TODO: optimize the mesh_quat_rotate to also use volatile arrays for speed
// TODO: optimize the inflation mesh modifier to use volatile arrays for passthru arrays
// TODO: add a real spheremapping module
// TODO: quaternion rotation from 2 vertex id's: vector from point to point, normal, crossproduct = matrix -> quaternion


#include "module_mesh_dummy.h"
#include "module_mesh_vertex_picker.h"
#include "module_mesh_to_float3_arrays.h"
#include "module_mesh_calc_attachment.h"
#include "module_mesh_quat_rotate.h"
#include "module_mesh_quat_rotate_around_vertex.h"
#include "module_mesh_translate.h"
#include "module_mesh_scale.h"
#include "module_mesh_mirror.h"
#include "module_mesh_scale_normalize.h"
#include "module_mesh_translate_edge_wraparound.h"
#include "module_mesh_noise.h"
#include "module_mesh_rain_down.h"
#include "module_mesh_deformers_random_normal_distort.h"
#include "module_mesh_compute_tangents.h"
#include "module_mesh_compute_tangents_vertex_color_array.h"
#include "module_mesh_deformers_mesh_vertex_move.h"
#include "module_mesh_inflate_thread.h"
#include "module_mesh_vertex_distance_sort.h"
#include "module_mesh_vortex.h"
#include "module_segmesh_to_mesh.h"
#include "module_mesh_interpolate_2p.h"

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
#define MOD_CM vsx_module_mesh_modifiers_cm
#define MOD_DM vsx_module_mesh_modifiers_dm
#define MOD_NM vsx_module_mesh_modifiers_nm
#endif


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch(module)
  {
    case 0: return (vsx_module*)(new module_mesh_vertex_picker);
    case 1: return (vsx_module*)(new module_mesh_quat_rotate);
    case 2: return (vsx_module*)(new module_mesh_deformers_mesh_vertex_move);
    case 3: return (vsx_module*)(new module_mesh_translate);
    case 4: return (vsx_module*)(new module_mesh_to_float3_arrays);
    case 5: return (vsx_module*)(new module_mesh_compute_tangents);
    case 6: return (vsx_module*)(new module_mesh_inflate_thread);
    case 7: return (vsx_module*)(new module_mesh_deformers_random_normal_distort);
    case 8: return (vsx_module*)(new module_mesh_noise);
    case 9: return (vsx_module*)(new module_mesh_quat_rotate_around_vertex);
    case 10: return (vsx_module*)(new module_mesh_dummy);
    case 11: return (vsx_module*)(new module_mesh_scale);
    case 12: return (vsx_module*)(new module_mesh_rain_down);
    case 13: return (vsx_module*)(new module_mesh_calc_attachment);
    case 14: return (vsx_module*)(new module_mesh_vertex_distance_sort);
    case 15: return (vsx_module*)(new module_mesh_translate_edge_wraparound);
    case 16: return (vsx_module*)(new module_mesh_vortex);
    case 17: return (vsx_module*)(new module_mesh_scale_normalize);
    case 18: return (vsx_module*)(new module_mesh_segmesh_to_mesh);
    case 19: return (vsx_module*)(new module_mesh_compute_tangents_vertex_color_array);
    case 20: return (vsx_module*)(new module_mesh_mirror);
    case 21: return (vsx_module*)(new module_mesh_interpolate_2p);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module)
  {
    case 0: delete (module_mesh_vertex_picker*)m; break;
    case 1: delete (module_mesh_quat_rotate*)m; break;
    case 2: delete (module_mesh_deformers_mesh_vertex_move*)m; break;
    case 3: delete (module_mesh_translate*)m; break;
    case 4: delete (module_mesh_to_float3_arrays*)m; break;
    case 5: delete (module_mesh_compute_tangents*)m; break;
    case 6: delete (module_mesh_inflate_thread*)m; break;
    case 7: delete (module_mesh_deformers_random_normal_distort*)m; break;
    case 8: delete (module_mesh_noise*)m; break;
    case 9: delete (module_mesh_quat_rotate_around_vertex*)m; break;
    case 10: delete (module_mesh_dummy*)m; break;
    case 11: delete (module_mesh_scale*)m; break;
    case 12: delete (module_mesh_rain_down*)m; break;
    case 13: delete (module_mesh_calc_attachment*)m; break;
    case 14: delete (module_mesh_vertex_distance_sort*)m; break;
    case 15: delete (module_mesh_translate_edge_wraparound*)m; break;
    case 16: delete (module_mesh_vortex*)m; break;
    case 17: delete (module_mesh_scale_normalize*)m; break;
    case 18: delete (module_mesh_segmesh_to_mesh*)m; break;
    case 19: delete (module_mesh_compute_tangents_vertex_color_array*)m; break;
    case 20: delete (module_mesh_mirror*)m; break;
    case 21: delete (module_mesh_interpolate_2p*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment) {
  VSX_UNUSED(environment);
  return 22;
}
