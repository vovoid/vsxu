/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
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

#ifndef VSX_MODULE_DLL_INFO_H_
#define VSX_MODULE_DLL_INFO_H_
#include <vsx_platform.h>


//-----internal:
//#ifdef VSXU_EXE
class module_dll_info {
public:
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
	// dll handle
  HMODULE module_handle;
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
	// for linux / apple: pointer to dlopen handle
	// for mobile devices: pointer to a vsx_string by which to create the module
	void* module_handle;
#endif
  int module_id;
  bool hidden_from_gui;
};  
//-
//#endif

#endif /*VSX_MODULE_DLL_INFO_H_*/
