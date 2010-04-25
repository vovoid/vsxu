//========================================================================
// GLFW - An OpenGL framework
// File:        platform.h
// Platforms:   AmigaOS, MorphOS
// API version: 2.6
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Camilla Berglund
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#ifndef _platform_h_
#define _platform_h_

// First of all: a file that hides compiler specific stuff
#include "SDI_compiler.h"


// This is the AmigaOS version of GLFW
#define _GLFW_AMIGAOS

// Are we compiling for MorphOS?
#if defined(__MORPHOS__) || defined(MORPHOS)
  #define _GLFW_MORPHOS
#endif

// Mesa/OpenGL flavour (we only support StormMesa at the moment)
#if !defined(_GLFW_STORMMESA)
  #define _GLFW_STORMMESA
#endif



// Include files
#include <exec/exec.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/dosextens.h>
#include <intuition/intuition.h>
#include <graphics/displayinfo.h>
#include <graphics/rastport.h>
#include <devices/timer.h>
#include <devices/keymap.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <devices/gameport.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/timer.h>
#include <proto/keymap.h>
#include <proto/utility.h>

#include <clib/alib_protos.h>


// Amiga Mesa/OpenGL implementation dependent include
#ifdef _GLFW_STORMMESA
  #include <GL/Amigamesa.h>
#endif

// MorphOS support
#ifdef _GLFW_MORPHOS
  #include <emul/emulinterface.h>
#endif

// GLFW+GL+GLU defines
#ifdef __GNUC__
  #include "../../include/GL/glfw.h"
#else
  #include "//include/GL/glfw.h"
#endif


// Stack size for each thread (in bytes)
#define _GLFW_TASK_STACK_SIZE 50000


//========================================================================
// Global variables (GLFW internals)
//========================================================================

//------------------------------------------------------------------------
// Shared libraries
//------------------------------------------------------------------------

#if defined( _init_c_ )
struct GfxBase       * GfxBase;
struct IntuitionBase * IntuitionBase;
struct Library       * KeymapBase;
struct UtilityBase   * UtilityBase;
struct Device        * TimerBase;
#endif


//------------------------------------------------------------------------
// Window structure
//------------------------------------------------------------------------
typedef struct _GLFWwin_struct _GLFWwin;

struct _GLFWwin_struct {

// ========= PLATFORM INDEPENDENT MANDATORY PART =========================

    // User callback functions
    GLFWwindowsizefun    WindowSizeCallback;
    GLFWwindowclosefun   WindowCloseCallback;
    GLFWwindowrefreshfun WindowRefreshCallback;
    GLFWmousebuttonfun   MouseButtonCallback;
    GLFWmouseposfun      MousePosCallback;
    GLFWmousewheelfun    MouseWheelCallback;
    GLFWkeyfun           KeyCallback;
    GLFWcharfun          CharCallback;

    // User selected window settings
    int       Fullscreen;      // Fullscreen flag
    int       MouseLock;       // Mouse-lock flag
    int       AutoPollEvents;  // Auto polling flag
    int       SysKeysDisabled; // System keys disabled flag
    int       WindowNoResize;  // Resize- and maximize gadgets disabled flag

    // Window status & parameters
    int       Opened;          // Flag telling if window is opened or not
    int       Active;          // Application active flag
    int       Iconified;       // Window iconified flag
    int       Width, Height;   // Window width and heigth
    int       Accelerated;     // GL_TRUE if window is HW accelerated
    int       RedBits;
    int       GreenBits;
    int       BlueBits;
    int       AlphaBits;
    int       DepthBits;
    int       StencilBits;
    int       AccumRedBits;
    int       AccumGreenBits;
    int       AccumBlueBits;
    int       AccumAlphaBits;
    int       AuxBuffers;
    int       Stereo;
    int       RefreshRate;     // Vertical monitor refresh rate

    // Extensions & OpenGL version
    int       Has_GL_SGIS_generate_mipmap;
    int       Has_GL_ARB_texture_non_power_of_two;
    int       GLVerMajor,GLVerMinor;


// ========= PLATFORM SPECIFIC PART ======================================

    // Platform specific window resources
    struct Screen   *Screen;       // Screen handle
    struct Window   *Window;       // Window handle
    ULONG           ModeID;        // ModeID
    APTR            PointerSprite; // Memory for blank pointer sprite
    int             PointerHidden; // Is pointer hidden?
    struct MsgPort  *InputMP;      // Message port (pointer movement)
    struct IOStdReq *InputIO;      // I/O request (pointer movement)

    // Mesa/OpenGL flavour specific
#ifdef _GLFW_STORMMESA
    struct amigamesa_context *Context;  // GL context handle
#endif

    // Platform specific extensions

    // Various platform specific internal variables

};

GLFWGLOBAL _GLFWwin _glfwWin;


//------------------------------------------------------------------------
// User input status (most of this should go in _GLFWwin)
//------------------------------------------------------------------------
GLFWGLOBAL struct {

// ========= PLATFORM INDEPENDENT MANDATORY PART =========================

    // Mouse status
    int  MousePosX, MousePosY;
    int  WheelPos;
    char MouseButton[ GLFW_MOUSE_BUTTON_LAST+1 ];

    // Keyboard status
    char Key[ GLFW_KEY_LAST+1 ];
    int  LastChar;

    // User selected settings
    int  StickyKeys;
    int  StickyMouseButtons;
    int  KeyRepeat;


// ========= PLATFORM SPECIFIC PART ======================================

    // Platform specific internal variables
    int  MouseMoved, OldMouseX, OldMouseY;

} _glfwInput;


//------------------------------------------------------------------------
// Timer status
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    struct MsgPort     *TimerMP;
    struct timerequest *TimerIO;
    double             Resolution;
    long long          t0;
} _glfwTimer;


//------------------------------------------------------------------------
// Thread record (one for each thread)
//------------------------------------------------------------------------
typedef struct _GLFWthread_struct _GLFWthread;

struct _GLFWthread_struct {

// ========= PLATFORM INDEPENDENT MANDATORY PART =========================

    // Pointer to previous and next threads in linked list
    _GLFWthread    *Previous, *Next;

    // GLFW user side thread information
    GLFWthread     ID;

// ========= PLATFORM SPECIFIC PART ======================================

    // System side thread information
    GLFWthreadfun  Function;
    void           *Arg;
    struct Process *AmiProc;
    struct Task    *AmiTask;

    // "Wait for" object. Can be a thread, condition variable or NULL.
    void           *WaitFor;
    int            WaitSig;

    // MorphOS support
#ifdef _GLFW_MORPHOS
    struct EmulLibEntry mosEmulLibEntry;
#endif
};


//------------------------------------------------------------------------
// General thread information
//------------------------------------------------------------------------
GLFWGLOBAL struct {

// ========= PLATFORM INDEPENDENT MANDATORY PART =========================

    // Next thread ID to use (increments for every created thread)
    GLFWthread       NextID;

    // First thread in linked list (always the main thread)
    _GLFWthread      First;

// ========= PLATFORM SPECIFIC PART ======================================

    // Critical section lock
    struct SignalSemaphore CriticalSection;

    // Next condition variable ID (decrements for every created cond)
    unsigned int NextCondID;

} _glfwThrd;


//------------------------------------------------------------------------
// Joystick information & state
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    int                    Present;
    int                    GameDeviceOpen;
    struct IOStdReq        *GameIO;
    struct MsgPort         *GameMP;
    struct InputEvent      GameEvent;
    float                  Axis[ 2 ];
    unsigned char          Button[ 2 ];
} _glfwJoy;


//========================================================================
// Macros for encapsulating critical code sections (i.e. making parts
// of GLFW thread safe)
//========================================================================

// Thread list management
#define ENTER_THREAD_CRITICAL_SECTION ObtainSemaphore( &_glfwThrd.CriticalSection );
#define LEAVE_THREAD_CRITICAL_SECTION ReleaseSemaphore( &_glfwThrd.CriticalSection );


//========================================================================
// Prototypes for platform specific internal functions
//========================================================================

// Time
int  _glfwInitTimer( void );
void _glfwTerminateTimer( void );

// Fullscreen
int _glfwOpenScreen( int *width, int *height, int *r, int *g, int *b,
    int refresh );
int _glfwGetClosestVideoMode( int *w, int *h, int *r, int *g, int *b,
                              int refresh );
void _glfwGetModeIDInfo( ULONG ModeID, int *w, int *h, int *r, int *g,
                         int *b, int *refresh );
// Joystick
void _glfwInitJoysticks( void );
void _glfwTerminateJoysticks( void );


#endif // _platform_h_
