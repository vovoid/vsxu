// This is blatantly copied from SDL, should SDL change, this needs to be overwritten.
// What has been done is: s/SDL/VSX/g

#pragma once

typedef enum
{
    VSX_SCANCODE_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    VSX_SCANCODE_A = 4,
    VSX_SCANCODE_B = 5,
    VSX_SCANCODE_C = 6,
    VSX_SCANCODE_D = 7,
    VSX_SCANCODE_E = 8,
    VSX_SCANCODE_F = 9,
    VSX_SCANCODE_G = 10,
    VSX_SCANCODE_H = 11,
    VSX_SCANCODE_I = 12,
    VSX_SCANCODE_J = 13,
    VSX_SCANCODE_K = 14,
    VSX_SCANCODE_L = 15,
    VSX_SCANCODE_M = 16,
    VSX_SCANCODE_N = 17,
    VSX_SCANCODE_O = 18,
    VSX_SCANCODE_P = 19,
    VSX_SCANCODE_Q = 20,
    VSX_SCANCODE_R = 21,
    VSX_SCANCODE_S = 22,
    VSX_SCANCODE_T = 23,
    VSX_SCANCODE_U = 24,
    VSX_SCANCODE_V = 25,
    VSX_SCANCODE_W = 26,
    VSX_SCANCODE_X = 27,
    VSX_SCANCODE_Y = 28,
    VSX_SCANCODE_Z = 29,

    VSX_SCANCODE_1 = 30,
    VSX_SCANCODE_2 = 31,
    VSX_SCANCODE_3 = 32,
    VSX_SCANCODE_4 = 33,
    VSX_SCANCODE_5 = 34,
    VSX_SCANCODE_6 = 35,
    VSX_SCANCODE_7 = 36,
    VSX_SCANCODE_8 = 37,
    VSX_SCANCODE_9 = 38,
    VSX_SCANCODE_0 = 39,

    VSX_SCANCODE_RETURN = 40,
    VSX_SCANCODE_ESCAPE = 41,
    VSX_SCANCODE_BACKSPACE = 42,
    VSX_SCANCODE_TAB = 43,
    VSX_SCANCODE_SPACE = 44,

    VSX_SCANCODE_MINUS = 45,
    VSX_SCANCODE_EQUALS = 46,
    VSX_SCANCODE_LEFTBRACKET = 47,
    VSX_SCANCODE_RIGHTBRACKET = 48,
    VSX_SCANCODE_BACKSLASH = 49, /**< Located at the lower left of the return
                                  *   key on ISO keyboards and at the right end
                                  *   of the QWERTY row on ANSI keyboards.
                                  *   Produces REVERSE SOLIDUS (backslash) and
                                  *   VERTICAL LINE in a US layout, REVERSE
                                  *   SOLIDUS and VERTICAL LINE in a UK Mac
                                  *   layout, NUMBER SIGN and TILDE in a UK
                                  *   Windows layout, DOLLAR SIGN and POUND SIGN
                                  *   in a Swiss German layout, NUMBER SIGN and
                                  *   APOSTROPHE in a German layout, GRAVE
                                  *   ACCENT and POUND SIGN in a French Mac
                                  *   layout, and ASTERISK and MICRO SIGN in a
                                  *   French Windows layout.
                                  */
    VSX_SCANCODE_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate VSX_SCANCODE_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because LSK
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
    VSX_SCANCODE_SEMICOLON = 51,
    VSX_SCANCODE_APOSTROPHE = 52,
    VSX_SCANCODE_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                              *   and ISO keyboards). Produces GRAVE ACCENT and
                              *   TILDE in a US Windows layout and in US and UK
                              *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                              *   and NOT SIGN in a UK Windows layout, SECTION
                              *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                              *   layouts on ISO keyboards, SECTION SIGN and
                              *   DEGREE SIGN in a Swiss German layout (Mac:
                              *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                              *   DEGREE SIGN in a German layout (Mac: only on
                              *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                              *   French Windows layout, COMMERCIAL AT and
                              *   NUMBER SIGN in a French Mac layout on ISO
                              *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                              *   SIGN in a Swiss German, German, or French Mac
                              *   layout on ANSI keyboards.
                              */
    VSX_SCANCODE_COMMA = 54,
    VSX_SCANCODE_PERIOD = 55,
    VSX_SCANCODE_SLASH = 56,

    VSX_SCANCODE_CAPSLOCK = 57,

    VSX_SCANCODE_F1 = 58,
    VSX_SCANCODE_F2 = 59,
    VSX_SCANCODE_F3 = 60,
    VSX_SCANCODE_F4 = 61,
    VSX_SCANCODE_F5 = 62,
    VSX_SCANCODE_F6 = 63,
    VSX_SCANCODE_F7 = 64,
    VSX_SCANCODE_F8 = 65,
    VSX_SCANCODE_F9 = 66,
    VSX_SCANCODE_F10 = 67,
    VSX_SCANCODE_F11 = 68,
    VSX_SCANCODE_F12 = 69,

    VSX_SCANCODE_PRINTSCREEN = 70,
    VSX_SCANCODE_SCROLLLOCK = 71,
    VSX_SCANCODE_PAUSE = 72,
    VSX_SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    VSX_SCANCODE_HOME = 74,
    VSX_SCANCODE_PAGEUP = 75,
    VSX_SCANCODE_DELETE = 76,
    VSX_SCANCODE_END = 77,
    VSX_SCANCODE_PAGEDOWN = 78,
    VSX_SCANCODE_RIGHT = 79,
    VSX_SCANCODE_LEFT = 80,
    VSX_SCANCODE_DOWN = 81,
    VSX_SCANCODE_UP = 82,

    VSX_SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
    VSX_SCANCODE_KP_DIVIDE = 84,
    VSX_SCANCODE_KP_MULTIPLY = 85,
    VSX_SCANCODE_KP_MINUS = 86,
    VSX_SCANCODE_KP_PLUS = 87,
    VSX_SCANCODE_KP_ENTER = 88,
    VSX_SCANCODE_KP_1 = 89,
    VSX_SCANCODE_KP_2 = 90,
    VSX_SCANCODE_KP_3 = 91,
    VSX_SCANCODE_KP_4 = 92,
    VSX_SCANCODE_KP_5 = 93,
    VSX_SCANCODE_KP_6 = 94,
    VSX_SCANCODE_KP_7 = 95,
    VSX_SCANCODE_KP_8 = 96,
    VSX_SCANCODE_KP_9 = 97,
    VSX_SCANCODE_KP_0 = 98,
    VSX_SCANCODE_KP_PERIOD = 99,

    VSX_SCANCODE_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    VSX_SCANCODE_APPLICATION = 101, /**< windows contextual menu, compose */
    VSX_SCANCODE_POWER = 102, /**< The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
    VSX_SCANCODE_KP_EQUALS = 103,
    VSX_SCANCODE_F13 = 104,
    VSX_SCANCODE_F14 = 105,
    VSX_SCANCODE_F15 = 106,
    VSX_SCANCODE_F16 = 107,
    VSX_SCANCODE_F17 = 108,
    VSX_SCANCODE_F18 = 109,
    VSX_SCANCODE_F19 = 110,
    VSX_SCANCODE_F20 = 111,
    VSX_SCANCODE_F21 = 112,
    VSX_SCANCODE_F22 = 113,
    VSX_SCANCODE_F23 = 114,
    VSX_SCANCODE_F24 = 115,
    VSX_SCANCODE_EXECUTE = 116,
    VSX_SCANCODE_HELP = 117,
    VSX_SCANCODE_MENU = 118,
    VSX_SCANCODE_SELECT = 119,
    VSX_SCANCODE_STOP = 120,
    VSX_SCANCODE_AGAIN = 121,   /**< redo */
    VSX_SCANCODE_UNDO = 122,
    VSX_SCANCODE_CUT = 123,
    VSX_SCANCODE_COPY = 124,
    VSX_SCANCODE_PASTE = 125,
    VSX_SCANCODE_FIND = 126,
    VSX_SCANCODE_MUTE = 127,
    VSX_SCANCODE_VOLUMEUP = 128,
    VSX_SCANCODE_VOLUMEDOWN = 129,
/* not sure whether there's a reason to enable these */
/*     VSX_SCANCODE_LOCKINGCAPSLOCK = 130,  */
/*     VSX_SCANCODE_LOCKINGNUMLOCK = 131, */
/*     VSX_SCANCODE_LOCKINGSCROLLLOCK = 132, */
    VSX_SCANCODE_KP_COMMA = 133,
    VSX_SCANCODE_KP_EQUALSAS400 = 134,

    VSX_SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    VSX_SCANCODE_INTERNATIONAL2 = 136,
    VSX_SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
    VSX_SCANCODE_INTERNATIONAL4 = 138,
    VSX_SCANCODE_INTERNATIONAL5 = 139,
    VSX_SCANCODE_INTERNATIONAL6 = 140,
    VSX_SCANCODE_INTERNATIONAL7 = 141,
    VSX_SCANCODE_INTERNATIONAL8 = 142,
    VSX_SCANCODE_INTERNATIONAL9 = 143,
    VSX_SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
    VSX_SCANCODE_LANG2 = 145, /**< Hanja conversion */
    VSX_SCANCODE_LANG3 = 146, /**< Katakana */
    VSX_SCANCODE_LANG4 = 147, /**< Hiragana */
    VSX_SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
    VSX_SCANCODE_LANG6 = 149, /**< reserved */
    VSX_SCANCODE_LANG7 = 150, /**< reserved */
    VSX_SCANCODE_LANG8 = 151, /**< reserved */
    VSX_SCANCODE_LANG9 = 152, /**< reserved */

    VSX_SCANCODE_ALTERASE = 153, /**< Erase-Eaze */
    VSX_SCANCODE_SYSREQ = 154,
    VSX_SCANCODE_CANCEL = 155,
    VSX_SCANCODE_CLEAR = 156,
    VSX_SCANCODE_PRIOR = 157,
    VSX_SCANCODE_RETURN2 = 158,
    VSX_SCANCODE_SEPARATOR = 159,
    VSX_SCANCODE_OUT = 160,
    VSX_SCANCODE_OPER = 161,
    VSX_SCANCODE_CLEARAGAIN = 162,
    VSX_SCANCODE_CRSEL = 163,
    VSX_SCANCODE_EXSEL = 164,

    VSX_SCANCODE_KP_00 = 176,
    VSX_SCANCODE_KP_000 = 177,
    VSX_SCANCODE_THOUSANDSSEPARATOR = 178,
    VSX_SCANCODE_DECIMALSEPARATOR = 179,
    VSX_SCANCODE_CURRENCYUNIT = 180,
    VSX_SCANCODE_CURRENCYSUBUNIT = 181,
    VSX_SCANCODE_KP_LEFTPAREN = 182,
    VSX_SCANCODE_KP_RIGHTPAREN = 183,
    VSX_SCANCODE_KP_LEFTBRACE = 184,
    VSX_SCANCODE_KP_RIGHTBRACE = 185,
    VSX_SCANCODE_KP_TAB = 186,
    VSX_SCANCODE_KP_BACKSPACE = 187,
    VSX_SCANCODE_KP_A = 188,
    VSX_SCANCODE_KP_B = 189,
    VSX_SCANCODE_KP_C = 190,
    VSX_SCANCODE_KP_D = 191,
    VSX_SCANCODE_KP_E = 192,
    VSX_SCANCODE_KP_F = 193,
    VSX_SCANCODE_KP_XOR = 194,
    VSX_SCANCODE_KP_POWER = 195,
    VSX_SCANCODE_KP_PERCENT = 196,
    VSX_SCANCODE_KP_LESS = 197,
    VSX_SCANCODE_KP_GREATER = 198,
    VSX_SCANCODE_KP_AMPERSAND = 199,
    VSX_SCANCODE_KP_DBLAMPERSAND = 200,
    VSX_SCANCODE_KP_VERTICALBAR = 201,
    VSX_SCANCODE_KP_DBLVERTICALBAR = 202,
    VSX_SCANCODE_KP_COLON = 203,
    VSX_SCANCODE_KP_HASH = 204,
    VSX_SCANCODE_KP_SPACE = 205,
    VSX_SCANCODE_KP_AT = 206,
    VSX_SCANCODE_KP_EXCLAM = 207,
    VSX_SCANCODE_KP_MEMSTORE = 208,
    VSX_SCANCODE_KP_MEMRECALL = 209,
    VSX_SCANCODE_KP_MEMCLEAR = 210,
    VSX_SCANCODE_KP_MEMADD = 211,
    VSX_SCANCODE_KP_MEMSUBTRACT = 212,
    VSX_SCANCODE_KP_MEMMULTIPLY = 213,
    VSX_SCANCODE_KP_MEMDIVIDE = 214,
    VSX_SCANCODE_KP_PLUSMINUS = 215,
    VSX_SCANCODE_KP_CLEAR = 216,
    VSX_SCANCODE_KP_CLEARENTRY = 217,
    VSX_SCANCODE_KP_BINARY = 218,
    VSX_SCANCODE_KP_OCTAL = 219,
    VSX_SCANCODE_KP_DECIMAL = 220,
    VSX_SCANCODE_KP_HEXADECIMAL = 221,

    VSX_SCANCODE_LCTRL = 224,
    VSX_SCANCODE_LSHIFT = 225,
    VSX_SCANCODE_LALT = 226, /**< alt, option */
    VSX_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
    VSX_SCANCODE_RCTRL = 228,
    VSX_SCANCODE_RSHIFT = 229,
    VSX_SCANCODE_RALT = 230, /**< alt gr, option */
    VSX_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

    VSX_SCANCODE_MODE = 257,    /**< I'm not sure if this is really not covered
                                 *   by any of the above, but since there's a
                                 *   special KMOD_MODE for it I'm adding it here
                                 */

    /* @} *//* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    VSX_SCANCODE_AUDIONEXT = 258,
    VSX_SCANCODE_AUDIOPREV = 259,
    VSX_SCANCODE_AUDIOSTOP = 260,
    VSX_SCANCODE_AUDIOPLAY = 261,
    VSX_SCANCODE_AUDIOMUTE = 262,
    VSX_SCANCODE_MEDIASELECT = 263,
    VSX_SCANCODE_WWW = 264,
    VSX_SCANCODE_MAIL = 265,
    VSX_SCANCODE_CALCULATOR = 266,
    VSX_SCANCODE_COMPUTER = 267,
    VSX_SCANCODE_AC_SEARCH = 268,
    VSX_SCANCODE_AC_HOME = 269,
    VSX_SCANCODE_AC_BACK = 270,
    VSX_SCANCODE_AC_FORWARD = 271,
    VSX_SCANCODE_AC_STOP = 272,
    VSX_SCANCODE_AC_REFRESH = 273,
    VSX_SCANCODE_AC_BOOKMARKS = 274,

    /* @} *//* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
    /* @{ */

    VSX_SCANCODE_BRIGHTNESSDOWN = 275,
    VSX_SCANCODE_BRIGHTNESSUP = 276,
    VSX_SCANCODE_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    VSX_SCANCODE_KBDILLUMTOGGLE = 278,
    VSX_SCANCODE_KBDILLUMDOWN = 279,
    VSX_SCANCODE_KBDILLUMUP = 280,
    VSX_SCANCODE_EJECT = 281,
    VSX_SCANCODE_SLEEP = 282,

    VSX_SCANCODE_APP1 = 283,
    VSX_SCANCODE_APP2 = 284,

    /* @} *//* Walther keys */

    /* Add any other keys here. */

    VSX_NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
} vsx_input_scancode;
