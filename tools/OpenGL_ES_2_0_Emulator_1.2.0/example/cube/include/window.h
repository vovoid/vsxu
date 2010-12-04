/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#ifndef WINDOW_H
#define WINDOW_H

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

HWND create_window(int uiWidth, int uiHeight);
LRESULT CALLBACK process_window(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
#else

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "EGL/egl.h"

Window create_window(const char *title, int width, int height, Display* display, 
	EGLDisplay sEGLDisplay, EGLConfig FBConfig, Colormap *pColormap, 
	XVisualInfo **ppVisual);
#endif

#endif
