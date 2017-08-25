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
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <math/vsx_sequence.h>
#include <math/vsx_bspline.h>


#include "module_mesh_bspline_vertices.h"
#include "module_mesh_lightning_vertices.h"
#include "module_mesh_rays.h"
#include "module_mesh_rays_uniform.h"
#include "module_mesh_disc.h"
#include "module_mesh_supershape.h"
#include "module_mesh_planes.h"
#include "module_mesh_box.h"
#include "module_mesh_grid.h"
#include "module_mesh_sphere.h"
#include "module_mesh_abstract_hand.h"
#include "module_mesh_torus_knot.h"
#include "module_mesh_needle.h"
#include "module_mesh_ribbon.h"
#include "module_mesh_ribbon_cloth.h"
#include "module_mesh_plane_uv_distort.h"
#include "module_mesh_sphere_octahedron.h"
#include "module_mesh_sphere_icosahedron.h"
#include "module_mesh_kaleidoscope.h"
#include "module_mesh_star.h"
#include "module_mesh_metaballs.h"
#include "module_mesh_ocean_threaded.h"
#include "module_mesh_ocean_tunnel_threaded.h"
#include "module_mesh_cloud_plane.h"
#include "module_mesh_planeworld.h"
#include "module_mesh_thorn.h"
//#include "module_mesh_plasma_tree.h"
#include "module_segmesh_loft.h"
#include "module_segmesh_map_bspline.h"
#include "module_segmesh_shape_basic.h"

// vertices generators
#include "module_mesh_vertices_rand_points.h"
#include "module_mesh_vertices_ribbon.h"



#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules(vsx_module_engine_environment* environment);
}

#ifndef MOD_CM
#define MOD_CM vsx_module_mesh_generators_cm
#define MOD_DM vsx_module_mesh_generators_dm
#define MOD_NM vsx_module_mesh_generators_nm
#endif


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch(module)
  {
    case 0: return (vsx_module*)(new module_mesh_needle);
    case 1: return (vsx_module*)(new module_mesh_vertices_rand_points);
    case 2: return (vsx_module*)(new module_mesh_rays);
    case 3: return (vsx_module*)(new module_mesh_disc);
    case 4: return (vsx_module*)(new module_mesh_planes);
    case 5: return (vsx_module*)(new module_mesh_box);
    case 6: return (vsx_module*)(new module_mesh_sphere);
    case 7: return (vsx_module*)(new module_mesh_supershape);
    case 8: return (vsx_module*)(new module_mesh_ribbon);
    case 9: return (vsx_module*)(new module_mesh_abstract_hand);
    case 10: return (vsx_module*)(new module_mesh_torus_knot);
    case 11: return (vsx_module*)(new module_mesh_lightning_vertices);
    case 12: return (vsx_module*)(new module_mesh_ribbon_cloth);
    case 13: return (vsx_module*)(new module_mesh_bspline_vertices);
    case 14: return (vsx_module*)(new module_mesh_grid);
    case 15: return (vsx_module*)(new module_mesh_plane_uv_distort);
    case 16: return (vsx_module*)(new module_mesh_sphere_octahedron);
    case 17: return (vsx_module*)(new module_mesh_sphere_icosahedron);
    case 18: return (vsx_module*)(new module_mesh_kaleidoscope);
    case 19: return (vsx_module*)(new module_mesh_star);
    case 20: return (vsx_module*)(new module_mesh_metaballs);
    case 21: return (vsx_module*)(new module_mesh_ocean_threaded);
    case 22: return (vsx_module*)(new module_mesh_ocean_tunnel_threaded);
    case 23: return (vsx_module*)(new module_mesh_cloud_plane);
    case 24: return (vsx_module*)(new module_mesh_planeworld);
    case 25: return (vsx_module*)(new module_mesh_thorn);
    case 26: return (vsx_module*)(new module_segmesh_loft);
    case 27: return (vsx_module*)(new module_segmesh_map_bspline);
    case 28: return (vsx_module*)(new module_segmesh_shape_basic);
    case 29: return (vsx_module*)(new module_mesh_vertices_ribbon);
    case 30: return (vsx_module*)(new module_mesh_rays_uniform);
    //case 31: return (vsx_module*)(new module_mesh_plasma_tree);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module)
{
  switch(module) {
    case 0: delete (module_mesh_needle*)m; break;
    case 1: delete (module_mesh_vertices_rand_points*)m; break;
    case 2: delete (module_mesh_rays*)m; break;
    case 3: delete (module_mesh_disc*)m; break;
    case 4: delete (module_mesh_planes*)m; break;
    case 5: delete (module_mesh_box*)m; break;
    case 6: delete (module_mesh_sphere*)m; break;
    case 7: delete (module_mesh_supershape*)m; break;
    case 8: delete (module_mesh_ribbon*)m; break;
    case 9: delete (module_mesh_abstract_hand*)m; break;
    case 10: delete (module_mesh_torus_knot*)m; break;
    case 11: delete (module_mesh_lightning_vertices*)m; break;
    case 12: delete (module_mesh_ribbon_cloth*)m; break;
    case 13: delete (module_mesh_bspline_vertices*)m; break;
    case 14: delete (module_mesh_grid*)m; break;
    case 15: delete (module_mesh_plane_uv_distort*)m; break;
    case 16: delete (module_mesh_sphere_octahedron*)m; break;
    case 17: delete (module_mesh_sphere_icosahedron*)m; break;
    case 18: delete (module_mesh_kaleidoscope*)m; break;
    case 19: delete (module_mesh_star*)m; break;
    case 20: delete (module_mesh_metaballs*)m; break;
    case 21: delete (module_mesh_ocean_threaded*)m; break;
    case 22: delete (module_mesh_ocean_tunnel_threaded*)m; break;
    case 23: delete (module_mesh_cloud_plane*)m; break;
    case 24: delete (module_mesh_planeworld*)m; break;
    case 25: delete (module_mesh_thorn*)m; break;
    case 26: delete (module_segmesh_loft*)m; break;
    case 27: delete (module_segmesh_map_bspline*)m; break;
    case 28: delete (module_segmesh_shape_basic*)m; break;
    case 29: delete (module_mesh_vertices_ribbon*)m; break;
    case 30: delete (module_mesh_rays_uniform*)m; break;
    //case 31: delete (module_mesh_plasma_tree*)m; break;
  }
} 

unsigned long MOD_NM(vsx_module_engine_environment* environment)
{
  VSX_UNUSED(environment);
  return 31;
}
