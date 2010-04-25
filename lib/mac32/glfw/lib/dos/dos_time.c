//========================================================================
// GLFW - An OpenGL framework
// File:        dos_time.c
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


// We use the __i386 define later in the code. Check if there are any
// other defines that hint that we are compiling for 32-bit x86.
#ifndef __i386
 #if defined(__i386__) || defined(i386) || defined(X86) || defined(_M_IX86)
  #define __i386
 #endif
#endif // __i386

// Should we use inline x86 assembler?
#if defined(__i386) && defined(__GNUC__)
 #define _USE_X86_ASM
#endif



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

// Functions for accessing upper and lower parts of 64-bit integers
// (Note: These are endian dependent, but ONLY used on x86 platforms!)
#define _HIGH(x) ((unsigned int*)&x)[1]
#define _LOW(x)  *((unsigned int*)&x)


//========================================================================
// _glfwCPUID() - Execute x86 CPUID instruction
//========================================================================

#ifdef _USE_X86_ASM

static int _glfwCPUID( unsigned int ID, unsigned int *a, unsigned int *b,
    unsigned int *c, unsigned int *d )
{
    int has_cpuid;
    unsigned int local_a, local_b, local_c, local_d;

    // Inline assembly - GCC version
#if defined(__i386) && defined(__GNUC__)

    // Detect CPUID support
    asm(
        "pushf\n\t"
        "pop    %%eax\n\t"
        "movl   %%eax,%%ebx\n\t"
        "xorl   $0x00200000,%%eax\n\t"
        "push   %%eax\n\t"
        "popf\n\t"
        "pushf\n\t"
        "pop    %%eax\n\t"
        "xorl   %%eax,%%ebx\n\t"
        "movl   %%eax,%0\n\t"
        : "=m" (has_cpuid)
        :
        : "%eax", "%ebx"
    );
    if( !has_cpuid )
    {
        return GL_FALSE;
    }

    // Execute CPUID
    asm(
        "movl   %4,%%eax\n\t"
        "cpuid\n\t"
        "movl   %%eax,%0\n\t"
        "movl   %%ebx,%1\n\t"
        "movl   %%ecx,%2\n\t"
        "movl   %%edx,%3\n\t"
        : "=m" (local_a), "=m" (local_b), "=m" (local_c), "=m" (local_d)
        : "m" (ID)
        : "%eax", "%ebx", "%ecx", "%edx"
    );

#endif

    // Common code for all compilers
    *a = local_a;
    *b = local_b;
    *c = local_c;
    *d = local_d;
    return GL_TRUE;
}

#endif // _USE_X86_ASM


//========================================================================
// _glfwHasRDTSC() - Check for RDTSC availability AND usefulness
//========================================================================

static int _glfwHasRDTSC( void )
{
#ifdef _USE_X86_ASM

    unsigned int cpu_name1, cpu_name2, cpu_name3;
    unsigned int cpu_signature, cpu_brandID;
    unsigned int max_base, feature_flags;
    unsigned int dummy;

    // Get processor vendor string (will return 0 if CPUID is not
    // supported)
    if( !_glfwCPUID( 0, &max_base, &cpu_name1, &cpu_name3, &cpu_name2 ) )
    {
        return GL_FALSE;
    }

    // Does the processor support base CPUID function 1?
    if( max_base < 1 )
    {
        return GL_FALSE;
    }

    // Get CPU capabilities, CPU Brand ID & CPU Signature
    _glfwCPUID( 1, &cpu_signature, &cpu_brandID, &dummy, &feature_flags );

    // Is RDTSC supported?
    if( !(feature_flags & 0x00000010) )
    {
        return GL_FALSE;
    }

    return GL_TRUE;

#else

    // Not a supported compiler
    return GL_FALSE;

#endif
}


//------------------------------------------------------------------------
// _RDTSC() - Get CPU cycle count using the RDTSC instruction
//------------------------------------------------------------------------

#if defined(__i386) && defined(__GNUC__)

// Read 64-bit processor Time Stamp Counter - GCC version
#define _RDTSC( hi, lo ) \
    asm( \
        "rdtsc\n\t" \
        "movl %%edx,%0\n\t" \
        "movl %%eax,%1" \
        : "=m" (hi), "=m" (lo) \
        :  \
        : "%edx", "%eax" \
    );

#else

#define _RDTSC( hi, lo ) {hi=lo=0;}

#endif




//========================================================================
// _glfwInitTimer() - Initialize timer
//========================================================================

int _glfwInitTimer( void )
{
    clock_t   t, t1, t2;
    long long c1, c2;

    // Do we have RDTSC?
    _glfwTimer.HasRDTSC = _glfwHasRDTSC();
    if( _glfwTimer.HasRDTSC )
    {
        // Measure the CPU clock with the raw DOS clock (18.2 Hz)
        t = clock();
        while( (t1=clock()) == t );
        _RDTSC( _HIGH(c1), _LOW(c1) );
        t = t1+CLOCKS_PER_SEC/3;
        while( (t2=clock()) < t );
        _RDTSC( _HIGH(c2), _LOW(c2) );

        // Calculate CPU clock period
        _glfwTimer.Period = (double)(t2-t1) /
                            (CLOCKS_PER_SEC * (double)(c2-c1));
        _RDTSC( _HIGH(_glfwTimer.t0), _LOW(_glfwTimer.t0) );
    }
    else
    {
        // Use the raw DOS clock (18.2 Hz)
        _glfwTimer.Period = 1.0 / CLOCKS_PER_SEC;
        _glfwTimer.t0 = clock();
    }

    return 1;
}


//========================================================================
// _glfwTerminateTimer() - Terminate timer
//========================================================================

void _glfwTerminateTimer( void )
{
    // Nothing to do here
}


//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetTime() - Return timer value in seconds
//========================================================================

double _glfwPlatformGetTime( void )
{
    long long t_now;

    // Get current clock count
    if( _glfwTimer.HasRDTSC )
    {
        _RDTSC( _HIGH(t_now), _LOW(t_now) );
    }
    else
    {
        t_now = (long long) clock();
    }

    // Convert to seconds
    return (t_now-_glfwTimer.t0) * _glfwTimer.Period;
}


//========================================================================
// _glfwPlatformSetTime() - Set timer value in seconds
//========================================================================

void _glfwPlatformSetTime( double t )
{
    long long t_now;

    // Get current clock count
    if( _glfwTimer.HasRDTSC )
    {
        _RDTSC( _HIGH(t_now), _LOW(t_now) );
    }
    else
    {
        t_now = (long long) clock();
    }

    // Set timer
    _glfwTimer.t0 = t_now - (long long)(t/_glfwTimer.Period);
}


//========================================================================
// _glfwPlatformSleep() - Put a thread to sleep for a specified amount of
// time
//========================================================================

void _glfwPlatformSleep( double time )
{
    // TODO: Proper threaded version
    if( time > 0 )
    {
        if( time < 0.001 )
        {
            delay( 1 );
        }
        else
        {
            delay( (unsigned int)(time*1000.0+0.5) );
        }
    }
}
