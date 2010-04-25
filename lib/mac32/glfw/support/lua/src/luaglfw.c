//========================================================================
// GLFW - An OpenGL framework
// File:        luaglfw.c
// Platform:    Lua 5.0
// API version: 2.5
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2005 Camilla Berglund
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

#include <lauxlib.h>
#include <GL/glfw.h>
#include "luaglfw.h"


//************************************************************************
//****                   Internal type definitions                    ****
//************************************************************************

struct lua_constant
{
    char *str;
    int  value;
};


//************************************************************************
//****                        Handy functions                         ****
//************************************************************************

// Check the number of arguments on the Lua stack
static int badArgs( lua_State *L, int n, char const *name )
{
    if( lua_gettop(L) != n )
    {
        lua_settop( L, 0 );
        lua_pushstring( L, "Bad arguments passed to function: " );
        lua_pushstring( L, name );
        lua_concat( L, 2 );
        lua_error( L );
        return 1;
    }
    return 0;
}

// Get a numeric value from a Lua table
static void getNumber( lua_State *L, int index, char *key, lua_Number *num )
{
    lua_pushstring( L, key );
    if( index < 0 ) index --;
    lua_rawget( L, index );
    if( !lua_isnil( L, -1 ) )
    {
        *num = lua_tonumber( L, -1 );
    }
    lua_remove( L, -1 );
}

// Add constants to the table on top of the stack
static void addConstants( lua_State *L, struct lua_constant *cn )
{
    while( cn->str )
    {
        lua_pushstring( L, cn->str );
        lua_pushnumber( L, cn->value );
        lua_rawset( L, -3 );
        ++ cn;
    }
}

// Report error for unsupported functions
static void unsupportedFunction( lua_State *L, char const *name )
{
    lua_settop( L, 0 );
    lua_pushstring( L, "Unsupported function: " );
    lua_pushstring( L, name );
    lua_concat( L, 2 );
    lua_error( L );
}



//************************************************************************
//***                   Callback wrapper functions                    ****
//************************************************************************

static lua_State * callback_lua_state = (lua_State *) 0;

static const char * windowsize_name;
static const char * windowclose_name;
static const char * windowrefresh_name;
static const char * mousebutton_name;
static const char * mousepos_name;
static const char * mousewheel_name;
static const char * key_name;
static const char * char_name;


void GLFWCALL luaglfw_windowsizefun( int w, int h )
{
    lua_State *L = callback_lua_state;
    if( L == NULL ) return;

    lua_getglobal( L, windowsize_name );
    if( lua_isfunction( L, -1 ) )
    {
        lua_pushnumber( L, (lua_Number)w );
        lua_pushnumber( L, (lua_Number)h );
        lua_pcall( L, 2, 0, 0 );
    }
}

int GLFWCALL luaglfw_windowclosefun( void )
{
    lua_State *L = callback_lua_state;
    int do_close = 1;
    if( L == NULL ) return 1;

    lua_getglobal( L, windowclose_name );
    if( lua_isfunction( L, -1 ) )
    {
        lua_pcall( L, 0, 1, 0 );
        do_close = (int) lua_tonumber( L, -1 );
        lua_pop( L, 1 );
    }
    return do_close;
}

void GLFWCALL luaglfw_windowrefreshfun( void )
{
    lua_State *L = callback_lua_state;
    if( L == NULL ) return;

    lua_getglobal( L, windowrefresh_name );
    if( lua_isfunction( L, -1 ) )
    {
        lua_pcall( L, 0, 0, 0 );
    }
}

void GLFWCALL luaglfw_mousebuttonfun( int button, int action )
{
    lua_State *L = callback_lua_state;
    if( L == NULL ) return;

    lua_getglobal( L, mousebutton_name );
    if( lua_isfunction( L, -1 ) )
    {
        lua_pushnumber( L, (lua_Number)button );
        lua_pushnumber( L, (lua_Number)action );
        lua_pcall( L, 2, 0, 0 );
    }
}

void GLFWCALL luaglfw_mouseposfun( int x, int y )
{
    lua_State *L = callback_lua_state;
    if( L == NULL ) return;

    lua_getglobal( L, mousepos_name );
    if( lua_isfunction( L, -1 ) )
    {
        lua_pushnumber( L, (lua_Number)x );
        lua_pushnumber( L, (lua_Number)y );
        lua_pcall( L, 2, 0, 0 );
    }
}

void GLFWCALL luaglfw_mousewheelfun( int wheelpos )
{
    lua_State *L = callback_lua_state;
    if( L == NULL ) return;

    lua_getglobal( L, mousewheel_name );
    if( lua_isfunction( L, -1 ) )
    {
        lua_pushnumber( L, (lua_Number)wheelpos );
        lua_pcall( L, 1, 0, 0 );
    }
}

void GLFWCALL luaglfw_keyfun( int key, int action )
{
    lua_State *L = callback_lua_state;
    if( L == NULL ) return;

    lua_getglobal( L, key_name );
    if( lua_isfunction( L, -1 ) )
    {
        lua_pushnumber( L, (lua_Number)key );
        lua_pushnumber( L, (lua_Number)action );
        lua_pcall( L, 2, 0, 0 );
    }
}

void GLFWCALL luaglfw_charfun( int key, int action )
{
    lua_State *L = callback_lua_state;
    if( L == NULL ) return;

    lua_getglobal( L, char_name );
    if( lua_isfunction( L, -1 ) )
    {
        lua_pushnumber( L, (lua_Number)key );
        lua_pushnumber( L, (lua_Number)action );
        lua_pcall( L, 2, 0, 0 );
    }
}



//************************************************************************
//****                      GLFW proxy functions                      ****
//************************************************************************


//========================================================================
// Init/termination & version info
//========================================================================

static int glfw_Init( lua_State *L )
{
    int res;
    lua_settop( L, 0 );
    res = glfwInit();
    lua_pushnumber( L, (lua_Number)res );
    return 1;
}

static int glfw_Terminate( lua_State *L )
{
    glfwTerminate();
    return 0;
}

static int glfw_GetVersion( lua_State *L )
{
    int major, minor, rev;
    glfwGetVersion( &major, &minor, &rev );
    lua_pushnumber( L, major );
    lua_pushnumber( L, minor );
    lua_pushnumber( L, rev );
    return 3;
}


//========================================================================
// Window handling
//========================================================================

static int glfw_OpenWindow( lua_State *L )
{
    lua_Number  w, h, r, g, b, a, depth, stencil, mode;
    int         argc, res;

    // Check arguments
    if( badArgs( L, 9, "OpenWindow" ) ) return 0;

    // Get all arguments to glfwOpenWindow
    w       = lua_tonumber( L, 1 );
    h       = lua_tonumber( L, 2 );
    r       = lua_tonumber( L, 3 );
    g       = lua_tonumber( L, 4 );
    b       = lua_tonumber( L, 5 );
    a       = lua_tonumber( L, 6 );
    depth   = lua_tonumber( L, 7 );
    stencil = lua_tonumber( L, 8 );
    mode    = lua_tonumber( L, 9 );

    // Call glfwOpenWindow
    lua_settop( L,0 );
    res = glfwOpenWindow( (int)w, (int)h, (int)r, (int)g, (int)b,
                          (int)a, (int)depth, (int)stencil,
                          (int)mode );

    // Return result
    lua_pushnumber( L, (lua_Number)res );
    return 1;
}

static int glfw_OpenWindowHint( lua_State *L )
{
    lua_Number target, hint;
    if( badArgs( L, 2, "OpenWindowHint" ) ) return 0;
    target = lua_tonumber( L, 1 );
    hint   = lua_tonumber( L, 2 );
    lua_settop( L, 0 );
    glfwOpenWindowHint( (int)target, (int)hint );
    return 0;
}

static int glfw_CloseWindow( lua_State *L )
{
    glfwCloseWindow();
    return 0;
}

static int glfw_SetWindowTitle( lua_State *L )
{
    const char *str;
    if( badArgs( L, 1, "SetWindowTitle" ) ) return 0;
    str = lua_tostring( L, 1 );
    lua_remove( L, 1 );
    glfwSetWindowTitle( str );
    return 0;
}

static int glfw_SetWindowSize( lua_State *L )
{
    lua_Number w, h;
    if( badArgs( L, 2, "SetWindowSize" ) ) return 0;
    w = lua_tonumber( L, 1 );
    h = lua_tonumber( L, 2 );
    lua_settop( L, 0 );
    glfwSetWindowSize( (int)w, (int)h );
    return 0;
}

static int glfw_GetWindowSize( lua_State *L )
{
    int w, h;
    glfwGetWindowSize( &w, &h );
    lua_settop( L, 0 );
    lua_pushnumber( L, (lua_Number)w );
    lua_pushnumber( L, (lua_Number)h );
    return 2;
}

static int glfw_SetWindowPos( lua_State *L )
{
    lua_Number x, y;
    if( badArgs( L, 2, "SetWindowPos" ) ) return 0;
    x = lua_tonumber( L, 1 );
    y = lua_tonumber( L, 2 );
    lua_settop( L, 0 );
    glfwSetWindowPos( (int)x, (int)y );
    return 0;
}

static int glfw_IconifyWindow( lua_State *L )
{
    glfwIconifyWindow();
    return 0;
}

static int glfw_RestoreWindow( lua_State *L )
{
    glfwRestoreWindow();
    return 0;
}

static int glfw_GetWindowParam( lua_State *L )
{
    lua_Number n;
    if( badArgs( L, 1, "GetWindowParam" ) ) return 0;
    n = lua_tonumber( L, 1 );
    lua_settop( L, 0 );
    n = glfwGetWindowParam( (int)n );
    lua_pushnumber( L, n );
    return 1;
}



//========================================================================
// Buffer swapping
//========================================================================

static int glfw_SwapBuffers( lua_State *L )
{
    glfwSwapBuffers();
    return 0;
}

static int glfw_SwapInterval( lua_State *L )
{
    lua_Number interval;
    if( badArgs( L, 1, "SwapInterval" ) ) return 0;
    interval = lua_tonumber( L, 1 );
    lua_remove( L, 1 );
    glfwSwapInterval( (int)interval );
    return 0;
}


//========================================================================
// Video modes
//========================================================================

static int glfw_GetVideoModes( lua_State *L )
{
    // !!TODO!!
    unsupportedFunction( L, "GetVideoModes" );
    return 0;
}

static int glfw_GetDesktopMode( lua_State *L )
{
    GLFWvidmode mode;
    glfwGetDesktopMode( &mode );

    lua_newtable( L );
    lua_pushstring( L, "Width" );
    lua_pushnumber( L, mode.Width );
    lua_rawset( L, -3 );
    lua_pushstring( L, "Height" );
    lua_pushnumber( L, mode.Height );
    lua_rawset( L, -3 );

    return 1;
}


//========================================================================
// Event polling/waiting
//========================================================================

static int glfw_PollEvents( lua_State *L )
{
    glfwPollEvents();
    return 0;
}

static int glfw_WaitEvents( lua_State *L )
{
    glfwWaitEvents();
    return 0;
}


//========================================================================
// Key and mouse input
//========================================================================

static int glfw_GetKey( lua_State *L )
{
    int n = 0;

    if( badArgs( L, 1, "GetKey" ) ) return 0;

    switch( lua_type( L, 1 ) )
    {
        case LUA_TSTRING:
            n = (lua_tostring(L,1)[0]);
            break;

        case LUA_TNUMBER:
            n = lua_tonumber(L,1);
            break;
    }

    lua_settop( L, 0 );
    n = glfwGetKey( n );
    lua_pushnumber( L, n );
    return 1;
}

static int glfw_GetMouseButton( lua_State *L )
{
    lua_Number n;
    if( badArgs( L, 1, "GetMouseButton" ) ) return 0;
    n = lua_tonumber( L, 1 );
    lua_settop( L, 0 );
    lua_pushnumber( L, glfwGetMouseButton( (int)n ) );
    return 1;
}

static int glfw_GetMousePos( lua_State *L )
{
    int x, y;
    glfwGetMousePos( &x, &y );
    lua_settop( L, 0 );
    lua_pushnumber( L, (lua_Number)x );
    lua_pushnumber( L, (lua_Number)y );
    return 2;
}

static int glfw_SetMousePos( lua_State *L )
{
    lua_Number x, y;
    if( badArgs( L, 2, "SetMousePos" ) ) return 0;
    x = lua_tonumber( L, 1 );
    y = lua_tonumber( L, 2 );
    lua_settop( L, 0 );
    glfwSetMousePos( (int)x, (int)y );
    return 0;
}

static int glfw_GetMouseWheel( lua_State *L )
{
    int pos;
    pos = glfwGetMouseWheel();
    lua_settop( L, 0 );
    lua_pushnumber( L, (lua_Number)pos );
    return 1;
}

static int glfw_SetMouseWheel( lua_State *L )
{
    lua_Number pos;
    if( badArgs( L, 1, "SetMouseWheel" ) ) return 0;
    pos = lua_tonumber( L, 1 );
    lua_settop( L, 0 );
    glfwSetMouseWheel( (int)pos );
    return 0;
}


//========================================================================
// Joystick input
//========================================================================

static int glfw_GetJoystickParam( lua_State *L )
{
    lua_Number joy, param;
    int        res;
    if( badArgs( L, 2, "GetJoystickParam" ) ) return 0;
    joy   = lua_tonumber( L, 1 );
    param = lua_tonumber( L, 2 );
    lua_settop( L, 0 );
    res = glfwGetJoystickParam( (int)joy, (int)param );
    lua_pushnumber( L, (lua_Number)res );
    return 1;
}

#define LUAGLFW_MAX_JOY_AXES 256

static int glfw_GetJoystickPos( lua_State *L )
{
    int   joy, numaxes, res, i;
    float pos[ LUAGLFW_MAX_JOY_AXES ];

    // Get arguments
    if( badArgs( L, 2, "GetJoystickPos" ) ) return 0;
    joy     = (int)lua_tonumber( L, 1 );
    numaxes = (int)lua_tonumber( L, 2 );
    lua_settop( L, 0 );
    if( numaxes < 1 ) return 0;
    if( numaxes > LUAGLFW_MAX_JOY_AXES ) numaxes = LUAGLFW_MAX_JOY_AXES;

    // Call GLFW funciton
    res = glfwGetJoystickPos( joy, pos, numaxes );

    // Create result array
    lua_newtable( L );
    for( i = 0; i < res; ++ i )
    {
        lua_pushnumber( L, (lua_Number)(i+1) );
        lua_pushnumber( L, pos[ i ] );
        lua_rawset( L, -3 );
    }

    return 1;
}

#define LUAGLFW_MAX_JOY_BUTTONS 256

static int glfw_GetJoystickButtons( lua_State *L )
{
    int           joy, numbuttons, res, i;
    unsigned char buttons[ LUAGLFW_MAX_JOY_AXES ];

    // Get arguments
    if( badArgs( L, 2, "GetJoystickButtons" ) ) return 0;
    joy        = (int)lua_tonumber( L, 1 );
    numbuttons = (int)lua_tonumber( L, 2 );
    lua_settop( L, 0 );
    if( numbuttons < 1 ) return 0;
    if( numbuttons > LUAGLFW_MAX_JOY_BUTTONS )
        numbuttons = LUAGLFW_MAX_JOY_BUTTONS;

    // Call GLFW funciton
    res = glfwGetJoystickButtons( joy, buttons, numbuttons );

    // Create result array
    lua_newtable( L );
    for( i = 0; i < res; ++ i )
    {
        lua_pushnumber( L, (lua_Number)(i+1) );
        lua_pushnumber( L, (lua_Number)buttons[ i ] );
        lua_rawset( L, -3 );
    }

    return 1;
}


//========================================================================
// Timing
//========================================================================

static int glfw_GetTime( lua_State *L )
{
    lua_Number t;
    t = glfwGetTime();
    lua_settop( L, 0 );
    lua_pushnumber( L, t );
    return 1;
}

static int glfw_SetTime( lua_State *L )
{
    lua_Number t;
    if ( badArgs( L, 1, "SetTime" ) ) return 0;
    t = lua_tonumber( L, 1 );
    lua_settop( L, 0 );
    glfwSetTime( (double)t );
    return 0;
}

static int glfw_Sleep( lua_State *L )
{
    lua_Number t;
    if ( badArgs( L, 1, "Sleep" ) ) return 0;
    t = lua_tonumber( L, 1 );
    lua_settop( L, 0 );
    glfwSleep( (double)t );
    return 0;
}


//========================================================================
// Threading support (not possible in Lua)
//========================================================================

static int glfw_CreateThread( lua_State *L )
{
    unsupportedFunction( L, "CreateThread" );
    return 0;
}

static int glfw_DestroyThread( lua_State *L )
{
    unsupportedFunction( L, "DestroyThread" );
    return 0;
}

static int glfw_WaitThread( lua_State *L )
{
    unsupportedFunction( L, "WaitThread" );
    return 0;
}

static int glfw_GetThreadID( lua_State *L )
{
    unsupportedFunction( L, "GetThreadID" );
    return 0;
}

static int glfw_CreateMutex( lua_State *L )
{
    unsupportedFunction( L, "CreateMutex" );
    return 0;
}

static int glfw_DestroyMutex( lua_State *L )
{
    unsupportedFunction( L, "DestroyMutex" );
    return 0;
}

static int glfw_LockMutex( lua_State *L )
{
    unsupportedFunction( L, "LockMutex" );
    return 0;
}

static int glfw_UnlockMutex( lua_State *L )
{
    unsupportedFunction( L, "UnlockMutex" );
    return 0;
}

static int glfw_CreateCond( lua_State *L )
{
    unsupportedFunction( L, "CreateCond" );
    return 0;
}

static int glfw_DestroyCond( lua_State *L )
{
    unsupportedFunction( L, "DestroyCond" );
    return 0;
}

static int glfw_WaitCond( lua_State *L )
{
    unsupportedFunction( L, "WaitCond" );
    return 0;
}

static int glfw_SignalCond( lua_State *L )
{
    unsupportedFunction( L, "SignalCond" );
    return 0;
}

static int glfw_BroadcastCond( lua_State *L )
{
    unsupportedFunction( L, "BroadcastCond" );
    return 0;
}

static int glfw_GetNumberOfProcessors( lua_State *L )
{
    int n;
    n = glfwGetNumberOfProcessors();
    lua_settop( L,0 );
    lua_pushnumber( L, (lua_Number)n );
    return 1;
}


//========================================================================
// Extension handling
//========================================================================

static int glfw_GetGLVersion( lua_State *L )
{
    int major, minor, rev;
    glfwGetGLVersion( &major, &minor, &rev );
    lua_settop( L,0 );
    lua_pushnumber( L, (lua_Number)major );
    lua_pushnumber( L, (lua_Number)minor );
    lua_pushnumber( L, (lua_Number)rev );
    return 3;
}

static int glfw_ExtensionSupported( lua_State *L )
{
    const char *str;
    int        res;
    if( badArgs( L, 1, "ExtensionSupported" ) ) return 0;
    str = lua_tostring( L, 1 );
    lua_settop( L, 0 );
    res = glfwExtensionSupported( str );
    lua_pushnumber( L, (lua_Number)res );
    return 1;
}


//========================================================================
// Enable/Disable
//========================================================================

static int glfw_Enable( lua_State *L )
{
    lua_Number param;
    if ( badArgs( L, 1, "Enable" ) ) return 0;
    param = lua_tonumber( L, 1 );
    lua_settop( L, 0 );
    glfwEnable( (int)param );
    return 0;
}

static int glfw_Disable( lua_State *L )
{
    lua_Number param;
    if ( badArgs( L, 1, "Disable" ) ) return 0;
    param = lua_tonumber( L, 1 );
    lua_settop( L, 0 );
    glfwDisable( (int)param );
    return 0;
}


//========================================================================
// Image/texture I/O support
//========================================================================

static int glfw_ReadImage( lua_State *L )
{
    unsupportedFunction( L, "ReadImage" );
    return 0;
}

static int glfw_FreeImage( lua_State *L )
{
    unsupportedFunction( L, "FreeImage" );
    return 0;
}

static int glfw_LoadTexture2D( lua_State *L )
{
    const char *name;
    lua_Number flags;
    int        res;
    if( badArgs( L, 2, "LoadTexture2D" ) ) return 0;
    name  = lua_tostring( L, 1 );
    flags = lua_tonumber( L, 2 );
    lua_settop( L, 0 );
    res = glfwLoadTexture2D( name, (int)flags );
    lua_pushnumber( L, (lua_Number)res );
    return 1;
}


//========================================================================
// Callback function management
//========================================================================

static int glfw_SetWindowSizeCallback( lua_State *L )
{
    GLFWwindowsizefun fun = NULL;
    if( lua_isstring( L, 1 ) )
    {
        windowsize_name = lua_tostring( L, 1 );
        fun = luaglfw_windowsizefun;
    }
    glfwSetWindowSizeCallback( fun );
    return 0;
}

static int glfw_SetWindowCloseCallback( lua_State *L )
{
    GLFWwindowclosefun fun = NULL;
    if( lua_isstring( L, 1 ) )
    {
        windowclose_name = lua_tostring( L, 1 );
        fun = luaglfw_windowclosefun;
    }
    glfwSetWindowCloseCallback( fun );
    return 0;
}

static int glfw_SetWindowRefreshCallback( lua_State *L )
{
    GLFWwindowrefreshfun fun = NULL;
    if( lua_isstring( L, 1 ) )
    {
        windowrefresh_name = lua_tostring( L, 1 );
        fun = luaglfw_windowrefreshfun;
    }
    glfwSetWindowRefreshCallback( fun );
    return 0;
}

static int glfw_SetMouseButtonCallback( lua_State *L )
{
    GLFWmousebuttonfun fun = NULL;
    if( lua_isstring( L, 1 ) )
    {
        mousebutton_name = lua_tostring( L, 1 );
        fun = luaglfw_mousebuttonfun;
    }
    glfwSetMouseButtonCallback( fun );
    return 0;
}

static int glfw_SetMousePosCallback( lua_State *L )
{
    GLFWmouseposfun fun = NULL;
    if( lua_isstring( L, 1 ) )
    {
        mousepos_name = lua_tostring( L, 1 );
        fun = luaglfw_mouseposfun;
    }
    glfwSetMousePosCallback( fun );
    return 0;
}

static int glfw_SetMouseWheelCallback( lua_State *L )
{
    GLFWmousewheelfun fun = NULL;
    if( lua_isstring( L, 1 ) )
    {
        mousewheel_name = lua_tostring( L, 1 );
        fun = luaglfw_mousewheelfun;
    }
    glfwSetMouseWheelCallback( fun );
    return 0;
}

static int glfw_SetKeyCallback( lua_State *L )
{
    GLFWkeyfun fun = NULL;
    if( lua_isstring( L, 1 ) )
    {
        key_name = lua_tostring( L, 1 );
        fun = luaglfw_keyfun;
    }
    glfwSetKeyCallback( fun );
    return 0;
}

static int glfw_SetCharCallback( lua_State *L )
{
    GLFWcharfun fun = NULL;
    if( lua_isstring( L, 1 ) )
    {
        char_name = lua_tostring( L, 1 );
        fun = luaglfw_charfun;
    }
    glfwSetCharCallback( fun );
    return 0;
}



//************************************************************************
//****                    LUA Library Registration                    ****
//************************************************************************

// GLFW constants are stored in the global Lua table "glfw"
static struct lua_constant glfw_constants[] =
{
    // GL constants (GL_TRUE/GL_FALSE)
    { "TRUE", GL_TRUE },
    { "FALSE", GL_FALSE },

    // GLFW version
    { "VERSION_MAJOR", GLFW_VERSION_MAJOR },
    { "VERSION_MINOR", GLFW_VERSION_MINOR },
    { "VERSION_REVISION", GLFW_VERSION_REVISION },

    // Key and button state/action definitions
    { "PRESS", GLFW_PRESS },
    { "RELEASE", GLFW_RELEASE },

    // Keyboard key definitions
    { "KEY_SPACE", GLFW_KEY_SPACE },
    { "KEY_SPECIAL", GLFW_KEY_SPECIAL },
    { "KEY_ESC", GLFW_KEY_ESC },
    { "KEY_F1", GLFW_KEY_F1 },
    { "KEY_F2", GLFW_KEY_F2 },
    { "KEY_F3", GLFW_KEY_F3 },
    { "KEY_F4", GLFW_KEY_F4 },
    { "KEY_F5", GLFW_KEY_F5 },
    { "KEY_F6", GLFW_KEY_F6 },
    { "KEY_F7", GLFW_KEY_F7 },
    { "KEY_F8", GLFW_KEY_F8 },
    { "KEY_F9", GLFW_KEY_F9 },
    { "KEY_F10", GLFW_KEY_F10 },
    { "KEY_F11", GLFW_KEY_F11 },
    { "KEY_F12", GLFW_KEY_F12 },
    { "KEY_F13", GLFW_KEY_F13 },
    { "KEY_F14", GLFW_KEY_F14 },
    { "KEY_F15", GLFW_KEY_F15 },
    { "KEY_F16", GLFW_KEY_F16 },
    { "KEY_F17", GLFW_KEY_F17 },
    { "KEY_F18", GLFW_KEY_F18 },
    { "KEY_F19", GLFW_KEY_F19 },
    { "KEY_F20", GLFW_KEY_F20 },
    { "KEY_F21", GLFW_KEY_F21 },
    { "KEY_F22", GLFW_KEY_F22 },
    { "KEY_F23", GLFW_KEY_F23 },
    { "KEY_F24", GLFW_KEY_F24 },
    { "KEY_F25", GLFW_KEY_F25 },
    { "KEY_UP",  GLFW_KEY_UP },
    { "KEY_DOWN", GLFW_KEY_DOWN },
    { "KEY_LEFT", GLFW_KEY_LEFT },
    { "KEY_RIGHT", GLFW_KEY_RIGHT },
    { "KEY_LSHIFT", GLFW_KEY_LSHIFT },
    { "KEY_RSHIFT", GLFW_KEY_RSHIFT },
    { "KEY_LCTRL", GLFW_KEY_LCTRL },
    { "KEY_RCTRL", GLFW_KEY_RCTRL },
    { "KEY_LALT", GLFW_KEY_LALT },
    { "KEY_RALT", GLFW_KEY_RALT },
    { "KEY_TAB", GLFW_KEY_TAB },
    { "KEY_ENTER", GLFW_KEY_ENTER },
    { "KEY_BACKSPACE", GLFW_KEY_BACKSPACE },
    { "KEY_INSERT", GLFW_KEY_INSERT },
    { "KEY_DEL", GLFW_KEY_DEL },
    { "KEY_PAGEUP", GLFW_KEY_PAGEUP },
    { "KEY_PAGEDOWN", GLFW_KEY_PAGEDOWN },
    { "KEY_HOME", GLFW_KEY_HOME },
    { "KEY_END", GLFW_KEY_END },
    { "KEY_KP_0", GLFW_KEY_KP_0 },
    { "KEY_KP_1", GLFW_KEY_KP_1 },
    { "KEY_KP_2", GLFW_KEY_KP_2 },
    { "KEY_KP_3", GLFW_KEY_KP_3 },
    { "KEY_KP_4", GLFW_KEY_KP_4 },
    { "KEY_KP_5", GLFW_KEY_KP_5 },
    { "KEY_KP_6", GLFW_KEY_KP_6 },
    { "KEY_KP_7", GLFW_KEY_KP_7 },
    { "KEY_KP_8", GLFW_KEY_KP_8 },
    { "KEY_KP_9", GLFW_KEY_KP_9 },
    { "KEY_KP_DIVIDE", GLFW_KEY_KP_DIVIDE },
    { "KEY_KP_MULTIPLY", GLFW_KEY_KP_MULTIPLY },
    { "KEY_KP_SUBTRACT", GLFW_KEY_KP_SUBTRACT },
    { "KEY_KP_ADD", GLFW_KEY_KP_ADD },
    { "KEY_KP_DECIMAL", GLFW_KEY_KP_DECIMAL },
    { "KEY_KP_EQUAL", GLFW_KEY_KP_EQUAL },
    { "KEY_KP_ENTER", GLFW_KEY_KP_ENTER },
    { "KEY_LAST", GLFW_KEY_LAST },

    // Mouse button definitions
    { "MOUSE_BUTTON_1", GLFW_MOUSE_BUTTON_1 },
    { "MOUSE_BUTTON_2", GLFW_MOUSE_BUTTON_2 },
    { "MOUSE_BUTTON_3", GLFW_MOUSE_BUTTON_3 },
    { "MOUSE_BUTTON_4", GLFW_MOUSE_BUTTON_4 },
    { "MOUSE_BUTTON_5", GLFW_MOUSE_BUTTON_5 },
    { "MOUSE_BUTTON_6", GLFW_MOUSE_BUTTON_6 },
    { "MOUSE_BUTTON_7", GLFW_MOUSE_BUTTON_7 },
    { "MOUSE_BUTTON_8", GLFW_MOUSE_BUTTON_8 },
    { "MOUSE_BUTTON_LAST", GLFW_MOUSE_BUTTON_LAST },

    // Mouse button aliases
    { "MOUSE_BUTTON_LEFT", GLFW_MOUSE_BUTTON_LEFT },
    { "MOUSE_BUTTON_RIGHT", GLFW_MOUSE_BUTTON_RIGHT },
    { "MOUSE_BUTTON_MIDDLE", GLFW_MOUSE_BUTTON_MIDDLE },

    // Joystick identifiers
    { "JOYSTICK_1", GLFW_JOYSTICK_1 },
    { "JOYSTICK_2", GLFW_JOYSTICK_2 },
    { "JOYSTICK_3", GLFW_JOYSTICK_3 },
    { "JOYSTICK_4", GLFW_JOYSTICK_4 },
    { "JOYSTICK_5", GLFW_JOYSTICK_5 },
    { "JOYSTICK_6", GLFW_JOYSTICK_6 },
    { "JOYSTICK_7", GLFW_JOYSTICK_7 },
    { "JOYSTICK_8", GLFW_JOYSTICK_8 },
    { "JOYSTICK_9", GLFW_JOYSTICK_9 },
    { "JOYSTICK_10", GLFW_JOYSTICK_10 },
    { "JOYSTICK_11", GLFW_JOYSTICK_11 },
    { "JOYSTICK_12", GLFW_JOYSTICK_12 },
    { "JOYSTICK_13", GLFW_JOYSTICK_13 },
    { "JOYSTICK_14", GLFW_JOYSTICK_14 },
    { "JOYSTICK_15", GLFW_JOYSTICK_15 },
    { "JOYSTICK_16", GLFW_JOYSTICK_16 },
    { "JOYSTICK_LAST", GLFW_JOYSTICK_LAST },

    // glfwOpenWindow modes
    { "WINDOW", GLFW_WINDOW },
    { "FULLSCREEN", GLFW_FULLSCREEN },

    // glfwGetWindowParam tokens
    { "OPENED", GLFW_OPENED },
    { "ACTIVE", GLFW_ACTIVE },
    { "ICONIFIED", GLFW_ICONIFIED },
    { "ACCELERATED", GLFW_ACCELERATED },
    { "RED_BITS", GLFW_RED_BITS },
    { "GREEN_BITS", GLFW_GREEN_BITS },
    { "BLUE_BITS", GLFW_BLUE_BITS },
    { "ALPHA_BITS", GLFW_ALPHA_BITS },
    { "DEPTH_BITS", GLFW_DEPTH_BITS },
    { "STENCIL_BITS", GLFW_STENCIL_BITS },

    // Constants for glfwGetWindowParam and glfwOpenWindowHint
    { "REFRESH_RATE", GLFW_REFRESH_RATE },
    { "ACCUM_RED_BITS", GLFW_ACCUM_RED_BITS },
    { "ACCUM_GREEN_BITS", GLFW_ACCUM_GREEN_BITS },
    { "ACCUM_BLUE_BITS", GLFW_ACCUM_BLUE_BITS },
    { "ACCUM_ALPHA_BITS", GLFW_ACCUM_ALPHA_BITS },
    { "AUX_BUFFERS", GLFW_AUX_BUFFERS },
    { "STEREO", GLFW_STEREO },

    // glfwEnable/glfwDisable tokens
    { "MOUSE_CURSOR", GLFW_MOUSE_CURSOR },
    { "STICKY_KEYS", GLFW_STICKY_KEYS },
    { "STICKY_MOUSE_BUTTONS", GLFW_STICKY_MOUSE_BUTTONS },
    { "SYSTEM_KEYS", GLFW_SYSTEM_KEYS },
    { "KEY_REPEAT", GLFW_KEY_REPEAT },
    { "AUTO_POLL_EVENTS", GLFW_AUTO_POLL_EVENTS },

    // glfwWaitThread wait modes
    { "WAIT", GLFW_WAIT },
    { "NOWAIT", GLFW_NOWAIT },

    // glfwGetJoystickParam tokens
    { "PRESENT", GLFW_PRESENT },
    { "AXES", GLFW_AXES },
    { "BUTTONS", GLFW_BUTTONS },

    // glfwReadImage/glfwLoadTexture2D flags
    { "NO_RESCALE_BIT", GLFW_NO_RESCALE_BIT },
    { "ORIGIN_UL_BIT", GLFW_ORIGIN_UL_BIT },
    { "BUILD_MIPMAPS_BIT", GLFW_BUILD_MIPMAPS_BIT },
    { "ALPHA_MAP_BIT", GLFW_ALPHA_MAP_BIT },

    // Time spans longer than this (seconds) are considered to be infinity
    { "INFINITY", GLFW_INFINITY },

    { NULL, 0 }
};


// Library functions to register
static const luaL_reg glfwlib[] = {
    { "Init", glfw_Init },
    { "Terminate", glfw_Terminate },
    { "GetVersion", glfw_GetVersion },
    { "OpenWindow", glfw_OpenWindow },
    { "OpenWindowHint", glfw_OpenWindowHint },
    { "CloseWindow", glfw_CloseWindow },
    { "SetWindowTitle", glfw_SetWindowTitle },
    { "SetWindowSize", glfw_SetWindowSize },
    { "GetWindowSize", glfw_GetWindowSize },
    { "SetWindowPos", glfw_SetWindowPos },
    { "IconifyWindow", glfw_IconifyWindow },
    { "RestoreWindow", glfw_RestoreWindow },
    { "GetWindowParam", glfw_GetWindowParam },
    { "SwapBuffers", glfw_SwapBuffers },
    { "SwapInterval", glfw_SwapInterval },
    { "GetVideoModes", glfw_GetVideoModes },
    { "GetDesktopMode", glfw_GetDesktopMode },
    { "PollEvents", glfw_PollEvents },
    { "WaitEvents", glfw_WaitEvents },
    { "GetKey", glfw_GetKey },
    { "GetMouseButton", glfw_GetMouseButton },
    { "GetMousePos", glfw_GetMousePos },
    { "SetMousePos", glfw_SetMousePos },
    { "GetMouseWheel", glfw_GetMouseWheel },
    { "SetMouseWheel", glfw_SetMouseWheel },
    { "GetJoystickParam", glfw_GetJoystickParam },
    { "GetJoystickPos", glfw_GetJoystickPos },
    { "GetJoystickButtons", glfw_GetJoystickButtons },
    { "GetTime", glfw_GetTime },
    { "SetTime", glfw_SetTime },
    { "Sleep", glfw_Sleep },
    { "GetGLVersion", glfw_GetGLVersion },
    { "ExtensionSupported", glfw_ExtensionSupported },
    { "CreateThread", glfw_CreateThread },
    { "DestroyThread", glfw_DestroyThread },
    { "WaitThread", glfw_WaitThread },
    { "GetThreadID", glfw_GetThreadID },
    { "CreateMutex", glfw_CreateMutex },
    { "DestroyMutex", glfw_DestroyMutex },
    { "LockMutex", glfw_LockMutex },
    { "UnlockMutex", glfw_UnlockMutex },
    { "CreateCond", glfw_CreateCond },
    { "DestroyCond", glfw_DestroyCond },
    { "WaitCond", glfw_WaitCond },
    { "SignalCond", glfw_SignalCond },
    { "BroadcastCond", glfw_BroadcastCond },
    { "GetNumberOfProcessors", glfw_GetNumberOfProcessors },
    { "Enable", glfw_Enable },
    { "Disable", glfw_Disable },
    { "ReadImage", glfw_ReadImage },
    { "FreeImage", glfw_FreeImage },
    { "LoadTexture2D", glfw_LoadTexture2D },
    { "SetWindowSizeCallback", glfw_SetWindowSizeCallback },
    { "SetWindowCloseCallback", glfw_SetWindowCloseCallback },
    { "SetWindowRefreshCallback", glfw_SetWindowRefreshCallback },
    { "SetMouseButtonCallback", glfw_SetMouseButtonCallback },
    { "SetMousePosCallback", glfw_SetMousePosCallback },
    { "SetMouseWheelCallback", glfw_SetMouseWheelCallback },
    { "SetKeyCallback", glfw_SetKeyCallback },
    { "SetCharCallback", glfw_SetCharCallback },
    { NULL, NULL }
};



int luaopen_glfw( lua_State *L )
{
    // Store GLFW functions in "glfw" table
    luaL_openlib( L, "glfw", glfwlib, 0 );

    // Store GLFW constants in "glfw" table
    lua_pushstring( L, "glfw" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    addConstants( L, glfw_constants );
    lua_settop( L, 0 );

    // Remember Lua state for callback functions
    callback_lua_state = L;

    return 0;
}
