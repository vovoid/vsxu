//========================================================================
// GLFW - An OpenGL framework
// File:        dos_events.c
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
// Global variables
//========================================================================

// Event buffer
#define _GLFW_EVENT_BUFFER_SIZE 1024

static volatile struct {
    volatile int Start, End;
    volatile _GLFWdosevent *Event;
} _glfwEventBuffer;

static int _glfwEventsInitialized = 0;



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwWaitNextEvent() - Wait for an event to appear in the event FIFO
// NOTE: Must not be called from an interrupt routine
//========================================================================

void _glfwWaitNextEvent( void )
{
    int noevent = 1;

    while( noevent )
    {
        DISABLE();
        noevent = ( _glfwEventBuffer.Start == _glfwEventBuffer.End );
        ENABLE();
        // Wait for an interrupt to happen?...
    }
}


//========================================================================
// _glfwGetNextEvent() - Get an event from the event FIFO
// NOTE: Must not be called from an interrupt routine
//========================================================================

int _glfwGetNextEvent( _GLFWdosevent *event )
{
    DISABLE();

    if( _glfwEventBuffer.Start == _glfwEventBuffer.End )
    {
        ENABLE();
        return 0;
    }

    *event = _glfwEventBuffer.Event[ _glfwEventBuffer.Start ++ ];
    if( _glfwEventBuffer.Start >= _GLFW_EVENT_BUFFER_SIZE )
    {
        _glfwEventBuffer.Start = 0;
    }

    ENABLE();

    return 1;
}


//========================================================================
// _glfwPostDOSEvent() - Put an event onto the event FIFO
// NOTE: Must only be called from an interrupt routine
//========================================================================

void _glfwPostDOSEvent( _GLFWdosevent *event )
{
    // Add event
    _glfwEventBuffer.Event[ _glfwEventBuffer.End ++ ] = *event;

    // End of FIFO buffer?
    if( _glfwEventBuffer.End >= _GLFW_EVENT_BUFFER_SIZE )
        _glfwEventBuffer.End = 0;

    // If the buffer is full, drop the oldest event
    if( _glfwEventBuffer.End == _glfwEventBuffer.Start)
    {
        _glfwEventBuffer.Start ++;
        if( _glfwEventBuffer.Start >= _GLFW_EVENT_BUFFER_SIZE )
            _glfwEventBuffer.Start = 0;
    }
} ENDOFUNC(_glfwPostDOSEvent)


//========================================================================
// _glfwInitEvents() - Initialize event management functions and the FIFO
//========================================================================

int _glfwInitEvents( void )
{
    int fifosize;

    // Allocate memory for the event FIFO buffer
    fifosize = _GLFW_EVENT_BUFFER_SIZE * sizeof(_GLFWdosevent);
    _glfwEventBuffer.Event = malloc( fifosize );
    if( !_glfwEventBuffer.Event )
    {
        return 0;
    }

    // Lock data & functions
    LOCKBUFF( _glfwEventBuffer.Event, fifosize );
    LOCKDATA( _glfwEventBuffer );
    LOCKFUNC( _glfwPostDOSEvent );

    // Initialize event FIFO
    _glfwEventBuffer.Start = _glfwEventBuffer.End   = 0;

    _glfwEventsInitialized = 1;

    return 1;
}


//========================================================================
// _glfwTerminateEvents() - Terminate event management
//========================================================================

void _glfwTerminateEvents( void )
{
    if( !_glfwEventsInitialized )
    {
        return;
    }

    _glfwEventsInitialized = 0;

    // Free memory for the event FIFO buffer
    if( _glfwEventBuffer.Event )
    {
        free( (void *) _glfwEventBuffer.Event );
        _glfwEventBuffer.Event = NULL;
    }
}
