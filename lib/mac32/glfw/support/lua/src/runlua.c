//========================================================================
// A simple Lua interpreter with support for GLFW and OpenGL
// File:        runlua.c
// Platform:    Lua 5.0 + LuaGL + luaglfw
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

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <LuaGL.h>
#include "luaglfw.h"


//========================================================================
// main() - Program entry point
//========================================================================

int main( int argc, char **argv )
{
    lua_State *L;

    // Check arguments
    if( argc != 2 )
    {
       printf( "Usage: %s filename.lua\n", argv[0] );
       return -1;
    }

    // Init Lua
    L = lua_open();
    if( !L )
    {
        printf("Unable to start Lua\n");
        return -1;
    }

    // Open all Lua standard libraries
    luaopen_base( L );
    luaopen_table( L );
    luaopen_io( L );
    luaopen_string( L );
    luaopen_math( L );

    // Open LuaGL and luaglfw libraries
    luaopen_opengl( L );
    luaopen_glfw( L );

    // Load and run the selected Lua program
    if( luaL_loadfile( L, argv[1] ) || lua_pcall( L, 0, 0, 0 ) )
    {
       printf("Error running Lua program: %s\n", lua_tostring( L, -1 ) );
       lua_pop( L, 1 );
       return -1;
    }

    return 0;
}
