//========================================================================
// GLFW - An OpenGL framework
// File:        dos_mouse.c
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

#include "internal.h"

//========================================================================
// Most of the code in this source file is based on the mouse driver in
// Daniel Borca's GLUT implementation for DOS/MESA.
//========================================================================

//========================================================================
// Definitions
//========================================================================

#define _GLFW_MOUSE_STACK_SIZE 16384


// Assembler function prototypes
extern void _glfwMouseWrap( void );
extern int  _glfwMouseWrap_end[];


//========================================================================
// Global variables
//========================================================================

static int _glfwMouseInstalled = 0;

static struct {
    long         Callback;
    int          Emulate3;
    int          OldX, OldY, OldB;
    __dpmi_regs  Regs;
} _glfwMouseDrv;



//************************************************************************
//**** Mouse Interrupt ***************************************************
//************************************************************************

//========================================================================
// _glfwMouseInt() - Mouse interrupt handler
//========================================================================

static void _glfwMouseInt( __dpmi_regs *r )
{
    int newx, newy, dx, dy, dz, buttons;
    _GLFWdosevent event;
    struct mousemove_event   *mousemove   = &event.MouseMove;
    struct mousewheel_event  *mousewheel  = &event.MouseWheel;
    struct mousebutton_event *mousebutton = &event.MouseButton;

    // Calculate mouse deltas
    newx = (signed short)r->x.si;
    newy = (signed short)r->x.di;
    dx   = newx - _glfwMouseDrv.OldX;
    dy   = newy - _glfwMouseDrv.OldY;
    dz   = (signed char)r->h.bh;

    // Get mouse buttons status
    buttons = r->h.bl;

    // Emulate 3rd mouse button?
    if( _glfwMouseDrv.Emulate3 )
    {
        if( (buttons & 3) == 3 )
        {
            buttons = 4;
        }
    }

    // Mouse moved?
    if( dx || dy )
    {
        mousemove->Type = _GLFW_DOS_MOUSE_MOVE_EVENT;
        mousemove->DeltaX = dx;
        mousemove->DeltaY = dy;
        _glfwPostDOSEvent( &event );
    }

    // Mouse wheel moved?
    if( dz )
    {
        mousewheel->Type = _GLFW_DOS_MOUSE_WHEEL_EVENT;
        mousewheel->WheelDelta = dz;
        _glfwPostDOSEvent( &event );
    }

    // Button state changed?
    if( buttons != _glfwMouseDrv.OldB )
    {
        mousebutton->Type = _GLFW_DOS_MOUSE_BUTTON_EVENT;

        // Left mouse button changed?
        if( (_glfwMouseDrv.OldB & 1) && !(buttons & 1) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_LEFT;
            mousebutton->Action = GLFW_RELEASE;
            _glfwPostDOSEvent( &event );
        }
        else if( !(_glfwMouseDrv.OldB & 1) && (buttons & 1) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_LEFT;
            mousebutton->Action = GLFW_PRESS;
            _glfwPostDOSEvent( &event );
        }

        // Right mouse button changed?
        if( (_glfwMouseDrv.OldB & 2) && !(buttons & 2) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_RIGHT;
            mousebutton->Action = GLFW_RELEASE;
            _glfwPostDOSEvent( &event );
        }
        else if( !(_glfwMouseDrv.OldB & 2) && (buttons & 2) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_RIGHT;
            mousebutton->Action = GLFW_PRESS;
            _glfwPostDOSEvent( &event );
        }

        // Middle mouse button changed?
        if( (_glfwMouseDrv.OldB & 4) && !(buttons & 4) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_MIDDLE;
            mousebutton->Action = GLFW_RELEASE;
            _glfwPostDOSEvent( &event );
        }
        else if( !(_glfwMouseDrv.OldB & 4) && (buttons & 4) )
        {
            mousebutton->Button = GLFW_MOUSE_BUTTON_MIDDLE;
            mousebutton->Action = GLFW_PRESS;
            _glfwPostDOSEvent( &event );
        }

    }

    // Remember old mouse state
    _glfwMouseDrv.OldX = newx;
    _glfwMouseDrv.OldY = newy;
    _glfwMouseDrv.OldB = buttons;
} ENDOFUNC(_glfwMouseInt)



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwInitMouse() - Initialize mouse driver
//========================================================================

int _glfwInitMouse( void )
{
    int buttons;

    // Already installed?
    if( _glfwMouseInstalled )
    {
        return 0;
    }

    // Reset mouse and get status
    __asm("\n\
                xorl    %%eax, %%eax    \n\
                int     $0x33           \n\
                andl    %%ebx, %%eax    \n\
                movl    %%eax, %0       \n\
    ":"=g" (buttons)::"%eax", "%ebx");
    if( !buttons )
    {
        return 0;
    }

    // Lock data and functions
    LOCKDATA( _glfwMouseDrv );
    LOCKBUFF( _glfwMouseWrap_end, 8 );
    LOCKFUNC( _glfwMouseInt );
    LOCKFUNC( _glfwMouseWrap );

    _glfwMouseWrap_end[1] = __djgpp_ds_alias;

    // Grab a locked stack
    _glfwMouseWrap_end[0] = (int)malloc( _GLFW_MOUSE_STACK_SIZE );
    if( _glfwMouseWrap_end[0] == NULL )
    {
        return 0;
    }
    LOCKBUFF( _glfwMouseWrap_end[0], _GLFW_MOUSE_STACK_SIZE );

    // Try to hook a call-back
    __asm("\n\
                pushl   %%ds            \n\
                pushl   %%es            \n\
                movw    $0x0303, %%ax   \n\
                pushl   %%ds            \n\
                pushl   %%cs            \n\
                popl    %%ds            \n\
                popl    %%es            \n\
                int     $0x31           \n\
                popl    %%es            \n\
                popl    %%ds            \n\
                jc      0f              \n\
                shll    $16, %%ecx      \n\
                movw    %%dx, %%cx      \n\
                movl    %%ecx, %0       \n\
        0:                              \n\
    ":"=g"(_glfwMouseDrv.Callback)
    :"S" (_glfwMouseWrap), "D"(&_glfwMouseDrv.Regs)
    :"%eax", "%ecx", "%edx");
    if( !_glfwMouseDrv.Callback )
    {
        free( (void *)_glfwMouseWrap_end[0] );
        return 0;
    }

    // Adjust stack
    _glfwMouseWrap_end[0] += _GLFW_MOUSE_STACK_SIZE;

    // Install the handler
    _glfwMouseDrv.Regs.x.ax = 0x000c;
    _glfwMouseDrv.Regs.x.cx = 0x7f | 0x80;
    _glfwMouseDrv.Regs.x.dx = _glfwMouseDrv.Callback & 0xffff;
    _glfwMouseDrv.Regs.x.es = _glfwMouseDrv.Callback >> 16;
    __dpmi_int( 0x33, &_glfwMouseDrv.Regs );

    // Clear mickeys
    __asm __volatile ("\n\
        movw $0xb, %%ax;    \n\
        int $0x33           \n\
    ":::"%eax", "%ecx", "%edx");

    _glfwMouseDrv.OldX = 0;
    _glfwMouseDrv.OldY = 0;
    _glfwMouseDrv.OldB = 0;

    // Emulate third mouse button?
    _glfwMouseDrv.Emulate3 = buttons < 3;

    return 1;
}


//========================================================================
// _glfwTerminateMouse() - Terminate mouse driver
//========================================================================

void _glfwTerminateMouse( void )
{
    if( !_glfwMouseInstalled )
    {
        return;
    }

    __asm("\n\
            movl    %%edx, %%ecx    \n\
            shrl    $16, %%ecx      \n\
            movw    $0x0304, %%ax   \n\
            int     $0x31           \n\
            movw    $0x000c, %%ax   \n\
            xorl    %%ecx, %%ecx    \n\
            int     $0x33           \n\
    "::"d"(_glfwMouseDrv.Callback):"%eax", "%ecx");
    _glfwMouseDrv.Callback = 0;

    free( (void *)(_glfwMouseWrap_end[0] - _GLFW_MOUSE_STACK_SIZE) );

    _glfwMouseInstalled = 0;
}



//========================================================================
// _glfwMouseWrap()
//========================================================================

// Hack alert: `_glfwMouseWrap_end' actually holds the address of stack in
// a safe data selector.

__asm("\n\
                .text                           \n\
                .p2align 5,,31                  \n\
                .global __glfwMouseWrap         \n\
__glfwMouseWrap:                                \n\
                cld                             \n\
                lodsl                           \n\
                movl    %eax, %es:42(%edi)      \n\
                addw    $4, %es:46(%edi)        \n\
                pushl   %es                     \n\
                movl    %ss, %ebx               \n\
                movl    %esp, %esi              \n\
                lss     %cs:__glfwMouseWrap_end, %esp\n\
                pushl   %ss                     \n\
                pushl   %ss                     \n\
                popl    %es                     \n\
                popl    %ds                     \n\
                movl    ___djgpp_dos_sel, %fs   \n\
                pushl   %fs                     \n\
                popl    %gs                     \n\
                pushl   %edi                    \n\
                call    __glfwMouseInt          \n\
                popl    %edi                    \n\
                movl    %ebx, %ss               \n\
                movl    %esi, %esp              \n\
                popl    %es                     \n\
                iret                            \n\
                .global __glfwMouseWrap_end     \n\
__glfwMouseWrap_end:.long   0, 0");
