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

/*
 * _configuration.h
 */
#ifdef BUILDING_DLL
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

// size optimization togglers
//#define VSX_NO_TEXTURE
//#define VSX_NO_SEQUENCE
//#define VSX_NO_MESH
//#define VSX_STRINGLIB_NOSTL
//#define VSX_NO_GL

// parameter defines
#define VSX_P_INT
#define VSX_P_FLOAT
#define VSX_P_FLOAT3
#define VSX_P_FLOAT4
//#define VSX_P_DOUBLE
#define VSX_P_MATRIX
//#define VSX_P_FLOAT_ARRAY
//#define VSX_P_FLOAT3_ARRAY
//#define VSX_P_STRING
#define VSX_P_SEQUENCE
//#define VSX_P_ABSTRACT
//#define VSX_P_QUATERNION
//#define VSX_P_QUATERNION_ARRAY
#define VSX_P_TEXTURE
//#define VSX_P_BITMAP
//#define VSX_P_PARTICLESYSTEM
#define VSX_P_SEGMENT_MESH
#define VSX_P_MESH
#define VSX_P_RENDER
//#define VSX_P_RESOURCE

#endif /* _CONFIGURATION_H_ */
#endif
