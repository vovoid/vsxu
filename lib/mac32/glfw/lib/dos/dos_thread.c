//========================================================================
// GLFW - An OpenGL framework
// File:        dos_thread.c
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
// _glfwInitThreads() - Initialize GLFW thread package
//========================================================================

int _glfwInitThreads( void )
{
    // TODO
    _glfwThrd.First.Previous = NULL;
    _glfwThrd.First.Next     = NULL;
    _glfwThrd.First.ID       = 0;
    _glfwThrd.NextID = 1;

    return 1;
}


//========================================================================
// _glfwTerminateThreads() - Terminate GLFW thread package
//========================================================================

void _glfwTerminateThreads( void )
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
        // TODO

        // Free memory allocated for this thread
        free( (void *) t );

        // Select next thread in list
        t = t_next;
    }

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformCreateThread() - Create a new thread
//========================================================================

GLFWthread _glfwPlatformCreateThread( GLFWthreadfun fun, void *arg )
{
    // TODO
    return -1;
}


//========================================================================
// _glfwPlatformDestroyThread() - Kill a thread. NOTE: THIS IS A VERY
// DANGEROUS OPERATION, AND SHOULD NOT BE USED EXCEPT IN EXTREME
// SITUATIONS!
//========================================================================

void _glfwPlatformDestroyThread( GLFWthread ID )
{
    _GLFWthread *t, *t_wait;

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Get thread information pointer
    t = _glfwGetThreadPointer( ID );
    if( t == NULL )
    {
        LEAVE_THREAD_CRITICAL_SECTION
        return;
    }

    // Simply murder the process, no mercy!
    // TODO

    // Remove thread from thread list
    _glfwRemoveThread( t );

    // Signal any waiting threads that the thread has died
    // TODO

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION
}


//========================================================================
// _glfwPlatformWaitThread() - Wait for a thread to die
//========================================================================

int _glfwPlatformWaitThread( GLFWthread ID, int waitmode )
{
    // TODO
    return GL_FALSE;
}


//========================================================================
// _glfwPlatformGetThreadID() - Return the thread ID for the current
// thread
//========================================================================

GLFWthread _glfwPlatformGetThreadID( void )
{
    // TODO
    return 0;
}


//========================================================================
// _glfwPlatformCreateMutex() - Create a mutual exclusion object
//========================================================================

GLFWmutex _glfwPlatformCreateMutex( void )
{
    // TODO
    return NULL;
}


//========================================================================
// _glfwPlatformDestroyMutex() - Destroy a mutual exclusion object
//========================================================================

void _glfwPlatformDestroyMutex( GLFWmutex mutex )
{
    // TODO
}


//========================================================================
// _glfwPlatformLockMutex() - Request access to a mutex
//========================================================================

void _glfwPlatformLockMutex( GLFWmutex mutex )
{
    // TODO
}


//========================================================================
// _glfwPlatformUnlockMutex() - Release a mutex
//========================================================================

void _glfwPlatformUnlockMutex( GLFWmutex mutex )
{
    // TODO
}


//========================================================================
// _glfwPlatformCreateCond() - Create a new condition variable object
//========================================================================

GLFWcond _glfwPlatformCreateCond( void )
{
    // TODO
    return NULL;
}


//========================================================================
// _glfwPlatformDestroyCond() - Destroy a condition variable object
//========================================================================

void _glfwPlatformDestroyCond( GLFWcond cond )
{
    // TODO
}


//========================================================================
// _glfwPlatformWaitCond() - Wait for a condition to be raised
//========================================================================

void _glfwPlatformWaitCond( GLFWcond cond, GLFWmutex mutex,
    double timeout )
{
    // TODO
}


//========================================================================
// _glfwPlatformSignalCond() - Signal a condition to one waiting thread
//========================================================================

void _glfwPlatformSignalCond( GLFWcond cond )
{
    // TODO
}


//========================================================================
// _glfwPlatformBroadcastCond() - Broadcast a condition to all waiting
// threads
//========================================================================

void _glfwPlatformBroadcastCond( GLFWcond cond )
{
    // TODO
}


//========================================================================
// _glfwPlatformGetNumberOfProcessors() - Return the number of processors
// in the system.
//========================================================================

int _glfwPlatformGetNumberOfProcessors( void )
{
    // Return number of processors online (DOS does not support multiple
    // CPUs...)
    return 1;
}
