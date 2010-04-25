//========================================================================
// GLFW - An OpenGL framework
// File:        dos_keyboard.c
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


//========================================================================
// Most of the code in this source file is based on two sources of
// information:
// 1) The Allegro DOS keyboard driver (allegro\src\dos\dkeybd.c)
// 2) The document "IBM PC KEYBOARD INFORMATION FOR SOFTWARE DEVELOPERS"
//    by Chris Giese.
//========================================================================

//========================================================================
// Unicode in GLFW for DOS
// =======================
//
// Keyboard mapping tables in GLFW for DOS use Unicode encoding. The codes
// are 16-bit unsigned integers, and thus do not cover the entire Unicode
// standard (but a great deal is covered).
//
// Keys or characters that are not supported by GLFW (for instance the
// PrtScr or Windows keys that are found on most PC keyboards) are coded
// with 0xFFFF ("not a character" according to the Unicode standard).
//
// GLFW special keys, as defined in glfw.h (e.g. GLFW_KEY_LSHIFT) are
// encoded in the private area of the Unicode standard (i.e. codes in the
// range E000-F8FF). The encoding is as follows:
//
//   unicode = 0xE000 + glfw_key - GLFW_KEY_SPECIAL;
//
// Every key in the keyboard matrix has a description consisting of four
// entries: Normal, Shift, Caps, and AltGr.
//========================================================================

//========================================================================
// Definitions
//========================================================================

// Keyboard interrupt number
#define KEYB_IRQ 1

// Qualifier flags
#define QUAL_SCROLOCK   0x0001  // Same bits as for controller cmd 0xED
#define QUAL_NUMLOCK    0x0002  // (set leds)
#define QUAL_CAPSLOCK   0x0004  // --"--
#define QUAL_LSHIFT     0x0008
#define QUAL_RSHIFT     0x0010
#define QUAL_LALT       0x0020
#define QUAL_RALT       0x0040
#define QUAL_LCTRL      0x0080
#define QUAL_RCTRL      0x0100

// Qualifier groups
#define QUAL_MODIFIERS (QUAL_LSHIFT|QUAL_RSHIFT|QUAL_LALT|QUAL_RALT|\
                        QUAL_LCTRL|QUAL_RCTRL)
#define QUAL_LEDS      (QUAL_SCROLOCK|QUAL_NUMLOCK|QUAL_CAPSLOCK)


// Additional non-GLFW keys, defined here for internal processing only
#define GLFW_KEY_CAPSLOCK (GLFW_KEY_SPECIAL+0x0200)
#define GLFW_KEY_NUMLOCK  (GLFW_KEY_SPECIAL+0x0201)
#define GLFW_KEY_SCROLOCK (GLFW_KEY_SPECIAL+0x0202)
#define GLFW_KEY_PAUSE    (GLFW_KEY_SPECIAL+0x0203)

// Keymap entry definition
struct key {
    unsigned short Normal, Caps, Shift, AltGr;
};

// Keymap entry macros
#define NOCHAR(x) {x+0xDF00,x+0xDF00,x+0xDF00,x+0xDF00}
#define UNDEFINED {0xFFFF,0xFFFF,0xFFFF,0xFFFF}


//========================================================================
// Global variables
//========================================================================

static struct {
    int volatile KeyEnhanced, KeyPauseLoop, Qualifiers;
    int          LedsOK;
    int          Interrupt;
} _glfwKeyDrv;

static int _glfwKeyboardInstalled = 0;



//========================================================================
// scancode_to_key_us[] - Mapping table for US keyboard layout (XT)
//========================================================================

static struct key scancode_to_key_us[256] =
{
/* Standard hardware scancodes */
/* 0x00 */ UNDEFINED,                   NOCHAR(GLFW_KEY_ESC),
/* 0x02 */ {'1','1','!',0xFFFF},        {'2','2','@',0xFFFF},
/* 0x04 */ {'3','3','#',0xFFFF},        {'4','4','$',0xFFFF},
/* 0x06 */ {'5','5','%',0xFFFF},        {'6','6','^',0xFFFF},
/* 0x08 */ {'7','7','&',0xFFFF},        {'8','8','*',0xFFFF},
/* 0x0A */ {'9','9','(',0xFFFF},        {'0','0',')',0xFFFF},
/* 0x0C */ {'-','-','_',0xFFFF},        {'=','=','+',0xFFFF},
/* 0x0E */ NOCHAR(GLFW_KEY_BACKSPACE),  NOCHAR(GLFW_KEY_TAB),
/* 0x10 */ {'q','Q','Q',0xFFFF},        {'w','W','W',0xFFFF},
/* 0x12 */ {'e','E','E',0xFFFF},        {'r','R','R',0xFFFF},
/* 0x14 */ {'t','T','T',0xFFFF},        {'y','Y','Y',0xFFFF},
/* 0x16 */ {'u','U','U',0xFFFF},        {'i','I','I',0xFFFF},
/* 0x18 */ {'o','O','O',0xFFFF},        {'p','P','P',0xFFFF},
/* 0x1A */ {'[','[','{',0xFFFF},        {']',']','}',0xFFFF},
/* 0x1C */ NOCHAR(GLFW_KEY_ENTER),      NOCHAR(GLFW_KEY_LCTRL),
/* 0x1E */ {'a','A','A',0xFFFF},        {'s','S','S',0xFFFF},
/* 0x20 */ {'d','D','D',0xFFFF},        {'f','F','F',0xFFFF},
/* 0x22 */ {'g','G','G',0xFFFF},        {'h','H','H',0xFFFF},
/* 0x24 */ {'j','J','J',0xFFFF},        {'k','K','K',0xFFFF},
/* 0x26 */ {'l','L','L',0xFFFF},        {';',';',':',0xFFFF},
/* 0x28 */ {'\'','\'','"',0xFFFF},      {'\\','\\','|',0xFFFF},
/* 0x2A */ NOCHAR(GLFW_KEY_LSHIFT),     {'\\','\\','|',0xFFFF},
/* 0x2C */ {'z','Z','Z',0xFFFF},        {'x','X','X',0xFFFF},
/* 0x2E */ {'c','C','C',0xFFFF},        {'v','V','V',0xFFFF},
/* 0x30 */ {'b','B','B',0xFFFF},        {'n','N','N',0xFFFF},
/* 0x32 */ {'m','M','M',0xFFFF},        {',',',','<',0xFFFF},
/* 0x34 */ {'.','.','>',0xFFFF},        {'/','/','?',0xFFFF},
/* 0x36 */ NOCHAR(GLFW_KEY_RSHIFT),     NOCHAR(GLFW_KEY_KP_MULTIPLY),
/* 0x38 */ NOCHAR(GLFW_KEY_LALT),       {' ',' ',' ',0xFFFF},
/* 0x3A */ NOCHAR(GLFW_KEY_CAPSLOCK),   NOCHAR(GLFW_KEY_F1),
/* 0x3C */ NOCHAR(GLFW_KEY_F2),         NOCHAR(GLFW_KEY_F3),
/* 0x3E */ NOCHAR(GLFW_KEY_F4),         NOCHAR(GLFW_KEY_F5),
/* 0x40 */ NOCHAR(GLFW_KEY_F6),         NOCHAR(GLFW_KEY_F7),
/* 0x42 */ NOCHAR(GLFW_KEY_F8),         NOCHAR(GLFW_KEY_F9),
/* 0x44 */ NOCHAR(GLFW_KEY_F10),        NOCHAR(GLFW_KEY_NUMLOCK),
/* 0x46 */ NOCHAR(GLFW_KEY_SCROLOCK),   NOCHAR(GLFW_KEY_KP_7),
/* 0x48 */ NOCHAR(GLFW_KEY_KP_8),       NOCHAR(GLFW_KEY_KP_9),
/* 0x4A */ NOCHAR(GLFW_KEY_KP_SUBTRACT), NOCHAR(GLFW_KEY_KP_4),
/* 0x4C */ NOCHAR(GLFW_KEY_KP_5),       NOCHAR(GLFW_KEY_KP_6),
/* 0x4E */ NOCHAR(GLFW_KEY_KP_ADD),     NOCHAR(GLFW_KEY_KP_1),
/* 0x50 */ NOCHAR(GLFW_KEY_KP_2),       NOCHAR(GLFW_KEY_KP_3),
/* 0x52 */ NOCHAR(GLFW_KEY_KP_0),       NOCHAR(GLFW_KEY_KP_DECIMAL),
/* 0x54 */ UNDEFINED, /* PRTSCR */      UNDEFINED,
/* 0x56 */ {'\\','\\','|',0xFFFF},      NOCHAR(GLFW_KEY_F11),
/* 0x58 */ NOCHAR(GLFW_KEY_F12),        UNDEFINED,
/* 0x5A */ UNDEFINED,                   UNDEFINED, /* LWIN */
/* 0x5C */ UNDEFINED, /* RWIN */        UNDEFINED, /* MENU */
/* 0x5E */ UNDEFINED,                   UNDEFINED,
/* 0x60 */ UNDEFINED,                   UNDEFINED,
/* 0x62 */ UNDEFINED,                   UNDEFINED,
/* 0x64 */ UNDEFINED,                   UNDEFINED,
/* 0x66 */ UNDEFINED,                   UNDEFINED,
/* 0x68 */ UNDEFINED,                   UNDEFINED,
/* 0x6A */ UNDEFINED,                   UNDEFINED,
/* 0x6C */ UNDEFINED,                   UNDEFINED,
/* 0x6E */ UNDEFINED,                   UNDEFINED,
/* 0x70 */ UNDEFINED, /* KANA */        UNDEFINED,
/* 0x72 */ UNDEFINED,                   UNDEFINED, /* ABNT_C1 */
/* 0x74 */ UNDEFINED,                   UNDEFINED,
/* 0x76 */ UNDEFINED,                   UNDEFINED,
/* 0x78 */ UNDEFINED,                   UNDEFINED, /* CONVERT */
/* 0x7A */ UNDEFINED,                   UNDEFINED, /* NOCONVERT */
/* 0x7C */ UNDEFINED,                   UNDEFINED, /* YEN */
/* 0x7E */ UNDEFINED,                   UNDEFINED,

/* Extended hardware scancodes (index=scancode+0x80) */
/* 0xE000 */ UNDEFINED,                   NOCHAR(GLFW_KEY_ESC),
/* 0xE002 */ {'1','1','!',0xFFFF},        {'2','2','@',0xFFFF},
/* 0xE004 */ {'3','3','#',0xFFFF},        {'4','4','$',0xFFFF},
/* 0xE006 */ {'5','5','%',0xFFFF},        {'6','6','^',0xFFFF},
/* 0xE008 */ {'7','7','&',0xFFFF},        {'8','8','*',0xFFFF},
/* 0xE00A */ {'9','9','(',0xFFFF},        {'0','0',')',0xFFFF},
/* 0xE00C */ {'-','-','_',0xFFFF},        {'=','=','+',0xFFFF},
/* 0xE00E */ NOCHAR(GLFW_KEY_BACKSPACE),  NOCHAR(GLFW_KEY_TAB),
/* 0xE010 */ UNDEFINED, /* CIRCUMFLEX */  UNDEFINED, /* AT */
/* 0xE012 */ UNDEFINED, /* COLON2 */      {'r','R','R',0xFFFF},
/* 0xE014 */ UNDEFINED, /* KANJI */       {'y','Y','Y',0xFFFF},
/* 0xE016 */ {'u','U','U',0xFFFF},        {'i','I','I',0xFFFF},
/* 0xE018 */ {'o','O','O',0xFFFF},        {'p','P','P',0xFFFF},
/* 0xE01A */ {'[','[','{',0xFFFF},        {']',']','}',0xFFFF},
/* 0xE01C */ NOCHAR(GLFW_KEY_KP_ENTER),   NOCHAR(GLFW_KEY_RCTRL),
/* 0xE01E */ {'a','A','A',0xFFFF},        {'s','S','S',0xFFFF},
/* 0xE020 */ {'d','D','D',0xFFFF},        {'f','F','F',0xFFFF},
/* 0xE022 */ {'g','G','G',0xFFFF},        {'h','H','H',0xFFFF},
/* 0xE024 */ {'j','J','J',0xFFFF},        {'k','K','K',0xFFFF},
/* 0xE026 */ {'l','L','L',0xFFFF},        {';',';',':',0xFFFF},
/* 0xE028 */ {'\'','\'','"',0xFFFF},      {'`','`','~',0xFFFF},
/* 0xE02A */ UNDEFINED,                   {'\\','\\','|',0xFFFF},
/* 0xE02C */ {'z','Z','Z',0xFFFF},        {'x','X','X',0xFFFF},
/* 0xE02E */ {'c','C','C',0xFFFF},        {'v','V','V',0xFFFF},
/* 0xE030 */ {'b','B','B',0xFFFF},        {'n','N','N',0xFFFF},
/* 0xE032 */ {'m','M','M',0xFFFF},        {',',',','<',0xFFFF},
/* 0xE034 */ {'.','.','>',0xFFFF},        NOCHAR(GLFW_KEY_KP_DIVIDE),
/* 0xE036 */ UNDEFINED,                   UNDEFINED, /* PRTSCR */
/* 0xE038 */ NOCHAR(GLFW_KEY_RALT),       {' ',' ',' ',0xFFFF},
/* 0xE03A */ NOCHAR(GLFW_KEY_CAPSLOCK),   NOCHAR(GLFW_KEY_F1),
/* 0xE03C */ NOCHAR(GLFW_KEY_F2),         NOCHAR(GLFW_KEY_F3),
/* 0xE03E */ NOCHAR(GLFW_KEY_F4),         NOCHAR(GLFW_KEY_F5),
/* 0xE040 */ NOCHAR(GLFW_KEY_F6),         NOCHAR(GLFW_KEY_F7),
/* 0xE042 */ NOCHAR(GLFW_KEY_F8),         NOCHAR(GLFW_KEY_F9),
/* 0xE044 */ NOCHAR(GLFW_KEY_F10),        NOCHAR(GLFW_KEY_NUMLOCK),
/* 0xE046 */ NOCHAR(GLFW_KEY_PAUSE),      NOCHAR(GLFW_KEY_HOME),
/* 0xE048 */ NOCHAR(GLFW_KEY_UP),         NOCHAR(GLFW_KEY_PAGEUP),
/* 0xE04A */ NOCHAR(GLFW_KEY_KP_SUBTRACT), NOCHAR(GLFW_KEY_LEFT),
/* 0xE04C */ NOCHAR(GLFW_KEY_KP_5),       NOCHAR(GLFW_KEY_RIGHT),
/* 0xE04E */ NOCHAR(GLFW_KEY_KP_ADD),     NOCHAR(GLFW_KEY_END),
/* 0xE050 */ NOCHAR(GLFW_KEY_DOWN),       NOCHAR(GLFW_KEY_PAGEDOWN),
/* 0xE052 */ NOCHAR(GLFW_KEY_INSERT),     NOCHAR(GLFW_KEY_DEL),
/* 0xE054 */ UNDEFINED, /* PRTSCR */      UNDEFINED,
/* 0xE056 */ {'\\','\\','|',0xFFFF},      NOCHAR(GLFW_KEY_F11),
/* 0xE058 */ NOCHAR(GLFW_KEY_F12),        UNDEFINED,
/* 0xE05A */ UNDEFINED,                   UNDEFINED, /* LWIN */
/* 0xE05C */ UNDEFINED, /* RWIN */        UNDEFINED, /* MENU */
/* 0xE05E */ UNDEFINED,                   UNDEFINED,
/* 0xE060 */ UNDEFINED,                   UNDEFINED,
/* 0xE062 */ UNDEFINED,                   UNDEFINED,
/* 0xE064 */ UNDEFINED,                   UNDEFINED,
/* 0xE066 */ UNDEFINED,                   UNDEFINED,
/* 0xE068 */ UNDEFINED,                   UNDEFINED,
/* 0xE06A */ UNDEFINED,                   UNDEFINED,
/* 0xE06C */ UNDEFINED,                   UNDEFINED,
/* 0xE06E */ UNDEFINED,                   UNDEFINED,
/* 0xE070 */ UNDEFINED,                   UNDEFINED,
/* 0xE072 */ UNDEFINED,                   UNDEFINED,
/* 0xE074 */ UNDEFINED,                   UNDEFINED,
/* 0xE076 */ UNDEFINED,                   UNDEFINED,
/* 0xE078 */ UNDEFINED,                   UNDEFINED,
/* 0xE07A */ UNDEFINED,                   UNDEFINED,
/* 0xE07C */ UNDEFINED,                   UNDEFINED,
/* 0xE07E */ UNDEFINED,                   UNDEFINED
};



//************************************************************************
//**** Keyboard Decoding *************************************************
//************************************************************************

//========================================================================
// _glfwMapRawKey() - Map a raw scancode to a Unicode character
//========================================================================

static int _glfwMapRawKey( int scancode, int qualifiers )
{
    struct key *keyvals;
    int        keycode;

    // Get possible key codings for this scancode
    keyvals = &scancode_to_key_us[ scancode ];

    // Select Unicode code depending on qualifiers
    if( qualifiers & QUAL_RALT )
    {
        keycode = keyvals->AltGr;
    }
    else if( qualifiers & (QUAL_LSHIFT|QUAL_RSHIFT) )
    {
        if( (qualifiers & QUAL_CAPSLOCK) &&
            (keyvals->Normal != keyvals->Caps) )
        {
            keycode = keyvals->Normal;
        }
        else
        {
            keycode = keyvals->Shift;
        }
    }
    else if( qualifiers & QUAL_CAPSLOCK )
    {
        keycode = keyvals->Caps;
    }
    else
    {
        keycode = keyvals->Normal;
    }

    // Special interpretations
    if( keycode >= 0xE000 && keycode <= 0xE8FF )
    {
        keycode = -(keycode - 0xE000 + GLFW_KEY_SPECIAL);
    }
    else if( keycode == 0xFFFF )
    {
        keycode = 0;
    }

    return keycode;
} ENDOFUNC(_glfwMapRawKey)


//========================================================================
// _glfwCreateKeyEvent() - Add a keyboard event to the event FIFO
//========================================================================

static void _glfwCreateKeyEvent( int scancode, int qualifiers, int action )
{
    _GLFWdosevent event;
    struct key_event *key = &event.Key;

    // Create event
    key->Type     = _GLFW_DOS_KEY_EVENT;
    key->Key      = _glfwMapRawKey( scancode, qualifiers );
    key->KeyNoMod = _glfwMapRawKey( scancode, QUAL_CAPSLOCK );
    key->Action   = action;

    // Post event
    _glfwPostDOSEvent( &event );
} ENDOFUNC(_glfwCreateKeyEvent)



//************************************************************************
//**** Keyboard Communication ********************************************
//************************************************************************

//========================================================================
// _glfwWaitForReadReady() / _glfwWaitForWriteReady()
// Wait for the keyboard controller to set the ready-for-read/write bit
//========================================================================

static int _glfwWaitForReadReady( void )
{
    int timeout = 16384;
    while( (timeout>0) && (!(inportb(0x64)&1)) ) timeout--;
    return timeout > 0;
} ENDOFUNC(_glfwWaitForReadReady)

static int _glfwWaitForWriteReady( void )
{
    int timeout = 4096;
    while( (timeout>0) && (inportb(0x64)&2) ) timeout--;
    return timeout > 0;
} ENDOFUNC(_glfwWaitForWriteReady)


//========================================================================
// _glfwSendKeyboardByte() - Send a byte to the keyboard controller
//========================================================================

static int _glfwSendKeyboardByte( unsigned char data )
{
    int resends = 4;
    int timeout, ret;

    do
    {
        if( !_glfwWaitForWriteReady() ) return 0;

        outportb( 0x60, data );

        timeout = 4096;
        while( --timeout > 0 )
        {
            if( !_glfwWaitForReadReady() ) return 0;

            ret = inportb( 0x60 );
            if( ret == 0xFA ) return 1;
            if( ret == 0xFE ) break;
        }
    }
    while( (resends-- > 0) && (timeout > 0) );

    return 0;
} ENDOFUNC(_glfwSendKeyboardByte)


//========================================================================
// _glfwSendKeyboardCommand() - Send a command sequence to the keyboard
//========================================================================

static int _glfwSendKeyboardCommand( unsigned char *cmd, int count )
{
    int i, ok = 1;

    // Force atomic keyboard communication session
    if( !_glfwKeyDrv.Interrupt ) DISABLE();

    // Send command sequence
    for( i = 0; i < count; ++ i )
    {
        if( !_glfwSendKeyboardByte( cmd[i] ) )
        {
            ok = 0;
            break;
        }
    }

    // Send "clear output buffer, enable keyboard"
    _glfwSendKeyboardByte( 0xF4 );

    if( !_glfwKeyDrv.Interrupt ) ENABLE();

    return ok;
} ENDOFUNC(_glfwSendKeyboardCommand)



//************************************************************************
//**** Miscellaneous Handling ********************************************
//************************************************************************

//========================================================================
// _glfwUpdateLeds() - Update keyboard leds
//========================================================================

static void _glfwUpdateLeds( int qualifiers )
{
    unsigned char cmd[2];
    cmd[0] = 0xED;
    cmd[1] = qualifiers & 7;
    _glfwSendKeyboardCommand( cmd, 2 );
} ENDOFUNC(_glfwUpdateLeds)



//************************************************************************
//**** Keyboard Interrupt Handler ****************************************
//************************************************************************

//========================================================================
// _glfwHandleCode() - Handle new scancode event
//========================================================================

static void _glfwHandleCode( int scancode, int keypress )
{
    if( scancode == GLFW_KEY_PAUSE && keypress )
    {
        // Pause
        _glfwCreateKeyEvent( GLFW_KEY_PAUSE, 0, GLFW_PRESS );
    }
    else if( scancode )
    {
        int tmp, qualifier;

        // Check if this is a qualifier key
        tmp = scancode_to_key_us[scancode].Normal;
        tmp += GLFW_KEY_SPECIAL - 0xE000;
        if( tmp == GLFW_KEY_LSHIFT )        qualifier = QUAL_LSHIFT;
        else if( tmp == GLFW_KEY_RSHIFT )   qualifier = QUAL_RSHIFT;
        else if( tmp == GLFW_KEY_LCTRL )    qualifier = QUAL_LCTRL;
        else if( tmp == GLFW_KEY_RCTRL )    qualifier = QUAL_RCTRL;
        else if( tmp == GLFW_KEY_LALT )     qualifier = QUAL_LALT;
        else if( tmp == GLFW_KEY_RALT )     qualifier = QUAL_RALT;
        else if( tmp == GLFW_KEY_NUMLOCK )  qualifier = QUAL_NUMLOCK;
        else if( tmp == GLFW_KEY_SCROLOCK ) qualifier = QUAL_SCROLOCK;
        else if( tmp == GLFW_KEY_CAPSLOCK ) qualifier = QUAL_CAPSLOCK;
        else                                qualifier = 0;

        if( keypress )
        {
            // Key press
            if( qualifier & QUAL_MODIFIERS )
            {
                _glfwKeyDrv.Qualifiers |= qualifier;
            }
            if( !(qualifier & QUAL_LEDS) )
            {
                _glfwCreateKeyEvent( scancode, _glfwKeyDrv.Qualifiers,
                                GLFW_PRESS );
            }
            else
            {
                _glfwKeyDrv.Qualifiers ^= qualifier;
                _glfwUpdateLeds( _glfwKeyDrv.Qualifiers );
            }
        }
        else
        {
            // Key release
            if( qualifier & QUAL_MODIFIERS )
            {
                _glfwKeyDrv.Qualifiers &= ~qualifier;
            }
            if( !(qualifier & QUAL_LEDS) )
            {
                _glfwCreateKeyEvent( scancode, _glfwKeyDrv.Qualifiers,
                                GLFW_RELEASE );
            }
        }
    }
} ENDOFUNC(_glfwHandleCode)


//========================================================================
// _glfwKeyInterrupt() - Keyboard interrupt routine
//========================================================================

static int _glfwKeyInterrupt( void )
{
    unsigned char keycode, scancode;

    _glfwKeyDrv.Interrupt ++;

    keycode = inportb( 0x60 );

    if( keycode <= 0xE1 )
    {
        if( _glfwKeyDrv.KeyPauseLoop )
        {
            if( ! --_glfwKeyDrv.KeyPauseLoop )
                _glfwHandleCode( GLFW_KEY_PAUSE, 1 );
        }
        else
        {
            switch( keycode )
            {
                case 0xE0:
                    _glfwKeyDrv.KeyEnhanced = 1;
                    break;
                case 0xE1:
                    _glfwKeyDrv.KeyPauseLoop = 5;
                    break;
                default:
                    scancode = keycode & 0x7F;
                    if( _glfwKeyDrv.KeyEnhanced )
                    {
                        scancode |= 0x80;
                        _glfwKeyDrv.KeyEnhanced = 0;
                    }
                    _glfwHandleCode( scancode, !(keycode & 0x80) );
            }
        }
    }

    _glfwKeyDrv.Interrupt --;

    if( ((keycode==0x4F) || (keycode==0x53)) &&
        (_glfwKeyDrv.Qualifiers & QUAL_LCTRL) &&
        (_glfwKeyDrv.Qualifiers & QUAL_RALT) )
    {
        // Hack alert:
        // Only SIGINT (but not Ctrl-Break) calls the destructors and will
        // safely clean up
        asm(
            "movb   $0x79,%%al\n\t"
            "call   ___djgpp_hw_exception\n\t"
            :
            :
            : "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory"
        );
    }

    asm(
        "inb    $0x61,%%al\n\t"
        "movb   %%al,%%ah\n\t"
        "orb    $0x80,%%al\n\t"
        "outb   %%al,$0x61\n\t"
        "xchgb  %%al,%%ah\n\t"
        "outb   %%al,$0x61\n\t"
        "movb   $0x20,%%al\n\t"
        "outb   %%al,$0x20\n\t"
        :
        :
        : "%eax"
    );

    return 0;
} ENDOFUNC(_glfwKeyInterrupt)



//************************************************************************
//****              Keyboard driver interface functions               ****
//************************************************************************

//========================================================================
// _glfwInitKeyboard() - Initialize keyboard driver
//========================================================================

int _glfwInitKeyboard( void )
{
    int s1, s2, s3;

    if( _glfwKeyboardInstalled )
    {
        return 0;
    }

    // Init keyboard state
    _glfwKeyDrv.LedsOK       = 1;
    _glfwKeyDrv.Interrupt    = 0;
    _glfwKeyDrv.KeyEnhanced  = 0;
    _glfwKeyDrv.KeyPauseLoop = 0;

    // Lock data buffers
    LOCKDATA(_glfwKeyboardInstalled);
    LOCKDATA(_glfwKeyDrv);
    LOCKDATA(scancode_to_key_us);

    // Lock functions
    LOCKFUNC(_glfwMapRawKey);
    LOCKFUNC(_glfwCreateKeyEvent);
    LOCKFUNC(_glfwWaitForReadReady);
    LOCKFUNC(_glfwWaitForWriteReady);
    LOCKFUNC(_glfwSendKeyboardByte);
    LOCKFUNC(_glfwSendKeyboardCommand);
    LOCKFUNC(_glfwUpdateLeds);
    LOCKFUNC(_glfwHandleCode);
    LOCKFUNC(_glfwKeyInterrupt);

    _farsetsel( __djgpp_dos_sel );
    _farnspokew( 0x41c, _farnspeekw(0x41a) );

    // Get current state of key qualifiers
    s1 = _farnspeekb( 0x417 );
    s2 = _farnspeekb( 0x418 );
    s3 = _farnspeekb( 0x496 );
    _glfwKeyDrv.Qualifiers = 0;
    if( s1 & 1 )  _glfwKeyDrv.Qualifiers |= QUAL_RSHIFT;
    if( s1 & 2 )  _glfwKeyDrv.Qualifiers |= QUAL_LSHIFT;
    if( s2 & 1 )  _glfwKeyDrv.Qualifiers |= QUAL_LCTRL;
    if( s2 & 2 )  _glfwKeyDrv.Qualifiers |= QUAL_LALT;
    if( s3 & 4 )  _glfwKeyDrv.Qualifiers |= QUAL_RCTRL;
    if( s3 & 8 )  _glfwKeyDrv.Qualifiers |= QUAL_RALT;
    if( s1 & 16 ) _glfwKeyDrv.Qualifiers |= QUAL_SCROLOCK;
    if( s1 & 32 ) _glfwKeyDrv.Qualifiers |= QUAL_NUMLOCK;
    if( s1 & 64 ) _glfwKeyDrv.Qualifiers |= QUAL_CAPSLOCK;
    _glfwUpdateLeds( _glfwKeyDrv.Qualifiers );

    // Install keyboard interrupt handler
    if( _glfwInstallDOSIrq( KEYB_IRQ, _glfwKeyInterrupt ) )
    {
        return 0;
    }

    _glfwKeyboardInstalled = 1;

    return 1;
}


//========================================================================
// _glfwTerminateKeyboard() - Terminate keyboard driver
//========================================================================

void _glfwTerminateKeyboard( void )
{
    int s1, s2, s3;

    if( !_glfwKeyboardInstalled )
    {
        return;
    }

    _glfwKeyboardInstalled = 0;

    // Uninstall keyboard interrupt handler
    _glfwRemoveDOSIrq( KEYB_IRQ );

    _farsetsel( __djgpp_dos_sel );
    _farnspokew( 0x41c, _farnspeekw(0x41a) );

    // Set current state of key qualifiers
    s1 = _farnspeekb( 0x417 ) & 0x80;
    s2 = _farnspeekb( 0x418 ) & 0xFC;
    s3 = _farnspeekb( 0x496 ) & 0xF3;
    if(_glfwKeyDrv.Qualifiers & QUAL_RSHIFT)   s1 |= 1;
    if(_glfwKeyDrv.Qualifiers & QUAL_LSHIFT)   s1 |= 2;
    if(_glfwKeyDrv.Qualifiers & QUAL_LCTRL)    {s2 |= 1; s1 |= 4;}
    if(_glfwKeyDrv.Qualifiers & QUAL_LALT)     {s2 |= 2; s1 |= 8;}
    if(_glfwKeyDrv.Qualifiers & QUAL_RCTRL)    {s3 |= 4; s1 |= 4;}
    if(_glfwKeyDrv.Qualifiers & QUAL_RALT)     {s3 |= 8; s1 |= 8;}
    if(_glfwKeyDrv.Qualifiers & QUAL_SCROLOCK) s1 |= 16;
    if(_glfwKeyDrv.Qualifiers & QUAL_NUMLOCK)  s1 |= 32;
    if(_glfwKeyDrv.Qualifiers & QUAL_CAPSLOCK) s1 |= 64;
    _farnspokeb( 0x417, s1 );
    _farnspokeb( 0x418, s2 );
    _farnspokeb( 0x496, s3 );
    _glfwUpdateLeds( _glfwKeyDrv.Qualifiers );
}
