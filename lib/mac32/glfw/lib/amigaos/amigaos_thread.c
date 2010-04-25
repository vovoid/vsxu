//========================================================================
// GLFW - An OpenGL framework
// File:        amigaos_thread.c
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
// _glfwNewThread() - This is simply a "wrapper" for calling the user
// thread function.
//========================================================================

int _glfwNewThread( void )
{
    GLFWthreadfun threadfun;
    void          *arg;
    _GLFWthread   *t, *t_wait;
    struct Task   *amiTask;
    int           waitSig;

    // Allocate a signal to use for waiting (glfwWaitThread and
    // glfwWaitCond)
    waitSig = AllocSignal( -1 );
    if( waitSig == -1 )
    {
        // If we could not get a signal (VERY UNLIKELY), exit immediately
        return 0;
    }

    // Get current task
    amiTask = FindTask( NULL );

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // The task's user data points to the GLFW thread struct
    t = (_GLFWthread *) amiTask->tc_UserData;

    // Store wait signal handle
    t->WaitSig = waitSig;
    t->WaitFor = NULL;

    // Get user thread function pointer and argument
    threadfun = t->Function;
    arg       = t->Arg;

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

    // Call the user thread function
    threadfun( arg );

    ENTER_THREAD_CRITICAL_SECTION

    // Remove thread from thread list
    _glfwRemoveThread( t );

    // Signal any waiting threads that we have died
    for( t_wait = &_glfwThrd.First; t_wait; t_wait = t_wait->Next )
    {
        if( t_wait->WaitFor == (void *) t )
        {
            Signal( t_wait->AmiTask, 1L<<t_wait->WaitSig );
            t_wait->WaitFor = NULL;
        }
    }

    LEAVE_THREAD_CRITICAL_SECTION

    // When the process function returns, the process will die...
    return 0;
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformCreateThread() - Create a new thread
//========================================================================

GLFWthread _glfwPlatformCreateThread( GLFWthreadfun fun, void *arg )
{
    GLFWthread     ID;
    _GLFWthread    *t;
    struct TagItem tagList[ 10 ];
    int            tagNR;


    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Create a new thread information memory area
    t = (_GLFWthread *) malloc( sizeof(_GLFWthread) );
    if( t == NULL )
    {
        // Leave critical section
        LEAVE_THREAD_CRITICAL_SECTION
        return -1;
    }

    // Get a new unique thread id
    ID = _glfwThrd.NextID ++;

    // Store thread information in the thread list
    t->ID       = ID;
    t->Function = fun;
    t->Arg      = arg;

#ifdef _GLFW_MORPHOS
    // For MorphOS, we set up a 68k -> PPC switch trap instruction.
    // CreateNewProc actually creates a 68k process (emulated), so we make
    // sure that the first 68k instruction that is executed is a trap
    // instruction that forces the execution model to change from emulated
    // 68k to native PPC (and starts execution at _glfwNewThread).
    t->mosEmulLibEntry.Trap      = TRAP_LIB;
    t->mosEmulLibEntry.Extension = 0;
    t->mosEmulLibEntry.Func      = _glfwNewThread;
#endif

    // Create new process
    tagNR = 0;
    tagList[ tagNR   ].ti_Tag  = NP_Entry;
#ifdef _GLFW_MORPHOS
    tagList[ tagNR++ ].ti_Data = (ULONG) &t->mosEmulLibEntry;
#else
    tagList[ tagNR++ ].ti_Data = (ULONG) _glfwNewThread;
#endif
    tagList[ tagNR   ].ti_Tag  = NP_StackSize;
    tagList[ tagNR++ ].ti_Data = _GLFW_TASK_STACK_SIZE;
    tagList[ tagNR   ].ti_Tag  = NP_Input;
    tagList[ tagNR++ ].ti_Data = (ULONG) Input();
    tagList[ tagNR   ].ti_Tag  = NP_Output;
    tagList[ tagNR++ ].ti_Data = (ULONG) Output();
    tagList[ tagNR   ].ti_Tag  = NP_CloseInput;
    tagList[ tagNR++ ].ti_Data = FALSE;
    tagList[ tagNR   ].ti_Tag  = NP_CloseOutput;
    tagList[ tagNR++ ].ti_Data = FALSE;
    tagList[ tagNR   ].ti_Tag  = TAG_DONE;
    t->AmiProc = CreateNewProc( tagList );

    // Did the process creation fail?
    if( !t->AmiProc )
    {
        free( (void *) t );
        LEAVE_THREAD_CRITICAL_SECTION
        return -1;
    }

    // Get pointer to task structure
    t->AmiTask = &(t->AmiProc->pr_Task);

    // Store GLFW thread struct pointer in task user data
    t->AmiTask->tc_UserData = (APTR) t;

    // Append thread to thread list
    _glfwAppendThread( t );

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

    // Return the GLFW thread ID
    return ID;
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
    // ?? How about Process resources ??
    RemTask( t->AmiTask );

    // Remove thread from thread list
    _glfwRemoveThread( t );

    // Signal any waiting threads that the thread has died
    for( t_wait = &_glfwThrd.First; t_wait; t_wait = t_wait->Next )
    {
        if( t_wait->WaitFor == (void *) t )
        {
            Signal( t_wait->AmiTask, 1L<<t_wait->WaitSig );
            t_wait->WaitFor = NULL;
        }
    }

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION
}


//========================================================================
// _glfwPlatformWaitThread() - Wait for a thread to die
//========================================================================

int _glfwPlatformWaitThread( GLFWthread ID, int waitmode )
{
    struct Task *amiTask;
    _GLFWthread *t, *t_this;
    int         waitSig;

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Get thread information pointer
    t = _glfwGetThreadPointer( ID );

    // Is the thread already dead?
    if( t == NULL )
    {
        LEAVE_THREAD_CRITICAL_SECTION
        return GL_TRUE;
    }

    // If got this far, the thread is alive => polling returns FALSE
    if( waitmode == GLFW_NOWAIT )
    {
        LEAVE_THREAD_CRITICAL_SECTION
        return GL_FALSE;
    }

    // Find pointer to this threads structure
    amiTask = FindTask( NULL );
    t_this  = (_GLFWthread *) amiTask->tc_UserData;

    // Store information in our thread structure that we want to wait for
    // the specified thread to die
    t_this->WaitFor = (void *) t;
    waitSig         = t_this->WaitSig;

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

    // Wait for thread to die
    Wait( 1L<<waitSig );

    return GL_TRUE;
}


//========================================================================
// _glfwPlatformGetThreadID() - Return the thread ID for the current
// thread
//========================================================================

GLFWthread _glfwPlatformGetThreadID( void )
{
    _GLFWthread *t;
    struct Task *amiTask;

    // Get current task
    amiTask = FindTask( NULL );

    // The task's user data points to the GLFW thread struct
    t = (_GLFWthread *) amiTask->tc_UserData;

    // Return the found GLFW thread identifier
    return t->ID;
}


//========================================================================
// _glfwPlatformCreateMutex() - Create a mutual exclusion object
//========================================================================

GLFWmutex _glfwPlatformCreateMutex( void )
{
    struct SignalSemaphore *mutex;

    // Allocate memory for mutex
    mutex = (struct SignalSemaphore *) malloc( sizeof(struct SignalSemaphore) );
    if( !mutex )
    {
        return NULL;
    }

    // Initialize mutex object
    memset( mutex, 0, sizeof(struct SignalSemaphore) );
    InitSemaphore( mutex );

    // Cast to GLFWmutex and return
    return (GLFWmutex) mutex;
}


//========================================================================
// _glfwPlatformDestroyMutex() - Destroy a mutual exclusion object
//========================================================================

void _glfwPlatformDestroyMutex( GLFWmutex mutex )
{
    // Free memory for mutex object
    free( (void *) mutex );
}


//========================================================================
// _glfwPlatformLockMutex() - Request access to a mutex
//========================================================================

void _glfwPlatformLockMutex( GLFWmutex mutex )
{
    // Wait for mutex to be released
    ObtainSemaphore( (struct SignalSemaphore *) mutex );
}


//========================================================================
// _glfwPlatformUnlockMutex() - Release a mutex
//========================================================================

void _glfwPlatformUnlockMutex( GLFWmutex mutex )
{
    // Release mutex
    ReleaseSemaphore( (struct SignalSemaphore *) mutex );
}


//========================================================================
// _glfwPlatformCreateCond() - Create a new condition variable object
//========================================================================

GLFWcond _glfwPlatformCreateCond( void )
{
    unsigned int cond;

    // Generate a new unique cond ID
    ENTER_THREAD_CRITICAL_SECTION
    cond = _glfwThrd.NextCondID --;
    LEAVE_THREAD_CRITICAL_SECTION

    // Cast to GLFWcond and return
    return (GLFWcond) cond;
}


//========================================================================
// _glfwPlatformDestroyCond() - Destroy a condition variable object
//========================================================================

void _glfwPlatformDestroyCond( GLFWcond cond )
{
}


//========================================================================
// _glfwPlatformWaitCond() - Wait for a condition to be raised
//========================================================================

void _glfwPlatformWaitCond( GLFWcond cond, GLFWmutex mutex,
    double timeout )
{
    struct Task *amiTask;
    _GLFWthread *t_this;

    // Do we need a limited timeout?
    if( timeout < GLFW_INFINITY )
    {
        // Oooops! Not implemented properly yet!
        ReleaseSemaphore( (struct SignalSemaphore *) mutex );
        Delay( 1 );
        ObtainSemaphore( (struct SignalSemaphore *) mutex );
        return;
    }

    // Find pointer to this threads structure
    amiTask = FindTask( NULL );
    t_this  = (_GLFWthread *) amiTask->tc_UserData;

    // Store information in our thread structure that we want to wait for
    // the specified condition variable to be signaled
    ENTER_THREAD_CRITICAL_SECTION
    t_this->WaitFor = (void *) cond;
    LEAVE_THREAD_CRITICAL_SECTION

    // Release the mutex
    ReleaseSemaphore( (struct SignalSemaphore *) mutex );

    // Wait for condition variable
    Wait( 1L<<(t_this->WaitSig) );

    // Reacquire the mutex
    ObtainSemaphore( (struct SignalSemaphore *) mutex );
}


//========================================================================
// _glfwPlatformSignalCond() - Signal a condition to one waiting thread
//========================================================================

void _glfwPlatformSignalCond( GLFWcond cond )
{
    _GLFWthread *t_wait;

    // Broadcast condition to one waiting thread
    ENTER_THREAD_CRITICAL_SECTION
    for( t_wait = &_glfwThrd.First; t_wait; t_wait = t_wait->Next )
    {
        if( t_wait->WaitFor == (void *) cond )
        {
            Signal( t_wait->AmiTask, 1L<<t_wait->WaitSig );
            t_wait->WaitFor = NULL;
            break;
        }
    }
    LEAVE_THREAD_CRITICAL_SECTION
}


//========================================================================
// _glfwPlatformBroadcastCond() - Broadcast a condition to all waiting
// threads
//========================================================================

void _glfwPlatformBroadcastCond( GLFWcond cond )
{
    _GLFWthread *t_wait;

    // Broadcast condition to any waiting threads
    ENTER_THREAD_CRITICAL_SECTION
    for( t_wait = &_glfwThrd.First; t_wait; t_wait = t_wait->Next )
    {
        if( t_wait->WaitFor == (void *) cond )
        {
            Signal( t_wait->AmiTask, 1L<<t_wait->WaitSig );
            t_wait->WaitFor = NULL;
        }
    }
    LEAVE_THREAD_CRITICAL_SECTION
}


//========================================================================
// _glfwPlatformGetNumberOfProcessors() - Return the number of processors
// in the system.
//========================================================================

int _glfwPlatformGetNumberOfProcessors( void )
{
    // Return number of processors online (MorphOS has SMP support, so we
    // should do something useful here...)
    return 1;
}
