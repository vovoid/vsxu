/*
 * $Copyright:
 * ----------------------------------------------------------------
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 *   (C) COPYRIGHT 2009 ARM Limited
 *       ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 * $
 */

/* -*- mode: c; tab-width: 8; -*- */
/* vi: set sw=4 ts=8: */
/* Platform-specific types and definitions for egl.h */

#ifndef __eglplatform_h_
#define __eglplatform_h_

#include <sys/types.h>

/* Macros used in EGL function prototype declarations.
 *
 * EGLAPI return-type EGLAPIENTRY eglFunction(arguments);
 * typedef return-type (EXPAPIENTRYP PFNEGLFUNCTIONPROC) (arguments);
 *
 * On Windows, EGLAPIENTRY can be defined like APIENTRY.
 * On most other platforms, it should be empty.
 */

#ifndef EGLAPI
#  if (defined(_WIN32) || defined(__VC32__)) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__) /* Win32 and WinCE */
#     define EGLAPI __declspec(dllimport)
#  elif defined (__SYMBIAN32__)                        /* Symbian */
#     define EGLAPI IMPORT_C
#  else
#     define EGLAPI
#  endif
#endif

#if (defined(_WIN32) || defined(__VC32__)) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__) && !defined(_WIN32_WCE) /* Win32 */
#define EGLAPIENTRY __stdcall
#else
#define EGLAPIENTRY
#endif

#define EGLAPIENTRYP EGLAPIENTRY *

/* The types NativeDisplayType, NativeWindowType, and NativePixmapType
 * are aliases of window-system-dependent types, such as X Display * or
 * Windows Device Context. They must be defined in platform-specific
 * code below. The EGL-prefixed versions of Native*Type are the same
 * types, renamed in EGL 1.3 so all types in the API start with "EGL".
 */

#if defined(_WIN32) || defined(__VC32__) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)  /* Win32 and WinCE */
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

typedef HDC NativeDisplayType;
typedef HBITMAP NativePixmapType;
typedef HWND NativeWindowType;

#elif defined(__WINSCW__) || defined(__SYMBIAN32__)  /* Symbian */

typedef int NativeDisplayType;
typedef void *NativeWindowType;
typedef void *NativePixmapType;

#elif defined(__unix__)

/* X11 (tentative)  */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef Display *NativeDisplayType;
typedef Pixmap NativePixmapType;
typedef Window NativeWindowType;

#else
#error "Platform not recognized"
#endif

/* EGL 1.2 types, renamed for consistency in EGL 1.3 */
typedef NativeDisplayType EGLNativeDisplayType;
typedef NativePixmapType EGLNativePixmapType;
typedef NativeWindowType EGLNativeWindowType;

/* Obtain int32_t for egl.h */
#if (defined(__STDC__) && __STDC__ && __STDC_VERSION__ >= 199901L) || defined(__GNUC__)
#include <stdint.h>
#elif (defined(_WIN32) || defined(__VC32__)) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)  /* Win32 and WinCE */
#include <windows.h>
typedef INT32 int32_t;
#else
typedef int int32_t;
#endif

typedef int32_t EGLint;

#endif /* __eglplatform_h */
