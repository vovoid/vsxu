//========================================================================
// GLFW - An OpenGL framework
// File:        amigaos_fullscreen.c
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
// _glfwBPP2RGB() - Convert BPP to RGB bits (based on "best guess")
//========================================================================

static void _glfwBPP2RGB( int bpp, int *r, int *g, int *b )
{
    int delta;

    // Special case: bpp = 32
    if( bpp == 32 ) bpp = 24;

    // Convert "bits per pixel" to red, green & blue sizes
    *r = *g = *b = bpp / 3;
    delta = bpp - (*r * 3);
    if( delta >= 1 )
    {
        *g = *g + 1;
    }
    if( delta == 2 )
    {
        *r = *r + 1;
    }
}


//========================================================================
// _glfwGetModeIDInfo() - Return video mode information about a ModeID
//========================================================================

void _glfwGetModeIDInfo( ULONG ModeID, int *w, int *h, int *r, int *g,
                        int *b, int *refresh )
{
    struct DimensionInfo dimsInfo;
    struct DisplayInfo   dispInfo;
    struct MonitorInfo   monInfo;

    // Get various display info
    (void) GetDisplayInfoData( NULL,
                               (BYTE*) &dimsInfo,
                               sizeof(struct DimensionInfo),
                               DTAG_DIMS,
                               ModeID );
    (void) GetDisplayInfoData( NULL,
                               (BYTE*) &dispInfo,
                               sizeof(struct DisplayInfo),
                               DTAG_DISP,
                               ModeID );
    (void) GetDisplayInfoData( NULL,
                               (BYTE*) &monInfo,
                               sizeof(struct MonitorInfo),
                               DTAG_MNTR,
                               ModeID );

    // Extract nominal width & height
    if( w != NULL && h != NULL )
    {
        *w = (int) (dimsInfo.Nominal.MaxX - dimsInfo.Nominal.MinX) + 1;
        *h = (int) (dimsInfo.Nominal.MaxY - dimsInfo.Nominal.MinY) + 1;
    }

    // Extract color bits
    if( r != NULL && g != NULL && g != NULL )
    {
        *r = (int) dispInfo.RedBits;
        *g = (int) dispInfo.GreenBits;
        *b = (int) dispInfo.BlueBits;

        // If depth < sum of RGB bits, we're probably not true color,
        // which means that DisplayInfo red/green/blue bits do not refer
        // to actual pixel color depth => use pixel depth info instead
        if( dimsInfo.MaxDepth < (*r + *g + *b) )
        {
            _glfwBPP2RGB( dimsInfo.MaxDepth, r, g, b );
        }
    }

    // Extract refresh rate
    if( refresh != NULL )
    {
        *refresh = (int) (1.0 / ((double) monInfo.TotalRows *
                                 (double) monInfo.TotalColorClocks *
                                 280.0e-9) + 0.5 );
    }
}


//========================================================================
// _glfwGetClosestVideoMode()
//========================================================================

int _glfwGetClosestVideoMode( int *w, int *h, int *r, int *g, int *b,
                              int refresh )
{
    int                  modeID;

    // Find best mode
    modeID = BestModeID(
        BIDTAG_NominalWidth,    *w,
        BIDTAG_NominalHeight,   *h,
        BIDTAG_Depth,           *r + *g + *b,
        BIDTAG_RedBits,         *r,
        BIDTAG_GreenBits,       *g,
        BIDTAG_BlueBits,        *b,
        TAG_DONE, 0
    );

    // Did we get a proper mode?
    if( !modeID )
    {
        return 0;
    }

    // Get actual display info
    _glfwGetModeIDInfo( modeID, w, h, r, g, b, NULL );

    return modeID;
}


//========================================================================
// _glfwOpenScreen() - Open an AmigaOS screen
//========================================================================

int _glfwOpenScreen( int *width, int *height, int *r, int *g, int *b,
    int refresh )
{
    int bpp, modeID;

    // Calculate BPP
    bpp = *r + *g + *b;

    // If colorbits < 8 (e.g. 0) or >= 24, default to 24 bpp
    if( bpp < 8 || bpp >= 24 )
    {
        *r = *g = *b = 8;
    }

    // Find best matching video mode
    modeID = _glfwGetClosestVideoMode( width, height, r, g, b, refresh );

    // Open screen
    _glfwWin.Screen = OpenScreenTags(
        NULL,
        SA_Width,     *width,
        SA_Height,    *height,
        SA_DisplayID, modeID,
        SA_Type,      CUSTOMSCREEN,
        SA_SysFont,   1,
        SA_ShowTitle, FALSE,
        TAG_DONE, 0
    );

    // Did we succeed?
    if( !_glfwWin.Screen )
    {
        printf( "Failed to open Amiga screen\n" );
        return GL_FALSE;
    }

    // Remember Mode ID
    _glfwWin.ModeID = modeID;

/*
    // Debugging information
    printf( "Amiga Screen opened:\n" );
    printf( "  ModeID:     0x%08X\n", modeID );
    printf( "  Dimensions: %d x %d\n", *width, *height );
    printf( "  Color bits: %d : %d : %d\n", *r, *g, *b );
*/

    return GL_TRUE;
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetVideoModes() - List available video modes
//========================================================================

int _glfwPlatformGetVideoModes( GLFWvidmode *list, int maxcount )
{
    ULONG modeID;
    int   w, h, r, g, b, bpp, m1, m2, i, j, count;

    count  = 0;
    modeID = INVALID_ID;
    do
    {
        // Enumarate all ModeIDs with NextDisplayInfo
        modeID = NextDisplayInfo( modeID );
        if( modeID != INVALID_ID )
        {
            // Get related video mode information
            _glfwGetModeIDInfo( modeID, &w, &h, &r, &g, &b, NULL );

            // Convert RGB to BPP
            bpp = r + g + b;

            // We only support true-color modes, which means at least 15
            // bits per pixel (reasonable?) - Sorry, AGA users!
            if( bpp >= 15 )
            {
                // Mode "code" for this mode
                m1 = (bpp << 25) | (w*h);

                // Insert mode in list (sorted), and avoid duplicates
                for( i = 0; i < count; i ++ )
                {
                    // Mode "code" for already listed mode
                    bpp = list[i].RedBits + list[i].GreenBits +
                          list[i].BlueBits;
                    m2 = (bpp << 25) | (list[i].Width * list[i].Height);
                    if( m1 <= m2 )
                    {
                        break;
                    }
                }

                // New entry at the end of the list?
                if( i >= count )
                {
                    list[count].Width     = w;
                    list[count].Height    = h;
                    list[count].RedBits   = r;
                    list[count].GreenBits = g;
                    list[count].BlueBits  = b;
                    count ++;
                }
                // Insert new entry in the list?
                else if( m1 < m2 )
                {
                    for( j = count; j > i; j -- )
                    {
                        list[j] = list[j-1];
                    }
                    list[i].Width     = w;
                    list[i].Height    = h;
                    list[i].RedBits   = r;
                    list[i].GreenBits = g;
                    list[i].BlueBits  = b;
                    count ++;
                }
            }
        }
    }
    while( modeID != INVALID_ID && count < maxcount );

    return count;
}


//========================================================================
// _glfwPlatformGetDesktopMode() - Get the desktop video mode
//========================================================================

void _glfwPlatformGetDesktopMode( GLFWvidmode *mode )
{
    struct Screen        *pubscreen;
    ULONG                modeID;

    // Get default public screen screen handle
    pubscreen = LockPubScreen( NULL );

    // Get screen width and height (use actual screen size rather than
    // ModeID nominal size)
    mode->Width  = (int) pubscreen->Width;
    mode->Height = (int) pubscreen->Height;

    // Get ModeID for public screen
    modeID = GetVPModeID( &pubscreen->ViewPort );

    // Release workbench screen
    UnlockPubScreen( NULL, pubscreen );

    // Get color bits information
    _glfwGetModeIDInfo( modeID, NULL, NULL, &mode->RedBits,
                        &mode->GreenBits, &mode->BlueBits, NULL );
}
