//========================================================================
// GLFW - An OpenGL framework
// File:        amigaos_joystick.c
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
// _glfwSendJoyReadRequest() - Send joystick read request
//========================================================================

static void _glfwSendJoyReadRequest( void )
{
    // Send read request
    _glfwJoy.GameIO->io_Command = GPD_READEVENT;
    _glfwJoy.GameIO->io_Flags   = 0;
    _glfwJoy.GameIO->io_Data    = (APTR) &_glfwJoy.GameEvent;
    _glfwJoy.GameIO->io_Length  = sizeof(struct InputEvent);
    SendIO( (struct IORequest *) _glfwJoy.GameIO );
}


//========================================================================
// _glfwInitJoysticks() - Initialize joystick interface
//========================================================================

void _glfwInitJoysticks( void )
{
    unsigned char controller_type = 0;
    struct GamePortTrigger game_trigger;

    // Start by clearing all handles
    _glfwJoy.GameMP         = NULL;
    _glfwJoy.GameIO         = NULL;
    _glfwJoy.Present        = 0;
    _glfwJoy.GameDeviceOpen = 0;

    // Create gameport.device message port
    if( !(_glfwJoy.GameMP = CreatePort( NULL, 0 )) )
    {
        return;
    }

    // Create gameport.device I/O request
    if( !( _glfwJoy.GameIO = (struct IOStdReq *)
           CreateExtIO(_glfwJoy.GameMP, sizeof(struct IOStdReq)) ) )
    {
        _glfwTerminateJoysticks();
        return;
    }

    // Open gameport.device (unit 1 = port 2)
    _glfwJoy.GameIO->io_Message.mn_Node.ln_Type = NT_UNKNOWN;
    if( OpenDevice( "gameport.device", 1,
                    (struct IORequest *)_glfwJoy.GameIO, 0 ) )
    {
        _glfwTerminateJoysticks();
        return;
    }
    _glfwJoy.GameDeviceOpen = 1;

    // Start critical section
    Forbid();

    // Find out if anyone else is using the stick
    _glfwJoy.GameIO->io_Command = GPD_ASKCTYPE;
    _glfwJoy.GameIO->io_Flags   = IOF_QUICK;
    _glfwJoy.GameIO->io_Data    = (APTR) &controller_type;
    _glfwJoy.GameIO->io_Length  = 1;
    DoIO( (struct IORequest *) _glfwJoy.GameIO );

    // Was it already allocated?
    if( controller_type != GPCT_NOCONTROLLER )
    {
        Permit();
        _glfwTerminateJoysticks();
        return;
    }

    // Allocate joystick
    controller_type = GPCT_ABSJOYSTICK;
    _glfwJoy.GameIO->io_Command = GPD_SETCTYPE;
    _glfwJoy.GameIO->io_Flags   = IOF_QUICK;
    _glfwJoy.GameIO->io_Data    = (APTR) &controller_type;
    _glfwJoy.GameIO->io_Length  = 1;
    DoIO( (struct IORequest *) _glfwJoy.GameIO );
    _glfwJoy.Present = 1;

    // End critical section
    Permit();

    // Set trigger conditions
    game_trigger.gpt_Keys       = GPTF_UPKEYS | GPTF_DOWNKEYS;
    game_trigger.gpt_XDelta     = 1;
    game_trigger.gpt_YDelta     = 1;
    game_trigger.gpt_Timeout    = (UWORD) 0xFFFF; // ~20 minutes
    _glfwJoy.GameIO->io_Command = GPD_SETTRIGGER;
    _glfwJoy.GameIO->io_Flags   = IOF_QUICK;
    _glfwJoy.GameIO->io_Data    = (APTR) &game_trigger;
    _glfwJoy.GameIO->io_Length  = (LONG) sizeof(struct GamePortTrigger);
    DoIO( (struct IORequest *) _glfwJoy.GameIO );

    // Flush buffer
    _glfwJoy.GameIO->io_Command = CMD_CLEAR;
    _glfwJoy.GameIO->io_Flags   = IOF_QUICK;
    _glfwJoy.GameIO->io_Data    = NULL;
    _glfwJoy.GameIO->io_Length  = 0;
    DoIO( (struct IORequest *) _glfwJoy.GameIO );

    // Send joystick read request (asynchronous)
    _glfwSendJoyReadRequest();
}


//========================================================================
// _glfwTerminateJoysticks() - Close all opened joystick handles
//========================================================================

void _glfwTerminateJoysticks( void )
{
    unsigned char controller_type;

    // Remove any remaining asynchronous messages
    if( _glfwJoy.GameIO )
    {
        if( !CheckIO( (struct IORequest *)_glfwJoy.GameIO ) )
        {
            AbortIO( (struct IORequest *)_glfwJoy.GameIO );
            WaitIO( (struct IORequest *)_glfwJoy.GameIO );
        }
    }

    // Deallocate joystick
    if( _glfwJoy.Present )
    {
        controller_type = GPCT_NOCONTROLLER;
        _glfwJoy.GameIO->io_Command = GPD_SETCTYPE;
        _glfwJoy.GameIO->io_Flags   = IOF_QUICK;
        _glfwJoy.GameIO->io_Data    = (APTR) &controller_type;
        _glfwJoy.GameIO->io_Length  = 1;
        DoIO( (struct IORequest *) _glfwJoy.GameIO );
        _glfwJoy.Present = 0;
    }

    // Close gameport.device
    if( _glfwJoy.GameDeviceOpen )
    {
        CloseDevice( (struct IORequest *) _glfwJoy.GameIO );
        _glfwJoy.GameDeviceOpen = 0;
    }

    // Delete I/O request
    if( _glfwJoy.GameIO )
    {
        DeleteExtIO( (struct IORequest *) _glfwJoy.GameIO );
        _glfwJoy.GameIO = NULL;
    }

    // Delete message port
    if( _glfwJoy.GameMP )
    {
        DeletePort( _glfwJoy.GameMP );
        _glfwJoy.GameMP = NULL;
    }
}


//========================================================================
// _glfwPollJoystickEvents() - Empty joystick event queue
//========================================================================

static void _glfwPollJoystickEvents( void )
{
    int got_event = 0;

    // Do we have a stick?
    if( !_glfwJoy.Present )
    {
        return;
    }

    // Empty the message queue
    while( GetMsg( _glfwJoy.GameMP ) != NULL )
    {
        // Flag: we got an event
        got_event = 1;

        switch( _glfwJoy.GameEvent.ie_Code )
        {
            // Left button pressed
            case IECODE_LBUTTON:
                _glfwJoy.Button[ 0 ] = 1;
                break;

            // Left button released
            case (IECODE_LBUTTON | IECODE_UP_PREFIX):
                _glfwJoy.Button[ 0 ] = 0;
                break;

            // Right button pressed
            case IECODE_RBUTTON:
                _glfwJoy.Button[ 1 ] = 1;
                break;

            // Right button released
            case (IECODE_RBUTTON | IECODE_UP_PREFIX):
                _glfwJoy.Button[ 1 ] = 0;
                break;

            // Axis event
            case IECODE_NOBUTTON:
                _glfwJoy.Axis[ 0 ] = (float) _glfwJoy.GameEvent.ie_X;
                _glfwJoy.Axis[ 1 ] = (float) -_glfwJoy.GameEvent.ie_Y;
                break;

            default:
                break;
        }
    }

    // Did we get any events?
    if( got_event )
    {
        // Send joystick read request (asynchronous)
        _glfwSendJoyReadRequest();
    }
}



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformGetJoystickParam() - Determine joystick capabilities
//========================================================================

int _glfwPlatformGetJoystickParam( int joy, int param )
{
    // Is joystick present?
    if( joy != GLFW_JOYSTICK_1 || !_glfwJoy.Present )
    {
        return 0;
    }

    // We assume that the joystick is connected, and has two axes and two
    // buttons (since there is no way of retrieving this information from
    // AmigaOS)
    switch( param )
    {
    case GLFW_PRESENT:
        return GL_TRUE;

    case GLFW_AXES:
        return 2;

    case GLFW_BUTTONS:
        return 2;

    default:
        break;
    }

    return 0;
}


//========================================================================
// _glfwPlatformGetJoystickPos() - Get joystick axis positions
//========================================================================

int _glfwPlatformGetJoystickPos( int joy, float *pos, int numaxes )
{
    int k;

    // Is joystick present?
    if( joy != GLFW_JOYSTICK_1 || !_glfwJoy.Present )
    {
        return 0;
    }

    // Update joystick state
    _glfwPollJoystickEvents();

    // Copy axis position information to output vector
    if( numaxes > 2 )
    {
        numaxes = 2;
    }
    for( k = 0; k < numaxes; ++ k )
    {
        pos[ k ] = _glfwJoy.Axis[ k ];
    }

    return numaxes;
}


//========================================================================
// _glfwPlatformGetJoystickButtons() - Get joystick button states
//========================================================================

int _glfwPlatformGetJoystickButtons( int joy, unsigned char *buttons,
    int numbuttons )
{
    int k;

    // Is joystick present?
    if( joy != GLFW_JOYSTICK_1 || !_glfwJoy.Present )
    {
        return 0;
    }

    // Update joystick state
    _glfwPollJoystickEvents();

    // Copy button information to output vector
    if( numbuttons > 2 )
    {
        numbuttons = 2;
    }
    for( k = 0; k < numbuttons; ++ k )
    {
        buttons[ k ] = _glfwJoy.Button[ k ];
    }

    return numbuttons;
}
