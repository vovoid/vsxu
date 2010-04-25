//========================================================================
// GLFW - An OpenGL framework
// File:        platform.h
// Platform:    DOS
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


// This is the DOS version of GLFW
#define _GLFW_DOS


// Include files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <dos.h>
#include <dpmi.h>
#include <fcntl.h>
#include <pc.h>
#include <sys/stat.h>
#include <sys/exceptn.h>
#include <sys/farptr.h>
#include <sys/segments.h>

// GLFW+GL+GLU defines
#include "../../include/GL/glfw.h"

// DOS Mesa (include this AFTER gl.h!)
#include <GL/dmesa.h>


// Stack size for each thread (in bytes)
#define _GLFW_TASK_STACK_SIZE 50000




//========================================================================
// Global variables (GLFW internals)
//========================================================================


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
    DMesaVisual  Visual;
    DMesaContext Context;
    DMesaBuffer  Buffer;

    // Standard output redirection
    char OutName[L_tmpnam];
    char ErrName[L_tmpnam];
    int  hOut,hOutOld,hErr,hErrOld;

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

} _glfwInput;


//------------------------------------------------------------------------
// Timer status
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    double    Period;
    long long t0;
    int       HasRDTSC;
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

} _glfwThrd;


//------------------------------------------------------------------------
// Joystick information & state
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    int dummy;
} _glfwJoy;


//========================================================================
// Macros for encapsulating critical code sections (i.e. making parts
// of GLFW thread safe)
//========================================================================

// Thread list management
#define ENTER_THREAD_CRITICAL_SECTION \
        ;
#define LEAVE_THREAD_CRITICAL_SECTION \
        ;


//========================================================================
// DOS events
//========================================================================

// Valid event types
#define _GLFW_DOS_KEY_EVENT          1
#define _GLFW_DOS_MOUSE_MOVE_EVENT   2
#define _GLFW_DOS_MOUSE_WHEEL_EVENT  3
#define _GLFW_DOS_MOUSE_BUTTON_EVENT 4

// Keyboard event structure
struct key_event {
    int Type;
    int Key;
    int KeyNoMod;
    int Action;
};

// Mouse move event structure
struct mousemove_event {
    int Type;
    int DeltaX, DeltaY;
};

// Mouse wheel event structure
struct mousewheel_event {
    int Type;
    int WheelDelta;
};

// Mouse button event structure
struct mousebutton_event {
    int Type;
    int Button;
    int Action;
};

// DOS event structure
typedef union {
    int Type;
    struct key_event            Key;
    struct mousemove_event      MouseMove;
    struct mousewheel_event     MouseWheel;
    struct mousebutton_event    MouseButton;
} _GLFWdosevent;



//========================================================================
// Prototypes for platform specific internal functions
//========================================================================

// Time
int  _glfwInitTimer( void );
void _glfwTerminateTimer( void );

// Fullscreen

// Events
int  _glfwInitEvents( void );
void _glfwTerminateEvents( void );
void _glfwWaitNextEvent( void );
int  _glfwGetNextEvent( _GLFWdosevent *event );
void _glfwPostDOSEvent( _GLFWdosevent *event );

// Mouse
int  _glfwInitMouse( void );
void _glfwTerminateMouse( void );

// Keyboard
int  _glfwInitKeyboard( void );
void _glfwTerminateKeyboard( void );

// Joystick
void _glfwInitJoysticks( void );
void _glfwTerminateJoysticks( void );

// Threads
int  _glfwInitThreads( void );
void _glfwTerminateThreads( void );

// Interrupt handling
int _glfwInstallDOSIrq( int i, int (*handler) () );
int _glfwRemoveDOSIrq( int i );

// Interrupt macros
#define ENABLE()  __asm __volatile("sti")
#define DISABLE() __asm __volatile("cli")

// Memory macros (for locking memory)
#define ENDOFUNC(x)    static void x##_end() { }
#define LOCKFUNC(x)    _go32_dpmi_lock_code((void *)x, (long)x##_end - (long)x)
#define LOCKDATA(x)    _go32_dpmi_lock_data((void *)&x, sizeof(x))
#define LOCKBUFF(x, l) _go32_dpmi_lock_data((void *)x, l)

#endif // _platform_h_
