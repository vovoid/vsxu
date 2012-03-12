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

#ifndef VSX_GL_GLOBAL_H
#define VSX_GL_GLOBAL_H
  #ifndef VSXU_NO_GL_GLOBAL
    #if defined(VSXU_OPENGL_ES)
      #if (PLATFORM == PLATFORM_LINUX)
        #include "GLES2/gl2.h"
        #include "EGL/egl.h"
        #include "vsx_gl_es_shader_wrapper.h"
      #else
        #include <OpenGLES/ES1/gl.h>
        #include <OpenGLES/ES1/glext.h>
      #endif
    #else // VSXU_OPENGL_ES
      #include <GL/glew.h>
      #ifdef _WIN32
        #include <GL/wglew.h>
      #else
        #include <GL/glxew.h>
      #endif
    #endif
  #endif
#endif
