#ifndef VSX_GL_GLOBAL_H
#define VSX_GL_GLOBAL_H

  #ifndef VSXU_NO_GL_GLOBAL
		#if defined(VSXU_OPENGL_ES)
			#import <OpenGLES/ES1/gl.h>
			#import <OpenGLES/ES1/glext.h>
		#else

			#if (BUILDING_DLL)
				#define VSX_NOGLUT
			#endif

			#ifdef VSXU_MAC_XCODE
				#include <glew.h>
			#else
				#include <GL/glew.h>
			#endif

			#ifdef _WIN32
				#include <GL/wglew.h>
			#else
				#if defined(VSXU_MAC_XCODE)
					//#include <glxew.h>
				#else
					#include <GL/glxew.h>
				#endif
			#endif

			#ifdef _WIN32 // bubbletrace
				#include <GL/glaux.h>
			#endif

			#if defined(VSXU_MAC_XCODE)
				#include <glext.h>
			#else
				#include <GL/glext.h>
			#endif
		#endif
	#endif
#endif
