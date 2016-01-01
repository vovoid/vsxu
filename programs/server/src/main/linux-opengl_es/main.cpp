/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "vsx_gl_es_shader_wrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "application.h"
#include <string/vsx_string.h>

int app_argc = 0;
char** app_argv;


Window hWindow;
Display *hDisplay;

#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = glGetError(); \
          if(glError != GL_NO_ERROR) { \
            fprintf(stderr, "glGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__); \
            exit(1); \
          } \
        }

#define EGL_CHECK(x) \
    x; \
    { \
        EGLint eglError = eglGetError(); \
        if(eglError != EGL_SUCCESS) { \
            fprintf(stderr, "eglGetError() = %i (0x%.8x) at line %i\n", eglError, eglError, __LINE__); \
            exit(1); \
        } \
    }


/* Waits for map notification */
Bool wait_for_map(Display *d, XEvent *e, char *win_ptr) {
  return (e->type == MapNotify && e->xmap.window == (*((Window*)win_ptr)));
}

/* Creates an X window */
Window create_window(const char *title, int width, int height, Display* display,
  EGLDisplay sEGLDisplay, EGLConfig FBConfig, Colormap *pColormap,
  XVisualInfo **ppVisual)
{
    XSetWindowAttributes wa;
    XSizeHints sh;
    XEvent e;
    XVisualInfo *visual, templ;

    unsigned long mask;
    long screen;

    Colormap colormap;

    int vID, n;

    Window window;

    screen = DefaultScreen(display);
    eglGetConfigAttrib(sEGLDisplay, FBConfig, EGL_NATIVE_VISUAL_ID, &vID);
    templ.visualid = vID;
    visual = XGetVisualInfo(display, VisualIDMask, &templ, &n);

  if (!visual) {
    printf("Couldn't get X visual info\n");
    exit(-1);
  }

    colormap = XCreateColormap(display, RootWindow(display, screen), visual->visual, AllocNone);

    wa.colormap = colormap;
    wa.background_pixel = 0xFFFFFFFF;
    wa.border_pixel = 0;
    wa.event_mask = StructureNotifyMask | ExposureMask;

    mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

    window = XCreateWindow(display, RootWindow(display, screen), 0, 0, width, height,
    0, visual->depth, InputOutput, visual->visual, mask, &wa);

    sh.flags = USPosition;
    sh.x = 10;
    sh.y = 10;

    XSetStandardProperties(display, window, title, title, None, 0, 0, &sh);
    XMapWindow(display, window);
    XIfEvent(display, &e, wait_for_map, (char*)&window);
    XSetWMColormapWindows(display, window, &window, 1);
    XFlush(display);

    *pColormap = colormap;
    *ppVisual = visual;

    return window;
}


/* 3D data. Vertex range -0.5..0.5 in all axes.
 * Z -0.5 is near, 0.5 is far. */
const float aVertices[] =
{
    /* Front face. */
    /* Bottom left */
    -0.5,  0.5, -0.5,
    0.5, -0.5, -0.5,
    -0.5, -0.5, -0.5,
    /* Top right */
    -0.5,  0.5, -0.5,
    0.5,  0.5, -0.5,
    0.5, -0.5, -0.5,
    /* Left face */
    /* Bottom left */
    -0.5,  0.5,  0.5,
    -0.5, -0.5, -0.5,
    -0.5, -0.5,  0.5,
    /* Top right */
    -0.5,  0.5,  0.5,
    -0.5,  0.5, -0.5,
    -0.5, -0.5, -0.5,
    /* Top face */
    /* Bottom left */
    -0.5,  0.5,  0.5,
    0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5,
    /* Top right */
    -0.5,  0.5,  0.5,
    0.5,  0.5,  0.5,
    0.5,  0.5, -0.5,
    /* Right face */
    /* Bottom left */
    0.5,  0.5, -0.5,
    0.5, -0.5,  0.5,
    0.5, -0.5, -0.5,
    /* Top right */
    0.5,  0.5, -0.5,
    0.5,  0.5,  0.5,
    0.5, -0.5,  0.5,
    /* Back face */
    /* Bottom left */
    0.5,  0.5,  0.5,
    -0.5, -0.5,  0.5,
    0.5, -0.5,  0.5,
    /* Top right */
    0.5,  0.5,  0.5,
    -0.5,  0.5,  0.5,
    -0.5, -0.5,  0.5,
    /* Bottom face */
    /* Bottom left */
    -0.5, -0.5, -0.5,
    0.5, -0.5,  0.5,
    -0.5, -0.5,  0.5,
    /* Top right */
    -0.5, -0.5, -0.5,
    0.5, -0.5, -0.5,
    0.5, -0.5,  0.5,
};

const float aColours[] =
{
    /* Front face */
    /* Bottom left */
    1.0, 0.0, 0.0, /* red */
    0.0, 0.0, 1.0, /* blue */
    0.0, 1.0, 0.0, /* green */
    /* Top right */
    1.0, 0.0, 0.0, /* red */
    1.0, 1.0, 0.0, /* yellow */
    0.0, 0.0, 1.0, /* blue */
    /* Left face */
    /* Bottom left */
    1.0, 1.0, 1.0, /* white */
    0.0, 1.0, 0.0, /* green */
    0.0, 1.0, 1.0, /* cyan */
    /* Top right */
    1.0, 1.0, 1.0, /* white */
    1.0, 0.0, 0.0, /* red */
    0.0, 1.0, 0.0, /* green */
    /* Top face */
    /* Bottom left */
    1.0, 1.0, 1.0, /* white */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 0.0, 0.0, /* red */
    /* Top right */
    1.0, 1.0, 1.0, /* white */
    0.0, 0.0, 0.0, /* black */
    1.0, 1.0, 0.0, /* yellow */
    /* Right face */
    /* Bottom left */
    1.0, 1.0, 0.0, /* yellow */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 0.0, 1.0, /* blue */
    /* Top right */
    1.0, 1.0, 0.0, /* yellow */
    0.0, 0.0, 0.0, /* black */
    1.0, 0.0, 1.0, /* magenta */
    /* Back face */
    /* Bottom left */
    0.0, 0.0, 0.0, /* black */
    0.0, 1.0, 1.0, /* cyan */
    1.0, 0.0, 1.0, /* magenta */
    /* Top right */
    0.0, 0.0, 0.0, /* black */
    1.0, 1.0, 1.0, /* white */
    0.0, 1.0, 1.0, /* cyan */
    /* Bottom face */
    /* Bottom left */
    0.0, 1.0, 0.0, /* green */
    1.0, 0.0, 1.0, /* magenta */
    0.0, 1.0, 1.0, /* cyan */
    /* Top right */
    0.0, 1.0, 0.0, /* green */
    0.0, 0.0, 1.0, /* blue */
    1.0, 0.0, 1.0, /* magenta */
};

int main(int argc, char **argv) {
  app_argc = argc;
  app_argv = argv;


  EGLDisplay  sEGLDisplay;
  EGLContext  sEGLContext;
  EGLSurface  sEGLSurface;


  /* EGL Configuration */

  EGLint aEGLAttributes[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_DEPTH_SIZE, 16,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
  };

  EGLint aEGLContextAttributes[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };

  EGLConfig aEGLConfigs[1];
  EGLint    cEGLConfigs;

  XSetWindowAttributes win_attrs;
  int attrs[64], idx = 0, num_config = 0;
  int major, minor;
  Colormap colormap;
      XVisualInfo *pVisual;
      XEvent e;

  GLint iLocPosition = 0;

  GLint iLocColour, iLocTexCoord, iLocNormal, iLocMVP;
  GLint iLocXangle, iLocYangle, iLocZangle;
  GLint iLocAspect, iLocLightPos, iLocSampler, iLocSampler2;

  GLuint uiProgram, uiFragShader, uiVertShader;

  GLenum myTex, myTex2;

  int bDone = 0;

  const unsigned int uiWidth  = 640;
  const unsigned int uiHeight = 480;

  int iXangle = 0, iYangle = 0, iZangle = 0;

  float aTBNmatrix1[9], aTBNmatrix2[9];

  float aLightPos[] = { 0.0f, 0.0f, -1.0f }; // Light is nearest camera.

  //unsigned char *myPixels = calloc(1, 128*128*4); // Holds texture data.
  //unsigned char *myPixels2 = calloc(1, 128*128*4); // Holds texture data.

  float aRotate[16], aModelView[16], aPerspective[16], aMVP[16];

  int i;

  /* EGL Init */

  hDisplay = XOpenDisplay(NULL);

  if (!hDisplay) {
    printf("Could not open display\n");
    exit(-1);
  }

  sEGLDisplay = EGL_CHECK(eglGetDisplay(hDisplay));

  EGL_CHECK(eglInitialize(sEGLDisplay, NULL, NULL));
  EGL_CHECK(eglChooseConfig(sEGLDisplay, aEGLAttributes, aEGLConfigs, 1, &cEGLConfigs));

  if (cEGLConfigs == 0) {
        printf("No EGL configurations were returned.\n");
    exit(-1);
    }

  hWindow = create_window("OpenGL ES 2.0 Example on a Linux Desktop", uiWidth,
    uiHeight, hDisplay, sEGLDisplay, aEGLConfigs[0], &colormap, &pVisual);

  sEGLSurface = EGL_CHECK(eglCreateWindowSurface(sEGLDisplay, aEGLConfigs[0], (EGLNativeWindowType)hWindow, NULL));

    if (sEGLSurface == EGL_NO_SURFACE) {
        printf("Failed to create EGL surface.\n");
    exit(-1);
    }

  sEGLContext = EGL_CHECK(eglCreateContext(sEGLDisplay, aEGLConfigs[0], EGL_NO_CONTEXT, aEGLContextAttributes));

    if (sEGLContext == EGL_NO_CONTEXT) {
        printf("Failed to create EGL context.\n");
    exit(-1);
    }

  EGL_CHECK(eglMakeCurrent(sEGLDisplay, sEGLSurface, sEGLSurface, sEGLContext));

  vsx_es_shader_wrapper_init_shaders();
  

    GL_CHECK(glDisable(GL_CULL_FACE));
    GL_CHECK(glDisable(GL_DEPTH_TEST));

  XSelectInput(hDisplay, hWindow, KeyPressMask | ExposureMask | EnterWindowMask
    | LeaveWindowMask | PointerMotionMask | VisibilityChangeMask | ButtonPressMask
    | ButtonReleaseMask | StructureNotifyMask
  );
  app_init(0);
  
  /* Enter event loop */
  while (!bDone) {
    while (XPending(hDisplay) > 0) {
            XNextEvent(hDisplay, &e);

            if (e.type == ButtonPress) {
                bDone = 1;
            }
        }
        //printf("inner loop\n");
        //rotate_matrix(iXangle, 1.0, 0.0, 0.0, aModelView);
        //rotate_matrix(iYangle, 0.0, 1.0, 0.0, aRotate);

        //multiply_matrix(aRotate, aModelView, aModelView);

        //rotate_matrix(iZangle, 0.0, 1.0, 0.0, aRotate);

        //multiply_matrix(aRotate, aModelView, aModelView);

    /* Pull the camera back from the cube */
        //aModelView[14] -= 2.5;

        //perspective_matrix(45.0, (double)uiWidth/(double)uiHeight, 0.01, 100.0, aPerspective);
        //multiply_matrix(aPerspective, aModelView, aMVP);

        //GL_CHECK(glUniformMatrix4fv(iLocMVP, 1, GL_FALSE, aMVP));
        /*iXangle += 3;
        iYangle += 2;
        iZangle += 1;

        if(iXangle >= 360) iXangle -= 360;
        if(iXangle < 0) iXangle += 360;
        if(iYangle >= 360) iYangle -= 360;
        if(iYangle < 0) iYangle += 360;
        if(iZangle >= 360) iZangle -= 360;
        if(iZangle < 0) iZangle += 360;*/

        //GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
        //GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));
        app_pre_draw();
        app_draw(0);

        if (!eglSwapBuffers(sEGLDisplay, sEGLSurface)) {
            printf("Failed to swap buffers.\n");
        }

    //usleep(20000);
  }

  /* Cleanup shaders */
    GL_CHECK(glUseProgram(0));
    GL_CHECK(glDeleteShader(uiVertShader));
    GL_CHECK(glDeleteShader(uiFragShader));
    GL_CHECK(glDeleteProgram(uiProgram));

    /* EGL clean up */
    EGL_CHECK(eglMakeCurrent(sEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
    EGL_CHECK(eglDestroySurface(sEGLDisplay, sEGLSurface));
    EGL_CHECK(eglDestroyContext(sEGLDisplay, sEGLContext));
    EGL_CHECK(eglTerminate(sEGLDisplay));

  /* X windows clean up */
    XDestroyWindow(hDisplay, hWindow);
    XFreeColormap(hDisplay, colormap);
    XFree(pVisual);
    XCloseDisplay(hDisplay);

  return 0;
}
