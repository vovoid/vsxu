/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2009 - 2010 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

//#include "cube.h"
//#include "shader.h"
//#include "window.h"
//#include "matrix.h"

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "application.h"


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




/*
 * Loads the shader source into memory.
 *
 * sFilename: String holding filename to load
 */
char* load_shader(char *sFilename) {
    char *pResult = NULL;
    FILE *pFile = NULL;
    long iLen = 0;

    pFile = fopen(sFilename, "r");

    if(pFile == NULL) {
        fprintf(stderr, "Error: Cannot read file '%s'\n", sFilename);
      exit(-1);
    }

    fseek(pFile, 0, SEEK_END); /* Seek end of file */
    iLen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET); /* Seek start of file again */
    pResult = (char*)calloc(iLen+1, sizeof(char));
    fread(pResult, sizeof(char), iLen, pFile);
    pResult[iLen] = '\0';
    fclose(pFile);

    return pResult;
}

/*
 * Create shader, load in source, compile, dump debug as necessary.
 *
 * pShader: Pointer to return created shader ID.
 * sFilename: Passed-in filename from which to load shader source.
 * iShaderType: Passed to GL, e.g. GL_VERTEX_SHADER.
 */
void process_shader(GLuint *pShader, char *sFilename, GLint iShaderType) {
  GLint iStatus;
  const char *aStrings[1] = { NULL };

  /* Create shader and load into GL. */
  *pShader = GL_CHECK(glCreateShader(iShaderType));

  aStrings[0] = load_shader(sFilename);

  GL_CHECK(glShaderSource(*pShader, 1, aStrings, NULL));

  /* Clean up shader source. */
  free((void *)aStrings[0]);
  aStrings[0] = NULL;

  /* Try compiling the shader. */
  GL_CHECK(glCompileShader(*pShader));
  GL_CHECK(glGetShaderiv(*pShader, GL_COMPILE_STATUS, &iStatus));

  // Dump debug info (source and log) if compilation failed.
  if(iStatus != GL_TRUE) {
#ifdef DEBUG
    GLint iLen;
    char *sDebugSource = NULL;
    char *sErrorLog = NULL;

    /* Get shader source. */
    GL_CHECK(glGetShaderiv(*pShader, GL_SHADER_SOURCE_LENGTH, &iLen));

    sDebugSource = (char*)malloc(iLen);

    GL_CHECK(glGetShaderSource(*pShader, iLen, NULL, sDebugSource));

    printf("Debug source START:\n%s\nDebug source END\n\n", sDebugSource);
    free(sDebugSource);

    /* Now get the info log. */
    GL_CHECK(glGetShaderiv(*pShader, GL_INFO_LOG_LENGTH, &iLen));

    sErrorLog = (char*)malloc(iLen);

    GL_CHECK(glGetShaderInfoLog(*pShader, iLen, NULL, sErrorLog));

    printf("Log START:\n%s\nLog END\n\n", sErrorLog);
    free(sErrorLog);
#endif

    exit(-1);
  }
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

  /* Shader Initialisation */
  process_shader(&uiVertShader, "shader.vert", GL_VERTEX_SHADER);
  process_shader(&uiFragShader, "shader.frag", GL_FRAGMENT_SHADER);

  /* Create uiProgram (ready to attach shaders) */
    uiProgram = GL_CHECK(glCreateProgram());

    /* Attach shaders and link uiProgram */
    GL_CHECK(glAttachShader(uiProgram, uiVertShader));
    GL_CHECK(glAttachShader(uiProgram, uiFragShader));
    GL_CHECK(glLinkProgram(uiProgram));

    /* Get attribute locations of non-fixed attributes like colour and texture coordinates. */
    iLocPosition = GL_CHECK(glGetAttribLocation(uiProgram, "av4position"));
    iLocColour = GL_CHECK(glGetAttribLocation(uiProgram, "av3colour"));

#ifdef DEBUG
  printf("iLocPosition = %i\n", iLocPosition);
  printf("iLocColour   = %i\n", iLocColour);
#endif

  /* Get uniform locations */
    iLocMVP = GL_CHECK(glGetUniformLocation(uiProgram, "mvp"));

#ifdef DEBUG
  printf("iLocMVP      = %i\n", iLocMVP);
#endif

  GL_CHECK(glUseProgram(uiProgram));

    /* Enable attributes for position, colour and texture coordinates etc. */
    GL_CHECK(glEnableVertexAttribArray(iLocPosition));
    GL_CHECK(glEnableVertexAttribArray(iLocColour));

  /* Populate attributes for position, colour and texture coordinates etc. */
    GL_CHECK(glVertexAttribPointer(iLocPosition, 3, GL_FLOAT, GL_FALSE, 0, aVertices));
    GL_CHECK(glVertexAttribPointer(iLocColour, 3, GL_FLOAT, GL_FALSE, 0, aColours));

    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glEnable(GL_DEPTH_TEST));

  XSelectInput(hDisplay, hWindow, KeyPressMask | ExposureMask | EnterWindowMask
    | LeaveWindowMask | PointerMotionMask | VisibilityChangeMask | ButtonPressMask
    | ButtonReleaseMask | StructureNotifyMask);
  app_init(0);
  
  /* Enter event loop */
  while (!bDone) {
    while (XPending(hDisplay) > 0) {
            XNextEvent(hDisplay, &e);

            if (e.type == ButtonPress) {
                bDone = 1;
            }
        }

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
        app_pre_draw();
        app_draw(0);
        iXangle += 3;
        iYangle += 2;
        iZangle += 1;

        if(iXangle >= 360) iXangle -= 360;
        if(iXangle < 0) iXangle += 360;
        if(iYangle >= 360) iYangle -= 360;
        if(iYangle < 0) iYangle += 360;
        if(iZangle >= 360) iZangle -= 360;
        if(iZangle < 0) iZangle += 360;

        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));

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
