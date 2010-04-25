//========================================================================
// GLFW - An OpenGL framework
// File:        amigaos_init.c
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

#include "internal.h"



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

//========================================================================
// _glfwInitLibraries() - Load shared libraries
//========================================================================

static int _glfwInitLibraries( void )
{
    // Note: exec.library & dos.library are always opened (by C startup)

    // Start by clearing all handles
    GfxBase       = NULL;
    IntuitionBase = NULL;
    KeymapBase    = NULL;
    UtilityBase   = NULL;

    // graphics.library
    GfxBase = (struct GfxBase *) OpenLibrary( "graphics.library", 40 );
    if( !GfxBase )
    {
        return 0;
    }

    // intuition.library
    IntuitionBase = (struct IntuitionBase *) OpenLibrary( "intuition.library", 40 );
    if( !IntuitionBase )
    {
        return 0;
    }

    // keymap.library
    KeymapBase = OpenLibrary( "keymap.library", 40 );
    if( !KeymapBase )
    {
        return 0;
    }

    // Utility.library
    UtilityBase = (struct UtilityBase *) OpenLibrary( "utility.library", 40 );
    if( !UtilityBase )
    {
        return 0;
    }

    return 1;
}


//========================================================================
// _glfwTerminateLibraries() - Unload shared libraries
//========================================================================

static void _glfwTerminateLibraries( void )
{
    // Close graphics.library
    if( GfxBase )
    {
        CloseLibrary( (struct Library *) GfxBase );
        GfxBase = NULL;
    }

    // Close intuition.library
    if( IntuitionBase )
    {
        CloseLibrary( (struct Library *) IntuitionBase );
        IntuitionBase = NULL;
    }

    // Close keymap.library
    if( KeymapBase )
    {
        CloseLibrary( KeymapBase );
        KeymapBase = NULL;
    }

    // Close utility.library
    if( UtilityBase )
    {
        CloseLibrary( (struct Library *) UtilityBase );
        UtilityBase = NULL;
    }
}


//========================================================================
// _glfwInitThreads() - Initialize GLFW thread package
//========================================================================

static int _glfwInitThreads( void )
{
    int waitSig;

    // Allocate a signal to use for waiting (glfwWaitThread and
    // glfwWaitCond)
    waitSig = AllocSignal( -1 );
    if( waitSig == -1 )
    {
        return 0;
    }

    // Initialize critical section handle
    memset( &_glfwThrd.CriticalSection,0,sizeof(struct SignalSemaphore) );
    InitSemaphore( &_glfwThrd.CriticalSection );

    // The first thread (the main thread) has ID 0
    _glfwThrd.NextID = 0;

    // The first condition variable has ID 0xFFFFFFFF
    _glfwThrd.NextCondID = 0xFFFFFFFF;

    // Fill out information about the main thread (this thread)
    _glfwThrd.First.Previous = NULL;
    _glfwThrd.First.Next     = NULL;
    _glfwThrd.First.ID       = _glfwThrd.NextID ++;
    _glfwThrd.First.Function = NULL;
    _glfwThrd.First.Arg      = NULL;
    _glfwThrd.First.AmiTask  = FindTask( NULL );
    _glfwThrd.First.AmiProc  = (struct Process *) _glfwThrd.First.AmiTask;
    _glfwThrd.First.WaitFor  = NULL;
    _glfwThrd.First.WaitSig  = waitSig;

    // Store GLFW thread struct pointer in task user data
    _glfwThrd.First.AmiTask->tc_UserData = (APTR) &_glfwThrd.First;

    return 1;
}


//========================================================================
// _glfwTerminateThreads() - Terminate GLFW thread package
//========================================================================

static void _glfwTerminateThreads( void )
{
    _GLFWthread *t, *t_next;

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Kill all threads (NOTE: THE USER SHOULD WAIT FOR ALL THREADS TO
    // DIE, _BEFORE_ CALLING glfwTerminate()!!!)
    t = _glfwThrd.First.Next;
    while( t != NULL )
    {
        // Get pointer to next thread
        t_next = t->Next;

        // Simply murder the process, no mercy!
        // ?? How about Process resources ??
        RemTask( t->AmiTask );

        // Free memory allocated for this thread
        free( (void *) t );

        // Select next thread in list
        t = t_next;
    }

    // Free waiting signal for main thread
    FreeSignal( _glfwThrd.First.WaitSig );

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION
}


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
    // Load shared libraries
    if( !_glfwInitLibraries() )
    {
        _glfwTerminateLibraries();
        return GL_FALSE;
    }

    // Initialize thread package
    if( !_glfwInitThreads() )
    {
        _glfwTerminateLibraries();
        return GL_FALSE;
    }

    // Start the timer
    if( !_glfwInitTimer() )
    {
        _glfwTerminateThreads();
        _glfwTerminateLibraries();
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
    if( FindTask( NULL ) != _glfwThrd.First.AmiTask )
    {
        return GL_FALSE;
    }

    // Close OpenGL window
    glfwCloseWindow();

    // Terminate joysticks
    _glfwTerminateJoysticks();

    // Kill timer
    _glfwTerminateTimer();

    // Kill thread package
    _glfwTerminateThreads();

    // Unload shared libraries
    _glfwTerminateLibraries();

    return GL_TRUE;
}
