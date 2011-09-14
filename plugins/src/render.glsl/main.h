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

#ifndef _VSXU_GLSL_
#define _VSXU_GLSL_

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */
//int explode(vsx_string& input, vsx_string& delimiter, vsx_avector<vsx_string>& results, int max_parts = 0);
//vsx_string str_replace(vsx_string search, vsx_string replace, vsx_string subject, int max_replacements = 0, int required_pos = -1);
#include <map>
#include "vsx_glsl.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#if BUILDING_DLL
extern "C" {
_declspec(dllexport) vsx_module* create_new_module(unsigned long module);
_declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
_declspec(dllexport) unsigned long get_num_modules();
_declspec(dllexport) void set_environment_info(vsx_engine_environment* environment);
}
#endif

#endif /* _DLL_H_ */
