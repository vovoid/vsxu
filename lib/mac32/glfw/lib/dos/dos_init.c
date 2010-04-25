//========================================================================
// GLFW - An OpenGL framework
// File:        dos_init.c
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



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwTerminate_atexit() - Terminate GLFW when exiting application
//========================================================================

void _glfwTerminate_atexit( void )
{
    glfwTerminate();
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformInit() - Initialize various GLFW state
//========================================================================

int _glfwPlatformInit( void )
{
    // Initialize thread package
    if( !_glfwInitThreads() )
    {
        return GL_FALSE;
    }

    // Start the timer
    if( !_glfwInitTimer() )
    {
        _glfwTerminateThreads();
        return GL_FALSE;
    }

    // Initialize joysticks
    _glfwInitJoysticks();

    // Install atexit() routine
    atexit( _glfwTerminate_atexit );

    return GL_TRUE;
}


//========================================================================
// _glfwPlatformTerminate() - Close window and kill all threads
//========================================================================

int _glfwPlatformTerminate( void )
{
    // Only the main thread is allowed to do this...
    // TODO

    // Close OpenGL window
    glfwCloseWindow();

    // Terminate joysticks
    _glfwTerminateJoysticks();

    // Kill timer
    _glfwTerminateTimer();

    // Kill thread package
    _glfwTerminateThreads();

    return GL_TRUE;
}
