/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @see The Lesser GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the Lesser GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the Lesser GNU General Public License
* for more details.
*
* You should have received a copy of the Lesser GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef     __FTGL__
#define     __FTGL__


typedef double   FTGL_DOUBLE;
typedef float    FTGL_FLOAT;

// Fixes for deprecated identifiers in 2.1.5
#ifndef FT_OPEN_MEMORY
    #define FT_OPEN_MEMORY (FT_Open_Flags)1
#endif

#ifndef FT_RENDER_MODE_MONO
    #define FT_RENDER_MODE_MONO ft_render_mode_mono
#endif

#ifndef FT_RENDER_MODE_NORMAL
    #define FT_RENDER_MODE_NORMAL ft_render_mode_normal
#endif

  
#ifdef WIN32

    // Under windows avoid including <windows.h> is overrated. 
    // Sure, it can be avoided and "name space pollution" can be
    // avoided, but why? It really doesn't make that much difference
    // these days.
    #define  WIN32_LEAN_AND_MEAN
    #include <windows.h>

    #ifndef __gl_h_
        #include <GL/gl.h>
        #include <GL/glu.h>
    #endif

#else

    // Non windows platforms - don't require nonsense as seen above :-)    
    #ifndef __gl_h_
        #ifdef __APPLE_CC__
            #include <OpenGL/gl.h>
            #include <OpenGL/glu.h>
        #else
            #include <GL/gl.h>
            #include <GL/glu.h>
        #endif                

    #endif

    // Required for compatibility with glext.h style function definitions of 
    // OpenGL extensions, such as in src/osg/Point.cpp.
    #ifndef APIENTRY
        #define APIENTRY
    #endif
#endif

// Compiler-specific conditional compilation
#ifdef _MSC_VER // MS Visual C++ 

    // Disable various warning.
    // 4786: template name too long
    #pragma warning( disable : 4251 )
    #pragma warning( disable : 4275 )
    #pragma warning( disable : 4786 )

    // The following definitions control how symbols are exported.
    // If the target is a static library ensure that FTGL_LIBRARY_STATIC
    // is defined. If building a dynamic library (ie DLL) ensure the
    // FTGL_LIBRARY macro is defined, as it will mark symbols for 
    // export. If compiling a project to _use_ the _dynamic_ library 
    // version of the library, no definition is required. 
    #ifdef FTGL_LIBRARY_STATIC      // static lib - no special export required
    #  define FTGL_EXPORT
    #elif FTGL_LIBRARY              // dynamic lib - must export/import symbols appropriately.
    #  define FTGL_EXPORT   __declspec(dllexport)
    #else
    #  define FTGL_EXPORT   __declspec(dllimport)
    #endif 

#else
    // Compiler that is not MS Visual C++.
    // Ensure that the export symbol is defined (and blank)
    //#define FTGL_EXPORT

    #ifdef _WIN32
    #if BUILDING_DLL
    # define FTGL_EXPORT __declspec (dllexport)
    #else /* Not BUILDING_DLL */
    # define FTGL_EXPORT __declspec (dllimport)
    #endif /* Not BUILDING_DLL */
    #else
    #define FTGL_EXPORT
    #endif


#endif  


// lifted from glext.h, to remove dependancy on glext.h
#ifndef GL_EXT_texture_object
    #define GL_TEXTURE_PRIORITY_EXT           0x8066
    #define GL_TEXTURE_RESIDENT_EXT           0x8067
    #define GL_TEXTURE_1D_BINDING_EXT         0x8068
    #define GL_TEXTURE_2D_BINDING_EXT         0x8069
    #define GL_TEXTURE_3D_BINDING_EXT         0x806A
#endif

#endif  //  __FTGL__
