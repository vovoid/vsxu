/*
 * $Copyright:
 * ----------------------------------------------------------------
 * This confidential and proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 *   (C) COPYRIGHT 2008 ARM Limited
 *       ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 * $
 *
 * egl_native_types.h - EGL Native types definitions for use with
 *                      OpenGL ES 2.O Emulation library
 */
#ifndef _EGL_NATIVE_TYPES_H_
#define _EGL_NATIVE_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef INT32 int32_t;

typedef HDC NativeDisplayType;
typedef HBITMAP NativePixmapType;
typedef HWND NativeWindowType;

#ifdef __cplusplus
}
#endif

#endif /* _EGL_NATIVE_TYPES_H_ */
