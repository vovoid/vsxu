//========================================================================
// GLFW - An OpenGL framework
// File:        dos_window.c
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
// _glfwRedirectOutput() - Standard output redirection
//========================================================================

static void _glfwRedirectOutput( void )
{
    // Generate temporary names
    tmpnam( _glfwWin.OutName );
    tmpnam( _glfwWin.ErrName );

    // Open temporary output files
    _glfwWin.hOut = open( _glfwWin.OutName, O_WRONLY | O_CREAT | O_TEXT |
                                            O_TRUNC, S_IREAD | S_IWRITE );
    _glfwWin.hErr = open( _glfwWin.ErrName, O_WRONLY | O_CREAT | O_TEXT |
                                            O_TRUNC, S_IREAD | S_IWRITE );

    // Redirect stdout
    if( _glfwWin.hOut > 0 )
    {
        _glfwWin.hOutOld = dup( STDOUT_FILENO );
        fflush( stdout );
        dup2( _glfwWin.hOut, STDOUT_FILENO );
    }

    // Redirect stderr
    if( _glfwWin.hErr > 0 )
    {
        _glfwWin.hErrOld = dup( STDERR_FILENO );
        fflush( stderr );
        dup2( _glfwWin.hErr, STDERR_FILENO );
    }
}


//========================================================================
// _glfwRestoreOutput() - Standard output redirection
//========================================================================

static void _glfwRestoreOutput( void )
{
    FILE *f;
    char *str = alloca( 512 );

    // Dump from temporary file to stdout
    if( _glfwWin.hOut > 0)
    {
        // Restore old stdout
        dup2( _glfwWin.hOutOld, STDOUT_FILENO );
        close( _glfwWin.hOut );
        close( _glfwWin.hOutOld );
        _glfwWin.hOut = 0;

        // Dump file to stdout
        f = fopen( _glfwWin.OutName, "rt" );
        while( fgets( str, 512, f ) )
        {
            fputs( str, stdout );
        }
        fclose( f );

        // Remove temporary file
        remove( _glfwWin.OutName );
     }

    // Dump from temporary file to stderr
    if( _glfwWin.hOut > 0)
    {
        // Restore old stderr
        dup2( _glfwWin.hErrOld, STDERR_FILENO );
        close( _glfwWin.hErr );
        close( _glfwWin.hErrOld );
        _glfwWin.hErr = 0;

        // Dump file to stderr
        f = fopen( _glfwWin.ErrName, "rt" );
        while( fgets( str, 512, f ) )
        {
            fputs( str, stderr );
        }
        fclose( f );

        // Remove temporary file
        remove( _glfwWin.ErrName );
     }
}


//========================================================================
// _glfwTranslateChar() - Translates a DOS key code to Unicode
//========================================================================

static int _glfwTranslateChar( int keycode )
{
    // Unicode?
    if( (keycode >= 32 && keycode <= 126) || keycode >= 160 )
    {
        return keycode;
    }

    return -1;
}


//========================================================================
// _glfwTranslateKey() - Translates a DOS key code to GLFW coding
//========================================================================

static int _glfwTranslateKey( int keycode )
{
    // ISO 8859-1?
    if( ((keycode>=32) && (keycode<=126)) ||
        ((keycode>=160) && (keycode<=254)) )
    {
        return keycode;
    }

    // Special keys?
    if( keycode < 0 )
    {
        return -keycode;
    }

    return -1;
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformOpenWindow() - Here is where the window is created, and
// the OpenGL rendering context is created
//========================================================================

int _glfwPlatformOpenWindow( int width, int height, int redbits,
    int greenbits, int bluebits, int alphabits, int depthbits,
    int stencilbits, int mode, int accumredbits, int accumgreenbits,
    int accumbluebits, int accumalphabits, int auxbuffers, int stereo,
    int refreshrate )
{
    GLint params[2];

    // Clear window resources
    _glfwWin.Visual  = NULL;
    _glfwWin.Context = NULL;
    _glfwWin.Buffer  = NULL;
    _glfwWin.hOut    = 0;
    _glfwWin.hErr    = 0;

    // For now, we only support 640x480, 800x600 and 1024x768
    if( (width*height) < (700*500) )
    {
        width  = 640;
        height = 480;
    }
    else if( (width*height) < (900*700) )
    {
        width  = 800;
        height = 600;
    }
    else
    {
        width  = 1024;
        height = 768;
    }

    // For now, we only support 5,6,5 and 8,8,8 color formats
    if( (redbits+greenbits+bluebits) < 20 )
    {
        redbits   = 5;
        greenbits = 6;
        bluebits  = 5;
    }
    else
    {
        redbits   = 8;
        greenbits = 8;
        bluebits  = 8;
    }

    // For now, we always set refresh rate = 0 (default)
    refreshrate = 0;

    // stdout/stderr redirection
    _glfwRedirectOutput();

    // Create visual
    _glfwWin.Visual = DMesaCreateVisual(
                        width, height,
                        redbits+greenbits+bluebits,
                        refreshrate,
                        GL_TRUE,                    // Double buffer
                        GL_TRUE,                    // RGB mode
                        alphabits?GL_TRUE:GL_FALSE, // Alpha buffer?
                        depthbits,
                        stencilbits,
                        (accumredbits+accumgreenbits+
                         accumbluebits+accumalphabits)>>2
                      );
    if( _glfwWin.Visual == NULL )
    {
        printf("Unable to create visual\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Create context
    _glfwWin.Context = DMesaCreateContext( _glfwWin.Visual, NULL );
    if( _glfwWin.Context == NULL )
    {
        printf("Unable to create context\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Create buffer
    _glfwWin.Buffer = DMesaCreateBuffer( _glfwWin.Visual, 0, 0,
                                         width, height );
    if( _glfwWin.Buffer == NULL )
    {
        printf("Unable to create buffer\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Make current context
    if( !DMesaMakeCurrent( _glfwWin.Context, _glfwWin.Buffer ) )
    {
        printf("Unable to make current context\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Start DOS event handler
    if( !_glfwInitEvents() )
    {
        printf("Unable to start event handler\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Start keyboard handler
    if( !_glfwInitKeyboard() )
    {
        printf("Unable to start keyboard driver\n");
        _glfwPlatformCloseWindow();
        return GL_FALSE;
    }

    // Start mouse handler
    if( !_glfwInitMouse() )
    {
        printf("***Warning: Unable to start mouse driver\n");
    }

    // Remember actual screen/window size
    _glfwWin.Width  = width;
    _glfwWin.Height = height;

    return GL_TRUE;
}


//========================================================================
// _glfwPlatformCloseWindow() - Properly kill the window/video display
//========================================================================

void _glfwPlatformCloseWindow( void )
{
    // Terminate mouse handler
    _glfwTerminateMouse();

    // Terminate keyboard handler
    _glfwTerminateKeyboard();

    // Terminate event handler
    _glfwTerminateEvents();

    // Destroy buffer
    if( _glfwWin.Buffer != NULL )
    {
        DMesaDestroyBuffer( _glfwWin.Buffer );
        _glfwWin.Buffer = NULL;
    }

    // Destroy context
    if( _glfwWin.Context != NULL )
    {
        DMesaDestroyContext( _glfwWin.Context );
        _glfwWin.Context = NULL;
    }

    // Destroy visual
    if( _glfwWin.Visual != NULL )
    {
        DMesaDestroyVisual( _glfwWin.Visual );
        _glfwWin.Visual = NULL;
    }

    // stdout/stderr redirection
    _glfwRestoreOutput();
}


//========================================================================
// _glfwPlatformSetWindowTitle() - Set the window title.
//========================================================================

void _glfwPlatformSetWindowTitle( const char *title )
{
    // Nothing to do here...
}


//========================================================================
// _glfwPlatformSetWindowSize() - Set the window size.
//========================================================================

void _glfwPlatformSetWindowSize( int width, int height )
{
    // TODO
}


//========================================================================
// _glfwPlatformSetWindowPos() - Set the window position.
//========================================================================

void _glfwPlatformSetWindowPos( int x, int y )
{
    // Nothing to do here...
}


//========================================================================
// _glfwPlatformIconfyWindow() - Window iconification
//========================================================================

void _glfwPlatformIconifyWindow( void )
{
    // Nothing to do here...
}


//========================================================================
// _glfwPlatformRestoreWindow() - Window un-iconification
//========================================================================

void _glfwPlatformRestoreWindow( void )
{
    // Nothing to do here...
}


//========================================================================
// _glfwPlatformSwapBuffers() - Swap buffers (double-buffering) and poll
// any new events.
//========================================================================

void _glfwPlatformSwapBuffers( void )
{
    DMesaSwapBuffers( _glfwWin.Buffer );
}


//========================================================================
// _glfwPlatformSwapInterval() - Set double buffering swap interval
//========================================================================

void _glfwPlatformSwapInterval( int interval )
{
    // TODO
}


//========================================================================
// _glfwPlatformRefreshWindowParams()
//========================================================================

void _glfwPlatformRefreshWindowParams( void )
{
    GLint     x;
    GLboolean b;

    // Fill out information
    _glfwWin.Accelerated    = GL_TRUE;
    glGetIntegerv( GL_RED_BITS, &x );
    _glfwWin.RedBits        = x;
    glGetIntegerv( GL_GREEN_BITS, &x );
    _glfwWin.GreenBits      = x;
    glGetIntegerv( GL_BLUE_BITS, &x );
    _glfwWin.BlueBits       = x;
    glGetIntegerv( GL_ALPHA_BITS, &x );
    _glfwWin.AlphaBits      = x;
    glGetIntegerv( GL_DEPTH_BITS, &x );
    _glfwWin.DepthBits      = x;
    glGetIntegerv( GL_STENCIL_BITS, &x );
    _glfwWin.StencilBits    = x;
    glGetIntegerv( GL_ACCUM_RED_BITS, &x );
    _glfwWin.AccumRedBits   = x;
    glGetIntegerv( GL_ACCUM_GREEN_BITS, &x );
    _glfwWin.AccumGreenBits = x;
    glGetIntegerv( GL_ACCUM_BLUE_BITS, &x );
    _glfwWin.AccumBlueBits  = x;
    glGetIntegerv( GL_ACCUM_ALPHA_BITS, &x );
    _glfwWin.AccumAlphaBits = x;
    glGetIntegerv( GL_AUX_BUFFERS, &x );
    _glfwWin.AuxBuffers     = x;
    glGetBooleanv( GL_AUX_BUFFERS, &b );
    _glfwWin.Stereo         = b ? GL_TRUE : GL_FALSE;
    _glfwWin.RefreshRate    = 0;
}


//========================================================================
// _glfwPlatformPollEvents() - Poll for new window and input events
//========================================================================

void _glfwPlatformPollEvents( void )
{
    _GLFWdosevent event;
    struct key_event         *key;
    struct mousemove_event   *mousemove;
    struct mousewheel_event  *mousewheel;
    struct mousebutton_event *mousebutton;

    // Empty the event queue
    while( _glfwGetNextEvent( &event ) )
    {
        switch( event.Type )
        {
        // Keyboard event?
        case _GLFW_DOS_KEY_EVENT:
            key = &event.Key;
            _glfwInputKey( _glfwTranslateKey( key->KeyNoMod ),
                           key->Action );
            _glfwInputChar( _glfwTranslateChar( key->Key ),
                            key->Action );
            break;

        // Mouse move event?
        case _GLFW_DOS_MOUSE_MOVE_EVENT:
            mousemove = &event.MouseMove;
            _glfwInput.MousePosX += mousemove->DeltaX;
            _glfwInput.MousePosY += mousemove->DeltaY;

            // Call user callback function
            if( _glfwWin.MousePosCallback )
            {
                _glfwWin.MousePosCallback( _glfwInput.MousePosX,
                                           _glfwInput.MousePosY );
            }
            break;

        // Mouse wheel event?
        case _GLFW_DOS_MOUSE_WHEEL_EVENT:
            mousewheel = &event.MouseWheel;
            _glfwInput.WheelPos += mousewheel->WheelDelta;

            // Call user callback function
            if( _glfwWin.MouseWheelCallback )
            {
                _glfwWin.MouseWheelCallback( _glfwInput.WheelPos );
            }
            break;

        // Mouse button event?
        case _GLFW_DOS_MOUSE_BUTTON_EVENT:
            mousebutton = &event.MouseButton;
            _glfwInputMouseClick( mousebutton->Button,
                                  mousebutton->Action );
            break;

        default:
            break;
        }

    }
}


//========================================================================
// _glfwPlatformWaitEvents() - Wait for new window and input events
//========================================================================

void _glfwPlatformWaitEvents( void )
{
    // Wait for new events
    _glfwWaitNextEvent;

    // Poll new events
    _glfwPlatformPollEvents();
}


//========================================================================
// _glfwPlatformHideMouseCursor() - Hide mouse cursor (lock it)
//========================================================================

void _glfwPlatformHideMouseCursor( void )
{
    // TODO
}


//========================================================================
// _glfwPlatformShowMouseCursor() - Show mouse cursor (unlock it)
//========================================================================

void _glfwPlatformShowMouseCursor( void )
{
    // TODO
}


//========================================================================
// _glfwPlatformSetMouseCursorPos() - Set physical mouse cursor position
//========================================================================

void _glfwPlatformSetMouseCursorPos( int x, int y )
{
    // TODO
}
