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


/*
 * PNG loader library for OpenGL v1.45 (10/07/00)
 * by Ben Wyatt ben@wyatt100.freeserve.co.uk
 * Using LibPNG 1.0.2 and ZLib 1.1.3
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the author be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is hereby granted to use, copy, modify, and distribute this
 * source code, or portions hereof, for any purpose, without fee, subject to
 * the following restrictions:
 *
 * 1. The origin of this source code must not be misrepresented. You must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered versions must be plainly marked as such and must not be
 *    misrepresented as being the original source.
 * 3. This notice must not be removed or altered from any source distribution.
 */

#ifndef _GLPNG_H_
#define _GLPNG_H_

#include <stdio.h>
#include <container/vsx_nw_vector.h>
#include "vsxfst.h"

#include "engine_graphics_dllimport.h"

/* Mipmapping parameters */
#define PNG_NOMIPMAPS      0 /* No mipmapping                        */
#define PNG_BUILDMIPMAPS  -1 /* Calls a clone of gluBuild2DMipmaps() */
#define PNG_SIMPLEMIPMAPS -2 /* Generates mipmaps without filtering  */

/* Who needs an "S" anyway? */
#define PNG_NOMIPMAP     PNG_NOMIPMAPS
#define PNG_BUILDMIPMAP  PNG_BUILDMIPMAPS
#define PNG_SIMPLEMIPMAP PNG_SIMPLEMIPMAPS

typedef struct {
	unsigned int Width;
	unsigned int Height;
	unsigned int Depth;
	unsigned int Alpha;
} pngInfo;

typedef struct {
  unsigned int Width;
  unsigned int Height;
  unsigned int Depth;
  unsigned int Alpha;

  unsigned int Components;
  unsigned char *Data;
  unsigned char *Palette;
} pngRawInfo;

extern int VSX_ENGINE_GRAPHICS_DLLIMPORT pngLoadRaw(const char* filename, pngRawInfo *rawinfo, vsxf* filesystem);

class VSX_ENGINE_GRAPHICS_DLLIMPORT CJPEGTest
{
public:
    CJPEGTest();

    // Load JPEG (colour or grayscale)
    bool LoadJPEG
    ( 
        const vsx_string<>& strFile, // Load this file
        vsx_string<>& strErr, // Returns error text on failure
        vsxf* filesystem
    );
    
    bool SaveJPEG( const vsx_string<>& strFile, vsx_string<>& strErr, const int nQFactor );
    
    // Get width
    int GetResX( void ) const;
    
    // Get height
    int GetResY( void ) const;
    
    unsigned char * m_pBuf; // pixel data
    int m_nResX; // width
    int m_nResY; // height    
};


#endif
